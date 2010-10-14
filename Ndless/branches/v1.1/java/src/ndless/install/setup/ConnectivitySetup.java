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

package ndless.install.setup;

import java.io.EOFException;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.Arrays;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

import ndless.install.common.ClassPathHacker;
import ndless.install.common.DirScanner;
import ndless.install.common.LibPathHacker;
import ndless.install.common.NdlessException;
import ndless.install.common.TempFileManager;
import ndless.install.common.Utils;
import ndless.install.gui.MainFrame;

public class ConnectivitySetup {
	private static final String NAVNET_TEMP_SUBDIR = "navnet";
	private File clInstallDir;

	public void setup() throws IOException {
		String clInstallDirPath = Utils.getComputerLinkInstallDir();
		if (clInstallDirPath == null)
			throw new NdlessException(
					"TI-Nspire Computer Link Software not found.\nGet it at http://education.ti.com/educationportal/sites/US/productDetail/us_nspire_computerlink.html");
		clInstallDir = new File(clInstallDirPath);
		MainFrame.log("Setting up USB connectivity...");
		makePatchedNavnetLib();
		makePatchedNavnetJar();
		loadNavnetLibs();
	}

	/**
	 * @param copyParentTempDir
	 *            relative path to the parent directory to which the file is
	 *            copied before being patched. <i>null</i> to patch the original
	 *            file without making a copy.
	 * @param replacedSequence
	 *            should be of the same length as searchedSequence
	 * @param patchAlignment
	 *            1: no alignment. N: search for N-bytes aligned sequences
	 * @param optionalPatch
	 *            if false, fails if the file cannot be pached because of a
	 *            version issue
	 */
	private void makePatchedFile(File origFile, String copyParentTempDir,
			byte[] searchedSequence, byte[] patchSequence, int patchAlignment,
			boolean optionalPatch) throws IOException {
		if (!origFile.exists())
			throw new NdlessException(origFile.getName() + "' not found");
		File patchedFile = copyParentTempDir != null ? Utils.copyToSubTempDir(
				origFile, copyParentTempDir) : origFile;
		RandomAccessFile ra = new RandomAccessFile(patchedFile, "rw");
		byte[] chunkRead = new byte[searchedSequence.length];
		boolean patched = false;
		try {
			while (true) {
				try {
					ra.readFully(chunkRead);
				} catch (EOFException e) {
					break;
				}
				if (Arrays.equals(chunkRead, searchedSequence)) {
					// point back to the chunk
					ra.seek(ra.getFilePointer() - chunkRead.length);
					ra.write(patchSequence);
					patched = true;
				} else {
					// point back after the chunk read
					ra.seek(ra.getFilePointer() - chunkRead.length
							+ patchAlignment);
				}
			}
			if (!patched && !optionalPatch)
				throw new NdlessException("Unknown version of '"
						+ origFile.getName() + "'");
		} finally {
			ra.close();
		}
	}

	/**
	 * Removes the filter which forbids '..' in file path parameters
	 */
	private void makePatchedNavnetLib() throws IOException {
		// Windows specific
		makePatchedFile(
				new File(getNavnetDir(), File.separator + "navnet.dll"),
				NAVNET_TEMP_SUBDIR, new byte[] { '\\', '.', '.', 0, '/', '.',
						'.', 0 }, new byte[] { '/', '/', '/', 0, '/', '/', '/',
						0 }, 2, false);
	}

	private File getNavnetDir() {
		// Windows specific
		File navNetDir;
		// For Computer Link < 1.4
		navNetDir = new File(clInstallDir, "lib" + File.separator + "navnet");
		if (navNetDir.exists())
			return navNetDir;
		String sharedSuffix = "/TI Shared/CommLib/1/NavNet";
		navNetDir = new File(System.getenv().get("CommonProgramFiles")
				+ sharedSuffix);
		if (navNetDir.exists())
			return navNetDir;
		navNetDir = new File(System.getenv().get("COMMONPROGRAMFILES")
				+ sharedSuffix);
		if (navNetDir.exists())
			return navNetDir;
		else
			throw new NdlessException("NavNet directory not found");
	}

	/**
	 * Forces the use of the property java.library.path for native calls to
	 * navnet.dll, to redirect them to our copy.
	 * 
	 * @throws IOException
	 */
	private void makePatchedNavnetJar() throws IOException {
		final File navnetFile = Utils.copyToSubTempDir(new File(getNavnetDir(),
				"navnet.jar"), NAVNET_TEMP_SUBDIR);
		JarFile jar = new JarFile(navnetFile);
		JarEntry jarEntry = jar.getJarEntry("com/ti/eps/navnet/NavNet.class");
		if (jarEntry == null)
			throw new NdlessException("NavNet.class not found in navnet.jar");
		File extractedFile = TempFileManager.createTempFile("NavNet.class");
		InputStream is = jar.getInputStream(jarEntry);
		FileOutputStream fos = new java.io.FileOutputStream(extractedFile);
		while (is.available() > 0)
			fos.write(is.read());
		fos.close();
		is.close();
		jar.close();
		// On Computer Link Software < 1.4, NavNet loads itself the DLL.
		// With this patch the conditions based on the 'file' protocol will fail,
		// and the java.library.path-based DLL loading will be used.
		makePatchedFile(extractedFile, null,
				new byte[] { 4, 'f', 'i', 'l', 'e' }, new byte[] { 4, ' ', ' ',
						' ', ' ' }, 1, true);
		Utils.addFilesToExistingZip(navnetFile,
				new Utils.FileToZip[] { new Utils.FileToZip(extractedFile,
						jarEntry.getName()) });
		extractedFile.delete();
		// Must be next to navnet.jar to work
		final File origConnectorsDir = new File(getNavnetDir(), "connectors");
		if (!origConnectorsDir.exists())
			throw new NdlessException(
					"Folder 'connectors' of TI-Nspire Computer Link Software not found");
		Utils.copyToSubTempDir(origConnectorsDir, NAVNET_TEMP_SUBDIR);
		// a JRE is run as an RMI server in Computer Link Software >= 1.4
		// For these versions, just copy the bin/java.exe wrapper that will
		// redirect the execution to NavNet's shared JRE
		if (new File(getNavnetDir(), "../jre").exists()) {
			Utils.copyToSubTempDir(new File("bin/java.exe"), "jre/bin");
		}
	}

	private void loadNavnetLibs() throws IOException {
		// our patch
		ClassPathHacker.addFile(TempFileManager
				.createTempFile(NAVNET_TEMP_SUBDIR + "/navnet.jar"));
		// the standard CL jars
		for (File clJarFile : DirScanner.scan(new File(clInstallDir, "lib"),
				"*.jar", "-navnet.jar"))
			ClassPathHacker.addFile(clJarFile);
		// make our copy of navnet.dll load
		LibPathHacker.addDir(TempFileManager.createTempFile(NAVNET_TEMP_SUBDIR)
				.getAbsolutePath());
	}
}
