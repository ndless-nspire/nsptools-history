package ndless.install;

import javax.swing.JOptionPane;

import ndless.install.common.TempFileManager;
import ndless.install.gui.ContinueDialog;
import ndless.install.gui.MainFrame;
import ndless.install.injection.Injector;
import ndless.install.setup.ConnectivitySetup;

public class Main {

	public static void main(String[] args) {
		MainFrame.create();
		ContinueDialog.message("You are about to install Ndless on a TI-Nspire.\n" +
				"\n" +
				"The installation process may copy files on the device, transfer\n" +
				"a new OS version to the device, reboot it, and install hooks in\n" +
				"the OS. The configuration of the device will be lost, but the\n" +
				"documents shouldn't be.\n" +
				"\n" +
				"If you are OK with it, make sure that TI-Nspire Computer Link\n" +
				"Software is installed on this computer and that the device is\n" +
				"plugged in, then do not interfer with the installation except\n" +
				"if asked to do so.\n" +
				"\n" +
				"Ready to continue?", JOptionPane.WARNING_MESSAGE);
		MainFrame.setProgressBarVisibility(true);
		restart();
	}

	public static void restart() {
		try {
			TempFileManager.getTempDir(true);
			new ConnectivitySetup().setup();
			new Injector().inject();
		} catch (Exception e) {
			MainFrame.abort();
			MainFrame.logException(e);
			MainFrame.log("Installation aborted.");
		}
	}

}
