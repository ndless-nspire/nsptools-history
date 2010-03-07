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

package ndless.install.injection;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import ndless.install.common.NdlessException;
import ndless.install.common.TempFileManager;
import ndless.install.common.Utils;
import ndless.install.common.Utils.Status;
import ndless.install.gui.ContinueDialog;
import ndless.install.gui.CurrentStep;
import ndless.install.gui.MainFrame;

import org.xml.sax.SAXException;

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
	private static final String deviceInstallerDir = "ndless";
	// created once Ndless has been run on the calc
	private static final String deviceInstalledDir = "/../phoenix/ndls";
	private static final String resDirPrefix = "res/";
	private File resDir;
	private static final String localRequiredOSFilename = "tinspire_1.1.tno";
	private static final File userFolder = new File("userfiles");
	private static final IPXConnectApp pxConnectApp = PXConnectApp
			.getInstance();
	private IConnectivityAPI conApi;
	private PXStatus pxStatus;
	private File requiredOSFile;
	// When anonymous classes need to exchange data
	private String anonSharedStr;
	private boolean hadNoOS;
	private boolean isNonCas;

	public void inject() throws Exception {
		try {
			MainFrame.setCurrentStep(this);
			MainFrame.log("Connecting to the device...");
			setupNavnet();

			DeviceInfo deviceInfo = pxConnectApp.connectedDevice()
					.getDeviceInfo();
			if (deviceInfo == null)
				throw new NdlessException("Cannot get device information");
			this.isNonCas = "NonCas".equals(deviceInfo.getDeviceType());
			this.resDir = new File(resDirPrefix
					+ (isNonCas ? "NON_CAS/" : "CAS/"));
			Version currentOsVersion = deviceInfo.getOsVersion();
			this.hadNoOS = hasNoOS(currentOsVersion);
			downgradeOSIfNeeded(currentOsVersion);

			File[] resFiles = getResFiles(new String[] { "loader.tns",
					"hook.tns", "demo.tns" });

			MainFrame.log("Creating Ndless installation directory...");
			MainFrame.log("Checking if Ndless is already installed... ", false);
			if (isNdlessUpgrade()) {
				MainFrame.log("Yes, upgrading.");
				MainFrame.log("Preparing required files on the device:");
				transferBinaries(resFiles);

			} else {
				MainFrame.log("No.");
				MainFrame.log("Preparing required files on the device:");
				mkDeviceDocDir(deviceInstallerDir);
				MainFrame.log(" - Backuping overwritten files");
				copyFileLocallyOnDevice(
						"/../phoenix/syst/locales/en/strings.res",
						deviceInstallerDir + "/strbackup.tns", true);
				transferBinaries(resFiles);
				MainFrame.log(" - Deleting copier");
				deleteFileOnDevice("/../phoenix/syst/locales/copysamples");
				copyCopier();

				MainFrame.log(" - Deleting configuration");
				deleteFileOnDevice("/../phoenix/syst/settings/initialized");

				rebootDevice(true);
				waitForDeviceInitialization();
			} // new installation

			rebootDevice(true);

			MainFrame.log("The device is ready!");
			MainFrame.log("You can now close the installer.");
			MainFrame.canExit();
		} catch (DeviceException de) {
			throw new NdlessException(deviceExceptionToErrorMsg(de));
		}
	}

	private String deviceExceptionToErrorMsg(DeviceException de) {
		return ResourceManagement.getRes().getString(de.getMessage());
	}

	/**
	 * Workaround to buggy status update sometimes
	 * 
	 * @return
	 */
	private void updatePxStatusWithException(DeviceException de) {
		pxStatus.setError(deviceExceptionToErrorMsg(de));
	}

	private File[] getResFiles(String[] filenames) {
		ArrayList<File> resFiles = new ArrayList<File>();
		for (String filename : filenames) {
			File resFile = new File(resDir, filename);
			if (!resFile.exists())
				throw new NdlessException(
						"Corrupted Ndless directory: missing file '"
								+ resFile.getPath() + "'");
			resFiles.add(resFile);
		}
		return (File[]) resFiles.toArray(new File[] {});
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
					"Another instance of Ndless or TI-Nspire Computer Link is"
							+ " already running");
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
			ContinueDialog
					.ask("Ndless needs to " + action + " 1.1.\nContinue?");
			requiredOSFile = new File(userFolder, localRequiredOSFilename);
			if (requiredOSFile.exists()) {
				MainFrame.log("Using the local OS copy '"
						+ requiredOSFile.getPath() + "'");
			} else {
				MainFrame.log("Downloading OS version 1.1...");
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
			final int statusUpdatePeriodInMs, boolean disableProgress)
			throws InterruptedException {
		if (disableProgress) {
			runnable.run();
			if (pxStatus.errorOccured())
				throw new NdlessException(pxStatus.getError());
			return;
		}
		MainFrame.initStepProgress();
		final Thread thread = new Thread(runnable);
		thread.start();
		while (pxStatus.inProgress() && thread.isAlive()) {
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
					Utils.downloadFile(new URL(
							"http://ti.bank.free.fr/modules/archives/"
									+ "download.php?id="
									+ (isNonCas ? 1394 : 1395)),
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
					.log("Ndless can't download the file. You may try to copy it"
							+ "manually to Ndless directory as '"
							+ new File(userFolder, localRequiredOSFilename)
									.getPath()
							+ "' and retry the installation.");
			throw dlStatus.getException();
		}
		try {
			setupUserFolder();
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
		}, 300, false);
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

	private boolean isNdlessUpgrade() throws DeviceException,
			InterruptedException {
		try {
			conApi.getFileAttributes(deviceInstalledDir);
		} catch (DeviceFileNotFoundException e) {
			return false;
		}
		return true;
	}

	private void transferBinaries(File[] resFiles) throws DeviceException,
			InterruptedException {
		for (File resFile : resFiles) {
			String filename = resFile.getName();
			MainFrame.log(" - Transferring "
					+ filename.substring(0, filename.indexOf(".tns")));
			sendFileToDevice(resFile.getPath(), deviceInstallerDir + "/"
					+ filename);
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
						// workaround for buggy status update
						pxStatus.setOpInProgress(false);
					} catch (GenericCommException e) {
						// when the source file doesn't exist
						pxStatus.setOpInProgress(false);
						anonSharedStr = "SRCPATH_DOESNT_EXIST";
					} catch (DeviceException e) {
						updatePxStatusWithException(e);
					}
				}
			}, 200, false);
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
					updatePxStatusWithException(e);
				}
			}
		}, 200, false);

	}

	/**
	 * @param src
	 *            can't contain ../, use getSystemFileFromDevice() instead
	 * @throws InterruptedException
	 */
	private void getFileFromDevice(final String src, final String dest,
			boolean withProgress) throws DeviceException, InterruptedException {
		pxStatus = new PXStatus();
		executeNavNetMethodWithProgress(new Runnable() {
			public void run() {
				try {
					conApi.getFile(src, dest, pxStatus);
					// workaround for buggy status update
					pxStatus.setOpInProgress(false);
				} catch (DeviceException e) {
					updatePxStatusWithException(e);
				}
			}
		}, 200, !withProgress);
	}

	/**
	 * deviceInstallerDir is used as a temporary folder for the transfer.
	 * 
	 * @param src
	 *            device path, starts with /../
	 * @param dest
	 *            computer path
	 * @throws InterruptedException
	 * @throws DeviceException
	 */
	private void getSystemFileFromDevice(String src, String dest,
			boolean withProgress) throws DeviceException, InterruptedException {
		String srcFileName = src.substring(src.lastIndexOf('/') + 1) + ".tns";
		String tempFileOnDevice = deviceInstallerDir + "/" + srcFileName;
		copyFileLocallyOnDevice(src, tempFileOnDevice, withProgress);
		getFileFromDevice(tempFileOnDevice, dest, withProgress);
		deleteFileOnDevice(tempFileOnDevice);
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
		final File copierFile = new File(resDir, "../copier.tno");
		MainFrame.log(" - Transferring new copier");
		if (!copierFile.exists())
			throw new NdlessException(
					"Corrupted Ndless directory: missing file '"
							+ copierFile.getPath() + "'");
		copyFileToDeviceThroughOSUpgrade(copierFile.getPath());
		// TODO slow after the last USB command, why...?
		MainFrame.log(" - Checking copier transfer");
		copyFileLocallyOnDevice("/../phoenix/syst/locales/copysamples",
				deviceInstallerDir + "/copier.tns", true);
		File readCopier = TempFileManager.createTempFile("read-copier.tns");
		readCopier.delete();
		getFileFromDevice(deviceInstallerDir + "/copier.tns", readCopier
				.getPath(), true);
		MainFrame.log(" - Deleting read copy");
		deleteFileOnDevice(deviceInstallerDir + "/copier.tns");
		if (!Utils.fileContentsEquals(copierFile, readCopier))
			throw new NdlessException(
					"Could not write correctly the copier to the device");

	}

	/**
	 * Initialization is required for copysamples to execute
	 */
	private void waitForDeviceInitialization() throws DeviceException,
			InterruptedException, XPathExpressionException, IOException,
			SAXException, ParserConfigurationException {
		boolean firstMenu = true;
		boolean initialized = false;
		while (!initialized) {
			MainFrame.setProgressBarVisibility(false);
			ContinueDialog
					.askFormatted(firstMenu ? "<html>Now confirm the setup dialogs on the device to<br/>"
							+ "bring up the Home Menu, and select 'My Documents'<br/>"
							+ "by pressing '6'.\n"
							+ "You <i>must</i> set 'English' as the system<br/>"
							+ "language.</html>"
							: "<html>It seems I wasn't clear enough.<br/>"
									+ "Could you please confirm the setup dialogs on the device<br/>"
									+ "to bring up the Home Menu, and then select 'My Documents'<br/>"
									+ "by pressing '6'?</html>");
			MainFrame.setProgressBarVisibility(true);
			if (firstMenu)
				MainFrame.log("Waiting for the device setup...");
			firstMenu = false;
			for (int i = 0; i < 10; i++) {
				Thread.sleep(2000);
				if (copyFileLocallyOnDevice(
						"/../phoenix/syst/settings/initialized",
						deviceInstallerDir + "/initialized.tns", false)) {
					initialized = true;
					break;
				}
			}
		}
		deleteFileOnDevice(deviceInstallerDir + "/initialized.tns");

		while (!isCurrentLanguageAsExpected()) {
			MainFrame.setProgressBarVisibility(false);
			// We cannot snoop the language change because the check discards
			// any menu
			ContinueDialog
					.ask("The system language must be kept to English during the\n"
							+ "installation, you'll be able to change it afterwards.\n"
							+ "Please set it back in 'System Info' -> System settings'\n"
							+ "then select 'Yes'.");
			MainFrame.setProgressBarVisibility(true);
		}
		MainFrame.log("Device set up.");
	}

	/**
	 * Caution, discards any menu currently displayed on the calculator.
	 * 
	 * @return true if the current language is English as expected
	 */
	private boolean isCurrentLanguageAsExpected() throws IOException,
			DeviceException, InterruptedException, XPathExpressionException,
			SAXException, ParserConfigurationException {
		String currentSettingsZipFile = TempFileManager.createTempFile(
				"current.zip").getPath();
		getSystemFileFromDevice("/../phoenix/syst/settings/current.zip",
				currentSettingsZipFile, false);
		File currentSettingsFile = Utils.extractFileFromZip(
				currentSettingsZipFile, "settings.xml");

		DocumentBuilderFactory domFactory = DocumentBuilderFactory
				.newInstance();
		domFactory.setNamespaceAware(true);
		XPath xpath = XPathFactory.newInstance().newXPath();
		double lang = (Double) xpath.evaluate("/settings/lang/text()",
				domFactory.newDocumentBuilder().parse(currentSettingsFile),
				XPathConstants.NUMBER);
		return lang == 1;
	}

	/**
	 * TODO sometimes fail
	 * 
	 * OS 1.1 required. Based on a heap overflow of OS manifest reading during
	 * OS download.
	 */
	private void rebootDevice(boolean waitForStartup)
			throws InterruptedException, IOException, DeviceException {
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
			// this hangs, but with undetermined progress bar, so OK for the
			// user
			pxConnectApp.enumerateConnectedDevices();
		} catch (Exception e) {
		}

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
		boolean deviceNotResponsive = false, deviceHasShutdown = false;
		for (int waitForNonResponsiveCount = 3; waitForNonResponsiveCount > 0; waitForNonResponsiveCount--) {
			if (!isDeviceStillResponsive()) {
				deviceNotResponsive = true;
				break;
			}
			Thread.sleep(3000);
		}
		if (deviceNotResponsive) {
			for (int waitForShutdownCount = 20; waitForShutdownCount > 0; waitForShutdownCount--) {
				PXDevice[] devices = pxConnectApp.enumerateConnectedDevices();
				if (devices.length == 0) {
					deviceHasShutdown = true;
					break;
				}
				Thread.sleep(3000);
			}
		}
		if (!deviceHasShutdown)
			throw new NdlessException("The device did not shut down");
		MainFrame.log("Connection lost, the device has probably shut down.");
	}

	private void waitForDeviceToStartup() throws InterruptedException {
		MainFrame.log("Waiting for the device to start up...");
		boolean deviceHasStartedup = false;
		for (int waitForStartupCount = 25; waitForStartupCount > 0; waitForStartupCount--) {
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
