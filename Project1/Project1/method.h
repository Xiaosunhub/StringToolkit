// Copyright(c)[2025][Ender_]
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __INC_METHOD_H__
#define __INC_METHOD_H__

#include "StringToolkit.h"

namespace sp
{
	void reverse_string(StringToolkit& str);

	void reverse_string_to(StringToolkit& str, StringToolkit& result);
	void reverse_string_to(StringToolkit&& str, StringToolkit& result) noexcept;

	void copy_to(StringToolkit& str, StringToolkit& result);
	void copy_to(StringToolkit&& str, StringToolkit& result) noexcept;

	void move_to(StringToolkit& str, StringToolkit& result);
	void move_to(StringToolkit&& str, StringToolkit& result) noexcept;

	void fill_with(StringToolkit& str, char ch);

	void fill(StringToolkit& str, char ch, std::size_t count);

	void remove_prefix(StringToolkit& str, const StringToolkit& prefix);
	void remove_suffix(StringToolkit& str, const StringToolkit& suffix);

	// ---- text utilities ----

	// Concatenate parts with a separator between them.
	StringToolkit join(const std::vector<StringToolkit>& parts, const StringToolkit& separator);
	StringToolkit join(const std::vector<StringToolkit>& parts, char separator);

	// Split on '\n', "\r\n" or '\r'. A trailing newline does not produce an empty last line.
	std::vector<StringToolkit> split_lines(const StringToolkit& str);

	// Pad to width characters (UTF-8 aware). No-op when already wide enough.
	void pad_left(StringToolkit& str, std::size_t width, char fill = ' ');
	void pad_right(StringToolkit& str, std::size_t width, char fill = ' ');

	// Uppercase the first ASCII letter and lowercase the rest.
	void capitalize(StringToolkit& str);

	// Swap ASCII letter case.
	void swap_case(StringToolkit& str);

	// Replace the first occurrence. Throws Invalid_Argument when from is empty.
	void replace_first(StringToolkit& str, const StringToolkit& from, const StringToolkit& to);

	// ---- file I/O ----

	// Read the whole file as bytes. Throws Invalid_Operation when the file cannot be opened.
	StringToolkit read_file(const String& path);
	std::vector<StringToolkit> read_lines(const String& path);

	// Write (or append) bytes to a file. Throws Invalid_Operation when the file cannot be opened.
	void write_file(const String& path, const StringToolkit& data, bool append = false);

	// ---- random ----

