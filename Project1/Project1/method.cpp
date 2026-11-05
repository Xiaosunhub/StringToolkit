#include "method.h"
#include "utf8.h"
#include <fstream>
#include <iterator>
#include <random>

void sp::reverse_string(StringToolkit& str)
{
	str.reverse();
}

void sp::reverse_string_to(StringToolkit& str, StringToolkit& result)
{
	result = str;
	result.reverse();
}

void sp::reverse_string_to(StringToolkit&& str, StringToolkit& result) noexcept
{
	result = std::move(str);
	result.reverse();
}

void sp::copy_to(StringToolkit& str, StringToolkit& result)
{
	result = str;
}

void sp::copy_to(StringToolkit&& str, StringToolkit& result) noexcept
{
	result = std::move(str);
}

void sp::move_to(StringToolkit& str, StringToolkit& result)
{
	result = std::move(str);
	str.reset();
}

void sp::move_to(StringToolkit&& str, StringToolkit& result) noexcept
{
	result = std::exchange(str, StringToolkit{});
}

void sp::fill_with(StringToolkit& str, char ch)
{
	str.reset(std::string(str.size(), ch));
}

void sp::fill(StringToolkit& str, char ch, std::size_t count)
{
	str.reset(std::string(count, ch));
}

void sp::remove_prefix(StringToolkit& str, const StringToolkit& prefix)
{
	str.remove_prefix(prefix);
}

void sp::remove_suffix(StringToolkit& str, const StringToolkit& suffix)
{
	str.remove_suffix(suffix);
}

sp::StringToolkit sp::join(const std::vector<StringToolkit>& parts, const StringToolkit& separator)
{
	StringToolkit result;

	for (std::size_t i = 0; i < parts.size(); ++i)
	{
		if (i != 0)
			result += separator;

		result += parts[i];
	}

	return result;
}

sp::StringToolkit sp::join(const std::vector<StringToolkit>& parts, char separator)
{
	return sp::join(parts, StringToolkit(String(1, separator)));
}

std::vector<sp::StringToolkit> sp::split_lines(const StringToolkit& str)
{
	std::vector<StringToolkit> lines;

	const String& src = str.s_str();
	std::size_t start = 0;
	std::size_t i = 0;

	while (i < src.size())
	{
		const char c = src[i];

		if (c == '\n' || c == '\r')
		{
			lines.emplace_back(src.substr(start, i - start));

			if (c == '\r' && i + 1 < src.size() && src[i + 1] == '\n')
				++i;

			start = i + 1;
		}

		++i;
	}

	if (start < src.size())
		lines.emplace_back(src.substr(start));

	return lines;
}

void sp::pad_left(StringToolkit& str, std::size_t width, char fill)
{
	const std::size_t count = utf8::length(str);
	if (count >= width)
		return;

	str.insert(0, String(count < width ? width - count : 0, fill));
}

void sp::pad_right(StringToolkit& str, std::size_t width, char fill)
{
	const std::size_t count = utf8::length(str);
	if (count >= width)
		return;

	str.append(width - count, fill);
}

void sp::capitalize(StringToolkit& str)
{
	if (str.empty())
		return;

	String& data = str.get_changable_string();

	data[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(data[0])));

	for (std::size_t i = 1; i < data.size(); ++i)
		data[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(data[i])));
}

void sp::swap_case(StringToolkit& str)
{
	String& data = str.get_changable_string();

	for (auto& ch : data)
	{
		const unsigned char c = static_cast<unsigned char>(ch);

		if (std::islower(c))
			ch = static_cast<char>(std::toupper(c));
		else if (std::isupper(c))
			ch = static_cast<char>(std::tolower(c));
	}
}

void sp::replace_first(StringToolkit& str, const StringToolkit& from, const StringToolkit& to)
{
	if (from.empty())
		throw sp::Invalid_Argument("The substring to replace cannot be empty.");

	const auto pos = str.find(from);

	if (pos != StringToolkit::npos)
		str.replace(pos, from.size(), to);
}

sp::StringToolkit sp::read_file(const String& path)
{
	std::ifstream stream(path, std::ios::binary);

	if (!stream.is_open())
		throw sp::Invalid_Operation("Cannot open file for reading: " + path);

	String data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	return StringToolkit(std::move(data));
}

