package org.docopt;

import static org.docopt.Python.bool;
import static org.docopt.Python.partition;
import static org.docopt.Python.repr;
import static org.docopt.Python.split;

import java.util.List;

import org.docopt.Python.Re;

final class Option extends LeafPattern {

	private final String $short;

	private final String $long;

	private final int argCount;

	// >>> def __init__(self, short=None, long=None, argcount=0, value=False)
	public Option(final String $short, final String $long, final int argCount,
			final Object value) {
		super(
		// >>> @property
		// >>> def name(self):
		// >>> return self.long or self.short
				($long != null) ? $long : $short,
				// >>> self.value = None if value is False and argcount else
				// value
				(Boolean.FALSE.equals(value) && argCount != 0) ? null : value);

		assert argCount == 0 || argCount == 1;

		this.$short = $short;
		this.$long = $long;
		this.argCount = argCount;
	}

	public Option(final String $short, final String $long, final int argCount) {
		this($short, $long, argCount, false);
	}

	public Option(final String $short, final String $long) {
		this($short, $long, 0);
	}

	public static Option parse(final String optionDescription) {
		String $short = null;
		String $long = null;
		int argCount = 0;
		Object value = false;

		String options;
		String description;

		// >>> options, _, description = option_description.strip().partition('
		// ')
		{
			final String[] a = partition(optionDescription.trim(), "  ");
			options = a[0];
			description = a[2];
		}

		options = options.replaceAll(",", " ").replaceAll("=", " ");

		for (final String s : split(options)) {
			if (s.startsWith("--")) {
				$long = s;
			}
			else if (s.startsWith("-")) {
				$short = s;
			}
			else {
				argCount = 1;
			}
		}

		if (argCount != 0) {
			final List<String> matched = Re.findAll("\\[default: (.*)\\]",
					description, Re.IGNORECASE);
			value = bool(matched) ? matched.get(0) : null;
		}

		return new Option($short, $long, argCount, value);
	}

	@Override
	protected SingleMatchResult singleMatch(final List<LeafPattern> left) {
		for (int n = 0; n < left.size(); n++) {
			final LeafPattern pattern = left.get(n);

			if (getName().equals(pattern.getName())) {
				return new SingleMatchResult(n, pattern);
			}
		}

		return new SingleMatchResult(null, null);
	}

	public String getShort() {
		return $short;
	}

	public String getLong() {
		return $long;
	}

	public int getArgCount() {
		return argCount;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = super.hashCode();
		result = prime * result + (($long == null) ? 0 : $long.hashCode());
		result = prime * result + (($short == null) ? 0 : $short.hashCode());
		result = prime * result + argCount;
		return result;
	}

	@Override
	public boolean equals(final Object obj) {
		if (this == obj) {
			return true;
		}
		if (!super.equals(obj)) {
			return false;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		final Option other = (Option) obj;
		if ($long == null) {
			if (other.$long != null) {
				return false;
			}
		}
		else if (!$long.equals(other.$long)) {
			return false;
		}
		if ($short == null) {
			if (other.$short != null) {
				return false;
			}
		}
		else if (!$short.equals(other.$short)) {
			return false;
		}
		if (argCount != other.argCount) {
			return false;
		}
		return true;
	}

	@Override
	public String toString() {
		return String.format("%s(%s, %s, %s, %s)", getClass().getSimpleName(),
				repr($short), repr($long), repr(argCount), repr(getValue()));
	}
}