package org.docopt;

/**
 * Error in construction of usage-message by developer.
 */
final class DocoptLanguageError extends Error {

	private static final long serialVersionUID = 1L;

	public DocoptLanguageError(final String message) {
		super(message);
	}
}