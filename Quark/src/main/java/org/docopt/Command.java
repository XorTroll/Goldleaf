package org.docopt;

import java.util.List;

final class Command extends Argument {

	public Command(final String name, final Object value) {
		super(name, value);
	}

	public Command(final String name) {
		this(name, false);
	}

	@Override
	protected SingleMatchResult singleMatch(final List<LeafPattern> left) {
		for (int n = 0; n < left.size(); n++) {
			final LeafPattern pattern = left.get(n);

			if (pattern.getClass() == Argument.class) {
				if (getName().equals(pattern.getValue())) {
					return new SingleMatchResult(n,
							new Command(getName(), true));
				}
				break;
			}
		}

		return new SingleMatchResult(null, null);
	}
}