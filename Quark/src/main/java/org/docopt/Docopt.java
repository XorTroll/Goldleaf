package org.docopt;

import static org.docopt.Python.bool;
import static org.docopt.Python.in;
import static org.docopt.Python.isUpper;
import static org.docopt.Python.join;
import static org.docopt.Python.list;
import static org.docopt.Python.partition;
import static org.docopt.Python.set;
import static org.docopt.Python.split;

import java.io.InputStream;
import java.io.PrintStream;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;

import org.docopt.Pattern.MatchResult;
import org.docopt.Python.Re;

// @formatter:off
/**
 * Command-line interface parser that will make you smile.
 * <p>
 * Parses arguments based on the same POSIX-style usage pattern that would be
 * displayed when using the --help option. Simplifies handling command line
 * arguments and ensures that the parser is consistent with the documentation.
 * <h3>Example</h3>
 *
 * <pre>
 * class NavalFate {
 *
 *   private static final String doc =
 *       &quot;Naval Fate.\n&quot;
 *       + &quot;\n&quot;
 *       + &quot;Usage:\n&quot;
 *       + &quot;  naval_fate ship new &lt;name&gt;...\n&quot;
 *       + &quot;  naval_fate ship &lt;name&gt; move &lt;x&gt; &lt;y&gt; [--speed=&lt;kn&gt;]\n&quot;
 *       + &quot;  naval_fate ship shoot &lt;x&gt; &lt;y&gt;\n&quot;
 *       + &quot;  naval_fate mine (set|remove) &lt;x&gt; &lt;y&gt; [--moored | --drifting]\n&quot;
 *       + &quot;  naval_fate (-h | --help)\n&quot;
 *       + &quot;  naval_fate --version\n&quot;
 *       + &quot;\n&quot;
 *       + &quot;Options:\n&quot;
 *       + &quot;  -h --help     Show this screen.\n&quot;
 *       + &quot;  --version     Show version.\n&quot;
 *       + &quot;  --speed=&lt;kn&gt;  Speed in knots [default: 10].\n&quot;
 *       + &quot;  --moored      Moored (anchored) mine.\n&quot;
 *       + &quot;  --drifting    Drifting mine.\n&quot;
 *       + &quot;\n&quot;;
 *
 *   public static void main(String[] args) {
 *     Map&lt;String, Object&gt; opts =
 *         new Docopt(doc).withVersion(&quot;Naval Fate 2.0&quot;).parse(args);
 *     System.out.println(opts);
 *   }
 * }
 * </pre>
 *
 * Licensed under terms of MIT license (see LICENSE).
 * <p>
 * Copyright (c) 2012 Vladimir Keleshev, vladimir@keleshev.com<br />
 * Copyright (c) 2014 Damien Giese, damien.giese@gmail.com
 *
 * @version 0.6.0
 * @see <a href="http://docopt.org">docopt.org</a>
 * @see <a href="https://github.com/docopt/docopt.java">docopt for Java</a>
 */
// @formatter:on
public final class Docopt {

