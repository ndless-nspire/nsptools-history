package ndless.install.common;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringWriter;
import java.net.URL;
import java.net.URLConnection;
import java.nio.ByteBuffer;
import java.nio.channels.Channels;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.util.Arrays;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

public class Utils {
	/**
	 * Derived from
	 * http://www.coderanch.com/t/132336/General-Computing/Read-Windows
	 * -Registry-from-java
	 * 
	 * @param key
	 * @param valueName
	 *            null for default
	 */
	public static String getWinRegistryValue(String key, String valueName) {
		try {
			Process process = Runtime.getRuntime().exec(
					"reg query \"" + key + "\" "
							+ (valueName != null ? "/v" + valueName : "/ve"));
			RegtoolStreamReader reader = new RegtoolStreamReader(process
					.getInputStream());
			reader.start();
			process.waitFor();
			reader.join();
			String result = reader.getResult();
			final String REGSTR_TOKEN = "REG_SZ";
			int p = result.indexOf(REGSTR_TOKEN);
			if (p == -1)
				return null;
			return result.substring(p + REGSTR_TOKEN.length()).trim();
		} catch (Exception e) {
			return null;
		}
	}

	private static class RegtoolStreamReader extends Thread {
		private InputStream is;
		private StringWriter sw;

		RegtoolStreamReader(InputStream is) {
			this.is = is;
			sw = new StringWriter();
		}

		@Override
		public void run() {
			try {
				int c;
				while ((c = is.read()) != -1)
					sw.write(c);
			} catch (IOException e) {
				;
			}
		}

		String getResult() {
			return sw.toString();
		}
	}

	/**
	 * @return null if not found
	 */
	public static String getComputerLinkInstallDir() {
		return Utils
				.getWinRegistryValue(
						"HKEY_LOCAL_MACHINE\\SOFTWARE\\Texas Instruments\\TI-Nspire Computer Link",
						null);
	}

	public static void copyFile(File src, File dest) throws IOException {
		copyFiles(src, dest);
	}

	/**
	 * Derived from http://www.dreamincode.net/code/snippet1443.htm
	 * 
	 * This function will copy files or directories from one location to
	 * another. note that the source and the destination must be mutually
	 * exclusive. This function can not be used to copy a directory to a sub
	 * directory of itself. The function will also have problems if the
	 * destination files already exist.
	 * 
	 * @param src
	 *            A File object that represents the source for the copy (file or
	 *            directory)
	 * @param dest
	 *            A File object that represents the destination for the copy
	 *            (file if src is a file, directory if it is a directory)
	 * @throws IOException
	 *             if unable to copy.
	 */
	public static void copyFiles(File src, File dest) throws IOException {
		// Check to ensure that the source is valid...
		if (!src.exists()) {
			throw new IOException("copyFiles: Can not find source: "
					+ src.getAbsolutePath() + ".");
		} else if (!src.canRead()) { // check to ensure we have rights to the
			// source...
			throw new IOException("copyFiles: No right to source: "
					+ src.getAbsolutePath() + ".");
		}
		// is this a directory copy?
		if (src.isDirectory()) {
			if (!dest.exists()) { // does the destination already exist?
				// if not we need to make it exist if possible (note this is
				// mkdirs not mkdir)
				if (!dest.mkdirs()) {
					throw new IOException(
							"copyFiles: Could not create direcotry: "
									+ dest.getAbsolutePath() + ".");
				}
			}
			// get a listing of files...
			String list[] = src.list();
			// copy all the files in the list.
			for (int i = 0; i < list.length; i++) {
				File dest1 = new File(dest, list[i]);
				File src1 = new File(src, list[i]);
				copyFiles(src1, dest1);
			}
		} else {
			// This was not a directory, so lets just copy the file
			FileInputStream fin = null;
			FileOutputStream fout = null;
			byte[] buffer = new byte[4096]; // Buffer 4K at a time (you can
			// change this).
			int bytesRead;
			try {
				// open the files for input and output
				fin = new FileInputStream(src);
				fout = new FileOutputStream(dest);
				// while bytesRead indicates a successful read, lets write...
				while ((bytesRead = fin.read(buffer)) >= 0) {
					fout.write(buffer, 0, bytesRead);
				}
			} catch (IOException e) { // Error copying file...
				IOException wrapper = new IOException(
						"copyFiles: Unable to copy file: "
								+ src.getAbsolutePath() + "to"
								+ dest.getAbsolutePath() + ".");
				wrapper.initCause(e);
				wrapper.setStackTrace(e.getStackTrace());
				throw wrapper;
			} finally { // Ensure that the files are closed (if they were open).
				if (fin != null) {
					fin.close();
				}
				if (fout != null) {
					fout.close();
				}
			}
		}
	}

