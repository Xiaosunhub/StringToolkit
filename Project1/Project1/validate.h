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

#ifndef __INC_VALIDATE_H__
#define __INC_VALIDATE_H__

#include "cppString.h"

namespace sp
{
	// Practical validators: strict on format, not on whether the address actually exists.
	namespace validate
	{
		// Dotted-quad IPv4. Leading zeros are rejected ("01.2.3.4" is invalid).
		bool is_ipv4(const cppString& s) noexcept;

		// IPv6 including "::" compression and embedded IPv4 tails ("::ffff:1.2.3.4").
		// Zone ids ("%eth0") and brackets ("[::1]") are NOT accepted.
		bool is_ipv6(const cppString& s) noexcept;

		// Pragmatic e-mail check: one '@', sane local part, dotted domain labels.
		bool is_email(const cppString& s) noexcept;

		// 8-4-4-4-12 hex groups, case-insensitive: "550e8400-e29b-41d4-a716-446655440000".
		bool is_uuid(const cppString& s) noexcept;

		// "scheme://rest" with a valid scheme (letter, then letters/digits/+/-/.)
		// and no whitespace in the remainder.
		bool is_url(const cppString& s) noexcept;

		// Six pairs of hex digits joined by ':' or '-' (the separator must be
		// consistent): "01:23:45:67:89:ab" or "01-23-45-67-89-AB".
		bool is_mac_address(const cppString& s) noexcept;

		// CSS-style hex color: '#' followed by 3, 4, 6 or 8 hex digits
		// (#RGB, #RGBA, #RRGGBB, #RRGGBBAA).
		bool is_hex_color(const cppString& s) noexcept;

		// Digits passing the Luhn checksum (bank cards, IMEI). At least two
		// digits; spaces and dashes are ignored.
		bool is_luhn(const cppString& s) noexcept;

		// True when every byte is below 0x80 (empty string included).
		bool is_ascii(const cppString& s) noexcept;

		// At least 8 characters containing a lowercase letter, an uppercase
		// letter, a digit and a non-alphanumeric symbol (all ASCII).
		bool is_strong_password(const cppString& s) noexcept;
	}
}

#endif