	/**
	 * <pre>
	 * long ::= '--' chars [ ( ' ' | '=' ) chars ] ;
	 * </pre>
	 */
	private static List<Option> parseLong(final Tokens tokens,
			final List<Option> options) {

		String $long;
		String eq;
		String value;

		// >>> long, eq, value = tokens.move().partition('=')
		{
			final String[] a = partition(tokens.move(), "=");
			$long = a[0];
			eq = a[1];
			value = a[2];
		}

		assert $long.startsWith("--");

		// >>> value = None if eq == value == '' else value
		if ("".equals(eq) && "".equals(value)) {
			value = null;
		}

		List<Option> similar;

		// >>> similar = [o for o in options if o.long == long]
		{
			similar = list();

			for (final Option o : options) {
				if ($long.equals(o.getLong())) {
					similar.add(o);
				}
			}
		}

		if (tokens.getError() == DocoptExitException.class && similar.isEmpty()) {
// @formatter:off
			// >>> similar = [o for o in options if o.long and  o.long.startswith(long)]
			// @formatter:on
			{
				for (final Option o : options) {
					if (o.getLong() != null && o.getLong().startsWith($long)) {
						similar.add(o);
					}
				}
			}
		}

		if (similar.size() > 1) {
			List<String> u;

			// >>> o.long for o in similar
			{
				u = list();
				for (final Option o : similar) {
					u.add(o.getLong());
				}
			}

			throw tokens.error("%s is not a unique prefix: %s?", $long,
					join(", ", u));
		}

		Option o;

		if (similar.size() < 1) {
			final int argCount = "=".equals(eq) ? 1 : 0;

			o = new Option(null, $long, argCount);

			options.add(o);

			if (tokens.getError() == DocoptExitException.class) {
				// @formatter:off
				// >>> o = Option(None, long, argcount, value if argcount else True)
				// @formatter:on
				o = new Option(null, $long, argCount, (argCount != 0) ? value
						: true);
			}
		}
		else {
// @formatter:off
			// >>> o = Option(similar[0].short, similar[0].long,
			// >>>            similar[0].argcount, similar[0].value)
			// @formatter:on
			{
				final Option u = similar.get(0);
				o = new Option(u.getShort(), u.getLong(), u.getArgCount(),
						u.getValue());
			}

			if (o.getArgCount() == 0) {
				if (value != null) {
					throw tokens.error("%s must not have an argument",
							o.getLong());
				}
			}
			else {
				if (value == null) {
					// >>> if tokens.current() in [None, '--']
					{
						final String u = tokens.current();
						if (u == null || "--".equals(u)) {
							throw tokens.error("%s requires argument",
									o.getLong());
						}
					}

					value = tokens.move();
				}
			}

			if (tokens.getError() == DocoptExitException.class) {
				o.setValue((value != null) ? value : true);
			}
		}

		return list(o);
	}

	/**
	 * <pre>
	 * shorts ::= '-' ( chars )* [ [ ' ' ] chars ] ;
	 * </pre>
	 */
	private static List<Option> parseShorts(final Tokens tokens,
			final List<Option> options) {
		final String token = tokens.move();
		assert token.startsWith("-") && !token.startsWith("--");
		String left = token.replaceFirst("^-+", "");

		final List<Option> parsed = list();

		while (!"".equals(left)) {
			final String $short = "-" + left.charAt(0);

			left = left.substring(1);

			List<Option> similar;

			// >>> similar = [o for o in options if o.short == short]
			{
				similar = list();

				for (final Option o : options) {
					if ($short.equals(o.getShort())) {
						similar.add(o);
					}
				}
			}

			if (similar.size() > 1) {
				throw tokens.error("%s is specified ambiguously %d times",
						$short, similar.size());
			}

			Option o;

			if (similar.size() < 1) {
				o = new Option($short, null, 0);

				options.add(o);

				if (tokens.getError() == DocoptExitException.class) {
					o = new Option($short, null, 0, true);
				}
			}
			else {
// @formatter:off
				// >>> o = Option(short, similar[0].long,
				// >>>            similar[0].argcount, similar[0].value)
				// @formatter:on
				{
					final Option u = similar.get(0);
					o = new Option($short, u.getLong(), u.getArgCount(),
							u.getValue());
				}

				String value = null;

				if (o.getArgCount() != 0) {
					if ("".equals(left)) {
						// >>> if tokens.current() in [None, '--']
						{
							final String u = tokens.current();
							if (u == null || "--".equals(u)) {
								throw tokens.error("%s requires argument",
										$short);
							}
							value = tokens.move();
						}
					}
					else {
						value = left;
						left = "";
					}
				}

				if (tokens.getError() == DocoptExitException.class) {
					o.setValue((value != null) ? value : true);
				}
			}

			parsed.add(o);
		}

		return parsed;
	}

	private static Required parsePattern(final String source,
			final List<Option> options) {
		final Tokens tokens = Tokens.fromPattern(source);
		final List<? extends Pattern> result = parseExpr(tokens, options);

		if (tokens.current() != null) {
			throw tokens.error("unexpected ending: %s", join(" ", tokens));
		}

		return new Required(result);
	}

	/**
	 * <pre>
	 * expr ::= seq ( '|' seq )* ;
	 * </pre>
	 */
	private static List<? extends Pattern> parseExpr(final Tokens tokens,
			final List<Option> options) {
		List<Pattern> seq = parseSeq(tokens, options);

		if (!"|".equals(tokens.current())) {
			return seq;
		}

		final List<Pattern> result = (seq.size() > 1) ? list((Pattern) new Required(
				seq))
				: seq;

		while ("|".equals(tokens.current())) {
			tokens.move();
			seq = parseSeq(tokens, options);
			result.addAll((seq.size() > 1) ? list(new Required(seq)) : seq);
		}

		return (result.size() > 1) ? list(new Either(result)) : result;
	}

