package org.docopt;

import java.util.List;

class Argument extends LeafPattern {

	public Argument(final String name, final Object value) {
		super(name, value);
	}

	public Argument(final String name) {
		super(name);
	}

	@Override
	protected SingleMatchResult singleMatch(final List<LeafPattern> left) {
		// >>> for n, pattern in enumerate(left)
		for (int n = 0; n < left.size(); n++) {
			final LeafPattern pattern = left.get(n);

			if (pattern.getClass() == Argument.class) {
				return new SingleMatchResult(n, new Argument(getName(),
						pattern.getValue()));
			}
		}

		return new SingleMatchResult(null, null);
	}
}