#include "coding.h"

namespace
{
	const char* const BASE64_ALPHABET =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	int base64_value(unsigned char c)
	{
		if (c >= 'A' && c <= 'Z') return c - 'A';
		if (c >= 'a' && c <= 'z') return c - 'a' + 26;
		if (c >= '0' && c <= '9') return c - '0' + 52;
		if (c == '+') return 62;
		if (c == '/') return 63;
		return -1;
	}

	int hex_value(unsigned char c)
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return -1;
	}

	bool is_unreserved(unsigned char c)
	{
		return (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z')
			|| (c >= '0' && c <= '9')
			|| c == '-' || c == '_' || c == '.' || c == '~';
	}

	char hex_digit(unsigned int value, bool uppercase)
	{
		static const char* lower = "0123456789abcdef";
		static const char* upper = "0123456789ABCDEF";
		return uppercase ? upper[value & 0x0F] : lower[value & 0x0F];
	}
}

sp::StringToolkit sp::coding::toBase64(const StringToolkit& data)
{
	const String& src = data.s_str();

	StringToolkit result;
	result.reserve(((src.size() + 2) / 3) * 4);

	std::size_t i = 0;
	while (i + 2 < src.size())
	{
		const unsigned int block =
			(static_cast<unsigned char>(src[i]) << 16) |
			(static_cast<unsigned char>(src[i + 1]) << 8) |
			(static_cast<unsigned char>(src[i + 2]));

		result += BASE64_ALPHABET[(block >> 18) & 0x3F];
		result += BASE64_ALPHABET[(block >> 12) & 0x3F];
		result += BASE64_ALPHABET[(block >> 6) & 0x3F];
		result += BASE64_ALPHABET[block & 0x3F];

		i += 3;
	}

	const std::size_t rest = src.size() - i;

	if (rest == 1)
	{
		const unsigned int block = static_cast<unsigned char>(src[i]) << 16;

		result += BASE64_ALPHABET[(block >> 18) & 0x3F];
		result += BASE64_ALPHABET[(block >> 12) & 0x3F];
		result += '=';
		result += '=';
	}
	else if (rest == 2)
	{
		const unsigned int block =
			(static_cast<unsigned char>(src[i]) << 16) |
			(static_cast<unsigned char>(src[i + 1]) << 8);

		result += BASE64_ALPHABET[(block >> 18) & 0x3F];
		result += BASE64_ALPHABET[(block >> 12) & 0x3F];
		result += BASE64_ALPHABET[(block >> 6) & 0x3F];
		result += '=';
	}

	return result;
}

sp::StringToolkit sp::coding::fromBase64(const StringToolkit& text)
{
	const String& src = text.s_str();

	String bytes;
	bytes.reserve((src.size() / 4) * 3);

	unsigned int accumulator = 0;
	int bits = 0;

	for (char raw : src)
	{
		const unsigned char c = static_cast<unsigned char>(raw);

		if (std::isspace(c))
			continue;

		if (c == '=')
			break; // padding: stop decoding

		const int value = base64_value(c);
		if (value < 0)
			throw sp::Invalid_Conversion("Invalid base64 character in fromBase64.");

		accumulator = (accumulator << 6) | static_cast<unsigned int>(value);
		bits += 6;

		if (bits >= 8)
		{
			bits -= 8;
			bytes += static_cast<char>((accumulator >> bits) & 0xFF);
		}
	}

	return StringToolkit(bytes);
}

sp::StringToolkit sp::coding::toHex(const StringToolkit& data, bool uppercase)
{
	const String& src = data.s_str();

	StringToolkit result;
	result.reserve(src.size() * 2);

	for (char raw : src)
	{
		const unsigned char c = static_cast<unsigned char>(raw);
		result += hex_digit(c >> 4, uppercase);
		result += hex_digit(c, uppercase);
	}

	return result;
}

sp::StringToolkit sp::coding::fromHex(const StringToolkit& text)
{
	const String& src = text.s_str();

	String bytes;
	bytes.reserve(src.size() / 2);

	int high = -1;

	for (char raw : src)
	{
		const unsigned char c = static_cast<unsigned char>(raw);

		if (std::isspace(c))
			continue;

		const int value = hex_value(c);
		if (value < 0)
			throw sp::Invalid_Conversion("Invalid hex character in fromHex.");

		if (high < 0)
		{
			high = value;
		}
		else
		{
			bytes += static_cast<char>((high << 4) | value);
			high = -1;
		}
	}

	if (high >= 0)
		throw sp::Invalid_Conversion("Odd number of hex digits in fromHex.");

	return StringToolkit(bytes);
}

