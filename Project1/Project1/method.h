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

#include "Twine.h"

namespace sp
{
	void reverse_string(Twine& str);

	void reverse_string_to(Twine& str, Twine& result);
	void reverse_string_to(Twine&& str, Twine& result) noexcept;

	void copy_to(Twine& str, Twine& result);
	void copy_to(Twine&& str, Twine& result) noexcept;

	void move_to(Twine& str, Twine& result);
	void move_to(Twine&& str, Twine& result) noexcept;

	void fill_with(Twine& str, char ch);

	void fill(Twine& str, char ch, std::size_t count);

	void remove_prefix(Twine& str, const Twine& prefix);
	void remove_suffix(Twine& str, const Twine& suffix);

	// ---- text utilities ----

	// Concatenate parts with a separator between them.
	Twine join(const std::vector<Twine>& parts, const Twine& separator);
	Twine join(const std::vector<Twine>& parts, char separator);

	// Split on '\n', "\r\n" or '\r'. A trailing newline does not produce an empty last line.
	std::vector<Twine> split_lines(const Twine& str);

	// Pad to width characters (UTF-8 aware). No-op when already wide enough.
	void pad_left(Twine& str, std::size_t width, char fill = ' ');
	void pad_right(Twine& str, std::size_t width, char fill = ' ');

	// Uppercase the first ASCII letter and lowercase the rest.
	void capitalize(Twine& str);

	// Swap ASCII letter case.
	void swap_case(Twine& str);

	// Replace the first occurrence. Throws Invalid_Argument when from is empty.
	void replace_first(Twine& str, const Twine& from, const Twine& to);

	// ---- file I/O ----

	// Read the whole file as bytes. Throws Invalid_Operation when the file cannot be opened.
	Twine read_file(const String& path);
	std::vector<Twine> read_lines(const String& path);

	// Write (or append) bytes to a file. Throws Invalid_Operation when the file cannot be opened.
	void write_file(const String& path, const Twine& data, bool append = false);

	// ---- random ----

	// Random string of the given length drawn from charset
	// (default: A-Z a-z 0-9). Throws Invalid_Argument when charset is empty.
	Twine generate_random(std::size_t length, const Twine& charset =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

	// ---- case-insensitive conveniences (ASCII only) ----

	bool icontains(const Twine& str, const Twine& sub);
	bool istarts_with(const Twine& str, const Twine& prefix);
	bool iends_with(const Twine& str, const Twine& suffix);

	// ---- splitting ----

	// Split on every occurrence of ANY character in delimiters.
	// Consecutive delimiters produce empty fields; a trailing delimiter produces no trailing empty field.
	std::vector<Twine> split_by_any(const Twine& str, const Twine& delimiters);

	// ---- pattern matching ----

	// Shell-style wildcard match: '*' matches any run (including empty), '?' matches exactly one char.
	// All other characters must match literally.
	bool wildcard_match(const Twine& str, const Twine& pattern);

	// ---- case style conversions ----

	// "HelloWorld" / "hello-world" / "Hello World" -> "hello_world"
	void to_snake_case(Twine& str);
	// "HelloWorld" / "hello_world" / "Hello World" -> "hello-world"
	void to_kebab_case(Twine& str);
	// "hello_world" / "hello-world" / "Hello World" -> "helloWorld"
	void to_camel_case(Twine& str);

	// ---- JSON ----

	namespace json
	{
		// Escape for embedding in a JSON string literal: " \ \b \f \n \r \t
		// and control characters below 0x20 (as \u00XX). Other bytes pass through unchanged.
		Twine escape(const Twine& text);

		// Inverse of escape. Handles \uXXXX including UTF-16 surrogate pairs.
		// Throws Invalid_Conversion on malformed escape sequences.
		Twine unescape(const Twine& text);
	}

	// ---- CSV ----

	namespace csv
	{
		// Quote and escape a single field per RFC 4180 when it contains , " CR or LF.
		Twine escape(const Twine& field);

		// Parse one CSV line: quoted fields may contain commas and doubled quotes.
		// Throws Invalid_Conversion on an unterminated quote.
		std::vector<Twine> split(const Twine& line);
	}

	// ---- statistics ----

	// Number of whitespace-separated words. A run of CJK characters counts as one word
	// because no space is required between them.
	std::size_t word_count(const Twine& str);

	// Levenshtein edit distance measured in characters (UTF-8 aware).
	std::size_t levenshtein(const Twine& a, const Twine& b);

	// 1.0 for identical strings, 0.0 for completely different ones.
	// Empty vs empty is 1.0; empty vs non-empty is 0.0.
	double similarity(const Twine& a, const Twine& b);

	// Longest common prefix of all parts, cut on a UTF-8 boundary. Empty input -> "".
	Twine longest_common_prefix(const std::vector<Twine>& parts);

	// ---- layout ----

	// Greedy word wrap at width characters (UTF-8 aware). Existing newlines are kept;
	// words longer than the width are left intact on their own line.
	std::vector<Twine> wrap(const Twine& str, std::size_t width);

	// Prefix every non-empty line with prefix.
	void indent(Twine& str, const Twine& prefix);

	// Group the integer part with sep: 1234567 -> "1,234,567". Keeps a leading '-'.
	Twine thousands_sep(long long value, char sep = ',');

	// Human readable byte size: 512 -> "512 B", 1536 -> "1.5 KB", 1048576 -> "1 MB".
	Twine human_size(unsigned long long bytes);

	// Replace every "{{key}}" in tmpl using vars. Unknown placeholders are left untouched.
	Twine render(const Twine& tmpl, const std::map<Twine, Twine>& vars);

	// Split at the first (or last) occurrence of delimiter.
	// When the delimiter is absent the whole string goes to first and second is empty.
	std::pair<Twine, Twine> split_pair(const Twine& str,
		const Twine& delimiter, bool first = true);

	// ---- convenience ----

	// Cut the string to at most max_chars characters (UTF-8 aware), appending
	// ellipsis when anything was removed. The ellipsis is included in max_chars.
	void truncate(Twine& str, std::size_t max_chars, const Twine& ellipsis = "...");

	// URL-friendly slug: lowercase, spaces/underscores -> '-', drop everything
	// outside [a-z0-9-], collapse repeated '-', trim leading/trailing '-'.
	void slug(Twine& str);

	// Split into chunks of size characters (UTF-8 aware; the last chunk may be shorter).
	// Throws Invalid_Argument when size is zero.
	std::vector<Twine> chunk(const Twine& str, std::size_t size);

	// Parse a boolean: "true/1/yes/on/y/t" and "false/0/no/off/n/f", case-insensitive.
	// Throws Invalid_Conversion on anything else.
	bool to_bool(const Twine& str);
}

#endif
