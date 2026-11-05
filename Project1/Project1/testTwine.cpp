#include "Twine.h"
#include "method.h"
#include "utf8.h"
#include "coding.h"
#include "path.h"
#include "checksum.h"
#include "validate.h"
#include "semver.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <unordered_map>

static int g_failed = 0;

static void check(bool ok, const char* name)
{
	if (ok)
	{
		std::cout << "[PASS] " << name << '\n';
	}
	else
	{
		std::cout << "[FAIL] " << name << '\n';
		++g_failed;
	}
}

int main()
{
	using sp::Twine;
	using size_type = Twine::size_type;

	// construction
	check(Twine{ "Hello" } == "Hello", "construct from const char*");
	check(Twine(42) == "42", "construct from int");
	check(Twine(3.14, 2) == "3.14", "construct with precision");
	check(Twine{ { 'h', 'i' } } == "hi", "construct from initializer_list");
	check(Twine{ std::string("abc") } == "abc", "construct from std::string");

	// operator+ / concat
	check(Twine{ "Hello" } + " World" == "Hello World", "operator+ (Twine + const char*)");
	check("Say " + Twine{ "Hi" } == "Say Hi", "operator+ (const char* + Twine)");
	check(Twine{ "x" } + Twine{ "y" } == "xy", "operator+ (Twine + Twine)");
	check(Twine{ "ab" }.concat("cd", Twine{ "ef" }) == "cdef", "concat");
	check((Twine{ "ab" } += "cd") == "abcd", "operator+=");

	// modify
	check(Twine{ "  pad  " }.copyTrim() == "pad", "copyTrim");
	check(Twine{ "aabbcc" }.copyReplaceAll("bb", "XX") == "aaXXcc", "copyReplaceAll");
	check(Twine{ "abc" }.copyUpper() == "ABC", "copyUpper");
	check(Twine{ "ABC" }.copyLower() == "abc", "copyLower");
	check(Twine{ "aaa--aaa" }.copyTrim('-') == "aaa--aaa", "trim(ch) keeps middle");
	check(Twine{ "--x--" }.copyTrim('-') == "x", "trim(ch) both sides");

	Twine repeat{ "ab" };
	repeat *= size_type{ 3 };
	check(repeat == "ababab", "operator*=");
	check(Twine{ "ab" } * size_type{ 2 } == "abab", "operator*");

	// query
	check(Twine{ "hello" }.startsWith("he"), "startsWith");
	check(Twine{ "hello" }.endsWith("lo"), "endsWith");
	check(Twine{ "hello" }.contains('e'), "contains(char)");
	check(Twine{ "hello" }.contains("ell"), "contains(substring)");
	check(Twine{ "hello" }.findFirstOf('l') == 2, "findFirstOf");
	check(Twine{ "hello" }.findLastOf('l') == 3, "findLastOf");
	check(Twine{ "a,b,,c" }.split(',') ==
		(std::vector<Twine>{ "a", "b", "c" }), "split(char)");

	// number conversion
	check(Twine{ "12345" }.toNumber<int>() == 12345, "toNumber<int>");
	check(Twine{ "3.5" }.toNumber<double>() == 3.5, "toNumber<double>");
	check(Twine{ 1 }.fromNumber(456) == "456", "fromNumber");

	// exceptions
	try { Twine{ "12a" }.toNumber<int>(); check(false, "toNumber throws on bad input"); }
	catch (const sp::Invalid_Conversion&) { check(true, "toNumber throws on bad input"); }

	try { Twine{ "abc" }[10]; check(false, "operator[] throws on bad index"); }
	catch (const sp::Index_Out_Of_Range&) { check(true, "operator[] throws on bad index"); }

	// utf8
	check(Twine{ "abc" }.is_utf8(), "is_utf8 (ascii)");
	check(Twine{ "\xC3\xA9" }.is_utf8(), "is_utf8 (2-byte sequence)");
	check(Twine{ "\xC3" }.is_not_utf8(), "is_utf8 rejects truncated sequence");
	check(Twine{ "\xE0\x80\x80" }.is_not_utf8(), "is_utf8 rejects overlong sequence");

	// method.h helpers
	Twine m{ "abc" };
	sp::reverse_string(m);
	check(m == "cba", "reverse_string");

	Twine p{ "prefix-body" };
	sp::remove_prefix(p, Twine{ "prefix-" });
	check(p == "body", "remove_prefix");

	// and_then / create
	check(sp::Twine::create()
		.and_then([](sp::Twine val) -> sp::Twine { return val + "ok"; })
		== "ok", "create + and_then");

	// container interface
	{
		Twine s{ "ace" };
		s.insert(1, "b");
		check(s == "abce", "insert(pos, const char*)");
		s.insert(0, 'X');
		check(s == "Xabce", "insert(pos, char)");
		s.erase(0, 1);
		check(s == "abce", "erase(pos, count)");
		check(Twine{ "abc" }.compare("abd") < 0, "compare");
		check(Twine{ "abcabc" }.rfind('b') == 4, "rfind(char)");

		Twine t;
		t.assign(3, 'z');
		check(t == "zzz", "assign(count, char)");
		t.resize(5, '!');
		check(t == "zzz!!", "resize(count, fill)");
	}

	// text utilities (method.h)
	{
		check(sp::join({ Twine{ "a" }, Twine{ "b" }, Twine{ "c" } }, "-") == "a-b-c",
			"join(parts, separator)");
		check(sp::join({ Twine{ "a" }, Twine{ "b" } }, ',') == "a,b", "join(parts, char)");

		auto lines = sp::split_lines(Twine{ "a\r\nb\nc" });
		check(lines.size() == 3 && lines[0] == "a" && lines[1] == "b" && lines[2] == "c",
			"split_lines");

		Twine pad{ "\xE4\xB8\xAD" }; // U+4E2D, one character, three bytes
		sp::pad_left(pad, 3, '*');
		check(pad == "**\xE4\xB8\xAD", "pad_left (utf8 aware)");

		sp::pad_right(pad, 4, '.');
		check(pad == "**\xE4\xB8\xAD.", "pad_right");

		Twine cap{ "hELLO" };
		sp::capitalize(cap);
		check(cap == "Hello", "capitalize");

		Twine sw{ "AbC" };
		sp::swap_case(sw);
		check(sw == "aBc", "swap_case");

		Twine rf{ "abcabc" };
		sp::replace_first(rf, "bc", "X");
		check(rf == "aXabc", "replace_first");
	}

	// utf8 module
	{
		Twine mixed{ "a\xE4\xB8\xAD" "b" }; // a, U+4E2D, b

		check(sp::utf8::is_valid(mixed), "utf8::is_valid");
		check(sp::utf8::length(mixed) == 3, "utf8::length");
		check(sp::utf8::chars(mixed).size() == 3, "utf8::chars");
		check(sp::utf8::sub(mixed, 1, 1) == "\xE4\xB8\xAD", "utf8::sub");
		check(sp::utf8::reverse(mixed) == "b\xE4\xB8\xAD" "a", "utf8::reverse");

		auto cps = sp::utf8::codepoints(mixed);
		check(cps.size() == 3 && cps[1] == 0x4E2D, "utf8::codepoints");
		check(sp::utf8::from_codepoint(0x4E2D) == "\xE4\xB8\xAD", "utf8::from_codepoint");

		Twine rev{ "ab\xE4\xB8\xAD" };
		rev.reverse();
		check(rev == "\xE4\xB8\xAD" "ba", "Twine::reverse (utf8 aware)");
	}

	// coding module
	{
		check(sp::coding::toBase64("Man") == "TWFu", "toBase64 (3 bytes)");
		check(sp::coding::toBase64("Ma") == "TWE=", "toBase64 (2 bytes + padding)");
		check(sp::coding::toBase64("M") == "TQ==", "toBase64 (1 byte + padding)");
		check(sp::coding::fromBase64("TWFu") == "Man", "fromBase64");
		check(sp::coding::fromHex("48656C6C6F") == "Hello", "fromHex");
		check(sp::coding::toHex("Hi") == "4869", "toHex");
		check(sp::coding::urlEncode("a b&c") == "a%20b%26c", "urlEncode");
		check(sp::coding::urlDecode("a%20b%26c") == "a b&c", "urlDecode");
		check(sp::coding::urlDecode("a+b") == "a b", "urlDecode ('+')");

		try { sp::coding::fromHex("123"); check(false, "fromHex throws on odd length"); }
		catch (const sp::Invalid_Conversion&) { check(true, "fromHex throws on odd length"); }

		// binary / octal / general base
		check(sp::coding::toBinary("Hi") == "0100100001101001", "toBinary");
		check(sp::coding::fromBinary("0100100001101001") == "Hi", "fromBinary");
		check(sp::coding::is_binary("01001"), "is_binary");
		check(!sp::coding::is_binary("01002"), "is_binary rejects non-bit");
		check(sp::coding::toOctal("Hi") == "110151", "toOctal");
		check(sp::coding::fromOctal("110151") == "Hi", "fromOctal");
		check(sp::coding::is_octal("1107"), "is_octal");
		check(sp::coding::toBase(255, 16) == "ff", "toBase(255, 16)");
		check(sp::coding::toBase(35, 36) == "z", "toBase(35, 36)");
		check(sp::coding::toBase(0, 2) == "0", "toBase(0)");
		check(sp::coding::fromBase("ff", 16) == 255, "fromBase hex");
		check(sp::coding::fromBase("-101", 2) == -5, "fromBase negative binary");
		check(sp::coding::fromBase("FF", 16) == 255, "fromBase case-insensitive");
		check(sp::coding::fromBase("z", 36) == 35, "fromBase base36");

		try { sp::coding::fromBinary("010"); check(false, "fromBinary throws on bad length"); }
		catch (const sp::Invalid_Conversion&) { check(true, "fromBinary throws on bad length"); }

		try { sp::coding::toBase(1, 37); check(false, "toBase throws on bad base"); }
		catch (const sp::Invalid_Argument&) { check(true, "toBase throws on bad base"); }
	}

	// file I/O (method.h)
	{
		const String path = "sp_test_tmp.txt";

		sp::write_file(path, Twine{ "line1\nline2\n" });
		check(sp::read_file(path) == "line1\nline2\n", "write_file / read_file");

		auto fileLines = sp::read_lines(path);
		check(fileLines.size() == 2 && fileLines[0] == "line1" && fileLines[1] == "line2",
			"read_lines");

		sp::write_file(path, Twine{ "x" }, true);
		check(sp::read_file(path) == "line1\nline2\nx", "write_file (append)");

		std::remove(path.c_str());
	}

	// iterators, streams, sorting, hashing
	{
		int charSum = 0;
		for (const char c : Twine{ "abc" })
			charSum += c;
		check(charSum == 'a' + 'b' + 'c', "range-for over Twine");

		std::vector<Twine> words{ Twine{ "pear" }, Twine{ "apple" }, Twine{ "fig" } };
		std::ranges::sort(words);
		check(words[0] == "apple" && words[1] == "fig" && words[2] == "pear", "operator<=> sorting");

		std::unordered_map<Twine, int> counts;
		++counts[Twine{ "key" }];
		++counts[Twine{ "key" }];
		check(counts[Twine{ "key" }] == 2, "std::hash usable as map key");
	}

	// random / case-insensitive / split_by_any / wildcard / case styles
	{
		const Twine rnd = sp::generate_random(16);
		check(rnd.size() == 16, "generate_random length");
		check(rnd.find_first_not_of(
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") == Twine::npos,
			"generate_random charset");

		const Twine custom = sp::generate_random(8, "ab");
		check(custom.size() == 8
			&& custom.find_first_not_of("ab") == Twine::npos, "generate_random custom charset");

		try { sp::generate_random(4, ""); check(false, "generate_random throws on empty charset"); }
		catch (const sp::Invalid_Argument&) { check(true, "generate_random throws on empty charset"); }

		check(sp::icontains("Hello World", "WORLD"), "icontains");
		check(sp::istarts_with("Hello World", "hello"), "istarts_with");
		check(sp::iends_with("Hello World", "wOrLd"), "iends_with");

		auto fields = sp::split_by_any(Twine{ "a,b;c d" }, ",; ");
		check(fields.size() == 4 && fields[2] == "c", "split_by_any");

		auto trailing = sp::split_by_any(Twine{ "a,b," }, ",");
		check(trailing.size() == 2, "split_by_any (no trailing empty)");

		check(sp::wildcard_match("hello.txt", "*.txt"), "wildcard_match *");
		check(sp::wildcard_match("hello.txt", "hell?.txt"), "wildcard_match ?");
		check(sp::wildcard_match("a/b/c", "a*"), "wildcard_match * across separators");
		check(!sp::wildcard_match("hello.txt", "*.cpp"), "wildcard_match negative");
		check(sp::wildcard_match("", "*"), "wildcard_match empty string");

		Twine sn{ "HelloWorld_test" };
		sp::to_snake_case(sn);
		check(sn == "hello_world_test", "to_snake_case");

		Twine kb{ "HelloWorld-test" };
		sp::to_kebab_case(kb);
		check(kb == "hello-world-test", "to_kebab_case");

		Twine cm{ "hello_world-test" };
		sp::to_camel_case(cm);
		check(cm == "helloWorldTest", "to_camel_case");
	}

	// JSON / CSV
	{
		check(sp::json::escape("He said \"hi\"\n") == "He said \\\"hi\\\"\\n", "json::escape");

		check(sp::json::unescape("a\\u4E2Db") == "a\xE4\xB8\xAD" "b", "json::unescape BMP");

		// U+1F600 (surrogate pair D83D DE00) -> F0 9F 98 80
		check(sp::json::unescape("\\uD83D\\uDE00") == "\xF0\x9F\x98\x80", "json::unescape surrogate pair");

		try { sp::json::unescape("bad\\q"); check(false, "json::unescape throws on unknown escape"); }
		catch (const sp::Invalid_Conversion&) { check(true, "json::unescape throws on unknown escape"); }

		check(sp::csv::escape("plain") == "plain", "csv::escape (no quotes needed)");
		check(sp::csv::escape("a,b") == "\"a,b\"", "csv::escape (comma)");
		check(sp::csv::escape("say \"hi\"") == "\"say \"\"hi\"\"\"", "csv::escape (quote doubling)");

		auto row = sp::csv::split(Twine{ "a,\"b,c\",\"say \"\"hi\"\"\"" });
		check(row.size() == 3 && row[0] == "a" && row[1] == "b,c" && row[2] == "say \"hi\"",
			"csv::split");

		try { sp::csv::split(Twine{ "a,\"unterminated" }); check(false, "csv::split throws on unterminated quote"); }
		catch (const sp::Invalid_Conversion&) { check(true, "csv::split throws on unterminated quote"); }
	}

	// path module
	{
		check(sp::path::join("a/b", "c") == "a/b/c", "path::join");
		check(sp::path::join("a/b", "/c") == "/c", "path::join (absolute child wins)");
		check(sp::path::join("a/b", "c/../d") == "a/b/d", "path::join resolves ..");
		check(sp::path::join("", "x\\y") == "x/y", "path::join (empty base, backslashes)");

		check(sp::path::normalize("C:\\a\\..\\b") == "C:/b", "path::normalize (drive + ..)");
		check(sp::path::normalize("a//b/./c/") == "a/b/c", "path::normalize (dots/dupes)");
		check(sp::path::normalize("/../a") == "/a", "path::normalize (.. at root)");
		check(sp::path::normalize("//server/share/x/..") == "//server/share", "path::normalize (UNC)");
		check(sp::path::normalize("") == ".", "path::normalize (empty)");

		check(sp::path::filename("a/b/c.txt") == "c.txt", "path::filename");
		check(sp::path::filename("a/b/") == "b", "path::filename (trailing sep)");
		check(sp::path::stem("a/b/c.tar.gz") == "c.tar", "path::stem");
		check(sp::path::stem(".gitignore") == ".gitignore", "path::stem (dotfile)");
		check(sp::path::extension("a/b/c.txt") == ".txt", "path::extension");
		check(sp::path::extension(".gitignore") == "", "path::extension (dotfile)");

		check(sp::path::parent("a/b/c.txt") == "a/b", "path::parent");
		check(sp::path::parent("/a") == "/", "path::parent (root)");
		check(sp::path::parent("a") == ".", "path::parent (single component)");

		check(sp::path::is_absolute("/x"), "path::is_absolute (/x)");
		check(sp::path::is_absolute("C:\\x"), "path::is_absolute (drive)");
		check(!sp::path::is_absolute("x/y"), "path::is_absolute (relative)");
	}

	// checksum module
	{
		// CRC-32/ISO-HDLC check value for "123456789".
		check(sp::checksum::crc32("123456789") == 0xCBF43926u, "crc32 check value");
		check(sp::checksum::crc32_hex("123456789") == "cbf43926", "crc32_hex");
		check(sp::checksum::crc32("") == 0u, "crc32 (empty)");

		check(sp::checksum::fnv1a64("a") == 0xAF63DC4C8601EC8Cull, "fnv1a64 check value");
		check(sp::checksum::fnv1a64_hex("a") == "af63dc4c8601ec8c", "fnv1a64_hex");

		check(sp::checksum::adler32("Wikipedia") == 0x11E60398u, "adler32 check value");
		check(sp::checksum::adler32_hex("Wikipedia") == "11e60398", "adler32_hex");
	}

	// statistics
	{
		check(sp::word_count("hello world  x") == 3, "word_count");
		check(sp::word_count("   ") == 0, "word_count (blank)");
		check(sp::levenshtein("kitten", "sitting") == 3, "levenshtein");
		check(sp::levenshtein("\xE4\xB8\xAD\xE6\x96\x87", "\xE4\xB8\xADx") == 1,
			"levenshtein (utf8 chars)");
		check(sp::similarity("abc", "abc") == 1.0, "similarity (identical)");
		check(sp::similarity("", "") == 1.0, "similarity (both empty)");
		check(sp::similarity("abc", "") == 0.0, "similarity (one empty)");
		check(sp::similarity("abcd", "abce") == 0.75, "similarity (one edit)");

		check(sp::longest_common_prefix({ "foobar", "foo", "fooz" }) == "foo",
			"longest_common_prefix");
		check(sp::longest_common_prefix({ "abc" }) == "abc", "longest_common_prefix (single)");
		check(sp::longest_common_prefix({}) == "", "longest_common_prefix (empty)");
		// Must not cut a multi-byte char in half: 中 vs 丮 share the first 2 bytes E4 B8.
		check(sp::longest_common_prefix({ "\xE4\xB8\xAD" "x", "\xE4\xB8\xAE" "y" }) == "",
			"longest_common_prefix (utf8 boundary)");
	}

	// layout
	{
		auto wrapped = sp::wrap(Twine{ "the quick brown fox" }, 10);
		check(wrapped.size() == 2 && wrapped[0] == "the quick" && wrapped[1] == "brown fox",
			"wrap");

		auto wide = sp::wrap(Twine{ "\xE4\xB8\xAD\xE6\x96\x87 abc" }, 4);
		check(wide.size() == 2 && wide[0] == "\xE4\xB8\xAD\xE6\x96\x87" && wide[1] == "abc",
			"wrap (utf8 aware)");

		Twine ind{ "a\n\nb" };
		sp::indent(ind, "  ");
		check(ind == "  a\n\n  b", "indent");

		check(sp::thousands_sep(1234567) == "1,234,567", "thousands_sep");
		check(sp::thousands_sep(-1234) == "-1,234", "thousands_sep (negative)");
		check(sp::thousands_sep(0) == "0", "thousands_sep (zero)");
		check(sp::thousands_sep(1234567, '.') == "1.234.567", "thousands_sep (custom)");

		check(sp::human_size(512) == "512 B", "human_size (bytes)");
		check(sp::human_size(1536) == "1.5 KB", "human_size (KB)");
		check(sp::human_size(1048576) == "1 MB", "human_size (MB)");

		const std::map<Twine, Twine> vars{
			{ "name", "Bob" },
			{ "lang", "C++" }
		};
		check(sp::render("Hi {{name}}, {{unknown}} here", vars) == "Hi Bob, {{unknown}} here",
			"render (template)");

		auto kv = sp::split_pair(Twine{ "key=value=1" }, "=");
		check(kv.first == "key" && kv.second == "value=1", "split_pair (first)");

		auto kvLast = sp::split_pair(Twine{ "key=value=1" }, "=", false);
		check(kvLast.first == "key=value" && kvLast.second == "1", "split_pair (last)");

		auto kvNone = sp::split_pair(Twine{ "nodelim" }, "=");
		check(kvNone.first == "nodelim" && kvNone.second.empty(), "split_pair (no delimiter)");
	}

	// validate module
	{
		check(sp::validate::is_ipv4("192.168.1.1"), "is_ipv4 (valid)");
		check(sp::validate::is_ipv4("255.255.255.255"), "is_ipv4 (max)");
		check(!sp::validate::is_ipv4("256.1.1.1"), "is_ipv4 rejects > 255");
		check(!sp::validate::is_ipv4("01.2.3.4"), "is_ipv4 rejects leading zeros");
		check(!sp::validate::is_ipv4("1.2.3"), "is_ipv4 rejects 3 groups");

		check(sp::validate::is_ipv6("::1"), "is_ipv6 (loopback)");
		check(sp::validate::is_ipv6("2001:db8::ff00:42:8329"), "is_ipv6 (compressed)");
		check(sp::validate::is_ipv6("2001:0db8:0000:0000:0000:ff00:0042:8329"),
			"is_ipv6 (full form)");
		check(sp::validate::is_ipv6("::ffff:1.2.3.4"), "is_ipv6 (ipv4 tail)");
		check(sp::validate::is_ipv6("::"), "is_ipv6 (all zeros)");
		check(!sp::validate::is_ipv6("::ffff:256.1.1.1"), "is_ipv6 rejects bad ipv4 tail");
		check(!sp::validate::is_ipv6("1::2::3"), "is_ipv6 rejects double ::");
		check(!sp::validate::is_ipv6("2001:db8::ff00:42:8329:extra"), "is_ipv6 rejects bad group");
		check(!sp::validate::is_ipv6("1:2:3:4:5:6:7:8:9"), "is_ipv6 rejects 9 groups");

		check(sp::validate::is_email("user@example.com"), "is_email");
		check(sp::validate::is_email("a.b+c@sub.domain.org"), "is_email (plus + sub)");
		check(!sp::validate::is_email("user@localhost"), "is_email requires a dot");
		check(!sp::validate::is_email("@example.com"), "is_email requires local part");
		check(!sp::validate::is_email("a@@b.com"), "is_email rejects double @");
		check(!sp::validate::is_email("a..b@example.com"), "is_email rejects consecutive dots");

		check(sp::validate::is_uuid("550e8400-e29b-41d4-a716-446655440000"), "is_uuid");
		check(sp::validate::is_uuid("550E8400-E29B-41D4-A716-446655440000"),
			"is_uuid (uppercase)");
		check(!sp::validate::is_uuid("550e8400e29b41d4a716446655440000"),
			"is_uuid requires hyphens");
		check(!sp::validate::is_uuid("550e8400-e29b-41d4-a716"), "is_uuid rejects truncated");

		check(sp::validate::is_url("https://example.com/path?q=1"), "is_url (https)");
		check(sp::validate::is_url("custom+v2://x"), "is_url (custom scheme)");
		check(!sp::validate::is_url("not a url"), "is_url rejects plain text");
		check(!sp::validate::is_url("1http://x"), "is_url scheme must start with a letter");
	}

	// semver module
	{
		check(sp::semver::is_valid("1.2.3"), "semver::is_valid");
		check(sp::semver::is_valid("1.0.0-alpha.1+build.5"), "semver::is_valid (pre + build)");
		check(!sp::semver::is_valid("1.02.3"), "semver rejects leading zeros");
		check(!sp::semver::is_valid("1.2"), "semver requires 3 parts");
		check(!sp::semver::is_valid("1.0.0-alpha..1"), "semver rejects empty identifier");

		check(sp::semver::compare("1.2.3", "1.2.4") < 0, "semver::compare (patch)");
		check(sp::semver::compare("1.10.0", "1.9.0") > 0, "semver::compare (numeric, not lexical)");
		check(sp::semver::compare("1.0.0", "1.0.0+build.5") == 0, "semver::compare (build ignored)");
		check(sp::semver::compare("1.0.0-alpha", "1.0.0") < 0,
			"semver::compare (prerelease < release)");
		check(sp::semver::compare("1.0.0-alpha", "1.0.0-alpha.1") < 0,
			"semver::compare (fewer identifiers < more)");
		check(sp::semver::compare("1.0.0-alpha.1", "1.0.0-alpha.beta") < 0,
			"semver::compare (numeric < alphanumeric)");
		check(sp::semver::compare("1.0.0-beta.11", "1.0.0-beta.2") > 0,
			"semver::compare (numeric identifiers by value)");

		try { sp::semver::compare("1.2", "1.2.3"); check(false, "semver::compare throws on invalid"); }
		catch (const sp::Invalid_Format&) { check(true, "semver::compare throws on invalid"); }
	}

	// convenience (method.h)
	{
		Twine longText{ "the quick brown fox jumps" };
		sp::truncate(longText, 12);
		check(longText == "the quick...", "truncate");

		Twine shortText{ "hi" };
		sp::truncate(shortText, 10);
		check(shortText == "hi", "truncate (short enough)");

		Twine cjk{ "\xE4\xB8\xAD\xE6\x96\x87\xE6\xB5\x8B\xE8\xAF\x95" }; // 4 chars
		sp::truncate(cjk, 3);
		// With max_chars <= ellipsis length there is no room for the ellipsis: plain cut.
		check(cjk == "\xE4\xB8\xAD\xE6\x96\x87\xE6\xB5\x8B", "truncate (utf8 aware, no room for ellipsis)");

		Twine s{ "Hello World!  C++_Dev" };
		sp::slug(s);
		check(s == "hello-world-c-dev", "slug");

		auto parts = sp::chunk(Twine{ "abcdefg" }, 3);
		check(parts.size() == 3 && parts[0] == "abc" && parts[1] == "def" && parts[2] == "g",
			"chunk");

		auto cjkParts = sp::chunk(Twine{ "a\xE4\xB8\xAD" "b" }, 2);
		check(cjkParts.size() == 2 && cjkParts[0] == "a\xE4\xB8\xAD" && cjkParts[1] == "b",
			"chunk (utf8 aware)");

		try { sp::chunk(Twine{ "abc" }, 0); check(false, "chunk throws on zero size"); }
		catch (const sp::Invalid_Argument&) { check(true, "chunk throws on zero size"); }

		check(sp::to_bool("true") && sp::to_bool("YES") && sp::to_bool("1"), "to_bool (true)");
		check(!sp::to_bool("false") && !sp::to_bool("OFF") && !sp::to_bool("0"), "to_bool (false)");

		try { sp::to_bool("maybe"); check(false, "to_bool throws on bad input"); }
		catch (const sp::Invalid_Conversion&) { check(true, "to_bool throws on bad input"); }
	}

	if (g_failed == 0)
	{
		std::cout << "All tests passed.\n";
	}
	else
	{
		std::cout << g_failed << " test(s) failed.\n";
	}

	return g_failed == 0 ? 0 : 1;
}