sp::StringToolkit sp::coding::urlEncode(const StringToolkit& data)
{
	const String& src = data.s_str();

	StringToolkit result;
	result.reserve(src.size());

	for (char raw : src)
	{
		const unsigned char c = static_cast<unsigned char>(raw);

		if (is_unreserved(c))
		{
			result += static_cast<char>(c);
		}
		else
		{
			result += '%';
			result += hex_digit(c >> 4, true);
			result += hex_digit(c, true);
		}
	}

	return result;
}

sp::StringToolkit sp::coding::urlDecode(const StringToolkit& text)
{
	const String& src = text.s_str();

	String bytes;
	bytes.reserve(src.size());

	for (std::size_t i = 0; i < src.size(); ++i)
	{
		const unsigned char c = static_cast<unsigned char>(src[i]);

		if (c == '+')
		{
			bytes += ' ';
		}
		else if (c == '%')
		{
			if (i + 2 >= src.size())
				throw sp::Invalid_Conversion("Truncated percent sequence in urlDecode.");

			const int high = hex_value(static_cast<unsigned char>(src[i + 1]));
			const int low = hex_value(static_cast<unsigned char>(src[i + 2]));

			if (high < 0 || low < 0)
				throw sp::Invalid_Conversion("Invalid percent sequence in urlDecode.");

			bytes += static_cast<char>((high << 4) | low);
			i += 2;
		}
		else
		{
			bytes += static_cast<char>(c);
		}
	}

	return StringToolkit(bytes);
}

namespace
{
	bool all_digits_in(const String& src, const char* digits)
	{
		return !src.empty() && src.find_first_not_of(digits) == String::npos;
	}
}

bool sp::coding::is_binary(const StringToolkit& text) noexcept
{
	return all_digits_in(text.s_str(), "01");
}

sp::StringToolkit sp::coding::toBinary(const StringToolkit& data)
{
	String bits;
	bits.reserve(data.size() * 8);

	for (const char c : data.s_str())
	{
		const unsigned char byte = static_cast<unsigned char>(c);

		for (int shift = 7; shift >= 0; --shift)
			bits += static_cast<char>('0' + ((byte >> shift) & 1));
	}

	return StringToolkit(std::move(bits));
}

sp::StringToolkit sp::coding::fromBinary(const StringToolkit& text)
{
	const String& src = text.s_str();

	if (src.empty() || src.size() % 8 != 0)
		throw sp::Invalid_Conversion("Binary text length must be a non-zero multiple of 8 in fromBinary.");

	if (!all_digits_in(src, "01"))
		throw sp::Invalid_Conversion("Binary text may only contain '0' and '1' in fromBinary.");

	String bytes;
	bytes.reserve(src.size() / 8);

	for (std::size_t i = 0; i < src.size(); i += 8)
	{
		unsigned char byte = 0;

		for (std::size_t j = 0; j < 8; ++j)
			byte = static_cast<unsigned char>((byte << 1) | (src[i + j] - '0'));

		bytes += static_cast<char>(byte);
	}

	return StringToolkit(std::move(bytes));
}

bool sp::coding::is_octal(const StringToolkit& text) noexcept
{
	return all_digits_in(text.s_str(), "01234567");
}

sp::StringToolkit sp::coding::toOctal(const StringToolkit& data)
{
	String digits;
	digits.reserve(data.size() * 3);

	for (const char c : data.s_str())
	{
		const unsigned char byte = static_cast<unsigned char>(c);

		digits += static_cast<char>('0' + ((byte >> 6) & 7));
		digits += static_cast<char>('0' + ((byte >> 3) & 7));
		digits += static_cast<char>('0' + (byte & 7));
	}

	return StringToolkit(std::move(digits));
}

sp::StringToolkit sp::coding::fromOctal(const StringToolkit& text)
{
	const String& src = text.s_str();

	if (src.empty() || src.size() % 3 != 0)
		throw sp::Invalid_Conversion("Octal text length must be a non-zero multiple of 3 in fromOctal.");

	if (!all_digits_in(src, "01234567"))
		throw sp::Invalid_Conversion("Octal text may only contain digits 0-7 in fromOctal.");

	String bytes;
	bytes.reserve(src.size() / 3);

	for (std::size_t i = 0; i < src.size(); i += 3)
	{
		const unsigned byte = static_cast<unsigned>(
			(src[i] - '0') * 64 + (src[i + 1] - '0') * 8 + (src[i + 2] - '0'));

		bytes += static_cast<char>(byte);
	}

	return StringToolkit(std::move(bytes));
}

