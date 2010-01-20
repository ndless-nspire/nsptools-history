package ndless.install.injection;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;

import ndless.install.common.NdlessException;
import ndless.install.common.TempFileManager;
import ndless.install.common.Utils;
import ndless.install.common.Utils.Status;
import ndless.install.gui.ContinueDialog;
import ndless.install.gui.CurrentStep;
import ndless.install.gui.MainFrame;

import com.ti.eps.navnet.Version;
import com.ti.eps.pxconnect.app.DeviceException;
import com.ti.eps.pxconnect.app.DeviceFileNotFoundException;
import com.ti.eps.pxconnect.app.DeviceInfo;
import com.ti.eps.pxconnect.app.GenericCommException;
import com.ti.eps.pxconnect.app.IPXConnectApp;
import com.ti.eps.pxconnect.app.PXConnectApp;
import com.ti.eps.pxconnect.app.PXDevice;
import com.ti.eps.pxconnect.app.PXStatus;
import com.ti.eps.pxconnect.connectivity.IConnectivityAPI;
import com.ti.eps.pxconnect.connectivity.IllegalNavnetStateException;
import com.ti.eps.pxconnect.connectivity.NavnetConnectivityAPI;
import com.ti.eps.pxconnect.util.ResourceManagement;
import com.ti.eps.util.LoggerUtil;

public class Injector implements CurrentStep {
	private static final boolean forceOSUpdate = false;
	private static final String deviceInstallDir = "ndless-installation";
	private static final String resPackage = "/ndless/install/resources/";
	// TODO update for CAS
	private static final String localRequiredOSFilename = "tinspire_1.1.9253.tno";
	private static final File userFolder = new File("userfiles");
	private static final IPXConnectApp pxConnectApp = PXConnectApp
			.getInstance();
	private IConnectivityAPI conApi;
	private PXStatus pxStatus;
	private File requiredOSFile;
	// When anonymous classes need to exchange data
	private String anonSharedStr;
	private boolean hadNoOS;

	public void inject() throws Exception {
		try {
			MainFrame.setCurrentStep(this);
			setupUserFolder();
			MainFrame.log("Connecting to the device...");
			setupNavnet();
			DeviceInfo deviceInfo = pxConnectApp.connectedDevice()
					.getDeviceInfo();
			if (!"NonCas".equals(deviceInfo.getDeviceType()))
				throw new NdlessException(
						"Only non-CAS TI-Nspire is currently supported.");
			Version currentOsVersion = deviceInfo.getOsVersion();
			this.hadNoOS = hasNoOS(currentOsVersion);
			downgradeOSIfNeeded(currentOsVersion);
			
			MainFrame.log("Preparing required files on the device:");
			MainFrame.log(" - Creating Ndless installation directory");
			mkDeviceDocDir(deviceInstallDir);
			// strings.res with shellcode run by buffer overflow at boot time
			File ndlessBootFile = TempFileManager
					.loadResourceToTempDir(resPackage + "boot.tns");
			if (!ndlessBootFile.exists())
				throw new NdlessException(
						"Corrupted Ndless directory: missing file '"
								+ ndlessBootFile.getPath() + "'");
			MainFrame.log(" - Backuping overwritten files");
			copyFileLocallyOnDevice("/../phoenix/syst/locales/en/strings.res", deviceInstallDir
					+ "/strbackup.tns", true);

			MainFrame.log(" - Transfering Ndless boot");
			sendFileToDevice(ndlessBootFile.getPath(), deviceInstallDir
					+ "/boot.tns");

			File execFile = new File(userFolder, "runme.tns");
			if (!execFile.exists()) {
				MainFrame.log(" - Transfering executable ('"
						+ execFile.getPath() + "' not found, using demo file)");
				execFile = TempFileManager.loadResourceToTempDir(resPackage
						+ "runme.tns");
			} else
				MainFrame.log(" - Transfering executable");
			sendFileToDevice(execFile.getPath(), deviceInstallDir
					+ "/runme.tns");

			MainFrame.log(" - Deleting copier");
			deleteFileOnDevice("/../phoenix/syst/locales/copysamples");
			copyCopier();
			
			MainFrame.log(" - Deleting configuration");
			deleteFileOnDevice("/../phoenix/syst/settings/initialized");
			
			rebootDevice(true);
			waitForDeviceInitialization();
			rebootDevice(false);
			
			MainFrame.log("The device is ready! Just wait for the reboot.");
			MainFrame.log("You can now close the installer.");
			MainFrame.canExit();
		} catch (DeviceException de) {
			throw new NdlessException(ResourceManagement.getRes().getString(
					de.getMessage()));
		}
	}

