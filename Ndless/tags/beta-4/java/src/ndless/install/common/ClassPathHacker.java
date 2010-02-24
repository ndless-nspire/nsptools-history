package ndless.install.common;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;

/**
 * Derived from http://forums.sun.com/thread.jspa?threadID=300557
 */
public class ClassPathHacker {

	@SuppressWarnings("unchecked")
	private static final Class[] parameters = new Class[] { URL.class };

	public static void addFile(String s) throws IOException {
		File f = new File(s);
		addFile(f);
	}// end method

	public static void addFile(File f) throws IOException {
		addURL(f.toURI().toURL());
	}// end method

	public static void addURL(URL u) throws IOException {

		URLClassLoader sysloader = (URLClassLoader) ClassLoader
				.getSystemClassLoader();
		Class<URLClassLoader> sysclass = URLClassLoader.class;

		try {
			Method method = sysclass.getDeclaredMethod("addURL", parameters);
			method.setAccessible(true);
			method.invoke(sysloader, new Object[] { u });
		} catch (Throwable t) {
			t.printStackTrace();
			throw new IOException(
					"Error, could not add URL to system classloader");
		}// end try catch

	}// end method

}// end class
