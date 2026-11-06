#include "validate.h"

namespace
{
	bool is_hex_digit(char c) noexcept
	{
		return std::isxdigit(static_cast<unsigned char>(c)) != 0;
	}

	// Parses "a.b.c.d" strictly. Returns false on any deviation.
	bool parse_ipv4(const String& s) noexcept
	{
		if (s.empty())
			return false;

		std::size_t start = 0;
		int groups = 0;

		while (true)
		{
			const std::size_t dot = s.find('.', start);
			const String group = s.substr(start,
				dot == String::npos ? String::npos : dot - start);

			if (group.empty() || group.size() > 3)
				return false;

			if (group.size() > 1 && group[0] == '0') // no leading zeros
				return false;

			for (const char c : group)
			{
				if (!std::isdigit(static_cast<unsigned char>(c)))
					return false;
			}

			const int value = std::stoi(group);

			if (value > 255)
				return false;

			++groups;

			if (dot == String::npos)
				break;

			start = dot + 1;
		}

		return groups == 4;
	}
}

bool sp::validate::is_ipv4(const cppString& s) noexcept
{
	return parse_ipv4(s.s_str());
}

bool sp::validate::is_ipv6(const cppString& s) noexcept
{
	const String& src = s.s_str();

	if (src.empty() || src.size() > 45)
		return false;

	std::vector<String> groups;
	std::size_t pos = 0;
	bool compressed = false;

	// A leading "::" starts compressed mode.
	if (src.starts_with("::"))
	{
		compressed = true;
		pos = 2;

		if (pos == src.size()) // just "::"
			return true;
	}

	while (pos < src.size())
	{
		// Read one group: hex digits, optionally ending in an IPv4 tail.
		std::size_t end = pos;

		while (end < src.size() && src[end] != ':')
			++end;

		const String group = src.substr(pos, end - pos);

		if (group.find('.') != String::npos) // embedded IPv4 tail, must be last
		{
			if (end != src.size() || !parse_ipv4(group))
				return false;

			groups.push_back("x");
			groups.push_back("x"); // an IPv4 tail counts as two 16-bit groups
		}
		else
		{
			if (group.empty() || group.size() > 4)
				return false;

			for (const char c : group)
			{
				if (!is_hex_digit(c))
					return false;
			}

			groups.push_back(group);
		}

		if (end == src.size())
			break;

		// src[end] is the ':' that terminated this group.
		if (end + 1 >= src.size())
			return false; // trailing ':'

		if (src[end + 1] == ':') // "::" compression, legal at most once
		{
			if (compressed)
				return false;

			compressed = true;
			pos = end + 2;
		}
		else
		{
			pos = end + 1;
		}
	}

	// With "::" compression, fewer than 8 groups is fine; without, exactly 8.
	return compressed ? groups.size() < 8 : groups.size() == 8;
}

bool sp::validate::is_email(const cppString& s) noexcept
{
	const String& src = s.s_str();
	const std::size_t at = src.find('@');

	if (at == String::npos || at == 0 || at + 1 >= src.size())
		return false;

	if (src.find('@', at + 1) != String::npos)
		return false; // exactly one '@'

	const String local = src.substr(0, at);
	const String domain = src.substr(at + 1);

	if (local.size() > 64)
		return false;

	// Local part: no spaces, no control characters, not starting/ending with '.',
	// no consecutive dots. Everything else is tolerated on purpose.
	if (local.front() == '.' || local.back() == '.' || local.find("..") != String::npos)
		return false;

	for (const char c : local)
	{
		const unsigned char uc = static_cast<unsigned char>(c);
		if (uc <= 0x20 || uc == 0x7F)
			return false;
	}

	if (domain.find("..") != String::npos || domain.front() == '.' || domain.back() == '.')
		return false;

	if (domain.find('.') == String::npos)
		return false; // require at least one dot: "user@localhost" is not an e-mail here

	for (const char c : domain)
	{
		const unsigned char uc = static_cast<unsigned char>(c);
		if (uc <= 0x20 || uc == 0x7F || c == '(' || c == ')' || c == ',' || c == ':'
			|| c == ';' || c == '<' || c == '>' || c == '[' || c == ']' || c == '\\')
			return false;
	}

	return true;
}