	/**
	 * Copy a file or directory at the root of the temporary directory, with the
	 * same name
	 * 
	 * @return the new file or directory
	 */
	public static File copyToTempDir(File src) throws IOException {
		final File tempFile = TempFileManager.createTempFile(src.getName());
		copyFiles(src, tempFile);
		return tempFile;
	}

	public static class FileToZip {
		public File file;
		public String zipEntryName;

		public FileToZip(File file, String zipEntryName) {
			this.file = file;
			this.zipEntryName = zipEntryName;
		}
	}

	/**
	 * Derived from http://snippets.dzone.com/posts/show/3468 Adds files to an
	 * existing Zip file. Overwrites zip entries with the same name as one of
	 * the new files.
	 */
	public static void addFilesToExistingZip(File zipFile, FileToZip[] files)
			throws IOException {
		// get a temp file
		File tempFile = File.createTempFile(zipFile.getName(), null);
		// delete it, otherwise you cannot rename your existing zip to it.
		tempFile.delete();

		boolean renameOk = zipFile.renameTo(tempFile);
		tempFile.deleteOnExit();
		if (!renameOk) {
			throw new IOException("Could not rename the file "
					+ zipFile.getAbsolutePath() + " to "
					+ tempFile.getAbsolutePath());
		}
		byte[] buf = new byte[1024];

		ZipInputStream zin = new ZipInputStream(new FileInputStream(tempFile));
		ZipOutputStream out = new ZipOutputStream(new FileOutputStream(zipFile));

		ZipEntry entry = zin.getNextEntry();
		while (entry != null) {
			String name = entry.getName();
			boolean notInFiles = true;
			for (FileToZip f : files) {
				if (f.zipEntryName.equals(name)) {
					notInFiles = false;
					break;
				}
			}
			if (notInFiles) {
				// Add ZIP entry to output stream.
				out.putNextEntry(new ZipEntry(name));
				// Transfer bytes from the ZIP file to the output file
				int len;
				while ((len = zin.read(buf)) > 0) {
					out.write(buf, 0, len);
				}
			}
			entry = zin.getNextEntry();
		}
		// Close the streams
		zin.close();
		// Compress the files
		for (int i = 0; i < files.length; i++) {
			InputStream in = new FileInputStream(files[i].file);
			// Add ZIP entry to output stream.
			out.putNextEntry(new ZipEntry(files[i].zipEntryName));
			// Transfer bytes from the file to the ZIP file
			int len;
			while ((len = in.read(buf)) > 0) {
				out.write(buf, 0, len);
			}
			// Complete the entry
			out.closeEntry();
			in.close();
		}
		// Complete the ZIP file
		out.close();
		tempFile.delete();
	}

	public static class Status {
		private int completionPercent = 0;
		private Exception exception = null;
		private boolean done = false;

		public void setCompletionPercent(int completionPercent) {
			this.completionPercent = completionPercent;
		}

		public int getCompletionPercent() {
			return completionPercent;
		}

		public void setException(Exception exception) {
			this.exception = exception;
		}

		/**
		 * null if no exception
		 */
		public Exception getException() {
			return exception;
		}

		public void setDone(boolean done) {
			this.done = done;
		}

		public boolean isDone() {
			return done;
		}
	}

	/**
	 * Derived from
	 * http://www.java2s.com/Code/Java/Network-Protocol/SaveURLcontentstoafile
	 * .htm
	 * 
	 * @param status
	 *            optional, regularly updated (run in a separate thread)
	 */
	public static void downloadFile(URL from, File to, Status status)
			throws IOException {
		BufferedInputStream urlin = null;
		BufferedOutputStream fout = null;
		try {
			int bufSize = 8 * 1024;
			final URLConnection urlConnection = from.openConnection();
			urlin = new BufferedInputStream(urlConnection.getInputStream(),
					bufSize);
			int contentLength = urlConnection.getContentLength();
			fout = new BufferedOutputStream(new FileOutputStream(to), bufSize);
			int read = -1;
			int totalRead = 0;
			byte[] buf = new byte[bufSize];
			while ((read = urlin.read(buf, 0, bufSize)) >= 0) {
				totalRead += read;
				if (status != null) {
					status
							.setCompletionPercent((int) (((float) totalRead / (float) contentLength) * 100));
				}
				fout.write(buf, 0, read);
			}
			fout.flush();
		} catch (IOException e) {
			if (status != null) {
				status.setException(e);
			} else
				throw e;
		} finally {
			if (status != null)
				status.setDone(true);
			if (urlin != null) {
				try {
					urlin.close();
				} catch (IOException cioex) {
				}
			}
			if (fout != null) {
				try {
					fout.close();
				} catch (IOException cioex) {
				}
			}
		}
	}

