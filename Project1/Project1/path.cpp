#include "path.h"

namespace
{
	bool is_sep(char c) noexcept
	{
		return c == '/' || c == '\\';
	}

	bool is_drive_prefix(const String& s) noexcept
	{
		return s.size() >= 2
			&& std::isalpha(static_cast<unsigned char>(s[0]))
			&& s[1] == ':';
	}

	String slashify(const String& src)
	{
		String s = src;

		for (auto& c : s)
		{
			if (c == '\\')
				c = '/';
		}

		return s;
	}
}

bool sp::path::is_absolute(const Twine& p) noexcept
{
	const String& s = p.s_str();

	if (s.empty())
		return false;

	if (is_sep(s[0]))
		return true;

	return is_drive_prefix(s);
}

sp::Twine sp::path::join(const Twine& base, const Twine& child)
{
	if (child.empty())
		return normalize(base);

	if (base.empty() || is_absolute(child))
		return normalize(child);

	Twine result{ base };

	if (!result.endsWith('/') && !result.endsWith('\\'))
		result += '/';

	result += child;

	return normalize(result);
}

sp::Twine sp::path::normalize(const Twine& p)
{
	String s = slashify(p.s_str());

	if (s.empty())
		return Twine{ "." };

	String prefix;      // "C:" for drives, "" otherwise
	bool absolute = false;
	bool unc = false;

	std::size_t pos = 0;

	if (s.size() >= 2 && is_sep(s[0]) && is_sep(s[1]))
	{
		unc = true;
		absolute = true;
		pos = 2;
	}
	else if (is_drive_prefix(s))
	{
		prefix = s.substr(0, 2);
		pos = 2;

		if (pos < s.size() && is_sep(s[pos]))
			absolute = true;
	}
	else if (is_sep(s[0]))
	{
		absolute = true;
		pos = 1;
	}

	std::vector<String> parts;

	while (pos <= s.size())
	{
		const std::size_t end = s.find('/', pos);

		String part = s.substr(pos, end == String::npos ? String::npos : end - pos);

		if (!part.empty() && part != ".")
		{
			if (part == "..")
			{
				if (!parts.empty() && parts.back() != "..")
					parts.pop_back();
				else if (!absolute)
					parts.push_back("..");
			}
			else
			{
				parts.push_back(std::move(part));
			}
		}

		if (end == String::npos)
			break;

		pos = end + 1;
	}

	String body;

	for (std::size_t i = 0; i < parts.size(); ++i)
	{
		if (i > 0)
			body += '/';

		body += parts[i];
	}

	if (unc)
		return Twine{ "//" + body };

	if (absolute)
		return Twine{ prefix + "/" + body };

	if (body.empty())
		return Twine{ "." };

	return Twine{ prefix + body };
}

sp::Twine sp::path::filename(const Twine& p)
{
	String s = slashify(p.s_str());

	while (!s.empty() && s.back() == '/')
		s.pop_back();

	const std::size_t sep = s.find_last_of('/');

	return Twine{ sep == String::npos ? s : s.substr(sep + 1) };
}

sp::Twine sp::path::extension(const Twine& p)
{
	const String name = filename(p).s_str();
	const std::size_t dot = name.find_last_of('.');

	if (dot == String::npos || dot == 0)
		return Twine{};

	return Twine{ name.substr(dot) };
}

sp::Twine sp::path::stem(const Twine& p)
{
	const String name = filename(p).s_str();
	const std::size_t dot = name.find_last_of('.');

	if (dot == String::npos || dot == 0)
		return Twine{ name };

	return Twine{ name.substr(0, dot) };
}

sp::Twine sp::path::parent(const Twine& p)
{
	Twine normalized{ normalize(p) };
	const String& s = normalized.s_str();

	// Already at a root: ".", "/", "//", "C:" or "C:/".
	if (s == "." || s == "/" || s == "//"
		|| (is_drive_prefix(s) && (s.size() == 2 || (s.size() == 3 && s[2] == '/'))))
	{
		return normalized;
	}

	String body = s;
	bool absolute = false;
	String prefix;

	if (is_drive_prefix(body))
	{
		prefix = body.substr(0, 2);
		body = body.substr(2);

		if (!body.empty() && body[0] == '/')
		{
			absolute = true;
			body.erase(0, 1);
		}
	}
	else if (!body.empty() && body[0] == '/')
	{
		absolute = true;
		body.erase(0, 1);
	}

	const std::size_t sep = body.find_last_of('/');

	if (sep == String::npos)
		return absolute ? Twine{ prefix + "/" } : Twine{ "." };

	return Twine{ prefix + (absolute ? "/" : "") + body.substr(0, sep) };
}
