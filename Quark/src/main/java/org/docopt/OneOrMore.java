package org.docopt;

import static org.docopt.Python.list;

import java.util.List;

final class OneOrMore extends BranchPattern {

	public OneOrMore(final List<? extends Pattern> children) {
		super(children);
	}

	@Override
	protected MatchResult match(final List<LeafPattern> left,
			List<LeafPattern> collected) {
		assert getChildren().size() == 1;

		if (collected == null) {
			collected = list();
		}

		List<LeafPattern> l = left;
		List<LeafPattern> c = collected;
		List<LeafPattern> l_ = null;
		final boolean matched = true;
		int times = 0;

		while (matched) {
			final MatchResult m = getChildren().get(0).match(l, c);

			l = m.getLeft();
			c = m.getCollected();

			if (m.matched()) {
				times++;
			}

			if ((l == null) ? (l_ == null) : l.equals(l_)) {
				break;
			}

			l_ = l;
		}

		if (times >= 1) {
			return new MatchResult(true, l, c);
		}

		return new MatchResult(false, left, collected);
	}
}