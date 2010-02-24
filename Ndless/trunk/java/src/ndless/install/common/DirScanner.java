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

package ndless.install.common;

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * Derived from http://www.cs.technion.ac.il/~imaman/programs/dirscanner.html
 * 	-> modified to return absolute files
 * 
 * The DirScanner class provides the functionality of recursive scanning thru directories,
 * using regular expressions (wild-cards) patterns. Here are some examples:
 * 
 * DirScanner.scan("*.java") --- (recursively) Find all files that have a ".java" suffix.
 * DirScanner.scan("* /swing/*.java") --- (recursively) Find all files that have a ".java"
 * 	suffix and one of their parent directories is named "swing".
 * DirScanner.scan("*.java", "-* /swing/*") --- (recursively) Find all files that have a
 * 	".java" suffix excluding files whose one of their parent directories is named "swing".
 */
public class DirScanner {
	private static final String SEP = System.getProperty("file.separator");
	private final List<File> files = new ArrayList<File>();
	private final List<String> patts = new ArrayList<String>();

	public static Iterable<File> scan(String... patterns) {
		return scan(new File(System.getProperty("user.dir")), patterns);
	}

	public static Iterable<File> scan(Collection<String> patterns) {
		return scan(patterns.toArray(new String[0]));
	}

	private static boolean isSubtract(String patt) {
		return patt.startsWith("-");
	}

	private static String rawPatt(String patt) {
		if (!isSubtract(patt))
			return patt;
		return patt.substring(1);
	}

	public static Iterable<File> scan(File dir, String... patterns) {
		DirScanner s = new DirScanner();
		for (String p : patterns) {
			p = p.replace(SEP, "/");
			p = p.replace(".", "\\.");
			p = p.replace("*", ".*");
			p = p.replace("?", ".?");
			s.patts.add(p);
		}

		s.scan(dir, dir, new File("/"));
		return s.files;
	}

	private void scan(File baseDir, File dir, File path) {
		File[] fs = dir.listFiles();
		for (File f : fs) {
			File rel = new File(path, f.getName());
			if (f.isDirectory()) {
				scan(baseDir, f, rel);
				continue;
			}

			if (match(patts, rel))
				files.add(new File(baseDir, rel.getPath()));
		}
	}

	private static boolean match(Iterable<String> patts, File rel) {

		boolean ok = false;
		for (String p : patts) {
			boolean subtract = isSubtract(p);
			p = rawPatt(p);

			boolean b = match(p, rel);
			if (b && subtract)
				return false;

			if (b)
				ok = true;
		}

		return ok;
	}

	private static boolean match(String p, File rel) {
		String s = rel.getName();
		if (p.indexOf('/') >= 0)
			s = rel.toString();

		s = s.replace(SEP, "/");
		return s.matches(p);
	}
}