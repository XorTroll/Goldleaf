package org.docopt;

import static org.docopt.Python.in;
import static org.docopt.Python.join;
import static org.docopt.Python.list;

import java.util.List;

/**
 * Branch/inner node of a pattern tree.
 */
abstract class BranchPattern extends Pattern {

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = super.hashCode();
		result = prime * result
				+ ((children == null) ? 0 : children.hashCode());
		return result;
	}

	@Override
	public boolean equals(final Object obj) {
		if (this == obj) {
			return true;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		final BranchPattern other = (BranchPattern) obj;
		if (children == null) {
			if (other.children != null) {
				return false;
			}
		}
		else if (!children.equals(other.children)) {
			return false;
		}
		return true;
	}

	private final List<Pattern> children;

	public BranchPattern(final List<? extends Pattern> children) {
		this.children = list(children);
	}

	@Override
	public String toString() {
		return String.format("%s(%s)", getClass().getSimpleName(),
				children.isEmpty() ? "" : join(", ", children));
	}

	@Override
	protected final List<Pattern> flat(final Class<?>... types) {
		if (in(getClass(), types)) {
			return list((Pattern) this);
		}

		// >>> return sum([child.flat(*types) for child in self.children], [])
		{
			final List<Pattern> result = list();

			for (final Pattern child : children) {
				result.addAll(child.flat(types));
			}

			return result;
		}
	}

	public List<Pattern> getChildren() {
		return children;
	}
}