	// Random string of the given length drawn from charset
	// (default: A-Z a-z 0-9). Throws Invalid_Argument when charset is empty.
	StringToolkit generate_random(std::size_t length, const StringToolkit& charset =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

	// ---- case-insensitive conveniences (ASCII only) ----

	bool icontains(const StringToolkit& str, const StringToolkit& sub);
	bool istarts_with(const StringToolkit& str, const StringToolkit& prefix);
	bool iends_with(const StringToolkit& str, const StringToolkit& suffix);

	// ---- splitting ----

	// Split on every occurrence of ANY character in delimiters.
	// Consecutive delimiters produce empty fields; a trailing delimiter produces no trailing empty field.
	std::vector<StringToolkit> split_by_any(const StringToolkit& str, const StringToolkit& delimiters);

	// ---- pattern matching ----

	// Shell-style wildcard match: '*' matches any run (including empty), '?' matches exactly one char.
	// All other characters must match literally.
	bool wildcard_match(const StringToolkit& str, const StringToolkit& pattern);

	// ---- case style conversions ----

	// "HelloWorld" / "hello-world" / "Hello World" -> "hello_world"
	void to_snake_case(StringToolkit& str);
	// "HelloWorld" / "hello_world" / "Hello World" -> "hello-world"
	void to_kebab_case(StringToolkit& str);
	// "hello_world" / "hello-world" / "Hello World" -> "helloWorld"
	void to_camel_case(StringToolkit& str);

	// ---- JSON ----

	namespace json
	{
		// Escape for embedding in a JSON string literal: " \ \b \f \n \r \t
		// and control characters below 0x20 (as \u00XX). Other bytes pass through unchanged.
		StringToolkit escape(const StringToolkit& text);

		// Inverse of escape. Handles \uXXXX including UTF-16 surrogate pairs.
		// Throws Invalid_Conversion on malformed escape sequences.
		StringToolkit unescape(const StringToolkit& text);
	}

	// ---- CSV ----

	namespace csv
	{
		// Quote and escape a single field per RFC 4180 when it contains , " CR or LF.
		StringToolkit escape(const StringToolkit& field);

		// Parse one CSV line: quoted fields may contain commas and doubled quotes.
		// Throws Invalid_Conversion on an unterminated quote.
		std::vector<StringToolkit> split(const StringToolkit& line);
	}

	// ---- statistics ----

	// Number of whitespace-separated words. A run of CJK characters counts as one word
	// because no space is required between them.
	std::size_t word_count(const StringToolkit& str);

	// Levenshtein edit distance measured in characters (UTF-8 aware).
	std::size_t levenshtein(const StringToolkit& a, const StringToolkit& b);

	// 1.0 for identical strings, 0.0 for completely different ones.
	// Empty vs empty is 1.0; empty vs non-empty is 0.0.
	double similarity(const StringToolkit& a, const StringToolkit& b);

	// Longest common prefix of all parts, cut on a UTF-8 boundary. Empty input -> "".
	StringToolkit longest_common_prefix(const std::vector<StringToolkit>& parts);

	// ---- layout ----

	// Greedy word wrap at width characters (UTF-8 aware). Existing newlines are kept;
	// words longer than the width are left intact on their own line.
	std::vector<StringToolkit> wrap(const StringToolkit& str, std::size_t width);

	// Prefix every non-empty line with prefix.
	void indent(StringToolkit& str, const StringToolkit& prefix);

	// Group the integer part with sep: 1234567 -> "1,234,567". Keeps a leading '-'.
	StringToolkit thousands_sep(long long value, char sep = ',');

	// Human readable byte size: 512 -> "512 B", 1536 -> "1.5 KB", 1048576 -> "1 MB".
	StringToolkit human_size(unsigned long long bytes);

	// Replace every "{{key}}" in tmpl using vars. Unknown placeholders are left untouched.
	StringToolkit render(const StringToolkit& tmpl, const std::map<StringToolkit, StringToolkit>& vars);

	// Split at the first (or last) occurrence of delimiter.
	// When the delimiter is absent the whole string goes to first and second is empty.
	std::pair<StringToolkit, StringToolkit> split_pair(const StringToolkit& str,
		const StringToolkit& delimiter, bool first = true);

	// ---- convenience ----

	// Cut the string to at most max_chars characters (UTF-8 aware), appending
	// ellipsis when anything was removed. The ellipsis is included in max_chars.
	void truncate(StringToolkit& str, std::size_t max_chars, const StringToolkit& ellipsis = "...");

	// URL-friendly slug: lowercase, spaces/underscores -> '-', drop everything
	// outside [a-z0-9-], collapse repeated '-', trim leading/trailing '-'.
	void slug(StringToolkit& str);

	// Split into chunks of size characters (UTF-8 aware; the last chunk may be shorter).
	// Throws Invalid_Argument when size is zero.
	std::vector<StringToolkit> chunk(const StringToolkit& str, std::size_t size);

	// Parse a boolean: "true/1/yes/on/y/t" and "false/0/no/off/n/f", case-insensitive.
	// Throws Invalid_Conversion on anything else.
	bool to_bool(const StringToolkit& str);

	// ---- string distance & diff helpers ----

	// Positional difference count; strings must have equal UTF-8 character
	// counts, otherwise Invalid_Argument is thrown.
	std::size_t hamming(const StringToolkit& a, const StringToolkit& b);

	// Longest shared leading/trailing part (never cuts a UTF-8 sequence).
	StringToolkit common_prefix(const StringToolkit& a, const StringToolkit& b);
	StringToolkit common_suffix(const StringToolkit& a, const StringToolkit& b);

	// ---- formatting ----

	// 90061 -> "1d 1h 1m 1s". Zero becomes "0s".
	StringToolkit human_duration(unsigned long long seconds);

	// Roman numerals for 1..3999 (throws Invalid_Argument outside that range).
	// from_roman accepts uppercase and lowercase, throws Invalid_Conversion
	// on malformed input such as "IIII" or "VX".
	StringToolkit to_roman(int value);
	int from_roman(const StringToolkit& text);

	// ---- HTML ----

	namespace html
	{
		// Escape & < > " ' as &amp; &lt; &gt; &quot; &#39;.
		StringToolkit escape(const StringToolkit& text);

		// Decode named entities (amp lt gt quot apos nbsp) and numeric
		// entities (&#65; / &#x41;) to UTF-8. Throws Invalid_Conversion on
		// malformed or out-of-range entities.
		StringToolkit unescape(const StringToolkit& text);
	}
}

#endif
