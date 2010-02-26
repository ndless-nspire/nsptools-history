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
	 * @param copyToTempDir
	 *            the file will be copied to the temporary directory with the
	 *            same name
	 * @param replacedSequence
	 *            should be of the same length as searchedSequence
	 * @param patchAlignment
	 *            1: no alignment. N: search for N-bytes aligned sequences
	 */
	private void makePatchedFile(File origFile, boolean copyToTempDir,
			byte[] searchedSequence, byte[] patchSequence, int patchAlignment)
			throws IOException {
		if (!origFile.exists())
			throw new NdlessException(origFile.getName() + "' not found");
		File patchedFile = copyToTempDir ? Utils.copyToTempDir(origFile) : origFile;
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
					ra
							.seek(ra.getFilePointer() - chunkRead.length
									+ patchAlignment);
				}
			}
			if (!patched)
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
		makePatchedFile(new File(clInstallDir, "lib" + File.separator
				+ "navnet" + File.separator + "navnet.dll"), true, new byte[] {
				'\\', '.', '.', 0, '/', '.', '.', 0 }, new byte[] { '/', '/',
				'/', 0, '/', '/', '/', 0 }, 2);
	}

	private File getNavnetDir() {
		return new File(clInstallDir, "lib" + File.separator + "navnet");
	}

	/**
	 * Forces the use of the property java.library.path for native calls to
	 * navnet.dll, to redirect them to our copy.
	 * 
	 * @throws IOException
	 */
	private void makePatchedNavnetJar() throws IOException {
		final File navnetFile = Utils.copyToTempDir(new File(getNavnetDir(),
				"navnet.jar"));
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
		// the conditions based on the 'file' protocol will fail, and the
		// java.library.path-based DLL loading will be used.
		makePatchedFile(extractedFile, false, new byte[] { 4, 'f', 'i', 'l',
				'e' }, new byte[] { 4, ' ', ' ', ' ', ' ' }, 1);
		Utils.addFilesToExistingZip(navnetFile,
				new Utils.FileToZip[] { new Utils.FileToZip(extractedFile,
						jarEntry.getName()) });
		extractedFile.delete();
		// Must be next to navnet.jar to work
		final File origConnectorsDir = new File(getNavnetDir(), "connectors");
		if (!origConnectorsDir.exists())
			throw new NdlessException(
					"Folder 'connectors' of TI-Nspire Computer Link Software not found");
		Utils.copyToTempDir(origConnectorsDir);
	}

	private void loadNavnetLibs() throws IOException {
		// our patch
		ClassPathHacker.addFile(TempFileManager.createTempFile("navnet.jar"));
		// the standard CL jars
		for (File clJarFile : DirScanner.scan(new File(clInstallDir, "lib"),
				"*.jar", "-navnet.jar"))
			ClassPathHacker.addFile(clJarFile);
		// make our copy of navnet.dll load
		LibPathHacker.addDir(TempFileManager.getTempDir().getAbsolutePath());
	}
}
