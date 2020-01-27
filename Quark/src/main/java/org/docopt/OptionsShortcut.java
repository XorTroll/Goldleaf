package org.docopt;

import java.util.Collections;

/**
 * Marker/placeholder for [options] shortcut.
 */
final class OptionsShortcut extends Optional {

	public OptionsShortcut() {
		super(Collections.<Pattern> emptyList());
	}
}