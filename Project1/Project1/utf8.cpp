#include "utf8.h"
#include <algorithm>

namespace
{
	std::size_t sequence_length(unsigned char lead)
	{
		if ((lead & 0x80) == 0x00) return 1;
		if ((lead & 0xE0) == 0xC0) return 2;
		if ((lead & 0xF0) == 0xE0) return 3;
		if ((lead & 0xF8) == 0xF0) return 4;
		return 1; // invalid lead byte, treated as a single byte
	}
}

bool sp::utf8::is_valid(const StringToolkit& s)
{
	const auto* bytes = reinterpret_cast<const unsigned char*>(s.data());
	const std::size_t len = s.size();

	std::size_t i = 0;
	while (i < len)
	{
		const unsigned char lead = bytes[i];

		if (lead <= 0x7F)
		{
			++i;
			continue;
		}

		std::size_t extra = 0;
		unsigned char second_min = 0x80;
		unsigned char second_max = 0xBF;

		if (lead >= 0xC2 && lead <= 0xDF)      extra = 1;
		else if (lead == 0xE0)                 { extra = 2; second_min = 0xA0; }
		else if (lead >= 0xE1 && lead <= 0xEC) extra = 2;
		else if (lead == 0xED)                 { extra = 2; second_max = 0x9F; }
		else if (lead >= 0xEE && lead <= 0xEF) extra = 2;
		else if (lead == 0xF0)                 { extra = 3; second_min = 0x90; }
		else if (lead >= 0xF1 && lead <= 0xF3) extra = 3;
		else if (lead == 0xF4)                 { extra = 3; second_max = 0x8F; }
		else return false;

		if (i + extra >= len)
			return false;

		if (bytes[i + 1] < second_min || bytes[i + 1] > second_max)
			return false;

		for (std::size_t j = 2; j <= extra; ++j)
		{
			if ((bytes[i + j] & 0xC0) != 0x80)
				return false;
		}

		i += extra + 1;
	}

	return true;
}

std::size_t sp::utf8::length(const StringToolkit& s)
{
	const auto* bytes = reinterpret_cast<const unsigned char*>(s.data());
	const std::size_t len = s.size();

	std::size_t count = 0;
	for (std::size_t i = 0; i < len; ++i)
	{
		if ((bytes[i] & 0xC0) != 0x80) // not a continuation byte
			++count;
	}

	return count;
}

std::size_t sp::utf8::offset(const StringToolkit& s, std::size_t char_index)
{
	const auto* bytes = reinterpret_cast<const unsigned char*>(s.data());
	const std::size_t len = s.size();

	std::size_t i = 0;
	std::size_t count = 0;

	while (i < len)
	{
		if (count == char_index)
			return i;

		i += sequence_length(bytes[i]);
		++count;
	}

	return (count == char_index) ? len : StringToolkit::npos;
}

sp::StringToolkit sp::utf8::sub(const StringToolkit& s, std::size_t start)
{
	if (s.is_empty())
		throw sp::Empty_String("Cannot call utf8::sub on an empty string.");

	const std::size_t count = length(s);

	if (start >= count)
		throw sp::Index_Out_Of_Range("Start index out of range in utf8::sub.");

	if (start == 0)
		return s;

	return StringToolkit(s.s_str().substr(offset(s, start)));
}

sp::StringToolkit sp::utf8::sub(const StringToolkit& s, std::size_t start, std::size_t len)
{
	if (s.is_empty())
		throw sp::Empty_String("Cannot call utf8::sub on an empty string.");

	const std::size_t count = length(s);

	if (start >= count)
		throw sp::Index_Out_Of_Range("Start index out of range in utf8::sub.");

	if (len == 0)
		return StringToolkit{};

	if (start + len > count)
		throw sp::Index_Out_Of_Range("Length exceeds string bounds in utf8::sub.");

	const std::size_t start_byte = offset(s, start);
	const std::size_t end_byte = offset(s, start + len);

	return StringToolkit(s.s_str().substr(start_byte, end_byte - start_byte));
}

std::vector<sp::StringToolkit> sp::utf8::chars(const StringToolkit& s)
{
	std::vector<StringToolkit> result;

	const String& str = s.s_str();
	std::size_t i = 0;

	while (i < str.size())
	{
		std::size_t step = sequence_length(static_cast<unsigned char>(str[i]));
		if (i + step > str.size())
			step = 1; // truncated tail: keep the byte as-is

		result.emplace_back(str.substr(i, step));
		i += step;
	}

	return result;
}

std::vector<char32_t> sp::utf8::codepoints(const StringToolkit& s)
{
	std::vector<char32_t> result;

	const String& str = s.s_str();
	std::size_t i = 0;

	while (i < str.size())
	{
		const unsigned char lead = static_cast<unsigned char>(str[i]);
		const std::size_t step = sequence_length(lead);

		if (step == 1 && lead > 0x7F)
			throw sp::Invalid_Conversion("Invalid UTF-8 lead byte in utf8::codepoints.");

		if (i + step > str.size())
			throw sp::Invalid_Conversion("Truncated UTF-8 sequence in utf8::codepoints.");

		char32_t cp = 0;
		switch (step)
		{
		case 1: cp = lead;         break;
		case 2: cp = lead & 0x1F;  break;
		case 3: cp = lead & 0x0F;  break;
		default: cp = lead & 0x07; break;
		}

		for (std::size_t j = 1; j < step; ++j)
		{
			const unsigned char c = static_cast<unsigned char>(str[i + j]);
			if ((c & 0xC0) != 0x80)
				throw sp::Invalid_Conversion("Invalid UTF-8 continuation byte in utf8::codepoints.");

			cp = (cp << 6) | (c & 0x3F);
		}

		result.push_back(cp);
		i += step;
	}

	return result;
}

sp::StringToolkit sp::utf8::from_codepoint(char32_t cp)
{
	String out;

	if (cp <= 0x7F)
	{
		out += static_cast<char>(cp);
	}
	else if (cp <= 0x7FF)
	{
		out += static_cast<char>(0xC0 | (cp >> 6));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
	else if (cp <= 0xFFFF)
	{
		if (cp >= 0xD800 && cp <= 0xDFFF)
			throw sp::Invalid_Argument("Surrogate code points cannot be encoded.");

		out += static_cast<char>(0xE0 | (cp >> 12));
		out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
	else if (cp <= 0x10FFFF)
	{
		out += static_cast<char>(0xF0 | (cp >> 18));
		out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
		out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
	else
	{
		throw sp::Invalid_Argument("Code point out of Unicode range.");
	}

	return StringToolkit(out);
}

sp::StringToolkit sp::utf8::reverse(const StringToolkit& s)
{
	auto parts = chars(s);
	std::reverse(parts.begin(), parts.end());

	String out;
	for (const auto& part : parts)
		out += part.s_str();

	return StringToolkit(out);
}
