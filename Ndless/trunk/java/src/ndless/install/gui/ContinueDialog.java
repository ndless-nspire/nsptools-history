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