std::vector<sp::StringToolkit> sp::read_lines(const String& path)
{
	return sp::split_lines(sp::read_file(path));
}

void sp::write_file(const String& path, const StringToolkit& data, bool append)
{
	std::ofstream stream(
		path,
		std::ios::binary | (append ? std::ios::app : std::ios::trunc)
	);

	if (!stream.is_open())
		throw sp::Invalid_Operation("Cannot open file for writing: " + path);

	stream.write(data.data(), static_cast<std::streamsize>(data.size()));

	if (!stream)
		throw sp::Invalid_Operation("Failed while writing file: " + path);
}

// ---- random ----

sp::StringToolkit sp::generate_random(std::size_t length, const StringToolkit& charset)
{
	const String& chars = charset.s_str();

	if (chars.empty())
		throw sp::Invalid_Argument("Charset cannot be empty in generate_random.");

	static thread_local std::mt19937 generator{ std::random_device{}() };

	std::uniform_int_distribution<std::size_t> pick(0, chars.size() - 1);

	String result;
	result.reserve(length);

	for (std::size_t i = 0; i < length; ++i)
		result += chars[pick(generator)];

	return StringToolkit(std::move(result));
}

// ---- case-insensitive conveniences (ASCII only) ----

namespace
{
	String to_lower_ascii(String s)
	{
		for (auto& c : s)
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

		return s;
	}
}

bool sp::icontains(const StringToolkit& str, const StringToolkit& sub)
{
	const String haystack = to_lower_ascii(str.s_str());
	const String needle = to_lower_ascii(sub.s_str());

	return haystack.find(needle) != String::npos;
}

bool sp::istarts_with(const StringToolkit& str, const StringToolkit& prefix)
{
	const String haystack = to_lower_ascii(str.s_str());
	const String needle = to_lower_ascii(prefix.s_str());

	return haystack.starts_with(needle);
}

bool sp::iends_with(const StringToolkit& str, const StringToolkit& suffix)
{
	const String haystack = to_lower_ascii(str.s_str());
	const String needle = to_lower_ascii(suffix.s_str());

	return haystack.ends_with(needle);
}

// ---- splitting ----

std::vector<sp::StringToolkit> sp::split_by_any(const StringToolkit& str, const StringToolkit& delimiters)
{
	const String& src = str.s_str();
	const String& delims = delimiters.s_str();

	if (delims.empty())
		throw sp::Invalid_Argument("Delimiters cannot be empty in split_by_any.");

	std::vector<StringToolkit> fields;

	std::size_t start = 0;

	while (true)
	{
		const std::size_t end = src.find_first_of(delims, start);

		if (end == String::npos)
		{
			fields.emplace_back(src.substr(start));
			break;
		}

		fields.emplace_back(src.substr(start, end - start));
		start = end + 1;
	}

	// A trailing delimiter produces no trailing empty field.
	if (!fields.empty() && fields.back().empty() && !src.empty()
		&& delims.find(src.back()) != String::npos)
	{
		fields.pop_back();
	}

	return fields;
}

// ---- pattern matching ----

bool sp::wildcard_match(const StringToolkit& str, const StringToolkit& pattern)
{
	const String& s = str.s_str();
	const String& p = pattern.s_str();

	std::size_t si = 0, pi = 0;
	std::size_t star = String::npos, mark = 0;

	while (si < s.size())
	{
		if (pi < p.size() && (p[pi] == '?' || p[pi] == s[si]))
		{
			++si;
			++pi;
		}
		else if (pi < p.size() && p[pi] == '*')
		{
			star = pi++;
			mark = si;
		}
		else if (star != String::npos)
		{
			pi = star + 1;
			si = ++mark;
		}
		else
		{
			return false;
		}
	}

	while (pi < p.size() && p[pi] == '*')
		++pi;

	return pi == p.size();
}

// ---- case style conversions ----

namespace
{
	bool is_separator(char c)
	{
		return c == ' ' || c == '-' || c == '_';
	}