	/**
	 * <pre>
	 * seq ::= ( atom [ '...' ] )* ;
	 * </pre>
	 */
	private static List<Pattern> parseSeq(final Tokens tokens,
			final List<Option> options) {
		final List<Pattern> result = list();

		// >>> while tokens.current() not in [None, ']', ')', '|']
		while (!in(tokens.current(), null, "]", ")", "|")) {
			List<? extends Pattern> atom = parseAtom(tokens, options);

			if ("...".equals(tokens.current())) {
				atom = list(new OneOrMore(atom));
				tokens.move();
			}

			result.addAll(atom);
		}

		return result;
	}

	// @formatter:off
	/**
	 * <pre>
	 * atom ::= '(' expr ')' | '[' expr ']' | 'options' | long | shorts | argument | command ;
	 * </pre>
	 */
	// @formatter:on
	private static List<? extends Pattern> parseAtom(final Tokens tokens,
			final List<Option> options) {
		final String token = tokens.current();

		List<Pattern> result = list();

		if ("(".equals(token) || "[".equals(token)) {
			tokens.move();

			String matching;

			// @formatter:off
			// >>> matching, pattern = {'(': [')', Required], '[': [']', Optional]}[token]
			// >>> result = pattern(*parse_expr(tokens, options))
			// @formatter:on
			{
				final List<? extends Pattern> u = parseExpr(tokens, options);

				if ("(".equals(token)) {
					matching = ")";
					result = list((Pattern) new Required(u));
				}
				else if ("[".equals(token)) {
					matching = "]";
					result = list((Pattern) new Optional(u));
				}
				else {
					throw new IllegalStateException();
				}
			}

			if (!matching.equals(tokens.move())) {
				throw tokens.error("unmatched '%s'", token);
			}

			return list(result);
		}

		if ("options".equals(token)) {
			tokens.move();
			return list(new OptionsShortcut());
		}

		if (token.startsWith("--") && !"--".equals(token)) {
			return parseLong(tokens, options);
		}

		if (token.startsWith("-") && !("-".equals(token) || "--".equals(token))) {
			return parseShorts(tokens, options);
		}

		if ((token.startsWith("<") && token.endsWith(">")) || isUpper(token)) {
			return list(new Argument(tokens.move()));
		}

		return list(new Command(tokens.move()));
	}

	/**
	 * Parse command-line argument vector.
	 * <p>
	 * If {@code optionsFirst} is {@code true}, the arguments must appear in the
	 * form:
	 *
	 * <pre>
	 * argv ::= [ long | shorts ]* [ argument ]* [ '--' [ argument ]* ] ;
	 * </pre>
	 *
	 * Otherwise, the arguments must appear in the form:
	 *
	 * <pre>
	 * argv ::= [ long | shorts | argument ]* [ '--' [ argument ]* ] ;
	 * </pre>
	 */
	private static List<LeafPattern> parseArgv(final Tokens tokens,
			final List<Option> options, final boolean optionsFirst) {
		final List<LeafPattern> parsed = list();

		while (tokens.current() != null) {
			if ("--".equals(tokens.current())) {
				// >>> return parsed + [Argument(None, v) for v in tokens]
				{
					for (final String v : tokens) {
						parsed.add(new Argument(null, v));
					}

					return parsed;
				}
			}

			// TODO: Why don't we check for tokens.current != "--" here?
			if (tokens.current().startsWith("--")) {
				parsed.addAll(parseLong(tokens, options));
			}
			else if (tokens.current().startsWith("-")
					&& !"-".equals(tokens.current())) {
				parsed.addAll(parseShorts(tokens, options));
			}
			else if (optionsFirst) {
				// >>> return parsed + [Argument(None, v) for v in tokens]
				{
					for (final String v : tokens) {
						parsed.add(new Argument(null, v));
					}

					return parsed;
				}
			}
			else {
				parsed.add(new Argument(null, tokens.move()));
			}
		}

		return parsed;
	}