	private void setupNavnet() throws InterruptedException, IOException {
		LoggerUtil.setupLogging(TempFileManager.getTempDir() + File.separator
				+ "pxcl.log", Level.OFF, Level.OFF);
		// NavnetConnectivityAPI() uses the level of the logger to set the
		// native library log level
		// Level OFF is unfortunately not supported. Logs will appear on stdout
		Logger.getLogger("com.ti.eps.pxconnect").setLevel(Level.SEVERE);
		try {
			conApi = NavnetConnectivityAPI.getInstance();
		} catch (IllegalNavnetStateException e) {
			throw new NdlessException(
					"Another instance of Ndless or TI-Nspire Computer Link is already running");
		}
		pxConnectApp.setConnectAPI(conApi);
		PXDevice[] devices;
		// required before NavNet.init and enumeration, else
		// no device would be found
		Thread.sleep(2500);
		devices = pxConnectApp.enumerateConnectedDevices();
		if (devices.length == 0)
			throw new NdlessException("No device connected");
		pxConnectApp.connect(devices[0]);
	}

	private void shutdownConApi() {
		if (conApi != null) {
			conApi.shutdown();
			conApi = null;
		}
	}

	private void setupUserFolder() {
		if (!userFolder.exists())
			userFolder.mkdir();
	}

	private boolean hasNoOS(Version currentOsVersion) {
		return currentOsVersion.getMajor() == 0
				&& currentOsVersion.getMinor() == 0
				&& currentOsVersion.getBuild() == 0;
	}

	private void downgradeOSIfNeeded(Version currentOsVersion)
			throws Exception, InterruptedException {
		if (forceOSUpdate || currentOsVersion.getMajor() != 1
				|| currentOsVersion.getMinor() != 1
				|| currentOsVersion.getBuild() != 9253) {
			String action = "downgrade the OS to version";
			String direction = "Downgrading";
			if (hadNoOS) {
				MainFrame.log("No OS is installed.");
				action = "install OS version";
				direction = "Upgrading";
			} else
				MainFrame.log("The current OS version is "
						+ currentOsVersion.getMajor() + "."
						+ currentOsVersion.getMinor() + "."
						+ currentOsVersion.getBuild());
			ContinueDialog.ask("Ndless needs to " + action
					+ " 1.1.9253.\nContinue?");
			requiredOSFile = new File(userFolder, localRequiredOSFilename);
			if (requiredOSFile.exists()) {
				MainFrame.log("Using the local OS copy '"
						+ requiredOSFile.getPath() + "'");
			} else {
				MainFrame.log("Downloading OS version 1.1.9253...");
				downloadRequiredOS();
			}
			MainFrame.log(direction + " the OS of the device...");
			installRequiredOS();
			waitForTheDeviceToReboot(true);
		}
	}

	/**
	 * The Runnable calls a NavNet method that uses pxStatus. The Runnable
	 * doesn't need to handle DeviceException, it can just discard it.
	 * 
	 * @param thread
	 * @throws InterruptedException
	 */
	private void executeNavNetMethodWithProgress(final Runnable runnable,
			final int statusUpdatePeriodInMs) throws InterruptedException {
		MainFrame.initStepProgress();
		new Thread(runnable).start();
		while (pxStatus.inProgress()) {
			MainFrame.updateStepProgress(pxStatus.progressPercentage());
			Thread.sleep(statusUpdatePeriodInMs);
		}
		if (pxStatus.errorOccured())
			throw new NdlessException(pxStatus.getError());
		MainFrame.setStepProgressCompleted();
	}

	/**
	 * Wait a number of ticks, regularly updating the progress of the step
	 * 
	 * @throws InterruptedException
	 */
	private void waitWithProgress(int ticksNum, int statusUpdatePeriodInMs)
			throws InterruptedException {
		MainFrame.initStepProgress();
		for (int i = 1; i <= ticksNum; i++) {
			Thread.sleep(statusUpdatePeriodInMs);
			MainFrame.updateStepProgress((int) ((float) i / ticksNum * 100));
		}
		MainFrame.setStepProgressCompleted();
	}

