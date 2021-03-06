package ndless.install.gui;

import javax.swing.JLabel;
import javax.swing.JOptionPane;

public class ContinueDialog {
	/**
	 * Exits the application if "No" choosen by the user
	 */
	public static void ask(String question) {
		message(question, JOptionPane.INFORMATION_MESSAGE);
	}
	
	/**
	 * Same as ask, but question contains HTML surrounded with <html>.
	 */
	public static void askFormatted(String question) {
		message(new JLabel(question), JOptionPane.INFORMATION_MESSAGE);
	}
	
	/**
	 * @param message String or JLabel with HTML
	 * @param messageType @see javax.swing.JOptionPane : messageType
	 */
	public static void message(Object message, int messageType) {
		if (JOptionPane.showConfirmDialog(MainFrame.getInstance(),
				message, "Ndless",
				JOptionPane.YES_NO_OPTION, messageType) != JOptionPane.YES_OPTION) {
			MainFrame.exit();
		}
	}
}
