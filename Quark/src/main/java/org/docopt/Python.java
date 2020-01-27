package org.docopt;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

final class Python {

	static final class Re {

		public static final int IGNORECASE = Pattern.CASE_INSENSITIVE;

		public static final int MULTILINE = Pattern.MULTILINE
				| Pattern.UNIX_LINES;

		public static List<String> findAll(final String pattern,
				final String string, final int flags) {
			return findAll(Pattern.compile(pattern, flags), string);
		}

		public static List<String> findAll(final Pattern pattern,
				final String string) {
			final Matcher matcher = pattern.matcher(string);

			final List<String> result = list();

			while (matcher.find()) {
				if (matcher.groupCount() == 0) {
					result.add(matcher.group());
				}
				else {
					for (int i = 0; i < matcher.groupCount(); i++) {
						final String match = matcher.group(i + 1);

						if (match != null) {
							result.add(match);
						}
					}
				}
			}

			return result;
		}

		/**
		 * Determines if {@code pattern} contains at least one capturing group.
		 */
		private static boolean hasGrouping(final String pattern) {
			int i = -1;

			// Find the potential beginning of a group by looking for a left
			// parenthesis character.
			while ((i = pattern.indexOf('(', i + 1)) != -1) {
				int c = 0;

				// Count the number of escape characters immediately preceding
				// the
				// left parenthesis character.
				for (int j = i - 1; j > -1; j--) {
					if (pattern.charAt(j) != '\\') {
						break;
					}

					c++;
				}

				// If there is an even number of consecutive escape characters,
				// the character is not escaped and begins a group.
				if (c % 2 == 0) {
					return true;
				}
			}

			return false;
		}

		public static List<String> split(final String pattern,
				final String string) {
			if (!hasGrouping(pattern)) {
				return list(string.split(pattern));
			}

			final Matcher matcher = Pattern.compile(pattern, 0).matcher(string);

			final List<String> matches = list();

			int start = 0;

			while (matcher.find()) {
				matches.add(string.substring(start, matcher.start()));

				for (int i = 0; i < matcher.groupCount(); i++) {
					matches.add(matcher.group(i + 1));
				}

				start = matcher.end();
			}

			matches.add(string.substring(start));

			return matches;
		}

		public static String sub(final String pattern, final String repl,
				final String string) {
			return Pattern.compile(pattern, 0).matcher(string).replaceAll(repl);
		}

		private Re() {
			// Prevent instantiation.
		}
	}

	/**
	 * http://docs.python.org/2/library/#truth-value-testing
	 */
	public static boolean bool(final Object o) {
		// None
		if (o == null) {
			return false;
		}

		// False
		if (o instanceof Boolean) {
			return ((Boolean) o).booleanValue();
		}

		// zero of any numeric type, for example, 0, 0L, 0.0, 0j
		if (o instanceof Number) {
			if (o instanceof Integer) {
				return !((Integer) o).equals(0);
			}

			if (o instanceof Long) {
				return !((Long) o).equals(0L);
			}

			if (o instanceof Double) {
				return !((Double) o).equals(0.0);
			}

			if (o instanceof Float) {
				return !((Float) o).equals(0.0F);
			}

			if (o instanceof Byte) {
				return !((Byte) o).equals((byte) 0);
			}

			if (o instanceof Short) {
				return !((Short) o).equals((short) 0);
			}

			if (o instanceof AtomicInteger) {
				return bool(((AtomicInteger) o).get());
			}

			if (o instanceof AtomicLong) {
				return bool(((AtomicLong) o).get());
			}

			if (o instanceof BigDecimal) {
				return !BigDecimal.ZERO.equals(o);
			}

			if (o instanceof BigInteger) {
				return !BigInteger.ZERO.equals(o);
			}

			throw new IllegalArgumentException("unknown numeric type: "
					+ o.getClass());
		}

		// any empty sequence, for example, '', (), []
		{
			if (o instanceof String) {
				return !"".equals(o);
			}

			if (o instanceof Object[]) {
				return ((Object[]) o).length != 0;
			}

			if (o instanceof Collection) {
				return !((Collection<?>) o).isEmpty();
			}
		}

		// any empty mapping, for example, {}
		if (o instanceof Map) {
			return !((Map<?, ?>) o).isEmpty();
		}

		// All other values are considered true - so objects of many types are
		// always true.
		return true;
	}

	public static <T> boolean in(final T left, final T... right) {
		for (final Object o : right) {
			if (left != null) {
				if (left.equals(o)) {
					return true;
				}
			}
			else {
				if (o == null) {
					return true;
				}
			}
		}

		return false;
	}

	// Plus operator
	public static <T> List<T> plus(final List<T> a, final List<T> b) {
		final List<T> c = new ArrayList<T>(a.size() + b.size());

		c.addAll(a);
		c.addAll(b);

		return c;
	}

	public static String repr(final Object o) {
		if (o == null) {
			return "null";
		}

		if (o instanceof String) {
			return "\"" + o + "\"";
		}

		if (o instanceof Object[]) {
			return Arrays.toString((Object[]) o);
		}

		return o.toString();
	}

	public static <T> List<T> list(final Iterable<? extends T> elements) {
		final List<T> list = list();

		for (final T element : elements) {
			list.add(element);
		}

		return list;
	}

	public static <T> List<T> list(final T[] elements) {
		final List<T> list = list();
		for (final T element : elements) {
			list.add(element);
		}
		return list;
	}

	public static <T> List<T> list(final T element) {
		final List<T> list = list();
		list.add(element);
		return list;
	}

	public static <T> List<T> list() {
		return new ArrayList<T>();
	}

	public static <T> int count(final List<T> self, final T obj) {
		int count = 0;

		for (final T element : self) {
			if (element.equals(obj)) {
				count++;
			}
		}

		return count;
	}

	public static <T> Set<T> set(final Iterable<T> elements) {
		final Set<T> set = new HashSet<T>();

		for (final T element : elements) {
			set.add(element);
		}

		return set;
	}

	public static String join(final String self, final Iterable<?> iterable) {
		final Iterator<?> i = iterable.iterator();

		if (!i.hasNext()) {
			return "";
		}

		final StringBuilder sb = new StringBuilder();

		while (i.hasNext()) {
			sb.append(i.next());
			sb.append(self);
		}

		sb.setLength(sb.length() - self.length());

		return sb.toString();
	}

	public static String[] partition(final String self, final String sep) {
		final int i = self.indexOf(sep);

		if (i == -1) {
			return new String[] { self, "", "" };
		}

		// Always <= s.length
		final int j = i + sep.length();

		return new String[] { self.substring(0, i), sep,
				(j < self.length()) ? self.substring(j) : "" };
	}

	public static boolean isUpper(final String self) {
		boolean result = false;

		for (final char c : self.toCharArray()) {
			if (Character.isLetter(c)) {
				if (Character.isUpperCase(c)) {
					result = true;
				}
				else {
					return false;
				}
			}
		}

		return result;
	}

	public static List<String> split(final String self) {
		return list(self.trim().split("\\s+"));
	}

	private Python() {
		// Prevent instantiation.
	}
}