	private void downloadRequiredOS() throws Exception {
		this.requiredOSFile = TempFileManager
				.createTempFile(localRequiredOSFilename);
		final Status dlStatus = new Utils.Status();
		MainFrame.initStepProgress();
		(new Thread() {
			public void run() {
				try {
					// TODO update for CAS
					Utils
							.downloadFile(
									new URL(
											"http://ti.bank.free.fr/modules/archives/download.php?id=1394"),
									requiredOSFile, dlStatus);
				} catch (Exception e) {
				} // handled below
			}
		}).start();
		while (!dlStatus.isDone()) {
			MainFrame.updateStepProgress(dlStatus.getCompletionPercent());
			Thread.sleep(2000);
		}
		MainFrame.setStepProgressCompleted();
		if (dlStatus.getException() != null) {
			MainFrame
					.log("Ndless can't download the file. You may try to copy it manually to Ndless directory as '"
							+ new File(userFolder, localRequiredOSFilename)
									.getPath()
							+ "' and retry the installation.");
			throw dlStatus.getException();
		}
		try {
			Utils.copyFile(requiredOSFile, new File(userFolder,
					localRequiredOSFilename));
			MainFrame.log("OS file kept locally for next installations.");
		} catch (Exception e) {
		}
	}

	private void installRequiredOS() throws InterruptedException {
		pxStatus = new PXStatus();
		executeNavNetMethodWithProgress(new Runnable() {
			public void run() {
				pxConnectApp.installOS(requiredOSFile.getPath(), pxStatus);
			}
		}, 300);
		MainFrame
				.log("Waiting for the installation of the OS by the device...");
		waitWithProgress(hadNoOS ? 20000 / 300 : 65000 / 300, 300);
	}

	/**
	 * Creates a document folder if it doesn't exist on the device
	 * 
	 * @throws DeviceException
	 */
	private void mkDeviceDocDir(String dirName) throws DeviceException {
		try {
			conApi.getFileAttributes(dirName);
		} catch (DeviceFileNotFoundException e) {
			conApi.mkDir(dirName);
		}
	}

	private boolean isUnknownCopySrcException(DeviceException e) {
		return "Error.DeviceFileXfer.Copy.BadPath".equals(e.getMessage());
	}

	/**
	 * Overwrites the destination if it exists.
	 * 
	 * srcPath may be prefixed with /.. to navigate up the file tree from
	 * /documents
	 * 
	 * @return false if the file pointed to by srcPath doesn't exist
	 * 
	 */
	private boolean copyFileLocallyOnDevice(final String srcPath,
			final String destPath, boolean withProgress)
			throws DeviceException, InterruptedException {
		pxStatus = new PXStatus();
		if (withProgress) {
			anonSharedStr = "SRCPATH_EXISTS";
			executeNavNetMethodWithProgress(new Runnable() {
				public void run() {
					try {
						conApi.copyFile(srcPath, destPath, pxStatus);
						pxStatus.setOpInProgress(false); // workaround for buggy
						// status update
					} catch (GenericCommException e) {
						// when the source file doesn't exist
						pxStatus.setOpInProgress(false);
						anonSharedStr = "SRCPATH_DOESNT_EXIST";
					} catch (DeviceException e) {
					}
				}
			}, 200);
			return anonSharedStr.equals("SRCPATH_EXISTS");
		} else {
			try {
				conApi.copyFile(srcPath, destPath, pxStatus);
				return true;
			} catch (GenericCommException e) {
				return false;
			} catch (DeviceException e) {
				if (isUnknownCopySrcException(e))
					return false;
				else
					throw e;
			}
		}
	}

	/**
	 * Does nothing if the file doesn't exist
	 * 
	 * path may be prefixed with /.. to navigate up the file tree from
	 * /documents
	 * 
	 * @throws DeviceException
	 */
	private void deleteFileOnDevice(String path) throws DeviceException {
		try {
			conApi.delete(path);
		} catch (DeviceFileNotFoundException e) {
		}
	}

