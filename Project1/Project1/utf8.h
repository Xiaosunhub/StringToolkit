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

#ifndef __INC_UTF8_H__
#define __INC_UTF8_H__

#include "Twine.h"

namespace sp
{
	namespace utf8
	{
		// Strict UTF-8 validation (rejects truncated, overlong, surrogate and > U+10FFFF sequences).
		bool is_valid(const Twine& s);

		// Number of code points (characters), not bytes.
		std::size_t length(const Twine& s);

		// Byte offset of the char_index-th character. Returns Twine::npos when out of range.
		std::size_t offset(const Twine& s, std::size_t char_index);

		// Substring by character index. Throws Empty_String on empty input, Index_Out_Of_Range on bad range.
		Twine sub(const Twine& s, std::size_t start);
		Twine sub(const Twine& s, std::size_t start, std::size_t length);

		// Split into characters.
		std::vector<Twine> chars(const Twine& s);

		// Decode to code points. Throws Invalid_Conversion on malformed input.
		std::vector<char32_t> codepoints(const Twine& s);

		// Encode one code point. Throws Invalid_Argument on surrogate or out-of-range value.
		Twine from_codepoint(char32_t cp);

		// Reverse by character (safe for multi-byte UTF-8).
		Twine reverse(const Twine& s);
	}
}

#endif