	private static List<Option> parseDefaults(final String doc) {
		final List<Option> defaults = list();

		for (String s : parseSection("options:", doc)) {
			// >>> u, u, s = s.partition(':') # get rid of "options:"
			{
				final String[] u = partition(s, ":");
				s = u[2];
			}

			List<String> split;

			// >>> split = re.split('\n *(-\S+?)', '\n' + s)[1:]
			{
				split = Re.split("\\n *(-\\S+?)", "\n" + s);
				split.remove(0);
			}

			// >>> split = [s1 + s2 for s1, s2 in zip(split[::2], split[1::2])];
			{
				final List<String> u = list();

				for (int i = 1; i < split.size(); i += 2) {
					u.add(split.get(i - 1) + split.get(i));
				}

				split = u;
			}

			// @formatter:off
			// >>> options = [Option.parse(s) for s in split if s.startswith('-')]
			// >>> defaults += options
			// @formatter:on
			{
				for (final String $s : split) {
					if ($s.startsWith("-")) {
						defaults.add(Option.parse($s));
					}
				}
			}
		}

		return defaults;
	}

	private static List<String> parseSection(final String name,
			final String source) {
		// >>> return [s.strip() for s in pattern.findall(source)]
		{
			final List<String> u = Re.findAll("^([^\\n]*" + name +
					"[^\\n]*\\n?(?:[ \\t].*?(?:\\n|$))*)", source,
					Re.IGNORECASE | Re.MULTILINE);

			for (int i = 0; i < u.size(); i++) {
				u.set(i, u.get(i).trim());
			}

			return u;
		}
	}

	private static String formalUsage(String section) {
		// >>> u, u, section = section.partition(':')
		{
			final String[] u = partition(section, ":");
			section = u[2];
		}

		final List<String> pu = split(section);

		// @formatter:off
		// >>> return '( ' + ' '.join(') | (' if s == pu[0] else s for s in pu[1:]) + ' )'
		// @formatter:on
		{
			final StringBuilder sb = new StringBuilder();

			sb.append("( ");

			final String u = pu.remove(0);

			if (!pu.isEmpty()) {
				for (final String s : pu) {
					if (s.equals(u)) {
						sb.append(") | (");
					}
					else {
						sb.append(s);
					}

					sb.append(" ");
				}

				sb.setLength(sb.length() - 1);
			}

			sb.append(" )");

			return sb.toString();
		}
	}

	private static void extras(final boolean help, final String version,
			final List<? extends LeafPattern> options, final String doc) {
		boolean u;

		// @formatter:off
		// >>> if help and any((o.name in ('-h', '--help')) and o.value for o in options)
		// @formatter:on
		{
			u = false;

			if (help) {
				for (final LeafPattern o : options) {
					if ("-h".equals(o.getName()) | "--help".equals(o.getName())) {
						if (bool(o.getValue())) {
							u = true;
							break;
						}
					}
				}
			}
		}

		// Default --help behavior: print documentation and exit with success
		// status.
		if (u) {
			throw new DocoptExitException(0, doc.replaceAll("^\\n+|\\n+$", ""),
					false);
		}

		// @formatter:off
		// >>> if version and any(o.name == '--version' and o.value for o in options)
		// @formatter:on
		{
			u = false;

			if (bool(version)) {
				for (final LeafPattern o : options) {
					if ("--version".equals(o.getName())) {
						u = true;
						break;
					}
				}
			}
		}

		// Default --version behavior: print version and exit with success
		// status.
		if (u) {
			throw new DocoptExitException(0, version, false);
		}
	}

	static String read(final InputStream stream, final String charset) {
		final Scanner scanner = new Scanner(stream, charset);

		try {
			scanner.useDelimiter("\\A");
			return scanner.hasNext() ? scanner.next() : "";
		}
		finally {
			scanner.close();
		}
	}

	static String read(final InputStream stream) {
		return read(stream, "UTF-8");
	}

	private final String doc;

	private final String usage;

	private final List<Option> options;

	private final Required pattern;

	private boolean help = true;

	private String version = null;

	private boolean optionsFirst = false;

	private boolean exit = true;

	private PrintStream out = System.out;

	private PrintStream err = System.err;