	private void sendFileToDevice(final String src, final String dest)
			throws DeviceException, InterruptedException {
		pxStatus = new PXStatus();
		executeNavNetMethodWithProgress(new Runnable() {
			public void run() {
				try {
					conApi.sendFile(src, dest, pxStatus);
					 // workaround for buggy status update
					pxStatus.setOpInProgress(false);
				} catch (DeviceException e) {
				}
			}
		}, 200);

	}

	/**
	 * @param src
	 *            can't contain ../, use copyFileLocallyOnDevice() with a
	 *            destination in a document folder
	 * @throws InterruptedException
	 */
	private void getFileFromDevice(final String src, final String dest)
			throws DeviceException, InterruptedException {
		pxStatus = new PXStatus();
		executeNavNetMethodWithProgress(new Runnable() {
			public void run() {
				try {
					conApi.getFile(src, dest, pxStatus);
					 // workaround for buggy status update
					pxStatus.setOpInProgress(false);
				} catch (DeviceException e) {
					e.printStackTrace();
				}
			}
		}, 200);
	}

	/**
	 * Based on ../ in OS upgrade file manifest and wrong clean of this
	 * temporary OS file. The file copied must bear a valid OS manifest.
	 * 
	 * @throws InterruptedException
	 */
	private void copyFileToDeviceThroughOSUpgrade(final String osFilePath)
			throws InterruptedException {
		MainFrame.initStepProgress();
		pxStatus = new PXStatus();
		(new Thread() {
			public void run() {
				pxConnectApp.installOS(osFilePath, pxStatus);
			}
		}).start();
		waitWithProgress(3000 / 200, 200);
		pxStatus.cancel(); // NavNet times out with a looong timeout
		MainFrame.setStepProgressCompleted();
	}

	/**
	 * OS 1.1 required.
	 * 
	 * Copies a special locales/copysamples, which copies boot.tns to
	 * strings.res
	 */
	private void copyCopier() throws InterruptedException, DeviceException,
			IOException {
		// TODO update for CAS
		final File copierFile = TempFileManager
				.loadResourceToTempDir(resPackage + "copier.tno");
		MainFrame.log(" - Transfering new copier");
		copyFileToDeviceThroughOSUpgrade(copierFile.getPath());
		// TODO slow after the last USB command, why...?
		MainFrame.log(" - Checking copier transfer"); 
		copyFileLocallyOnDevice("/../phoenix/syst/locales/copysamples",
				deviceInstallDir + "/copier.tns", true);
		File readCopier = TempFileManager.createTempFile("read-copier.tns");
		readCopier.delete();
		getFileFromDevice(deviceInstallDir + "/copier.tns", readCopier
				.getPath());
		MainFrame.log(" - Deleting read copy");
		deleteFileOnDevice(deviceInstallDir + "/copier.tns");
		if (!Utils.fileContentsEquals(copierFile, readCopier))
			throw new NdlessException(
					"Could not write correctly the copier to the device");

	}

	/**
	 * Initialization is required for copysamples to execute
	 */
	private void waitForDeviceInitialization() throws DeviceException,
			InterruptedException {
		boolean firstMenu = true;
		boolean initialized = false;
		while (!initialized) {
			MainFrame.setProgressBarVisibility(false);
			ContinueDialog
					.ask(firstMenu ? "Now confirm the setup dialogs on the device to\n"
							+ "bring up the Home Menu, and select 'My Documents'\n"
							+ "by pressing '6'."
							: "It seems I wasn't clear enough.\n"
									+ "Could you please confirm the setup dialogs on the device\n"
									+ "to bring up the Home Menu, and then select 'My Documents'\n"
									+ "by pressing '6'?");
			MainFrame.setProgressBarVisibility(true);
			if (firstMenu)
				MainFrame.log("Waiting for the device setup...");
			firstMenu = false;
			for (int i = 0; i < 10; i++) {
				Thread.sleep(2000);
				if (copyFileLocallyOnDevice(
						"/../phoenix/syst/settings/initialized",
						deviceInstallDir + "/initialized.tns", false)) {
					initialized = true;
					break;
				}
			}
		}
		deleteFileOnDevice(deviceInstallDir + "/initialized.tns");
		MainFrame.log("Device set up.");
	}

