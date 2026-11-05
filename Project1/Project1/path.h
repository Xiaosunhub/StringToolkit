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

#ifndef __INC_PATH_H__
#define __INC_PATH_H__

#include "Twine.h"

namespace sp
{
	// Pure string-level path handling: no filesystem access, no throw on missing files.
	// Accepts both '/' and '\\' as separators; every result uses '/'.
	namespace path
	{
		// Join base and child. An absolute child replaces base entirely.
		Twine join(const Twine& base, const Twine& child);

		// Collapse duplicate separators and resolve "." / ".." segments.
		// Keeps a drive prefix ("C:"), a leading "/" or a UNC "//server" prefix.
		// Returns "." for an empty path.
		Twine normalize(const Twine& p);

		// Last component: "a/b/c.txt" -> "c.txt"; a trailing separator is ignored.
		Twine filename(const Twine& p);

		// Filename without its last extension: "a/b/c.tar.gz" -> "c.tar";
		// a leading dot is not an extension (".gitignore" -> ".gitignore").
		Twine stem(const Twine& p);

		// Last extension including the dot, or "" when there is none.
		Twine extension(const Twine& p);

		// Directory part: "a/b/c.txt" -> "a/b"; "/a" -> "/"; "a" -> ".";
		Twine parent(const Twine& p);

		// True for "/x", "\\x", "C:/x", "C:x" and "//server/share".
		bool is_absolute(const Twine& p) noexcept;
	}
}

#endif
