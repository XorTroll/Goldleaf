package org.docopt;

/**
 * An exception thrown by {@link Docopt#parse} to indicate that the application
 * should exit. This could be normal (e.g. default {@code --help} behavior) or
 * abnormal (e.g. incorrect arguments).
 */
public final class DocoptExitException extends RuntimeException {

	private static final long serialVersionUID = 1L;

	private final int exitCode;

	private final boolean printUsage;

	DocoptExitException(final int exitCode, final String message,
			final boolean printUsage) {
		super(message);
		this.exitCode = exitCode;
		this.printUsage = printUsage;
	}

	DocoptExitException(final int exitCode) {
		this(exitCode, null, false);
	}

	/**
	 * Returns a numeric code indicating the cause of the exit. By convention, a
	 * non-zero code indicates abnormal termination.
	 *
	 * @return the exit code
	 */
	public int getExitCode() {
		return exitCode;
	}

	boolean getPrintUsage() {
		return printUsage;
	}
}