	/**
	 * Constructs an argument parser from a POSIX-style help message.
	 *
	 * @param doc
	 *            a POSIX-style help message
	 * @throws DocoptLanguageError
	 *             if the help message is malformed
	 * @see Docopt
	 */
	public Docopt(final String doc) {
		this.doc = doc;

		final List<String> usageSections = parseSection("usage:", doc);

		if (usageSections.size() == 0) {
			throw new DocoptLanguageError(
					"\"usage:\" (case-insensitive) not found.");
		}

		if (usageSections.size() > 1) {
			throw new DocoptLanguageError(
					"More than one \"usage:\" (case-insensitive).");
		}

		usage = usageSections.get(0);
		options = parseDefaults(doc);
		pattern = parsePattern(formalUsage(usage), options);
	}

	/**
	 * Constructs an argument parser from a POSIX-style help message.
	 *
	 * @param stream
	 *            a stream containing a POSIX-style help message
	 * @param charset
	 *            the character encoding of the stream
	 * @throws DocoptLanguageError
	 *             if the help message is malformed
	 * @see Docopt
	 */
	public Docopt(final InputStream stream, final Charset charset) {
		this(read(stream, charset.displayName()));
	}

	/**
	 * Constructs an argument parser from a POSIX-style help message.
	 *
	 * @param stream
	 *            a UTF-8 encoded stream containing a POSIX-style help message
	 * @throws DocoptLanguageError
	 *             if the help message is malformed
	 * @see Docopt
	 */
	public Docopt(final InputStream stream) {
		this(read(stream));
	}

	/**
	 * If {@code enabled} is {@code true}, the default behavior for the
	 * {@code --help} (or {@code -h}) option will be used when the
	 * {@link #parse} method is invoked, causing the parser to display the
	 * argument to the constructor.
	 * <p>
	 * Enabled by default.
	 *
	 * @param enabled
	 *            {@code true} to enable; {@code false} to disable
	 * @return this object
	 * @see Docopt
	 * @see #parse
	 */
	public Docopt withHelp(final boolean enabled) {
		this.help = enabled;
		return this;
	}

	/**
	 * If set to a non-{@code null} value, the {@code --version} option will be
	 * cause the parser to display the specified string and exit.
	 *
	 * @param version
	 *            the version information to display
	 * @return this object
	 */
	public Docopt withVersion(final String version) {
		this.version = version;
		return this;
	}

	/**
	 * If set to a non-{@code null} value, the {@code --version} option will be
	 * cause the parser to display the specified string and exit.
	 *
	 * @param stream
	 *            a stream to read containing the version information
	 * @param charset
	 *            the character encoding of {@code stream}
	 * @return this object
	 */
	public Docopt withVersion(final InputStream stream, final Charset charset) {
		this.version = read(stream, charset.displayName());
		return this;
	}

	/**
	 * If set to a non-{@code null} value, the {@code --version} option will be
	 * cause the parser to display the specified string and exit.
	 *
	 * @param stream
	 *            a UTF-8 encoded stream to read containing the version
	 *            information
	 * @return this object
	 */
	public Docopt withVersion(final InputStream stream) {
		this.version = read(stream);
		return this;
	}

	/**
	 * If {@code enabled} is {@code true}, the parser will require options
	 * (e.g&#46; {@code --verbose}) to precede positional arguments (e.g&#46;
	 * FILE).
	 * <p>
	 * Disabled by default.
	 *
	 * @param enabled
	 *            {@code true} to enable; {@code false} to disable
	 * @return this object
	 */
	public Docopt withOptionsFirst(final boolean enabled) {
		this.optionsFirst = enabled;
		return this;
	}

	/**
	 * If {@code enabled} is {@code true}, the parser may terminate the JVM.
	 * Otherwise, a {@link DocoptExitException} will be thrown instead.
	 * <p>
	 * Enabled by default.
	 *
	 * @param enabled
	 *            {@code true} to enable; {@code false} to disable
	 * @return this object
	 * @see #parse
	 * @see #withHelp
	 * @see #withVersion
	 */
	public Docopt withExit(final boolean enabled) {
		this.exit = enabled;
		return this;
	}