	/**
	 * TODO sometimes fail
	 * 
	 * OS 1.1 required. Based on a heap overflow of OS manifest reading during
	 * OS download.
	 */
	private void rebootDevice(boolean waitForStartup)
			throws InterruptedException, IOException, DeviceException {
		// TODO update for CAS
		final File corruptedOSFile = TempFileManager
				.createTempFile("reboot.tno");
		if (!corruptedOSFile.exists()) {
			MainFrame.log("Preparing resources to reboot...");
			FileOutputStream fos = new FileOutputStream(corruptedOSFile);
			byte[] dummy = new byte[500000];
			fos.write(dummy);
			fos.close();
		}
		MainFrame.log("Rebooting the device...");
		conApi.enumDirectory("/");
		pxStatus = new PXStatus();
		(new Thread() {
			public void run() {
				pxConnectApp.installOS(corruptedOSFile.getPath(), pxStatus);
			}
		}).start();
		long startOfOInst = System.currentTimeMillis();
		// wait for NavNet hanging, but not too long
		while (pxStatus.progressPercentage() < 50
				&& System.currentTimeMillis() < startOfOInst + 5000)
			Thread.sleep(200);
		pxStatus.cancel();
		
		// Dummy USB activity to make it reboot
		try {
			// this hangs, but with undetermined progress bar, so OK for the user
			pxConnectApp.enumerateConnectedDevices();
		} catch (Exception e) {
		}
//		try {
//			File dummyFile = TempFileManager.createTempFile("dummy.tns");
//			FileOutputStream fos = new FileOutputStream(dummyFile);
//			byte[] dummy = new byte[10000];
//			fos.write(dummy);
//			fos.close();
//			// TODO abort if too long (i.e. has rebooted)
//			sendFileToDevice(dummyFile.getPath(), deviceInstallDir
//					+ "/dummy.tns");
//		} catch (DeviceException e) {
//		}
		
		waitForTheDeviceToReboot(waitForStartup);
	}

	private void waitForTheDeviceToReboot(boolean waitForStartup)
			throws InterruptedException {
		waitForDeviceToShutdown();
		if (waitForStartup)
			waitForDeviceToStartup();
	}

	/**
	 * Replaces NavNet's method which is not implemented
	 */
	private boolean isDeviceStillResponsive() {
		DeviceInfo devInfo = new DeviceInfo();
		pxStatus = new PXStatus();
		try {
			pxConnectApp.getDeviceInfo(devInfo, pxStatus);
		} catch (Exception e) {
			return false;
		}
		return devInfo.getOsVersion() != null;
	}

	private void waitForDeviceToShutdown() throws InterruptedException {
		MainFrame.log("Waiting for the device to shut down...");
		boolean deviceHasShutdown = false;
		for (int waitForShutdownCount = 3; waitForShutdownCount > 0; waitForShutdownCount--) {
			if (!isDeviceStillResponsive()) {
				deviceHasShutdown = true;
				break;
			}
			Thread.sleep(3000);
		}
		if (!deviceHasShutdown)
			throw new NdlessException("The device did not shut down");
		MainFrame.log("Connection lost, the device has probably shut down.");
	}

	private void waitForDeviceToStartup() throws InterruptedException {
		MainFrame.log("Waiting for the device to start up...");
		boolean deviceHasStartedup = false;
		for (int waitForShutdownCount = 18; waitForShutdownCount > 0; waitForShutdownCount--) {
			PXDevice[] devices = pxConnectApp.enumerateConnectedDevices();
			if (devices.length != 0) {
				pxConnectApp.connect(devices[0]);
				deviceHasStartedup = true;
				break;
			}
			Thread.sleep(2000);
		}
		if (!deviceHasStartedup)
			throw new NdlessException("Could not connect back to the device");
		MainFrame.log("Device reconnected.");
	}

	@Override
	public void abort() {
		if (pxStatus != null && pxStatus.inProgress())
			pxStatus.cancel();
		final Object sync = new Object();
		(new Thread() {
			public void run() {
				shutdownConApi();
				synchronized (sync) {
					sync.notify();
				}
			}
		}).start();
		synchronized (sync) {
			// the shutdown may sometimes hang
			try {
				sync.wait(3000);
			} catch (InterruptedException e) {
			}
		}
	}
}
