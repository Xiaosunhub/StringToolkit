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

#ifndef __INC_CODING_H__
#define __INC_CODING_H__

#include "Twine.h"

namespace sp
{
	namespace coding
	{
		// Base64: standard alphabet with '=' padding.
		Twine toBase64(const Twine& data);
		Twine fromBase64(const Twine& text);   // throws Invalid_Conversion

		// Hex: two characters per byte, optional uppercase output.
		Twine toHex(const Twine& data, bool uppercase = false);
		Twine fromHex(const Twine& text);      // throws Invalid_Conversion

		// URL percent-encoding: unreserved characters (A-Z a-z 0-9 - _ . ~) are kept,
		// everything else becomes %XX (uppercase hex).
		Twine urlEncode(const Twine& data);
		Twine urlDecode(const Twine& text);    // accepts %XX and '+', throws Invalid_Conversion

		// Binary: 8 bits per byte, MSB first. fromBinary throws Invalid_Conversion
		// when the length is not a multiple of 8 or contains characters other than 0/1.
		Twine toBinary(const Twine& data);
		Twine fromBinary(const Twine& text);
		bool is_binary(const Twine& text) noexcept;

		// Octal: 3 octal digits per byte. fromOctal throws Invalid_Conversion
		// when the length is not a multiple of 3 or contains digits outside 0-7.
		Twine toOctal(const Twine& data);
		Twine fromOctal(const Twine& text);
		bool is_octal(const Twine& text) noexcept;

		// General integer radix (base 2-36, digits 0-9 then a-z).
		// toBase throws Invalid_Argument when base is out of range.
		// fromBase accepts an optional leading '-' and is case-insensitive;
		// throws Invalid_Conversion on bad digits or overflow.
		Twine toBase(unsigned long long value, unsigned base);
		long long fromBase(const Twine& text, unsigned base);
	}
}

#endif