	// Word boundaries: separators, or an uppercase letter that follows a lowercase letter or digit.
	std::vector<String> split_words(const String& src)
	{
		std::vector<String> words;
		String current;

		for (std::size_t i = 0; i < src.size(); ++i)
		{
			const char c = src[i];

			if (is_separator(c))
			{
				if (!current.empty())
				{
					words.push_back(std::move(current));
					current.clear();
				}
				continue;
			}

			const bool boundary =
				std::isupper(static_cast<unsigned char>(c))
				&& !current.empty()
				&& (std::islower(static_cast<unsigned char>(current.back()))
					|| std::isdigit(static_cast<unsigned char>(current.back())));

			if (boundary)
			{
				words.push_back(std::move(current));
				current.clear();
			}

			current += c;
		}

		if (!current.empty())
			words.push_back(std::move(current));

		return words;
	}
}

void sp::to_snake_case(StringToolkit& str)
{
	const auto words = split_words(str.s_str());

	String result;

	for (std::size_t i = 0; i < words.size(); ++i)
	{
		if (i > 0)
			result += '_';

		result += to_lower_ascii(words[i]);
	}

	str = StringToolkit(std::move(result));
}

void sp::to_kebab_case(StringToolkit& str)
{
	const auto words = split_words(str.s_str());

	String result;

	for (std::size_t i = 0; i < words.size(); ++i)
	{
		if (i > 0)
			result += '-';

		result += to_lower_ascii(words[i]);
	}

	str = StringToolkit(std::move(result));
}

void sp::to_camel_case(StringToolkit& str)
{
	const auto words = split_words(str.s_str());

	String result;

	for (std::size_t i = 0; i < words.size(); ++i)
	{
		String word = to_lower_ascii(words[i]);

		if (i > 0 && !word.empty())
			word[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(word[0])));

		result += word;
	}

	str = StringToolkit(std::move(result));
}

// ---- JSON ----

sp::StringToolkit sp::json::escape(const StringToolkit& text)
{
	static const char* const HEX = "0123456789abcdef";

	String result;
	result.reserve(text.size() + 2);

	for (const char c : text.s_str())
	{
		const unsigned char uc = static_cast<unsigned char>(c);

		switch (c)
		{
		case '"':  result += "\\\""; continue;
		case '\\': result += "\\\\"; continue;
		case '\b': result += "\\b";  continue;
		case '\f': result += "\\f";  continue;
		case '\n': result += "\\n";  continue;
		case '\r': result += "\\r";  continue;
		case '\t': result += "\\t";  continue;
		default: break;
		}

		if (uc < 0x20)
		{
			result += "\\u00";
			result += HEX[(uc >> 4) & 0xF];
			result += HEX[uc & 0xF];
		}
		else
		{
			result += c;
		}
	}

	return StringToolkit(std::move(result));
}

namespace
{
	// Decode one \uXXXX sequence starting at src[i] (i points at 'u').
	// Returns the code unit and advances i past the 4 hex digits.
	unsigned json_hex4(const String& src, std::size_t& i)
	{
		if (i + 4 >= src.size())
			throw sp::Invalid_Conversion("Truncated \\u escape in json::unescape.");

		unsigned value = 0;

		for (int k = 0; k < 4; ++k)
		{
			const char c = src[++i];
			unsigned digit;

			if (c >= '0' && c <= '9')      digit = static_cast<unsigned>(c - '0');
			else if (c >= 'a' && c <= 'f') digit = static_cast<unsigned>(c - 'a' + 10);
			else if (c >= 'A' && c <= 'F') digit = static_cast<unsigned>(c - 'A' + 10);
			else
				throw sp::Invalid_Conversion("Invalid hex digit in \\u escape in json::unescape.");

			value = (value << 4) | digit;
		}

		return value;
	}

