package org.docopt;

import static org.docopt.Python.count;
import static org.docopt.Python.list;
import static org.docopt.Python.set;
import static org.docopt.Python.split;

import java.util.Arrays;
import java.util.List;

abstract class Pattern {

	static class MatchResult {

		private final boolean match;

		private final List<LeafPattern> left;

		private final List<LeafPattern> collected;

		public MatchResult(final boolean match, final List<LeafPattern> left,
				final List<LeafPattern> collected) {
			this.match = match;
			this.left = left;
			this.collected = collected;
		}

		public boolean matched() {
			return match;
		}

		public List<LeafPattern> getLeft() {
			return left;
		}

		public List<LeafPattern> getCollected() {
			return collected;
		}
	}

	@SuppressWarnings("unchecked")
	private static final List<Class<? extends BranchPattern>> PARENTS = Arrays
			.asList(Required.class, Optional.class, OptionsShortcut.class,
					Either.class, OneOrMore.class);

	/**
	 * Expand pattern into an (almost) equivalent one, but with single Either.
	 * 
	 * Example: ((-a | -b) (-c | -d)) => (-a -c | -a -d | -b -c | -b -d) Quirks:
	 * [-a] => (-a), (-a...) => (-a -a)
	 */
	private static Either transform(final Pattern pattern) {
		final List<List<Pattern>> result = list();
		List<List<Pattern>> groups;

		// >>> groups = [[pattern]]
		{
			// Can't use "groups = list(list(pattern))" since the argument is
			// iterable.
			groups = list();
			groups.add(list(pattern));
		}

		while (!groups.isEmpty()) {
			final List<Pattern> children = groups.remove(0);

			BranchPattern child = null;

			// "If any parent type is the same type as the type of any child, select the first child that is of a type in parents."
			// >>> if any(t in map(type, children) for t in parents):
			// >>> child = [c for c in children if type(c) in parents][0]
			// TODO: I think that the "if" clause is redundant; instead, we just
			// try to get the child directly.
			for (final Pattern c : children) {
				if (PARENTS.contains(c.getClass())) {
					child = (BranchPattern) c;
					break;
				}
			}

			// See above for changes from python implementation.
			if (child != null) {
				children.remove(child);

				if (child.getClass() == Either.class) {
					for (final Pattern c : child.getChildren()) {
						// >>> groups.append([c] + children)
						final List<Pattern> group = list(c);
						group.addAll(children);
						groups.add(group);
					}
				}
				else if (child.getClass() == OneOrMore.class) {
					// >>> groups.append(child.children * 2 + children)
					final List<Pattern> group = list(child.getChildren());
					group.addAll(child.getChildren());
					group.addAll(children);
					groups.add(group);
				}
				else {
					// >>> groups.append(child.children + children)
					final List<Pattern> group = list(child.getChildren());
					group.addAll(children);
					groups.add(group);
				}
			}
			else {
				result.add(children);
			}
		}

		// >>> return Either(*[Required(*e) for e in result])
		{
			final List<Required> required = list();

			for (final List<Pattern> e : result) {
				required.add(new Required(e));
			}

			return new Either(required);
		}
	}

	public Pattern fix() {
		fixIdentities(null);
		fixRepeatingArguments();
		return this;
	}

	/**
	 * Make pattern-tree tips point to same object if they are equal.
	 */
	private void fixIdentities(List<Pattern> uniq) {
		// >>> if not hasattr(self, 'children')
		if (!(this instanceof BranchPattern)) {
			return;
		}

		if (uniq == null) {
			uniq = list(set(flat()));
		}

		final List<Pattern> children = ((BranchPattern) this).getChildren();

		for (int i = 0; i < children.size(); i++) {
			final Pattern child = children.get(i);

			if (!(child instanceof BranchPattern)) {
				assert uniq.contains(child);
				children.set(i, uniq.get(uniq.indexOf(child)));
			}
			else {
				child.fixIdentities(uniq);
			}
		}
	}

	/**
	 * Fix elements that should accumulate/increment values.
	 */
	private void fixRepeatingArguments() {
		List<List<Pattern>> either;

		// >>> either = [list(child.children) for child in
		// transform(self).children]
		{
			either = list();

			for (final Pattern child : transform(this).getChildren()) {
				either.add(list(((Required) child).getChildren()));
			}
		}

		for (final List<Pattern> $case : either) {
			// >>> for e in [child for child in case if case.count(child) > 1]
			for (final Pattern child : $case) { // ^^^
				if (count($case, child) > 1) { // ^^^
					final LeafPattern e = (LeafPattern) child; // ^^^

					if ((e.getClass() == Argument.class)
							|| ((e.getClass() == Option.class) && ((Option) e)
									.getArgCount() != 0)) {
						if (e.getValue() == null) {
							e.setValue(list());
						}
						else if (!(e.getValue() instanceof List)) {
							e.setValue(split(e.getValue().toString()));
						}
					}

					if ((e.getClass() == Command.class)
							|| ((e.getClass() == Option.class) && ((Option) e)
									.getArgCount() == 0)) {
						e.setValue(0);
					}
				}
			}
		}
	}

	protected abstract List<Pattern> flat(Class<?>... types);

	protected abstract MatchResult match(List<LeafPattern> left,
			List<LeafPattern> collected);

	protected MatchResult match(final List<LeafPattern> left) {
		return match(left, null);
	}

	@Override
	public abstract boolean equals(Object obj);
}