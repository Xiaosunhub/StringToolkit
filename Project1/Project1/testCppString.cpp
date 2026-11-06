#include "cppString.h"
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
	using sp::cppString;
	using size_type = cppString::size_type;

	// construction
	check(cppString{ "Hello" } == "Hello", "construct from const char*");
	check(cppString(42) == "42", "construct from int");
	check(cppString(3.14, 2) == "3.14", "construct with precision");
	check(cppString{ { 'h', 'i' } } == "hi", "construct from initializer_list");
	check(cppString{ std::string("abc") } == "abc", "construct from std::string");

	// operator+ / concat
	check(cppString{ "Hello" } + " World" == "Hello World", "operator+ (cppString + const char*)");
	check("Say " + cppString{ "Hi" } == "Say Hi", "operator+ (const char* + cppString)");
	check(cppString{ "x" } + cppString{ "y" } == "xy", "operator+ (cppString + cppString)");
	check(cppString{ "ab" }.concat("cd", cppString{ "ef" }) == "cdef", "concat");
	check((cppString{ "ab" } += "cd") == "abcd", "operator+=");

	// modify
	check(cppString{ "  pad  " }.copyTrim() == "pad", "copyTrim");
	check(cppString{ "aabbcc" }.copyReplaceAll("bb", "XX") == "aaXXcc", "copyReplaceAll");
	check(cppString{ "abc" }.copyUpper() == "ABC", "copyUpper");
	check(cppString{ "ABC" }.copyLower() == "abc", "copyLower");
	check(cppString{ "aaa--aaa" }.copyTrim('-') == "aaa--aaa", "trim(ch) keeps middle");
	check(cppString{ "--x--" }.copyTrim('-') == "x", "trim(ch) both sides");

	cppString repeat{ "ab" };
	repeat *= size_type{ 3 };
	check(repeat == "ababab", "operator*=");
	check(cppString{ "ab" } * size_type{ 2 } == "abab", "operator*");

	// query
	check(cppString{ "hello" }.startsWith("he"), "startsWith");
	check(cppString{ "hello" }.endsWith("lo"), "endsWith");
	check(cppString{ "hello" }.contains('e'), "contains(char)");
	check(cppString{ "hello" }.contains("ell"), "contains(substring)");
	check(cppString{ "hello" }.findFirstOf('l') == 2, "findFirstOf");
	check(cppString{ "hello" }.findLastOf('l') == 3, "findLastOf");
	check(cppString{ "a,b,,c" }.split(',') ==
		(std::vector<cppString>{ "a", "b", "c" }), "split(char)");

	// number conversion
	check(cppString{ "12345" }.toNumber<int>() == 12345, "toNumber<int>");
	check(cppString{ "3.5" }.toNumber<double>() == 3.5, "toNumber<double>");
	check(cppString{ 1 }.fromNumber(456) == "456", "fromNumber");

	// exceptions
	try { cppString{ "12a" }.toNumber<int>(); check(false, "toNumber throws on bad input"); }
	catch (const sp::Invalid_Conversion&) { check(true, "toNumber throws on bad input"); }

	try { cppString{ "abc" }[10]; check(false, "operator[] throws on bad index"); }
	catch (const sp::Index_Out_Of_Range&) { check(true, "operator[] throws on bad index"); }

	// utf8
	check(cppString{ "abc" }.is_utf8(), "is_utf8 (ascii)");
	check(cppString{ "\xC3\xA9" }.is_utf8(), "is_utf8 (2-byte sequence)");
	check(cppString{ "\xC3" }.is_not_utf8(), "is_utf8 rejects truncated sequence");
	check(cppString{ "\xE0\x80\x80" }.is_not_utf8(), "is_utf8 rejects overlong sequence");

	// method.h helpers
	cppString m{ "abc" };
	sp::reverse_string(m);
	check(m == "cba", "reverse_string");

	cppString p{ "prefix-body" };
	sp::remove_prefix(p, cppString{ "prefix-" });
	check(p == "body", "remove_prefix");

	// and_then / create
	check(sp::cppString::create()
		.and_then([](sp::cppString val) -> sp::cppString { return val + "ok"; })
		== "ok", "create + and_then");

	// container interface
	{
		cppString s{ "ace" };
		s.insert(1, "b");
		check(s == "abce", "insert(pos, const char*)");
		s.insert(0, 'X');
		check(s == "Xabce", "insert(pos, char)");
		s.erase(0, 1);
		check(s == "abce", "erase(pos, count)");
		check(cppString{ "abc" }.compare("abd") < 0, "compare");
		check(cppString{ "abcabc" }.rfind('b') == 4, "rfind(char)");

		cppString t;
		t.assign(3, 'z');
		check(t == "zzz", "assign(count, char)");
		t.resize(5, '!');
		check(t == "zzz!!", "resize(count, fill)");
	}

	// text utilities (method.h)
	{
		check(sp::join({ cppString{ "a" }, cppString{ "b" }, cppString{ "c" } }, "-") == "a-b-c",
			"join(parts, separator)");
		check(sp::join({ cppString{ "a" }, cppString{ "b" } }, ',') == "a,b", "join(parts, char)");

		auto lines = sp::split_lines(cppString{ "a\r\nb\nc" });
		check(lines.size() == 3 && lines[0] == "a" && lines[1] == "b" && lines[2] == "c",
			"split_lines");

		cppString pad{ "\xE4\xB8\xAD" }; // U+4E2D, one character, three bytes
		sp::pad_left(pad, 3, '*');
		check(pad == "**\xE4\xB8\xAD", "pad_left (utf8 aware)");

		sp::pad_right(pad, 4, '.');
		check(pad == "**\xE4\xB8\xAD.", "pad_right");

		cppString cap{ "hELLO" };
		sp::capitalize(cap);
		check(cap == "Hello", "capitalize");

		cppString sw{ "AbC" };
		sp::swap_case(sw);
		check(sw == "aBc", "swap_case");

		cppString rf{ "abcabc" };
		sp::replace_first(rf, "bc", "X");
		check(rf == "aXabc", "replace_first");
	}

	// utf8 module
	{
		cppString mixed{ "a\xE4\xB8\xAD" "b" }; // a, U+4E2D, b

		check(sp::utf8::is_valid(mixed), "utf8::is_valid");
		check(sp::utf8::length(mixed) == 3, "utf8::length");
		check(sp::utf8::chars(mixed).size() == 3, "utf8::chars");
		check(sp::utf8::sub(mixed, 1, 1) == "\xE4\xB8\xAD", "utf8::sub");
		check(sp::utf8::reverse(mixed) == "b\xE4\xB8\xAD" "a", "utf8::reverse");

		auto cps = sp::utf8::codepoints(mixed);
		check(cps.size() == 3 && cps[1] == 0x4E2D, "utf8::codepoints");
		check(sp::utf8::from_codepoint(0x4E2D) == "\xE4\xB8\xAD", "utf8::from_codepoint");

		cppString rev{ "ab\xE4\xB8\xAD" };
		rev.reverse();
		check(rev == "\xE4\xB8\xAD" "ba", "cppString::reverse (utf8 aware)");
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

		sp::write_file(path, cppString{ "line1\nline2\n" });
		check(sp::read_file(path) == "line1\nline2\n", "write_file / read_file");

		auto fileLines = sp::read_lines(path);
		check(fileLines.size() == 2 && fileLines[0] == "line1" && fileLines[1] == "line2",
			"read_lines");

		sp::write_file(path, cppString{ "x" }, true);
		check(sp::read_file(path) == "line1\nline2\nx", "write_file (append)");

		std::remove(path.c_str());
	}

	// iterators, streams, sorting, hashing
	{
		int charSum = 0;
		for (const char c : cppString{ "abc" })
			charSum += c;
		check(charSum == 'a' + 'b' + 'c', "range-for over cppString");

		std::vector<cppString> words{ cppString{ "pear" }, cppString{ "apple" }, cppString{ "fig" } };
		std::ranges::sort(words);
		check(words[0] == "apple" && words[1] == "fig" && words[2] == "pear", "operator<=> sorting");

		std::unordered_map<cppString, int> counts;
		++counts[cppString{ "key" }];
		++counts[cppString{ "key" }];
		check(counts[cppString{ "key" }] == 2, "std::hash usable as map key");
	}

	// random / case-insensitive / split_by_any / wildcard / case styles
	{
		const cppString rnd = sp::generate_random(16);
		check(rnd.size() == 16, "generate_random length");
		check(rnd.find_first_not_of(
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") == cppString::npos,
			"generate_random charset");

		const cppString custom = sp::generate_random(8, "ab");
		check(custom.size() == 8
			&& custom.find_first_not_of("ab") == cppString::npos, "generate_random custom charset");

		try { sp::generate_random(4, ""); check(false, "generate_random throws on empty charset"); }
		catch (const sp::Invalid_Argument&) { check(true, "generate_random throws on empty charset"); }

		check(sp::icontains("Hello World", "WORLD"), "icontains");
		check(sp::istarts_with("Hello World", "hello"), "istarts_with");
		check(sp::iends_with("Hello World", "wOrLd"), "iends_with");

		auto fields = sp::split_by_any(cppString{ "a,b;c d" }, ",; ");
		check(fields.size() == 4 && fields[2] == "c", "split_by_any");

		auto trailing = sp::split_by_any(cppString{ "a,b," }, ",");
		check(trailing.size() == 2, "split_by_any (no trailing empty)");

		check(sp::wildcard_match("hello.txt", "*.txt"), "wildcard_match *");
		check(sp::wildcard_match("hello.txt", "hell?.txt"), "wildcard_match ?");
		check(sp::wildcard_match("a/b/c", "a*"), "wildcard_match * across separators");
		check(!sp::wildcard_match("hello.txt", "*.cpp"), "wildcard_match negative");
		check(sp::wildcard_match("", "*"), "wildcard_match empty string");

		cppString sn{ "HelloWorld_test" };
		sp::to_snake_case(sn);
		check(sn == "hello_world_test", "to_snake_case");

		cppString kb{ "HelloWorld-test" };
		sp::to_kebab_case(kb);
		check(kb == "hello-world-test", "to_kebab_case");

		cppString cm{ "hello_world-test" };
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

		auto row = sp::csv::split(cppString{ "a,\"b,c\",\"say \"\"hi\"\"\"" });
		check(row.size() == 3 && row[0] == "a" && row[1] == "b,c" && row[2] == "say \"hi\"",
			"csv::split");

		try { sp::csv::split(cppString{ "a,\"unterminated" }); check(false, "csv::split throws on unterminated quote"); }
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
		auto wrapped = sp::wrap(cppString{ "the quick brown fox" }, 10);
		check(wrapped.size() == 2 && wrapped[0] == "the quick" && wrapped[1] == "brown fox",
			"wrap");

		auto wide = sp::wrap(cppString{ "\xE4\xB8\xAD\xE6\x96\x87 abc" }, 4);
		check(wide.size() == 2 && wide[0] == "\xE4\xB8\xAD\xE6\x96\x87" && wide[1] == "abc",
			"wrap (utf8 aware)");

		cppString ind{ "a\n\nb" };
		sp::indent(ind, "  ");
		check(ind == "  a\n\n  b", "indent");

		check(sp::thousands_sep(1234567) == "1,234,567", "thousands_sep");
		check(sp::thousands_sep(-1234) == "-1,234", "thousands_sep (negative)");
		check(sp::thousands_sep(0) == "0", "thousands_sep (zero)");
		check(sp::thousands_sep(1234567, '.') == "1.234.567", "thousands_sep (custom)");

		check(sp::human_size(512) == "512 B", "human_size (bytes)");
		check(sp::human_size(1536) == "1.5 KB", "human_size (KB)");
		check(sp::human_size(1048576) == "1 MB", "human_size (MB)");

		const std::map<cppString, cppString> vars{
			{ "name", "Bob" },
			{ "lang", "C++" }
		};
		check(sp::render("Hi {{name}}, {{unknown}} here", vars) == "Hi Bob, {{unknown}} here",
			"render (template)");

		auto kv = sp::split_pair(cppString{ "key=value=1" }, "=");
		check(kv.first == "key" && kv.second == "value=1", "split_pair (first)");

		auto kvLast = sp::split_pair(cppString{ "key=value=1" }, "=", false);
		check(kvLast.first == "key=value" && kvLast.second == "1", "split_pair (last)");

		auto kvNone = sp::split_pair(cppString{ "nodelim" }, "=");
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
		cppString longText{ "the quick brown fox jumps" };
		sp::truncate(longText, 12);
		check(longText == "the quick...", "truncate");

		cppString shortText{ "hi" };
		sp::truncate(shortText, 10);
		check(shortText == "hi", "truncate (short enough)");

		cppString cjk{ "\xE4\xB8\xAD\xE6\x96\x87\xE6\xB5\x8B\xE8\xAF\x95" }; // 4 chars
		sp::truncate(cjk, 3);
		// With max_chars <= ellipsis length there is no room for the ellipsis: plain cut.
		check(cjk == "\xE4\xB8\xAD\xE6\x96\x87\xE6\xB5\x8B", "truncate (utf8 aware, no room for ellipsis)");

		cppString s{ "Hello World!  C++_Dev" };
		sp::slug(s);
		check(s == "hello-world-c-dev", "slug");

		auto parts = sp::chunk(cppString{ "abcdefg" }, 3);
		check(parts.size() == 3 && parts[0] == "abc" && parts[1] == "def" && parts[2] == "g",
			"chunk");

		auto cjkParts = sp::chunk(cppString{ "a\xE4\xB8\xAD" "b" }, 2);
		check(cjkParts.size() == 2 && cjkParts[0] == "a\xE4\xB8\xAD" && cjkParts[1] == "b",
			"chunk (utf8 aware)");

		try { sp::chunk(cppString{ "abc" }, 0); check(false, "chunk throws on zero size"); }
		catch (const sp::Invalid_Argument&) { check(true, "chunk throws on zero size"); }

		check(sp::to_bool("true") && sp::to_bool("YES") && sp::to_bool("1"), "to_bool (true)");
		check(!sp::to_bool("false") && !sp::to_bool("OFF") && !sp::to_bool("0"), "to_bool (false)");

		try { sp::to_bool("maybe"); check(false, "to_bool throws on bad input"); }
		catch (const sp::Invalid_Conversion&) { check(true, "to_bool throws on bad input"); }
	}

	{
		// URL-safe Base64
		check(sp::coding::toBase64Url("Man") == "TWFu", "toBase64Url (no padding)");
		check(sp::coding::toBase64Url("M") == "TQ", "toBase64Url (1 byte, unpadded)");
		check(sp::coding::fromBase64Url("TWFu") == "Man", "fromBase64Url");
		check(sp::coding::fromBase64Url("TQ") == "M", "fromBase64Url (unpadded)");
		check(sp::coding::fromBase64Url("TQ==") == "M", "fromBase64Url (tolerates padding)");

		const cppString binaryish{ String{ ">>??" } };
		check(sp::coding::toBase64Url(binaryish).s_str().find_first_of("+/=")
			== String::npos, "toBase64Url (url-safe alphabet)");
		check(sp::coding::fromBase64Url(sp::coding::toBase64Url(binaryish)) == binaryish,
			"base64url round-trip");

		try { sp::coding::fromBase64Url("A"); check(false, "fromBase64Url throws on bad length"); }
		catch (const sp::Invalid_Conversion&) { check(true, "fromBase64Url throws on bad length"); }

		// hamming / common prefix & suffix
		check(sp::hamming("kitten", "sittin") == 2, "hamming");
		check(sp::hamming("abc", "abc") == 0, "hamming (identical)");

		try { sp::hamming("ab", "abc"); check(false, "hamming throws on length mismatch"); }
		catch (const sp::Invalid_Argument&) { check(true, "hamming throws on length mismatch"); }

		check(sp::common_prefix("flower", "flow") == "flow", "common_prefix");
		check(sp::common_suffix("running", "jogging") == "ing", "common_suffix");
		check(sp::common_prefix("abc", "xyz").empty(), "common_prefix (none)");
		check(sp::common_suffix("abc", "xyz").empty(), "common_suffix (none)");

		const cppString cjkA{ "\xE4\xB8\xAD\xE6\x96\x87" };   // 2 chars
		const cppString cjkB{ "\xE4\xB8\xAD\xE8\x8B\xB1" };   // shares 1 char
		check(sp::common_prefix(cjkA, cjkB) == "\xE4\xB8\xAD", "common_prefix (utf8 boundary)");

		// human_duration
		check(sp::human_duration(0) == "0s", "human_duration (zero)");
		check(sp::human_duration(90061) == "1d 1h 1m 1s", "human_duration");
		check(sp::human_duration(3600) == "1h", "human_duration (single unit)");

		// Roman numerals
		check(sp::to_roman(1994) == "MCMXCIV", "to_roman");
		check(sp::to_roman(4) == "IV", "to_roman (subtractive)");
		check(sp::to_roman(3999) == "MMMCMXCIX", "to_roman (max)");
		check(sp::from_roman("MCMXCIV") == 1994, "from_roman");
		check(sp::from_roman("xiv") == 14, "from_roman (lowercase)");

		try { sp::to_roman(0); check(false, "to_roman throws on zero"); }
		catch (const sp::Invalid_Argument&) { check(true, "to_roman throws on zero"); }
		try { sp::to_roman(4000); check(false, "to_roman throws above 3999"); }
		catch (const sp::Invalid_Argument&) { check(true, "to_roman throws above 3999"); }
		try { sp::from_roman("IIII"); check(false, "from_roman throws on IIII"); }
		catch (const sp::Invalid_Conversion&) { check(true, "from_roman throws on IIII"); }
		try { sp::from_roman("VX"); check(false, "from_roman throws on VX"); }
		catch (const sp::Invalid_Conversion&) { check(true, "from_roman throws on VX"); }

		// HTML
		check(sp::html::escape("<a href=\"x\">&'</a>")
			== "&lt;a href=&quot;x&quot;&gt;&amp;&#39;&lt;/a&gt;", "html::escape");
		check(sp::html::unescape("&lt;b&gt;&amp;&quot;&apos;&nbsp;")
			== "<b>&\"'\xC2\xA0", "html::unescape (named)");
		check(sp::html::unescape("&#65;&#x42;") == "AB", "html::unescape (numeric)");
		check(sp::html::unescape("&#x4E2D;") == "\xE4\xB8\xAD", "html::unescape (cjk codepoint)");
		check(sp::html::unescape(sp::html::escape("a<b>&\"c\"")) == "a<b>&\"c\"",
			"html round-trip");

		try { sp::html::unescape("&bogus;"); check(false, "html::unescape throws on unknown entity"); }
		catch (const sp::Invalid_Conversion&) { check(true, "html::unescape throws on unknown entity"); }
		try { sp::html::unescape("&amp"); check(false, "html::unescape throws on unterminated"); }
		catch (const sp::Invalid_Conversion&) { check(true, "html::unescape throws on unterminated"); }
	}

	{
		// classical ciphers
		check(sp::coding::rot13("Hello, World!") == "Uryyb, Jbeyq!", "rot13");
		check(sp::coding::rot13(sp::coding::rot13("attack at dawn")) == "attack at dawn",
			"rot13 (self-inverse)");
		check(sp::coding::caesar("abc XYZ", 3) == "def ABC", "caesar (+3)");
		check(sp::coding::caesar("abc", -1) == "zab", "caesar (negative wraps)");
		check(sp::coding::caesar(sp::coding::caesar("secret", 7), -7) == "secret",
			"caesar round-trip");

		// Morse
		check(sp::coding::toMorse("SOS") == "... --- ...", "toMorse (SOS)");
		check(sp::coding::toMorse("HI YOU") == ".... .. / -.-- --- ..-", "toMorse (words)");
		check(sp::coding::fromMorse("... --- ...") == "SOS", "fromMorse");
		check(sp::coding::fromMorse(sp::coding::toMorse("HELLO 123")) == "HELLO 123",
			"morse round-trip");
		check(sp::coding::toMorse("ab  cd") == ".- -... / -.-. -..", "toMorse (multi-space)");

		try { sp::coding::fromMorse(".-.-.-x"); check(false, "fromMorse throws on unknown group"); }
		catch (const sp::Invalid_Conversion&) { check(true, "fromMorse throws on unknown group"); }

		// text tools
		cppString titled{ "the lord OF the_rings" };
		sp::title_case(titled);
		check(titled == "The Lord Of The_Rings", "title_case");

		check(sp::center("hi", 6, '*') == "**hi**", "center (even)");
		check(sp::center("hi", 7, '*') == "**hi***", "center (odd goes right)");
		check(sp::center("toolong", 4) == "toolong", "center (no truncation)");
		check(sp::center("\xE4\xB8\xAD", 3, '-') == "-\xE4\xB8\xAD-", "center (utf8)");

		cppString tabbed{ "a\tb" };
		sp::expand_tabs(tabbed, 4);
		check(tabbed == "a   b", "expand_tabs (column aware)");

		cppString newline_tab{ "ab\tc\nx\ty" };
		sp::expand_tabs(newline_tab, 4);
		check(newline_tab == "ab  c\nx   y", "expand_tabs (newline resets column)");

		cppString words{ "one two  three" };
		sp::reverse_words(words);
		check(words == "three two one", "reverse_words");

		const auto grams = sp::ngrams("abcd", 2);
		check(grams.size() == 3 && grams[0] == "ab" && grams[1] == "bc" && grams[2] == "cd",
			"ngrams");
		check(sp::ngrams("ab", 5).empty(), "ngrams (n too large)");
		check(sp::ngrams("\xE4\xB8\xAD\xE6\x96\x87\xE5\xAD\x97", 2).size() == 2,
			"ngrams (utf8)");

		try { sp::ngrams("abc", 0); check(false, "ngrams throws on zero"); }
		catch (const sp::Invalid_Argument&) { check(true, "ngrams throws on zero"); }

		check(sp::is_palindrome("racecar"), "is_palindrome (yes)");
		check(!sp::is_palindrome("hello"), "is_palindrome (no)");
		check(sp::is_palindrome(""), "is_palindrome (empty)");
		check(sp::is_palindrome("\xE4\xB8\xAD" "a" "\xE4\xB8\xAD"), "is_palindrome (utf8)");

		// soundex
		check(sp::soundex("Robert") == "R163", "soundex (Robert)");
		check(sp::soundex("Rupert") == "R163", "soundex (Rupert == Robert)");
		check(sp::soundex("Ashcraft") == "A261", "soundex (Ashcraft)");
		check(sp::soundex("Tymczak") == "T522", "soundex (Tymczak)");

		try { sp::soundex("123"); check(false, "soundex throws without letters"); }
		catch (const sp::Invalid_Argument&) { check(true, "soundex throws without letters"); }

		// uuid
		const auto id1 = sp::uuid_v4();
		const auto id2 = sp::uuid_v4();
		check(sp::validate::is_uuid(id1), "uuid_v4 (valid format)");
		check(id1 != id2, "uuid_v4 (unique)");
		check(id1.s_str()[14] == '4', "uuid_v4 (version nibble)");
		check(String{ "89abAB" }.find(id1.s_str()[19]) != String::npos,
			"uuid_v4 (variant nibble)");
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