sp::StringToolkit sp::coding::toBase(unsigned long long value, unsigned base)
{
	if (base < 2 || base > 36)
		throw sp::Invalid_Argument("Base must be between 2 and 36 in toBase.");

	if (value == 0)
		return StringToolkit{ "0" };

	static constexpr char DIGITS[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	String result;

	while (value > 0)
	{
		result += DIGITS[value % base];
		value /= base;
	}

	std::ranges::reverse(result);

	return StringToolkit(std::move(result));
}

long long sp::coding::fromBase(const StringToolkit& text, unsigned base)
{
	if (base < 2 || base > 36)
		throw sp::Invalid_Argument("Base must be between 2 and 36 in fromBase.");

	String src = text.s_str();

	if (src.empty())
		throw sp::Invalid_Conversion("Cannot convert empty string in fromBase.");

	bool negative = false;
	std::size_t i = 0;

	if (src[0] == '-' || src[0] == '+')
	{
		negative = (src[0] == '-');
		i = 1;

		if (src.size() == 1)
			throw sp::Invalid_Conversion("Sign-only string in fromBase.");
	}

	unsigned long long value = 0;

	for (; i < src.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(src[i]);
		unsigned digit;

		if (c >= '0' && c <= '9')      digit = static_cast<unsigned>(c - '0');
		else if (c >= 'a' && c <= 'z') digit = static_cast<unsigned>(c - 'a' + 10);
		else if (c >= 'A' && c <= 'Z') digit = static_cast<unsigned>(c - 'A' + 10);
		else
			throw sp::Invalid_Conversion("Invalid character in fromBase.");

		if (digit >= base)
			throw sp::Invalid_Conversion("Digit out of range for the given base in fromBase.");

		const unsigned long long next = value * base + digit;

		if (next / base != value || next < value)
			throw sp::Invalid_Conversion("Overflow in fromBase.");

		value = next;
	}

	constexpr unsigned long long ULL_MAX_AS_LL =
		static_cast<unsigned long long>(std::numeric_limits<long long>::max());

	if (value > (negative ? ULL_MAX_AS_LL + 1ULL : ULL_MAX_AS_LL))
		throw sp::Invalid_Conversion("Overflow in fromBase.");

	if (negative)
		return static_cast<long long>(0ULL - value);

	return static_cast<long long>(value);
}

namespace
{
	constexpr char BASE64URL_ALPHABET[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

	int base64url_value(unsigned char c)
	{
		if (c >= 'A' && c <= 'Z') return c - 'A';
		if (c >= 'a' && c <= 'z') return c - 'a' + 26;
		if (c >= '0' && c <= '9') return c - '0' + 52;
		if (c == '-')              return 62;
		if (c == '_')              return 63;
		return -1;
	}
}

sp::StringToolkit sp::coding::toBase64Url(const StringToolkit& data)
{
	const String& src = data.s_str();

	StringToolkit result;
	result.reserve((src.size() * 8 + 5) / 6);

	std::size_t i = 0;
	while (i + 2 < src.size())
	{
		const unsigned block =
			(static_cast<unsigned char>(src[i]) << 16) |
			(static_cast<unsigned char>(src[i + 1]) << 8) |
			(static_cast<unsigned char>(src[i + 2]));

		result += BASE64URL_ALPHABET[(block >> 18) & 0x3F];
		result += BASE64URL_ALPHABET[(block >> 12) & 0x3F];
		result += BASE64URL_ALPHABET[(block >> 6) & 0x3F];
		result += BASE64URL_ALPHABET[block & 0x3F];

		i += 3;
	}

	const std::size_t rest = src.size() - i;

	if (rest == 1)
	{
		const unsigned block = static_cast<unsigned char>(src[i]) << 16;

		result += BASE64URL_ALPHABET[(block >> 18) & 0x3F];
		result += BASE64URL_ALPHABET[(block >> 12) & 0x3F];
	}
	else if (rest == 2)
	{
		const unsigned block =
			(static_cast<unsigned char>(src[i]) << 16) |
			(static_cast<unsigned char>(src[i + 1]) << 8);

		result += BASE64URL_ALPHABET[(block >> 18) & 0x3F];
		result += BASE64URL_ALPHABET[(block >> 12) & 0x3F];
		result += BASE64URL_ALPHABET[(block >> 6) & 0x3F];
	}

	return result;
}

sp::StringToolkit sp::coding::fromBase64Url(const StringToolkit& text)
{
	String src = text.s_str();

	// Tolerate optional padding, then validate the length class.
	while (!src.empty() && src.back() == '=')
		src.pop_back();

	if (src.size() % 4 == 1)
		throw sp::Invalid_Conversion("Invalid base64url length in fromBase64Url.");

	StringToolkit result;
	result.reserve((src.size() * 6) / 8);

	unsigned accumulator = 0;
	int bits = 0;

	for (const char ch : src)
	{
		const int value = base64url_value(static_cast<unsigned char>(ch));

		if (value < 0)
			throw sp::Invalid_Conversion("Invalid character in base64url text in fromBase64Url.");

		accumulator = (accumulator << 6) | static_cast<unsigned>(value);
		bits += 6;

		if (bits >= 8)
		{
			bits -= 8;
			result += static_cast<char>((accumulator >> bits) & 0xFF);
		}
	}

	return result;
}