	private final static int BIN_COMPARE8BUFFSIZE = 1024;

	/**
	 * Derived from
	 * http://www.velocityreviews.com/forums/t123770-re-java-code-for
	 * -determining-binary-file-equality.html
	 */
	public static boolean inputStreamEquals(InputStream is1, InputStream is2) {
		byte buff1[] = new byte[BIN_COMPARE8BUFFSIZE];
		byte buff2[] = new byte[BIN_COMPARE8BUFFSIZE];
		if (is1 == is2)
			return true;
		if (is1 == null && is2 == null)
			return true;
		if (is1 == null || is2 == null)
			return false;
		try {
			int read1 = -1;
			int read2 = -1;

			do {
				int offset1 = 0;
				while (offset1 < BIN_COMPARE8BUFFSIZE
						&& (read1 = is1.read(buff1, offset1,
								BIN_COMPARE8BUFFSIZE - offset1)) >= 0) {
					offset1 += read1;
				}

				int offset2 = 0;
				while (offset2 < BIN_COMPARE8BUFFSIZE
						&& (read2 = is2.read(buff2, offset2,
								BIN_COMPARE8BUFFSIZE - offset2)) >= 0) {
					offset2 += read2;
				}
				if (offset1 != offset2)
					return false;
				if (offset1 != BIN_COMPARE8BUFFSIZE) {
					Arrays.fill(buff1, offset1, BIN_COMPARE8BUFFSIZE, (byte) 0);
					Arrays.fill(buff2, offset2, BIN_COMPARE8BUFFSIZE, (byte) 0);
				}
				if (!Arrays.equals(buff1, buff2))
					return false;
			} while (read1 >= 0 && read2 >= 0);
			if (read1 < 0 && read2 < 0)
				return true; // both at EOF
			return false;

		} catch (Exception ei) {
			return false;
		}
	}

	/**
	 * Derived from
	 * http://www.velocityreviews.com/forums/t123770-re-java-code-for
	 * -determining-binary-file-equality.html
	 */
	public static boolean fileContentsEquals(File file1, File file2) {
		InputStream is1 = null;
		InputStream is2 = null;
		if (file1.length() != file2.length())
			return false;

		try {
			is1 = new FileInputStream(file1);
			is2 = new FileInputStream(file2);

			return inputStreamEquals(is1, is2);

		} catch (Exception ei) {
			return false;
		} finally {
			try {
				if (is1 != null)
					is1.close();
				if (is2 != null)
					is2.close();
			} catch (Exception ei2) {
			}
		}
	}

	/**
	 * Derived from
	 * http://thomaswabner.wordpress.com/2007/10/09/fast-stream-copy
	 * -using-javanio-channels/
	 */
	public static void streamCopy(InputStream src, OutputStream target)
			throws IOException {
		ReadableByteChannel srcC = Channels.newChannel(src);
		WritableByteChannel targetC = Channels.newChannel(target);
		final ByteBuffer buffer = ByteBuffer.allocateDirect(16 * 1024);
		while (srcC.read(buffer) != -1) {
			// prepare the buffer to be drained
			buffer.flip();
			// write to the channel, may block
			targetC.write(buffer);
			// If partial transfer, shift remainder down
			// If buffer is empty, same as doing clear()
			buffer.compact();
		}
		// EOF will leave buffer in fill state
		buffer.flip();
		// make sure the buffer is fully drained.
		while (buffer.hasRemaining()) {
			targetC.write(buffer);
		}
	}

	public static void copyResource(String resPath, File file)
			throws IOException {
		final FileOutputStream target = new FileOutputStream(file);
		streamCopy(Utils.class.getResourceAsStream(resPath),
				target);
		target.close();
	}

}
