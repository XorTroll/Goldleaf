package org.docopt;

import java.util.List;

import static org.docopt.Python.list;

class Optional extends BranchPattern {

	public Optional(final List<? extends Pattern> children) {
		super(children);
	}

	@Override
	protected MatchResult match(List<LeafPattern> left,
			List<LeafPattern> collected) {
		if (collected == null) {
			collected = list();
		}

		for (final Pattern pattern : getChildren()) {
			final MatchResult u = pattern.match(left, collected);
			left = u.getLeft();
			collected = u.getCollected();
		}

		return new MatchResult(true, left, collected);
	}
}
