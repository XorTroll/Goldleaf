package org.docopt;

import static org.docopt.Python.list;

import java.util.List;

final class Required extends BranchPattern {

	public Required(final List<? extends Pattern> children) {
		super(children);
	}

	@Override
	protected MatchResult match(final List<LeafPattern> left,
			List<LeafPattern> collected) {
		if (collected == null) {
			collected = list();
		}

		List<LeafPattern> l = left;
		List<LeafPattern> c = collected;

		for (final Pattern pattern : getChildren()) {
			final MatchResult m = pattern.match(l, c);

			l = m.getLeft();
			c = m.getCollected();

			if (!m.matched()) {
				return new MatchResult(false, left, collected);
			}
		}

		return new MatchResult(true, l, c);
	}
}