bool sp::validate::is_uuid(const cppString& s) noexcept
{
	const String& src = s.s_str();

	if (src.size() != 36)
		return false;

	for (std::size_t i = 0; i < 36; ++i)
	{
		const bool hyphen = (i == 8 || i == 13 || i == 18 || i == 23);

		if (hyphen)
		{
			if (src[i] != '-')
				return false;
		}
		else if (!is_hex_digit(src[i]))
		{
			return false;
		}
	}

	return true;
}

bool sp::validate::is_url(const cppString& s) noexcept
{
	const String& src = s.s_str();
	const std::size_t scheme_end = src.find("://");

	if (scheme_end == String::npos || scheme_end == 0)
		return false;

	if (!std::isalpha(static_cast<unsigned char>(src[0])))
		return false;

	for (std::size_t i = 1; i < scheme_end; ++i)
	{
		const char c = src[i];

		if (!std::isalnum(static_cast<unsigned char>(c))
			&& c != '+' && c != '-' && c != '.')
		{
			return false;
		}
	}

	if (scheme_end + 3 >= src.size())
		return false; // nothing after "://"

	for (std::size_t i = scheme_end + 3; i < src.size(); ++i)
	{
		const unsigned char uc = static_cast<unsigned char>(src[i]);
		if (uc <= 0x20 || uc == 0x7F)
			return false;
	}

	return true;
}

bool sp::validate::is_mac_address(const cppString& s) noexcept
{
	const String& src = s.s_str();

	if (src.size() != 17)
		return false;

	const char sep = src[2];

	if (sep != ':' && sep != '-')
		return false;

	for (std::size_t i = 0; i < src.size(); ++i)
	{
		if (i % 3 == 2)
		{
			if (src[i] != sep)
				return false;
		}
		else if (!is_hex_digit(src[i]))
		{
			return false;
		}
	}

	return true;
}

bool sp::validate::is_hex_color(const cppString& s) noexcept
{
	const String& src = s.s_str();

	if (src.empty() || src[0] != '#')
		return false;

	const std::size_t digits = src.size() - 1;

	if (digits != 3 && digits != 4 && digits != 6 && digits != 8)
		return false;

	for (std::size_t i = 1; i < src.size(); ++i)
	{
		if (!is_hex_digit(src[i]))
			return false;
	}

	return true;
}

bool sp::validate::is_luhn(const cppString& s) noexcept
{
	const String& src = s.s_str();

	int sum = 0;
	int digits = 0;
	bool double_digit = false;

	// Walk right to left, skipping spaces and dashes.
	for (std::size_t i = src.size(); i-- > 0;)
	{
		const char c = src[i];

		if (c == ' ' || c == '-')
			continue;

		if (c < '0' || c > '9')
			return false;

		int d = c - '0';

		if (double_digit)
		{
			d *= 2;

			if (d > 9)
				d -= 9;
		}

		sum += d;
		++digits;
		double_digit = !double_digit;
	}

	return digits >= 2 && sum % 10 == 0;
}

bool sp::validate::is_ascii(const cppString& s) noexcept
{
	for (const char c : s.s_str())
	{
		if (static_cast<unsigned char>(c) >= 0x80)
			return false;
	}

	return true;
}

bool sp::validate::is_strong_password(const cppString& s) noexcept
{
	const String& src = s.s_str();

	if (src.size() < 8)
		return false;

	bool lower = false, upper = false, digit = false, symbol = false;

	for (const char c : src)
	{
		const unsigned char uc = static_cast<unsigned char>(c);

		if (uc >= 0x80)
			return false;

		if (c >= 'a' && c <= 'z')      lower = true;
		else if (c >= 'A' && c <= 'Z') upper = true;
		else if (c >= '0' && c <= '9') digit = true;
		else                           symbol = true;
	}

	return lower && upper && digit && symbol;
}
