# cppString

A modern C++23 string utility library — a `std::string`-compatible core class
(`sp::cppString`) plus a collection of focused helper modules for UTF-8,
encoding, validation, paths, checksums, versioning, and everyday text work.

一个现代 C++23 字符串工具库：与 `std::string` 对齐的核心类 `sp::cppString`，
外加 UTF-8、编解码、校验、路径、哈希、语义化版本、文本处理等一系列独立模块。

## Features

### Core class — `sp::cppString` (`cppString.h`)

- Full `std::string` parity interface: `append` / `insert` / `erase` / `assign` /
  `resize` / `compare` / `rfind` / `find_first_of` / `find_first_not_of` / `substr` …
- Iterators (works with `std::ranges`), `operator<=>`, stream `<<` / `>>`,
  `std::hash` specialization, `std::formatter` support
- Rich extras: `split`, `join`, `trim`, `replace`, `toUpper` / `toLower`,
  `startsWith` / `endsWith` / `contains`, `format`, `concat`, `toNumber` / `fromNumber`,
  repetition (`operator*`), regex helpers
- Typed exception hierarchy (`String_Error`, `Invalid_Argument`,
  `Index_Out_Of_Range`, `Empty_String`, `Invalid_Conversion`, `Invalid_Operation`)

### `utf8.h` — `sp::utf8`

Strict validation (rejects truncation, overlongs, surrogates, > U+10FFFF),
codepoint-length, char-indexed substring, per-character split, codepoint
encode/decode, char-aware reverse, terminal `display_width` (CJK = 2 columns).

### `coding.h` — `sp::coding`

Base64 / Base32 / Base64URL (RFC 4648), hex, binary, octal, URL
percent-encoding, integer radix 2–36, ROT13 / Caesar cipher, Morse code.

### `method.h` — `sp::`

Everyday text tools: `join`, `split_lines`, `split_by_any`, `pad_left/right`,
`center`, `capitalize`, `title_case`, `swap_case`, `snake/kebab/camel/pascal
case`, `wildcard_match`, `truncate`, `slug`, `chunk`, `ngrams`, `is_palindrome`,
`soundex`, `levenshtein`, `hamming`, `similarity`, `dice_coefficient`,
`is_anagram`, `common_prefix/suffix`, `word_count`, `wrap`, `indent`, `dedent`,
`squeeze`, `count`, `expand_tabs`, `reverse_words`,
`thousands_sep`, `human_size`, `human_duration`, `to_roman` / `from_roman`,
`render` (`{{key}}` templates), `generate_random`, `uuid_v4`, `to_bool`,
case-insensitive `icontains` / `istarts_with` / `iends_with`,
`json::escape/unescape`, `csv::escape/split`, `html::escape/unescape`,
plus simple file I/O (`read_file` / `read_lines` / `write_file`).

### `path.h` — `sp::path`

Pure-string path handling (no filesystem access, normalized to `/`):
`join`, `normalize` (drive letters, UNC, `.`, `..`), `filename`, `stem`,
`extension`, `parent`, `is_absolute`, `relative`.

### `checksum.h` — `sp::checksum`

CRC-32 (compile-time table), FNV-1a 64, Adler-32, MD5 (legacy, cache keys
only) — value/bytes and hex forms.

### `validate.h` — `sp::validate`

`is_ipv4`, `is_ipv6` (with `::` compression and embedded IPv4 tails),
`is_email`, `is_uuid`, `is_url`, `is_mac_address`, `is_hex_color`,
`is_luhn`, `is_ascii`, `is_strong_password`.

### `semver.h` — `sp::semver`

Strict semver 2.0.0 validation and precedence comparison
(numeric identifiers, prerelease rules, build metadata ignored),
plus `bump_major` / `bump_minor` / `bump_patch`.

## Quick start

```cpp
#include "cppString.h"
#include "method.h"
#include "coding.h"
#include "utf8.h"

using namespace sp;

cppString s = "hello world";
s.toUpper();                       // "HELLO WORLD"

auto parts = cppString{"a,b,c"}.split(',');   // {"a", "b", "c"}

cppString enc = coding::toBase64Url("secret"); // "c2VjcmV0"
cppString id  = uuid_v4();          // "f47ac10b-58cc-4372-a567-0e02b2c3d479"

cppString cjk = "中文测试";
utf8::length(cjk);                 // 4 (characters, not bytes)
utf8::sub(cjk, 1, 2);              // "文测"
```

## Building

Requires a C++23 compiler (developed with MSVC v145 / VS 2026, `/std:c++latest`).
The library is plain `.h` / `.cpp` pairs — add them to your project and go.
No third-party dependencies; Windows targets link nothing extra.

To build and run the test suite (340+ assertions) from a VS developer prompt:

```
cl /std:c++latest /EHsc /W3 cppString.cpp method.cpp utf8.cpp coding.cpp ^
   path.cpp checksum.cpp validate.cpp semver.cpp testCppString.cpp
testCppString.exe
```

Or open `Project1.slnx` in Visual Studio and press F5.

## Repository layout

```
Project1/Project1/
├── cppString.h / .cpp   # core class
├── utf8.h / .cpp        # UTF-8 validation & manipulation, display width
├── coding.h / .cpp      # base64/base32/hex/binary/octal/url, radix, ciphers, morse
├── method.h / .cpp      # text utilities, file I/O, json/csv/html helpers
├── path.h / .cpp        # path manipulation
├── checksum.h / .cpp    # CRC-32, FNV-1a, Adler-32, MD5
├── validate.h / .cpp    # IP/email/UUID/URL/MAC/color/Luhn validation
├── semver.h / .cpp      # semantic versioning
└── testCppString.cpp    # 340+ assertion test suite
```

## License

[MIT](LICENSE) © 2025 Ender_