	private Map<String, Object> doParse(final List<String> argv) {
		final List<LeafPattern> $argv = parseArgv(
				Tokens.withExitException(argv), list(options), optionsFirst);
		final Set<Pattern> patternOptions = set(pattern.flat(Option.class));

		for (final Pattern optionsShortcut : pattern
				.flat(OptionsShortcut.class)) {
// @formatter:off
			// >>> options_shortcut.children = list(set(doc_options) - pattern_options			// @formatter:on
			// @formatter:on
			{
				final List<Pattern> u = ((BranchPattern) optionsShortcut)
						.getChildren();
				u.clear();
				u.addAll(set(options));
				Pattern o = null;
				for (final Iterator<Pattern> i = u.iterator(); i.hasNext();) {
					o = i.next();
					for (final Pattern x : patternOptions) {
						if (o.equals(x)) {
							i.remove();
							// Make sure we don't try to remove the same option
							// twice.
							break;
						}
					}
				}
			}
		}

		extras(help, version, $argv, doc);

		final MatchResult m = pattern.fix().match($argv);

		if (m.matched() && m.getLeft().isEmpty()) {
			// @formatter:off
			// >>> return Dict((a.name, a.value) for a in (pattern.flat() + collected))
			// @formatter:on
			final Map<String, Object> u = new HashMap<String, Object>();

			for (final Pattern p : pattern.flat()) {
				// TODO: Does flat always return LeafPattern objects?
				if (!(p instanceof LeafPattern)) {
					throw new IllegalStateException();
				}

				final LeafPattern lp = (LeafPattern) p;

				u.put(lp.getName(), lp.getValue());
			}

			for (final LeafPattern p : m.getCollected()) {
				u.put(p.getName(), p.getValue());
			}

			return u;
		}

		// Arguments did not match any usage pattern. Print usage and exit with
		// error status.
		throw new DocoptExitException(1, null, true);
	}

	/**
	 * Parses {@code argv} based on command-line interface described by the
	 * argument to the constructor.
	 * <p>
	 * This method will attempt to exit the invoking application if:
	 * <ul>
	 * <li>the arguments cannot be parsed
	 * <li>the default {@code --help} behavior is invoked
	 * <li>the default {@code --version} behavior is invoked
	 * </ul>
	 * <p>
	 * In any of these cases, the parser will either terminate the JVM or throw
	 * a {@link DocoptExitException}, depending on configuration via
	 * {@link #withExit}.
	 *
	 * @param argv
	 *            the command line arguments
	 * @return A {@code Map}, where keys are names of command-line elements,
	 *         such as "--verbose" and "&lt;path&gt;", and values are the parsed
	 *         values of those elements.
	 * @throws DocoptExitException
	 *             if the application should exit and JVM termination has been
	 *             disabled via {@link #withExit}
	 */
	public Map<String, Object> parse(final List<String> argv)
			throws DocoptExitException {
		try {
			return doParse(argv);
		}
		catch (final DocoptExitException e) {
			if (!exit) {
				throw e;
			}

			@SuppressWarnings("resource")
			final PrintStream ps = (e.getExitCode() == 0) ? out : err;

			if (ps != null) {
				final String message = e.getMessage();

				if (message != null) {
					ps.println(message);
				}

				if (e.getPrintUsage()) {
					ps.println(usage);
				}
			}

			System.exit(e.getExitCode());

			// Not reachable.
			throw new IllegalStateException();
		}
	}

	/**
	 * Parses {@code argv} based on command-line interface described by the
	 * argument to the constructor.
	 * <p>
	 * This method will attempt to exit the invoking application if:
	 * <ul>
	 * <li>the arguments cannot be parsed
	 * <li>the default {@code --help} behavior is invoked
	 * <li>the default {@code --version} behavior is invoked
	 * </ul>
	 * <p>
	 * In any of these cases, the parser will either terminate the JVM or throw
	 * a {@link DocoptExitException}, depending on configuration via
	 * {@link #withExit}.
	 *
	 * @param argv
	 *            the command line arguments
	 * @return A {@code Map}, where keys are names of command-line elements,
	 *         such as "--verbose" and "&lt;path&gt;", and values are the parsed
	 *         values of those elements.
	 * @throws DocoptExitException
	 *             if the application should exit and JVM termination has been
	 *             disabled via {@link #withExit}
	 */
	public Map<String, Object> parse(final String... argv) {
		return parse(Arrays.asList(argv));
	}

	Docopt withStdOut(final PrintStream out) {
		this.out = out;
		return this;
	}

	Docopt withStdErr(final PrintStream err) {
		this.err = err;
		return this;
	}
}