	void append_utf8(String& out, unsigned codepoint)
	{
		if (codepoint <= 0x7F)
		{
			out += static_cast<char>(codepoint);
		}
		else if (codepoint <= 0x7FF)
		{
			out += static_cast<char>(0xC0 | (codepoint >> 6));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
		else if (codepoint <= 0xFFFF)
		{
			out += static_cast<char>(0xE0 | (codepoint >> 12));
			out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
		else
		{
			out += static_cast<char>(0xF0 | (codepoint >> 18));
			out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
			out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			out += static_cast<char>(0x80 | (codepoint & 0x3F));
		}
	}
}

sp::StringToolkit sp::json::unescape(const StringToolkit& text)
{
	const String& src = text.s_str();

	String result;
	result.reserve(src.size());

	for (std::size_t i = 0; i < src.size(); ++i)
	{
		if (src[i] != '\\')
		{
			result += src[i];
			continue;
		}

		if (++i >= src.size())
			throw sp::Invalid_Conversion("Trailing backslash in json::unescape.");

		switch (src[i])
		{
		case '"':  result += '"';  break;
		case '\\': result += '\\'; break;
		case '/':  result += '/';  break;
		case 'b':  result += '\b'; break;
		case 'f':  result += '\f'; break;
		case 'n':  result += '\n'; break;
		case 'r':  result += '\r'; break;
		case 't':  result += '\t'; break;
		case 'u':
		{
			unsigned cp = json_hex4(src, i);

			if (cp >= 0xD800 && cp <= 0xDBFF) // high surrogate
			{
				if (i + 6 >= src.size() || src[i + 1] != '\\' || src[i + 2] != 'u')
					throw sp::Invalid_Conversion("Unpaired high surrogate in json::unescape.");

				i += 2;

				const unsigned low = json_hex4(src, i);

				if (low < 0xDC00 || low > 0xDFFF)
					throw sp::Invalid_Conversion("Invalid low surrogate in json::unescape.");

				cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
			}
			else if (cp >= 0xDC00 && cp <= 0xDFFF)
			{
				throw sp::Invalid_Conversion("Unpaired low surrogate in json::unescape.");
			}

			append_utf8(result, cp);
			break;
		}
		default:
			throw sp::Invalid_Conversion("Unknown escape sequence in json::unescape.");
		}
	}

	return StringToolkit(std::move(result));
}

// ---- CSV ----

sp::StringToolkit sp::csv::escape(const StringToolkit& field)
{
	const String& src = field.s_str();

	const bool needs_quotes =
		src.find_first_of(",\"\r\n") != String::npos;

	if (!needs_quotes)
		return field;

	String result;
	result.reserve(src.size() + 2);

	result += '"';

	for (const char c : src)
	{
		if (c == '"')
			result += '"';

		result += c;
	}

	result += '"';

	return StringToolkit(std::move(result));
}

std::vector<sp::StringToolkit> sp::csv::split(const StringToolkit& line)
{
	const String& src = line.s_str();

	std::vector<StringToolkit> fields;

	std::size_t i = 0;

	while (true)
	{
		String field;

		if (i < src.size() && src[i] == '"') // quoted field
		{
			++i;

			while (true)
			{
				if (i >= src.size())
					throw sp::Invalid_Conversion("Unterminated quoted field in csv::split.");

				if (src[i] == '"')
				{
					if (i + 1 < src.size() && src[i + 1] == '"') // doubled quote
					{
						field += '"';
						i += 2;
						continue;
					}

					++i;
					break;
				}

				field += src[i++];
			}

			if (i < src.size() && src[i] != ',')
				throw sp::Invalid_Conversion("Unexpected characters after closing quote in csv::split.");
		}
		else // bare field
		{
			while (i < src.size() && src[i] != ',')
				field += src[i++];
		}

		fields.emplace_back(std::move(field));

		if (i >= src.size())
			break;

		++i; // skip comma
	}

	return fields;
}

// ---- statistics ----

std::size_t sp::word_count(const StringToolkit& str)
{
	std::size_t words = 0;
	bool in_word = false;

	for (const char c : str.s_str())
	{
		const bool space = std::isspace(static_cast<unsigned char>(c)) != 0;

		if (space)
		{
			in_word = false;
		}
		else if (!in_word)
		{
			in_word = true;
			++words;
		}
	}

	return words;
}

std::size_t sp::levenshtein(const StringToolkit& a, const StringToolkit& b)
{
	const auto lhs = utf8::chars(a);
	const auto rhs = utf8::chars(b);

	const std::size_t m = lhs.size();
	const std::size_t n = rhs.size();

	if (m == 0) return n;
	if (n == 0) return m;

	std::vector<std::size_t> previous(n + 1);
	std::vector<std::size_t> current(n + 1);

	for (std::size_t j = 0; j <= n; ++j)
		previous[j] = j;

	for (std::size_t i = 1; i <= m; ++i)
	{
		current[0] = i;

		for (std::size_t j = 1; j <= n; ++j)
		{
			const std::size_t cost = (lhs[i - 1] == rhs[j - 1]) ? 0 : 1;

			current[j] = (std::min)({
				previous[j] + 1,        // deletion
				current[j - 1] + 1,     // insertion
				previous[j - 1] + cost  // substitution
			});
		}

		previous.swap(current);
	}

	return previous[n];
}

double sp::similarity(const StringToolkit& a, const StringToolkit& b)
{
	const std::size_t longest = (std::max)(utf8::length(a), utf8::length(b));

	if (longest == 0)
		return 1.0;

	return 1.0 - static_cast<double>(levenshtein(a, b)) / static_cast<double>(longest);
}

sp::StringToolkit sp::longest_common_prefix(const std::vector<StringToolkit>& parts)
{
	if (parts.empty())
		return StringToolkit{};

	const String* first = &parts.front().s_str();

	std::size_t limit = first->size();

	for (const auto& part : parts)
		limit = (std::min)(limit, part.size());

	std::size_t length = 0;

	while (length < limit)
	{
		bool same = true;

		for (std::size_t i = 1; i < parts.size(); ++i)
		{
			if (parts[i].s_str()[length] != (*first)[length])
			{
				same = false;
				break;
			}
		}

		if (!same)
			break;

		++length;
	}

	// Never cut in the middle of a UTF-8 sequence.
	while (length > 0 && length < first->size()
		&& (static_cast<unsigned char>((*first)[length]) & 0xC0) == 0x80)
	{
		--length;
	}

	return StringToolkit{ first->substr(0, length) };
}

// ---- layout ----

std::vector<sp::StringToolkit> sp::wrap(const StringToolkit& str, std::size_t width)
{
	if (width == 0)
		throw sp::Invalid_Argument("Width must be greater than zero in wrap.");

	std::vector<StringToolkit> lines;

	for (const auto& line : split_lines(str))
	{
		if (line.empty())
		{
			lines.emplace_back();
			continue;
		}

		StringToolkit current;
		std::size_t current_width = 0;

		for (const auto& word : line.split(' '))
		{
			const std::size_t word_width = utf8::length(word);

			if (current.empty())
			{
				current = word;
				current_width = word_width;
			}
			else if (current_width + 1 + word_width <= width)
			{
				current += ' ';
				current += word;
				current_width += 1 + word_width;
			}
			else
			{
				lines.push_back(current);
				current = word;
				current_width = word_width;
			}
		}

		if (!current.empty())
			lines.push_back(current);
	}

	return lines;
}

void sp::indent(StringToolkit& str, const StringToolkit& prefix)
{
	const auto lines = split_lines(str);

	String result;

	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		if (i > 0)
			result += '\n';

		if (!lines[i].empty())
			result += prefix.s_str();

		result += lines[i].s_str();
	}

	str = StringToolkit(std::move(result));
}

sp::StringToolkit sp::thousands_sep(long long value, char sep)
{
	const bool negative = value < 0;
	const unsigned long long magnitude = negative
		? 0ULL - static_cast<unsigned long long>(value)
		: static_cast<unsigned long long>(value);

	String digits = std::to_string(magnitude);
	String grouped;

	int counter = 0;

	for (auto it = digits.rbegin(); it != digits.rend(); ++it)
	{
		if (counter > 0 && counter % 3 == 0)
			grouped += sep;

		grouped += *it;
		++counter;
	}

	std::ranges::reverse(grouped);

	return StringToolkit{ (negative ? String("-") : String()) + grouped };
}

sp::StringToolkit sp::human_size(unsigned long long bytes)
{
	static constexpr const char* UNITS[] = { "B", "KB", "MB", "GB", "TB", "PB" };

	if (bytes < 1024)
		return StringToolkit{ std::to_string(bytes) + " B" };

	double value = static_cast<double>(bytes);
	std::size_t unit = 0;

	while (value >= 1024.0 && unit + 1 < std::size(UNITS))
	{
		value /= 1024.0;
		++unit;
	}

	StringToolkit number{ std::format("{:.1f}", value) };

	if (number.endsWith(".0"))
		number.resize(number.size() - 2);

	return StringToolkit{ number.s_str() + " " + UNITS[unit] };
}

sp::StringToolkit sp::render(const StringToolkit& tmpl, const std::map<StringToolkit, StringToolkit>& vars)
{
	const String& src = tmpl.s_str();

	String result;

	std::size_t pos = 0;

	while (pos < src.size())
	{
		const std::size_t open = src.find("{{", pos);

		if (open == String::npos)
		{
			result += src.substr(pos);
			break;
		}

		const std::size_t close = src.find("}}", open + 2);

		if (close == String::npos)
		{
			result += src.substr(pos);
			break;
		}

		result += src.substr(pos, open - pos);

		const String key = src.substr(open + 2, close - (open + 2));
		const auto it = vars.find(StringToolkit{ key });

		if (it != vars.end())
			result += it->second.s_str();
		else
			result += src.substr(open, close + 2 - open); // leave unknown placeholders intact

		pos = close + 2;
	}

	return StringToolkit(std::move(result));
}

std::pair<sp::StringToolkit, sp::StringToolkit> sp::split_pair(
	const StringToolkit& str, const StringToolkit& delimiter, bool first)
{
	if (delimiter.empty())
		throw sp::Invalid_Argument("Delimiter cannot be empty in split_pair.");

	const std::size_t pos = first ? str.find(delimiter) : str.rfind(delimiter);

	if (pos == StringToolkit::npos)
		return { str, StringToolkit{} };

	return {
		str.substr(0, pos),
		str.substr(pos + delimiter.size())
	};
}

// ---- convenience ----

void sp::truncate(StringToolkit& str, std::size_t max_chars, const StringToolkit& ellipsis)
{
	const std::size_t length = utf8::length(str);

	if (length <= max_chars)
		return;

	const std::size_t ellipsis_len = utf8::length(ellipsis);

	if (max_chars <= ellipsis_len)
	{
		str = utf8::sub(str, 0, max_chars);
		return;
	}

	str = utf8::sub(str, 0, max_chars - ellipsis_len) + ellipsis;
}

void sp::slug(StringToolkit& str)
{
	String result;
	result.reserve(str.size());

	bool last_was_dash = true; // treat the start as a dash to trim leading separators

	for (const char c : str.s_str())
	{
		const unsigned char uc = static_cast<unsigned char>(c);

		if (std::isalnum(uc))
		{
			result += static_cast<char>(std::tolower(uc));
			last_was_dash = false;
		}
		else if ((c == ' ' || c == '_' || c == '-') && !last_was_dash)
		{
			result += '-';
			last_was_dash = true;
		}
		// every other character is dropped
	}

	while (!result.empty() && result.back() == '-')
		result.pop_back();

	str = StringToolkit(std::move(result));
}

std::vector<sp::StringToolkit> sp::chunk(const StringToolkit& str, std::size_t size)
{
	if (size == 0)
		throw sp::Invalid_Argument("Chunk size must be greater than zero in chunk.");

	const std::size_t length = utf8::length(str);

	std::vector<StringToolkit> chunks;
	chunks.reserve((length + size - 1) / size);

	for (std::size_t start = 0; start < length; start += size)
		chunks.push_back(utf8::sub(str, start, (std::min)(size, length - start)));

	return chunks;
}

bool sp::to_bool(const StringToolkit& str)
{
	String lower = str.s_str();
	std::ranges::transform(lower, lower.begin(), [](char c) {
		return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	});

	if (lower == "true" || lower == "1" || lower == "yes"
		|| lower == "on" || lower == "y" || lower == "t")
		return true;

	if (lower == "false" || lower == "0" || lower == "no"
		|| lower == "off" || lower == "n" || lower == "f")
		return false;

	throw sp::Invalid_Conversion("Cannot convert string to bool in to_bool.");
}

// ---- string distance & diff helpers ----

std::size_t sp::hamming(const StringToolkit& a, const StringToolkit& b)
{
	const auto lhs = utf8::chars(a);
	const auto rhs = utf8::chars(b);

	if (lhs.size() != rhs.size())
		throw sp::Invalid_Argument("Strings must have equal character counts in hamming.");

	std::size_t distance = 0;

	for (std::size_t i = 0; i < lhs.size(); ++i)
	{
		if (lhs[i] != rhs[i])
			++distance;
	}

	return distance;
}

sp::StringToolkit sp::common_prefix(const StringToolkit& a, const StringToolkit& b)
{
	const String& lhs = a.s_str();
	const String& rhs = b.s_str();

	const std::size_t limit = (std::min)(lhs.size(), rhs.size());

	std::size_t length = 0;
	while (length < limit && lhs[length] == rhs[length])
		++length;

	// Never cut in the middle of a UTF-8 sequence.
	while (length > 0
		&& (static_cast<unsigned char>(lhs[length]) & 0xC0) == 0x80)
	{
		--length;
	}

	return StringToolkit{ lhs.substr(0, length) };
}

sp::StringToolkit sp::common_suffix(const StringToolkit& a, const StringToolkit& b)
{
	const String& lhs = a.s_str();
	const String& rhs = b.s_str();

	const std::size_t limit = (std::min)(lhs.size(), rhs.size());

	std::size_t length = 0;
	while (length < limit
		&& lhs[lhs.size() - 1 - length] == rhs[rhs.size() - 1 - length])
	{
		++length;
	}

	// The suffix must start on a character boundary: the byte right before
	// the suffix must not be a continuation byte of a split sequence.
	while (length > 0
		&& (static_cast<unsigned char>(lhs[lhs.size() - length]) & 0xC0) == 0x80)
	{
		--length;
	}

	return StringToolkit{ lhs.substr(lhs.size() - length) };
}

// ---- formatting ----

sp::StringToolkit sp::human_duration(unsigned long long seconds)
{
	if (seconds == 0)
		return StringToolkit{ "0s" };

	struct Unit { unsigned long long size; const char* suffix; };
	static constexpr Unit UNITS[] = {
		{ 86400, "d" }, { 3600, "h" }, { 60, "m" }, { 1, "s" },
	};

	String result;

	for (const auto& unit : UNITS)
	{
		if (seconds < unit.size)
			continue;

		const unsigned long long count = seconds / unit.size;
		seconds %= unit.size;

		if (!result.empty())
			result += ' ';

		result += std::to_string(count);
		result += unit.suffix;
	}

	return StringToolkit(std::move(result));
}

sp::StringToolkit sp::to_roman(int value)
{
	if (value < 1 || value > 3999)
		throw sp::Invalid_Argument("Roman numerals only cover 1..3999 in to_roman.");

	static constexpr struct { int value; const char* numeral; } TABLE[] = {
		{ 1000, "M"  }, { 900, "CM" }, { 500, "D"  }, { 400, "CD" },
		{ 100,  "C"  }, { 90,  "XC" }, { 50,  "L"  }, { 40,  "XL" },
		{ 10,   "X"  }, { 9,   "IX" }, { 5,   "V"  }, { 4,   "IV" },
		{ 1,    "I"  },
	};

	String result;

	for (const auto& entry : TABLE)
	{
		while (value >= entry.value)
		{
			result += entry.numeral;
			value -= entry.value;
		}
	}

	return StringToolkit(std::move(result));
}

int sp::from_roman(const StringToolkit& text)
{
	const String& src = text.s_str();

	if (src.empty())
		throw sp::Invalid_Conversion("Empty string in from_roman.");

	auto digit_value = [](char c) -> int
	{
		switch (static_cast<char>(std::toupper(static_cast<unsigned char>(c))))
		{
		case 'M': return 1000;
		case 'D': return 500;
		case 'C': return 100;
		case 'L': return 50;
		case 'X': return 10;
		case 'V': return 5;
		case 'I': return 1;
		default:  return 0;
		}
	};

	int total = 0;
	int previous = 0;

	for (auto it = src.rbegin(); it != src.rend(); ++it)
	{
		const int value = digit_value(*it);

		if (value == 0)
			throw sp::Invalid_Conversion("Invalid Roman numeral character in from_roman.");

		if (value < previous)
			total -= value;
		else
			total += value;

		previous = value;
	}

	// Reject non-canonical forms like "IIII", "VX", "IL".
	if (total < 1 || total > 3999 || to_roman(total).s_str() != [&] {
			String upper = src;
			for (auto& c : upper)
				c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
			return upper;
		}())
	{
		throw sp::Invalid_Conversion("Malformed Roman numeral in from_roman.");
	}

	return total;
}

// ---- HTML ----

sp::StringToolkit sp::html::escape(const StringToolkit& text)
{
	String result;
	result.reserve(text.size());

	for (const char c : text.s_str())
	{
		switch (c)
		{
		case '&':  result += "&amp;";  break;
		case '<':  result += "&lt;";   break;
		case '>':  result += "&gt;";   break;
		case '\"': result += "&quot;"; break;
		case '\'': result += "&#39;";  break;
		default:   result += c;        break;
		}
	}

	return StringToolkit(std::move(result));
}

sp::StringToolkit sp::html::unescape(const StringToolkit& text)
{
	const String& src = text.s_str();

	String result;
	result.reserve(src.size());

	auto named = [](const String& name) -> const char*
	{
		if (name == "amp")   return "&";
		if (name == "lt")    return "<";
		if (name == "gt")    return ">";
		if (name == "quot")  return "\"";
		if (name == "apos")  return "'";
		if (name == "nbsp")  return "\xC2\xA0"; // U+00A0 in UTF-8
		return nullptr;
	};

	for (std::size_t i = 0; i < src.size(); ++i)
	{
		if (src[i] != '&')
		{
			result += src[i];
			continue;
		}

		const std::size_t semi = src.find(';', i + 1);

		if (semi == String::npos || semi - i > 10)
			throw sp::Invalid_Conversion("Unterminated entity in html::unescape.");

		const String body = src.substr(i + 1, semi - i - 1);

		if (body.empty())
			throw sp::Invalid_Conversion("Empty entity in html::unescape.");

		if (body[0] == '#') // numeric entity
		{
			unsigned codepoint = 0;
			std::size_t k = 1;

			const bool hex = k < body.size() && (body[k] == 'x' || body[k] == 'X');

			if (hex)
				++k;

			if (k == body.size())
				throw sp::Invalid_Conversion("Empty numeric entity in html::unescape.");

			for (; k < body.size(); ++k)
			{
				const unsigned char c = static_cast<unsigned char>(body[k]);
				unsigned digit;

				if (c >= '0' && c <= '9')
					digit = static_cast<unsigned>(c - '0');
				else if (hex && c >= 'a' && c <= 'f')
					digit = static_cast<unsigned>(c - 'a' + 10);
				else if (hex && c >= 'A' && c <= 'F')
					digit = static_cast<unsigned>(c - 'A' + 10);
				else
					throw sp::Invalid_Conversion("Bad digit in numeric entity in html::unescape.");

				codepoint = hex ? codepoint * 16 + digit : codepoint * 10 + digit;

				if (codepoint > 0x10FFFF)
					throw sp::Invalid_Conversion("Entity out of range in html::unescape.");
			}

			if (codepoint == 0 || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
				throw sp::Invalid_Conversion("Invalid codepoint in html::unescape.");

			// Encode the codepoint as UTF-8.
			if (codepoint <= 0x7F)
				result += static_cast<char>(codepoint);
			else if (codepoint <= 0x7FF)
			{
				result += static_cast<char>(0xC0 | (codepoint >> 6));
				result += static_cast<char>(0x80 | (codepoint & 0x3F));
			}
			else if (codepoint <= 0xFFFF)
			{
				result += static_cast<char>(0xE0 | (codepoint >> 12));
				result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
				result += static_cast<char>(0x80 | (codepoint & 0x3F));
			}
			else
			{
				result += static_cast<char>(0xF0 | (codepoint >> 18));
				result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
				result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
				result += static_cast<char>(0x80 | (codepoint & 0x3F));
			}
		}
		else // named entity
		{
			const char* replacement = named(body);

			if (replacement == nullptr)
				throw sp::Invalid_Conversion("Unknown named entity in html::unescape.");

			result += replacement;
		}

		i = semi;
	}

	return StringToolkit(std::move(result));
}
