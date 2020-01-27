package org.docopt;

import static org.docopt.Python.list;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;

final class Either extends BranchPattern {

	public Either(final List<? extends Pattern> children) {
		super(children);
	}

	@Override
	protected MatchResult match(final List<LeafPattern> left,
			List<LeafPattern> collected) {
		if (collected == null) {
			collected = list();
		}

		final List<MatchResult> outcomes = list();

		for (final Pattern pattern : getChildren()) {
			final MatchResult m = pattern.match(left, collected);
			if (m.matched()) {
				outcomes.add(m);
			}
		}

		if (!outcomes.isEmpty()) {
			// >>> return min(outcomes, key=lambda outcome: len(outcome[1]))
			{
				return Collections.min(outcomes, new Comparator<MatchResult>() {

					@Override
					public int compare(final MatchResult o1,
							final MatchResult o2) {
						final Integer s1 = Integer.valueOf(o1.getLeft().size());
						final Integer s2 = Integer.valueOf(o2.getLeft().size());
						return s1.compareTo(s2);
					}
				});
			}
		}

		return new MatchResult(false, left, collected);
	}
}