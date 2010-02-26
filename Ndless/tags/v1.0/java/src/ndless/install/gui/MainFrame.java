/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Olivier ARMAND
 * <olivier.calc@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 */

package ndless.install.gui;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.plaf.FontUIResource;

import ndless.install.Main;
import ndless.install.common.NdlessException;

public class MainFrame extends JFrame {
	private static final long serialVersionUID = -644145495498137878L;

	private static MainFrame instance = null;

	private JTextPane logtext;

	private JProgressBar currentStepProgressBar;

	private boolean aborted = false;

	private CurrentStep currentStep;

	private JButton button;

	private JScrollPane logScrollPane;

	private JPanel currentStepProgressPanel;

	private Box progressBox;

	public static MainFrame getInstance() {
		return instance;
	}

	protected MainFrame() {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {
		}
		initPLAFFonts();
		JPanel logPanel = new JPanel();
		currentStepProgressPanel = new JPanel(new BorderLayout());
		currentStepProgressPanel.setBorder(BorderFactory.createEmptyBorder(0,
				10, 5, 10));
		final Container contentPane = this.getContentPane();
		contentPane.setLayout(new BorderLayout());
		contentPane.add(logPanel, BorderLayout.CENTER);
		contentPane.add(currentStepProgressPanel, BorderLayout.SOUTH);
		this.setTitle("Ndless Installer");
		this.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		this.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				if (aborted
						|| JOptionPane
								.showConfirmDialog(
										MainFrame.getInstance(),
										"Do you really want to abort the installation?",
										"Ndless", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
					exit();
				}
			}
		});
		logtext = new JTextPane();
		logtext.setEditable(false);
		logtext.setBorder(BorderFactory.createEtchedBorder());
		logScrollPane = new JScrollPane(logtext);
		logScrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		logScrollPane.setPreferredSize(new Dimension(400, 250));
		logPanel.add(logScrollPane);
		progressBox = Box.createHorizontalBox();
		currentStepProgressBar = new JProgressBar(0, 100);
		currentStepProgressBar.setIndeterminate(true);
		currentStepProgressBar.setVisible(false);
		progressBox.add(currentStepProgressBar);
		progressBox.add(Box.createHorizontalGlue());
		button = new JButton();
		progressBox.add(button);
		button.setVisible(false);
		currentStepProgressPanel.add(progressBox, BorderLayout.CENTER);
		this.pack();
		setLocationRelativeTo(null); // center it
		this.setResizable(false);
		this.setVisible(true);
	}

	private void changeFontFamilyForUIProperty(String UIPropertyName,
			String familyName) {
		String fontPropertyName = (new StringBuilder()).append(UIPropertyName)
				.append(".font").toString();
		FontUIResource origFont = (FontUIResource) UIManager
				.get(fontPropertyName);
		UIManager.put(fontPropertyName, new FontUIResource(familyName, origFont
				.getStyle(), origFont.getSize()));
	}

	private void initPLAFFonts() {
		changeFontFamilyForUIProperty("TextField", "Arial");
		changeFontFamilyForUIProperty("Label", "Arial");
		changeFontFamilyForUIProperty("TextPane", "Arial");
		changeFontFamilyForUIProperty("TextArea", "Arial");
	}

	/**
	 * Also: - Sets up the GUI - Shows itself
	 */
	public static void create() {
		instance = new MainFrame();
	}

	@SuppressWarnings("unchecked")
	private static String getClassName(Class c) {
		String FQClassName = c.getName();
		int firstChar;
		firstChar = FQClassName.lastIndexOf('.') + 1;
		if (firstChar > 0) {
			FQClassName = FQClassName.substring(firstChar);
		}
		return FQClassName;
	}

	private static void scrollLogToTheEnd() {
		instance.logtext.setCaretPosition(instance.logtext.getText().length());
	}

	public static void log(final String str) {
		log(str, true);
	}

	public static void log(final String str, final boolean appendNL) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.logtext.setText(instance.logtext.getText() + str
						+ (appendNL ? "\n" : ""));
				scrollLogToTheEnd();
			}
		});
	}

	public static void logException(Exception e) {
		log("ERROR"
				+ (e instanceof NdlessException ? "" : " ("
						+ getClassName(e.getClass()) + ")")
				+ (e.getMessage() != null ? ": " + e.getMessage() : ""));
	}

	public static void emptyLog() {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.logtext.setText(null);
			}
		});
	}

	/**
	 * Sets the activity status and shows the retry button
	 */
	public static void abort() {
		instance.aborted = true;
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.currentStepProgressBar.setVisible(false);
				instance.button.setText("Retry");
				instance.button.setVisible(true);
				instance.button.requestFocusInWindow();
			}
		});
		instance.button.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				instance.button.setVisible(false);
				instance.button.removeActionListener(this);
				restart();
			}
		});
	}

	public static void setProgressBarVisibility(final boolean visible) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.currentStepProgressBar.setVisible(visible);
				instance.pack(); // validate() doesn't seem to work...
			}
		});
	}

	/**
	 * Sets the activity status and shows the exit button
	 */
	public static void canExit() {
		instance.currentStep = null;
		instance.aborted = true;
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.currentStepProgressBar.setVisible(false);
				instance.button.setText("Exit");
				instance.button.setVisible(true);
				instance.button.requestFocusInWindow();
			}
		});
		instance.button.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				instance.setVisible(false);
				(new Thread() {
					public void run() {
						exit();
					}
				}).start();
			}
		});
	}

	protected static void restart() {
		emptyLog();
		instance.aborted = false;
		instance.currentStepProgressBar.setIndeterminate(true);
		instance.currentStepProgressBar.setVisible(true);
		instance.pack();
		(new Thread() {
			public void run() {
				Main.restart();
			}
		}).start();
	}

	public static void setCurrentStep(CurrentStep current) {
		instance.currentStep = current;
	}

	public static void initStepProgress() {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.currentStepProgressBar.setValue(0);
				instance.currentStepProgressBar.setStringPainted(true);
				instance.currentStepProgressBar.setIndeterminate(false);
			}
		});
	}

	public static void updateStepProgress(final int percent) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.currentStepProgressBar.setValue(percent);
			}
		});
	}

	public static void setStepProgressCompleted() {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.currentStepProgressBar.setStringPainted(false);
				instance.currentStepProgressBar.setIndeterminate(true);
			}
		});
	}

	public static void exit() {
		if (instance.currentStep != null)
			try {
				instance.currentStep.abort();
			} catch (Exception e) {
			}
		System.exit(0);
	}

}
