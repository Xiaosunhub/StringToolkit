#include "StringToolkit.h"
#include "utf8.h"
#include <Windows.h>

sp::String_Error::String_Error(const String& msg)             : message(msg) {}
sp::String_Error::String_Error(const char* msg)                    : message(msg ? msg : "The string has errors.") {}
[[nodiscard]] const char* sp::String_Error::what()                 const noexcept { return message.c_str(); }

sp::Index_Out_Of_Range::Index_Out_Of_Range(const String& msg) : String_Error(msg) {}
sp::Index_Out_Of_Range::Index_Out_Of_Range(const char* msg)        : String_Error(msg ? msg : "Index out of string range.") {}

sp::Empty_String::Empty_String(const String& msg)             : String_Error(msg) {}
sp::Empty_String::Empty_String(const char* msg)                    : String_Error(msg ? msg : "Empty string.") {}

sp::Invalid_Operation::Invalid_Operation(const String& msg)   : String_Error(msg) {}
sp::Invalid_Operation::Invalid_Operation(const char* msg)          : String_Error(msg ? msg : "Invalid Operation.") {}

sp::Invalid_Conversion::Invalid_Conversion(const String& msg)      : String_Error(msg) {}
sp::Invalid_Conversion::Invalid_Conversion(const char* msg)        : String_Error(msg ? msg : "Invalid Conversion.") {}

sp::Invalid_Argument::Invalid_Argument(const String& msg)          : String_Error(msg) {}
sp::Invalid_Argument::Invalid_Argument(const char* msg)            : String_Error(msg ? msg : "Invalid Argument.") {}

sp::StringToolkit::StringToolkit() {}
sp::StringToolkit::StringToolkit(const String& _data)           : m_data(_data) {}
sp::StringToolkit::StringToolkit(String&& _data)                : m_data(std::move(_data)) {}
sp::StringToolkit::StringToolkit(const char* _data)                  : m_data(_data) {}
sp::StringToolkit::StringToolkit(std::initializer_list<char> _data)
{
    for (auto& c : _data)
    {
        m_data += c;
    }
}
sp::StringToolkit::StringToolkit(std::vector<char> _data)
{
    for (auto& c : _data)
    {
        m_data += c;
    }
}
sp::StringToolkit::StringToolkit(std::set<char> _data)
{
	for (auto& c : _data)
	{
		m_data += c;
	}
}
sp::StringToolkit::StringToolkit(const StringToolkit& _data)           : m_data(_data.m_data) {}
sp::StringToolkit::StringToolkit(String _data, size_type size) : m_data(_data.substr(0, size)) {}

sp::StringToolkit sp::StringToolkit::create_from_this()                       const { return sp::StringToolkit(m_data); }

sp::StringToolkit& sp::StringToolkit::operator=(const String& _data)     { m_data = _data; return *this; }
sp::StringToolkit& sp::StringToolkit::operator=(String&& _data) noexcept { m_data = std::move(_data); return *this; }
sp::StringToolkit& sp::StringToolkit::operator=(const char* _data)            { m_data = (_data ? _data : ""); return *this; }

const String& sp::StringToolkit::s_str() const& noexcept { return m_data; }
String&& sp::StringToolkit::s_str() &&   noexcept { return std::move(m_data); }
const char* sp::StringToolkit::c_str()   const noexcept { return m_data.c_str(); }
const char* sp::StringToolkit::data()    const noexcept { return m_data.data(); }
char* sp::StringToolkit::data()          noexcept { return &m_data[0]; }
String sp::StringToolkit::operator*()    const { return m_data; }
String& sp::StringToolkit::get_changable_string()   { return m_data; }

sp::StringToolkit::operator std::string()     const& { return m_data; }
sp::StringToolkit::operator std::string() &&  { return std::move(m_data); }
sp::StringToolkit::operator const char* ()    const { return c_str(); }
sp::StringToolkit::operator bool()            const { return !empty(); }

bool sp::StringToolkit::empty()                          const noexcept { return m_data.empty(); }
sp::StringToolkit::size_type sp::StringToolkit::size()      const noexcept { return m_data.size(); }
sp::StringToolkit::size_type sp::StringToolkit::length()    const noexcept { return m_data.length(); }
sp::StringToolkit::size_type sp::StringToolkit::capacity()  const noexcept { return m_data.capacity(); }
void sp::StringToolkit::reserve(size_type new_cap)       { m_data.reserve(new_cap); }
void sp::StringToolkit::shrink_to_fit()                  { m_data.shrink_to_fit(); }

void sp::StringToolkit::clear()                                                            noexcept { m_data.clear(); }

sp::StringToolkit& sp::StringToolkit::insert(pos_type pos, const StringToolkit& str)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, str.m_data);
	return *this;
}

sp::StringToolkit& sp::StringToolkit::insert(pos_type pos, const String& str)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, str);
	return *this;
}

sp::StringToolkit& sp::StringToolkit::insert(pos_type pos, const char* str)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, str ? str : "");
	return *this;
}

sp::StringToolkit& sp::StringToolkit::insert(pos_type pos, char ch)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, 1, ch);
	return *this;
}

sp::StringToolkit& sp::StringToolkit::erase(pos_type pos, pos_type count)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in erase.");

	m_data.erase(pos, count);
	return *this;
}

sp::StringToolkit& sp::StringToolkit::assign(const StringToolkit& str)
{
	m_data = str.m_data;
	return *this;
}

sp::StringToolkit& sp::StringToolkit::assign(const String& str)
{
	m_data = str;
	return *this;
}

sp::StringToolkit& sp::StringToolkit::assign(const char* str)
{
	m_data = (str ? str : "");
	return *this;
}

sp::StringToolkit& sp::StringToolkit::assign(pos_type count, char ch)
{
	m_data.assign(count, ch);
	return *this;
}

void sp::StringToolkit::resize(size_type count)
{
	m_data.resize(count);
}

void sp::StringToolkit::resize(size_type count, char fill)
{
	m_data.resize(count, fill);
}

int sp::StringToolkit::compare(const StringToolkit& rhs) const
{
	return m_data.compare(rhs.m_data);
}

int sp::StringToolkit::compare(const String& rhs) const
{
	return m_data.compare(rhs);
}

int sp::StringToolkit::compare(const char* rhs) const
{
	return m_data.compare(rhs ? rhs : "");
}

sp::StringToolkit::pos_type sp::StringToolkit::rfind(const StringToolkit& s, pos_type pos) const noexcept
{
	return m_data.rfind(s.m_data, pos);
}

sp::StringToolkit::pos_type sp::StringToolkit::rfind(const char* s, pos_type pos) const noexcept
{
	return m_data.rfind(s ? s : "", pos);
}

sp::StringToolkit::pos_type sp::StringToolkit::rfind(char ch, pos_type pos) const noexcept
{
	return m_data.rfind(ch, pos);
}

sp::StringToolkit& sp::StringToolkit::append(const String& s)                            { m_data.append(s); return *this; }
sp::StringToolkit& sp::StringToolkit::append(const char* s, sp::StringToolkit::size_type count)  { m_data.append(s, count); return *this; }
sp::StringToolkit& sp::StringToolkit::append(const char* s)                                   { m_data.append(s); return *this; }
sp::StringToolkit& sp::StringToolkit::append(sp::StringToolkit::size_type count, char ch)        { m_data.append(count, ch); return *this; }

sp::StringToolkit& sp::StringToolkit::operator+=(const StringToolkit& rhs)   { m_data += rhs.m_data; return *this; }
sp::StringToolkit& sp::StringToolkit::operator+=(const String& rhs)  { m_data += rhs; return *this; }
sp::StringToolkit& sp::StringToolkit::operator+=(const char* rhs)         { m_data += rhs; return *this; }
sp::StringToolkit& sp::StringToolkit::operator+=(char ch)                 { m_data += ch; return *this; }

sp::StringToolkit sp::StringToolkit::operator+(const StringToolkit& rhs) const { return StringToolkit(m_data + rhs.m_data); }
sp::StringToolkit sp::StringToolkit::operator+(const String& rhs) const { return StringToolkit(m_data + rhs); }
sp::StringToolkit sp::StringToolkit::operator+(const char* rhs) const { return StringToolkit(m_data + (rhs ? rhs : "")); }

sp::StringToolkit sp::operator+(const char* lhs, const sp::StringToolkit& rhs) { return sp::StringToolkit((lhs ? lhs : "") + rhs.s_str()); }
sp::StringToolkit sp::operator+(const String& lhs, const sp::StringToolkit& rhs) { return sp::StringToolkit(lhs + rhs.s_str()); }

sp::StringToolkit sp::StringToolkit::operator*(sp::StringToolkit::size_type n) const
{
    String _Data{};

	for (size_type i = 0; i < n; ++i)
	{
		_Data += m_data;
	}

    return StringToolkit(_Data);
}

sp::StringToolkit& sp::StringToolkit::operator*=(sp::StringToolkit::size_type n)
{
	if (n == 0)
	{
		m_data.clear();
		return *this;
	}

	const String base = m_data;
	for (size_type i = 1; i < n; ++i)
	{
		m_data += base;
	}
	return *this;
}

sp::StringToolkit sp::StringToolkit::operator-(size_type n) const
{
	StringToolkit result = substr(n);

	return result;
}

sp::StringToolkit& sp::StringToolkit::operator-=(size_type n)
{
	if (n > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in operator-=.");

	if (n == m_data.size())
	{
		m_data.clear();
		return *this;
	}

	if (n == 0)
		return *this;

	if (n < 0)
		throw sp::Invalid_Argument("Negative value is not allowed in operator-=.");

	if (m_data.empty())
		throw sp::Empty_String("Cannot perform operator-= on an empty string.");

	m_data.resize(m_data.size() - n);

	return *this;
}

sp::StringToolkit& sp::StringToolkit::operator--()
{
	if (m_data.empty())
		throw sp::Empty_String("Cannot perform operator-- on an empty string.");

	if (m_data.size() == 1)
	{
		m_data.clear();
		return *this;
	}

	m_data.resize(m_data.size() - 1);

	return *this;
}

sp::StringToolkit sp::StringToolkit::operator&(const sp::StringToolkit& rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs.m_data.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] &= rhs.m_data[i];
    }
    return result;
}
sp::StringToolkit sp::StringToolkit::operator&(const String& rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] &= rhs[i];
    }
    return result;
}
sp::StringToolkit sp::StringToolkit::operator&(const char* rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type rhs_len = std::strlen(rhs);
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs_len);
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] &= rhs[i];
    }
    return result;
}

sp::StringToolkit sp::StringToolkit::operator|(const sp::StringToolkit& rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs.m_data.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] |= rhs.m_data[i];
    }
    return result;
}

sp::StringToolkit sp::StringToolkit::operator|(const String& rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] |= rhs[i];
    }
    return result;
}

sp::StringToolkit sp::StringToolkit::operator|(const char* rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type rhs_len = std::strlen(rhs);
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs_len);
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] |= rhs[i];
    }
    return result;
}


sp::StringToolkit sp::StringToolkit::operator^(const sp::StringToolkit& rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs.m_data.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] ^= rhs.m_data[i];
    }
    return result;
}

sp::StringToolkit sp::StringToolkit::operator^(const String& rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] ^= rhs[i];
    }
    return result;
}

sp::StringToolkit sp::StringToolkit::operator^(const char* rhs) const
{
    sp::StringToolkit result{ *this };
    sp::StringToolkit::size_type rhs_len = std::strlen(rhs);
    sp::StringToolkit::size_type min_size = (std::min)(result.m_data.size(), rhs_len);
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] ^= rhs[i];
    }
    return result;
}

sp::StringToolkit& sp::StringToolkit::operator&=(const sp::StringToolkit& rhs)
{
    sp::StringToolkit::size_type min_size = (std::min)(this->m_data.size(), rhs.m_data.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        m_data[i] &= rhs.m_data[i];
    }
    return *this;
}

sp::StringToolkit& sp::StringToolkit::operator|=(const sp::StringToolkit& rhs)
{
    sp::StringToolkit::size_type min_size = (std::min)(this->m_data.size(), rhs.m_data.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        m_data[i] |= rhs.m_data[i];
    }
    return *this;
}

sp::StringToolkit& sp::StringToolkit::operator^=(const sp::StringToolkit& rhs)
{
    sp::StringToolkit::size_type min_size = (std::min)(this->m_data.size(), rhs.m_data.size());
    for (sp::StringToolkit::size_type i = 0; i < min_size; ++i) {
        m_data[i] ^= rhs.m_data[i];
    }
    return *this;
}

 char& sp::StringToolkit::operator[](sp::StringToolkit::size_type pos) 
{
	if (pos >= m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in operator[].");

    return m_data[pos]; 
}
 const char& sp::StringToolkit::operator[](sp::StringToolkit::size_type pos) const 
{
	if (pos >= m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in operator[].");

    return m_data[pos]; 
}

char& sp::StringToolkit::at(sp::StringToolkit::size_type pos) { return m_data.at(pos); }
const char& sp::StringToolkit::at(sp::StringToolkit::size_type pos) const { return m_data.at(pos); }

char& sp::StringToolkit::front() { return m_data.front(); }
const char& sp::StringToolkit::front() const { return m_data.front(); }
char& sp::StringToolkit::back() { return m_data.back(); }
const char& sp::StringToolkit::back() const { return m_data.back(); }

sp::StringToolkit::iterator sp::StringToolkit::begin()                   noexcept { return m_data.begin(); }
sp::StringToolkit::const_iterator sp::StringToolkit::begin()             const noexcept { return m_data.begin(); }
sp::StringToolkit::const_iterator sp::StringToolkit::cbegin()            const noexcept { return m_data.cbegin(); }
sp::StringToolkit::iterator sp::StringToolkit::end()                     noexcept { return m_data.end(); }
sp::StringToolkit::const_iterator sp::StringToolkit::end()               const noexcept { return m_data.end(); }
sp::StringToolkit::const_iterator sp::StringToolkit::cend()              const noexcept { return m_data.cend(); }
sp::StringToolkit::reverse_iterator sp::StringToolkit::rbegin()          noexcept { return m_data.rbegin(); }
sp::StringToolkit::const_reverse_iterator sp::StringToolkit::rbegin()    const noexcept { return m_data.rbegin(); }
sp::StringToolkit::const_reverse_iterator sp::StringToolkit::crbegin()   const noexcept { return m_data.crbegin(); }
sp::StringToolkit::reverse_iterator sp::StringToolkit::rend()            noexcept { return m_data.rend(); }
sp::StringToolkit::const_reverse_iterator sp::StringToolkit::rend()      const noexcept { return m_data.rend(); }
sp::StringToolkit::const_reverse_iterator sp::StringToolkit::crend()     const noexcept { return m_data.crend(); }


sp::StringToolkit sp::StringToolkit::substr(
    sp::StringToolkit::pos_type pos,
    sp::StringToolkit::pos_type count
) const
{
    return sp::StringToolkit(m_data.substr(pos, count));
}

sp::StringToolkit sp::StringToolkit::substr_between(
    sp::StringToolkit::pos_type start_pos,
    sp::StringToolkit::pos_type end_pos,
    sp::StringToolkit::pos_type count
) const 
{
    using pos_t = sp::StringToolkit::pos_type;
    const pos_t data_len = m_data.size();

    if (m_data.size() == 0 || start_pos >= end_pos || start_pos > m_data.size() || end_pos > m_data.size())
        throw sp::Invalid_Operation("You cannot call substr on an empty string.");

    pos_t real_start = (start_pos >= data_len) ? data_len : start_pos;
    pos_t real_end = (end_pos > data_len) ? data_len : end_pos;

    pos_t take_len;
    if (count != npos) 
    {
        take_len = count;
    }
    else 
    {
        take_len = real_end - real_start;
    }
    
    if (take_len <= 0)
        return sp::StringToolkit{};

    return sp::StringToolkit(m_data.substr(real_start, take_len));
}

sp::StringToolkit::pos_type sp::StringToolkit::find(const sp::StringToolkit& s, sp::StringToolkit::pos_type pos) const noexcept
{
    return m_data.find(s.m_data, pos);
}

sp::StringToolkit::pos_type sp::StringToolkit::find(const char* s, sp::StringToolkit::pos_type pos)           const noexcept
{
    return m_data.find(s, pos);
}

sp::StringToolkit::pos_type sp::StringToolkit::find(char ch, sp::StringToolkit::pos_type pos)                 const noexcept
{
    return m_data.find(ch, pos);
}

 sp::StringToolkit& sp::StringToolkit::replace(pos_type pos, pos_type count, const StringToolkit& str)
{
	sp::StringToolkit::size_type data_len = m_data.size();
	if (pos > data_len)
		throw sp::Index_Out_Of_Range("Index out of range in replace.");

	m_data.replace(pos, count, str.m_data);

	return *this;
}

 sp::StringToolkit& sp::StringToolkit::replace(pos_type pos, pos_type count, const char* s)
{
	sp::StringToolkit::size_type data_len = m_data.size();

	if (pos > data_len)
		throw sp::Index_Out_Of_Range("Index out of range in replace.");

	m_data.replace(pos, count, s);

	return *this;
}

 sp::StringToolkit& sp::StringToolkit::replace(pos_type pos, pos_type count, const String& str)
{
	sp::StringToolkit::size_type data_len = m_data.size();

	if (pos > data_len)
		throw sp::Index_Out_Of_Range("Index out of range in replace.");

	m_data.replace(pos, count, str);

	return *this;
}

sp::StringToolkit& sp::StringToolkit::toUpper()
{
    std::transform(
        m_data.begin(),
        m_data.end(),
        m_data.begin(),
        [](unsigned char c) { return std::toupper(c); }
    );

    return *this;
}

sp::StringToolkit& sp::StringToolkit::toLower()
{
	std::transform(
		m_data.begin(),
		m_data.end(),
		m_data.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);

	return *this;
}

sp::StringToolkit sp::StringToolkit::copyUpper() const
{
	sp::StringToolkit result{ *this };
    result.toUpper();

    return result;
}

sp::StringToolkit sp::StringToolkit::copyLower() const
{
	sp::StringToolkit result{ *this };
	result.toLower();

    return result;
}

sp::StringToolkit& sp::StringToolkit::trimLeft()
{
	m_data.erase(
        m_data.begin(),
        std::find_if(
            m_data.begin(),
            m_data.end(),
            [](unsigned char ch) { return !std::isspace(ch); }
        )
    );

	return *this;
}

sp::StringToolkit& sp::StringToolkit::trimRight()
{
    m_data.erase(
        std::find_if(
            m_data.rbegin(),
            m_data.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }
        ).base(),
        m_data.end()
    );
    return *this;
}

sp::StringToolkit& sp::StringToolkit::trim()
{
	trimLeft();
	trimRight();
	return *this;
}

sp::StringToolkit& sp::StringToolkit::trim(char ch)
{
	m_data.erase(
		m_data.begin(),
		std::find_if(
			m_data.begin(),
			m_data.end(),
			[ch](unsigned char c) { return c != ch; }
		)
	);

	m_data.erase(
		std::find_if(
			m_data.rbegin(),
			m_data.rend(),
			[ch](unsigned char c) { return c != ch; }
		).base(),
		m_data.end()
	);

    return *this;
}



sp::StringToolkit sp::StringToolkit::copyTrim() const
{
    sp::StringToolkit result{ *this };
    result.trim();

    return result;
}

sp::StringToolkit sp::StringToolkit::copyTrim(char ch) const
{
    sp::StringToolkit result{ *this };
    result.trim(ch);

    return result;
}

 sp::StringToolkit& sp::StringToolkit::replaceAll(const StringToolkit& from, const StringToolkit& to)
{
	if (from.m_data.empty())
		throw sp::Invalid_Argument("The 'from' string cannot be empty in replaceAll.");

	pos_type start_pos = 0;

	while ((start_pos = m_data.find(from.m_data, start_pos)) != sp::StringToolkit::npos)
	{
		m_data.replace(start_pos, from.m_data.length(), to.m_data);
		start_pos += to.m_data.length();
	}

    return *this;
}

sp::StringToolkit sp::StringToolkit::copyReplaceAll(const StringToolkit& from, const StringToolkit& to)
{
	sp::StringToolkit result{ *this };
	result.replaceAll(from, to);

    return result;
}

 bool sp::StringToolkit::startsWith(const StringToolkit& prefix) const
{
	if (prefix.length() > m_data.length())
		throw sp::Invalid_Argument("The prefix string cannot be longer than the original string in startsWith.");

	return m_data.compare(0, prefix.length(), prefix.m_data) == 0;
}

 bool sp::StringToolkit::startsWith(const char* prefix) const
{
	if (prefix == nullptr)
		throw sp::Invalid_Argument("The prefix string cannot be null in startsWith.");
	size_type prefix_length = std::strlen(prefix);
	if (prefix_length > m_data.length())
		throw sp::Invalid_Argument("The prefix string cannot be longer than the original string in startsWith.");
	return m_data.compare(0, prefix_length, prefix) == 0;
}

 bool sp::StringToolkit::startsWith(const String& prefix) const
{
	if (prefix.length() > m_data.length())
		throw sp::Invalid_Argument("The prefix string cannot be longer than the original string in startsWith.");
	return m_data.compare(0, prefix.length(), prefix) == 0;
}

 bool sp::StringToolkit::startsWith(char ch) const
{
	if (m_data.empty())
		throw sp::Invalid_Argument("The original string cannot be empty in startsWith.");
	return m_data[0] == ch;
}

 bool sp::StringToolkit::endsWith(const StringToolkit& suffix) const
{
	if (suffix.length() > m_data.length())
		throw sp::Invalid_Argument("The suffix string cannot be longer than the original string in endsWith.");

	return m_data.compare(m_data.length() - suffix.length(), suffix.length(), suffix.m_data) == 0;
}

bool sp::StringToolkit::endsWith(const char* suffix) const
{
	if (suffix == nullptr)
		throw sp::Invalid_Argument("The suffix string cannot be null in endsWith.");
	size_type suffix_length = std::strlen(suffix);
	if (suffix_length > m_data.length())
		throw sp::Invalid_Argument("The suffix string cannot be longer than the original string in endsWith.");
	return m_data.compare(m_data.length() - suffix_length, suffix_length, suffix) == 0;
}

 bool sp::StringToolkit::endsWith(const String& suffix) const
{
	if (suffix.length() > m_data.length())
		throw sp::Invalid_Argument("The suffix string cannot be longer than the original string in endsWith.");
	return m_data.compare(m_data.length() - suffix.length(), suffix.length(), suffix) == 0;

}

 bool sp::StringToolkit::endsWith(char ch) const
{
	if (m_data.empty())
		throw sp::Invalid_Argument("The original string cannot be empty in endsWith.");
	return m_data[m_data.length() - 1] == ch;
}

bool sp::StringToolkit::contains(const StringToolkit& substring) const
{
	return m_data.find(substring.m_data) != sp::StringToolkit::npos;
}

bool sp::StringToolkit::contains(const char* substring) const
{
	return m_data.find(substring) != sp::StringToolkit::npos;
}

bool sp::StringToolkit::contains(const String& substring) const
{
    return m_data.find(substring) != sp::StringToolkit::npos;
}

bool sp::StringToolkit::contains(char ch) const
{
    return m_data.find(ch) != sp::StringToolkit::npos;
}

bool sp::StringToolkit::contains(const std::initializer_list<char>& chars) const
{
	return std::any_of(chars.begin(), chars.end(), [this](char ch) {
		return m_data.find(ch) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::vector<char>& chars) const
{
	return std::any_of(chars.begin(), chars.end(), [this](char ch) {
		return m_data.find(ch) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::set<char>& chars) const
{
	return std::any_of(chars.begin(), chars.end(), [this](char ch) {
		return m_data.find(ch) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::initializer_list<String>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String& substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::vector<String>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String& substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::set<String>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String& substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::initializer_list<StringToolkit>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const StringToolkit& substring) {
		return m_data.find(substring.m_data) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::vector<StringToolkit>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const StringToolkit& substring) {
		return m_data.find(substring.m_data) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::set<StringToolkit>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const StringToolkit& substring) {
		return m_data.find(substring.m_data) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::initializer_list<const char*>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const char* substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::vector<const char*>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const char* substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::set<const char*>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const char* substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::initializer_list<String_view>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String_view& substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::vector<String_view>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String_view& substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

bool sp::StringToolkit::contains(const std::set<String_view>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String_view& substring) {
		return m_data.find(substring) != sp::StringToolkit::npos;
		});
}

void sp::StringToolkit::push_back(char c)
{
    m_data.push_back(c);
}

void sp::StringToolkit::pop_back()
{
    m_data.pop_back();
}

bool sp::StringToolkit::matches(const StringToolkit& pattern) const
{
    try
    {
        std::regex re{ pattern.s_str() };
        return std::regex_match(m_data, re);
    }
    catch (std::regex_error) { throw sp::Invalid_Argument("Invalid regex pattern in matches."); }
    catch (std::bad_alloc) { throw sp::Invalid_Operation("Memory allocation failed during regex matching."); }
    catch (...) { throw sp::Invalid_Operation("An unknown error occurred during regex matching.");  }
}

bool sp::StringToolkit::matches(const char* pattern) const
{
    try
    {
		std::regex re{ pattern };
		return std::regex_match(m_data, re);
    }
	catch (std::regex_error) { throw sp::Invalid_Argument("Invalid regex pattern in matches."); }
	catch (std::bad_alloc) { throw sp::Invalid_Operation("Memory allocation failed during regex matching."); }
    catch (...) { throw sp::Invalid_Operation("An unknown error occurred during regex matching.");}
}

bool sp::StringToolkit::matches(const String& pattern) const
{
    try
    {
		std::regex re{ pattern };
		return std::regex_match(m_data, re);
	}
    catch (std::regex_error) { throw sp::Invalid_Argument("Invalid regex pattern in matches."); }
    catch (std::bad_alloc) { throw sp::Invalid_Operation("Memory allocation failed during regex matching."); }
	catch (...) { throw sp::Invalid_Operation("An unknown error occurred during regex matching."); }
}

void sp::StringToolkit::regexReplace(const StringToolkit& pattern, const StringToolkit& replacement)
{
    try
    {
		std::regex re{ pattern.s_str() };
		m_data = std::regex_replace(m_data, re, replacement.s_str());
	}
	catch (std::regex_error) { throw sp::Invalid_Argument("Invalid regex pattern in regexReplace."); }
	catch (std::bad_alloc) { throw sp::Invalid_Operation("Memory allocation failed during regex replacement."); }
    catch (...) { throw sp::Invalid_Operation("An unknown error occurred during regex replacement."); }
}

void sp::StringToolkit::regexReplace(const char* pattern, const char* replacement)
{
	try
	{
		std::regex re{ pattern };
		m_data = std::regex_replace(m_data, re, replacement);
	}
	catch (std::regex_error) { throw sp::Invalid_Argument("Invalid regex pattern in regexReplace."); }
	catch (std::bad_alloc) { throw sp::Invalid_Operation("Memory allocation failed during regex replacement."); }
	catch (...) { throw sp::Invalid_Operation("An unknown error occurred during regex replacement."); }
}

void sp::StringToolkit::regexReplace(const String& pattern, const String& replacement)
{
    try
    {
		std::regex re{ pattern };
		m_data = std::regex_replace(m_data, re, replacement);
    }
	catch (std::regex_error) { throw sp::Invalid_Argument("Invalid regex pattern in regexReplace."); }
	catch (std::bad_alloc) { throw sp::Invalid_Operation("Memory allocation failed during regex replacement."); }
	catch (...) { throw sp::Invalid_Operation("An unknown error occurred during regex replacement."); }
}

sp::StringToolkit& sp::StringToolkit::fromWide(const wString& wideStr)
{
	if (wideStr.empty())
	{
		m_data.clear();
		return *this;
	}

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), (int)wideStr.size(), NULL, 0, NULL, NULL);
	String str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), (int)wideStr.size(), &str[0], size_needed, NULL, NULL);

	m_data = std::move(str);

	return *this;
}

std::wstring sp::StringToolkit::toWide() const
{
	if (m_data.empty())
	{
		return std::wstring{};
	}

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, m_data.c_str(), (int)m_data.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, m_data.c_str(), (int)m_data.size(), &wideStr[0], size_needed);

	return wideStr;
}

std::vector<sp::StringToolkit> sp::StringToolkit::split(const StringToolkit& delimiter) const
{
	if (delimiter.empty())
		throw sp::Invalid_Argument("Delimiter cannot be empty in split.");

	if (m_data.empty())
		return {};

	if (m_data.find(delimiter.m_data) == sp::StringToolkit::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::StringToolkit> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter.m_data);

	while (end != sp::StringToolkit::npos) {
		if (end != start) { // Avoid adding empty tokens
			tokens.emplace_back(m_data.substr(start, end - start));
		}

		start = end + delimiter.m_data.length();
		end = m_data.find(delimiter.m_data, start);

		if (start >= m_data.size()) {
			break; // Prevents infinite loop if delimiter is at the end
		}
	}

	if (start < m_data.size()) {
		tokens.emplace_back(m_data.substr(start));
	}

	return tokens;
}

std::vector<sp::StringToolkit> sp::StringToolkit::split(const char* delimiter) const
{
	if (delimiter == nullptr || *delimiter == '\0')
		throw sp::Invalid_Argument("Delimiter cannot be null or empty in split.");

	if (m_data.empty())
		return {};

	if (m_data.find(delimiter) == sp::StringToolkit::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::StringToolkit> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter);

	while (end != sp::StringToolkit::npos) {
		if (end != start) { // Avoid adding empty tokens
			tokens.emplace_back(m_data.substr(start, end - start));
		}
		start = end + std::strlen(delimiter);
		end = m_data.find(delimiter, start);
		if (start >= m_data.size()) {
			break; // Prevents infinite loop if delimiter is at the end
		}
	}

	if (start < m_data.size()) {
		tokens.emplace_back(m_data.substr(start));
	}

	return tokens;
}

std::vector<sp::StringToolkit> sp::StringToolkit::split(const String& delimiter) const
{
	if (delimiter.empty())
		throw sp::Invalid_Argument("Delimiter cannot be empty in split.");

	if (m_data.empty())
		return {};

	if (m_data.find(delimiter) == sp::StringToolkit::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::StringToolkit> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter);

	while (end != sp::StringToolkit::npos) {
		if (end != start) { // Avoid adding empty tokens
			tokens.emplace_back(m_data.substr(start, end - start));
		}
		start = end + delimiter.length();
		end = m_data.find(delimiter, start);
		if (start >= m_data.size()) {
			break; // Prevents infinite loop if delimiter is at the end
		}
	}

	if (start < m_data.size()) {
		tokens.emplace_back(m_data.substr(start));
	}

	return tokens;
}

std::vector<sp::StringToolkit> sp::StringToolkit::split(char delimiter) const
{
	if (m_data.empty())
		return {};

	if (m_data.find(delimiter) == sp::StringToolkit::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::StringToolkit> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter);

	while (end != sp::StringToolkit::npos) {
		if (end != start) { // Avoid adding empty tokens
			tokens.emplace_back(m_data.substr(start, end - start));
		}
		start = end + 1;
		end = m_data.find(delimiter, start);
		if (start >= m_data.size()) {
			break; // Prevents infinite loop if delimiter is at the end
		}
	}

	if (start < m_data.size()) {
		tokens.emplace_back(m_data.substr(start));
	}

	return tokens;
}

sp::StringToolkit::size_type sp::StringToolkit::findFirstOf(const StringToolkit& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findFirstOf.");

	if (chars.size() == 1)
		return m_data.find(chars[0], pos);

	return m_data.find_first_of(chars.m_data, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findFirstOf(const char* chars, pos_type pos) const
{
	if (chars == nullptr || *chars == '\0')
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findFirstOf.");

	if (std::strlen(chars) == 1)
		return m_data.find(chars[0], pos);

	return m_data.find_first_of(chars, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findFirstOf(const String& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findFirstOf.");

	if (chars.size() == 1)
		return m_data.find(chars[0], pos);

	return m_data.find_first_of(chars, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findFirstOf(char ch, pos_type pos) const
{
	return m_data.find(ch, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findLastOf(const StringToolkit& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findLastOf.");

	if (chars.size() == 1)
		return m_data.find_last_of(chars[0], pos);

	return m_data.find_last_of(chars.m_data, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findLastOf(const char* chars, pos_type pos) const
{
	if (chars == nullptr || *chars == '\0')
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findLastOf.");

	if (std::strlen(chars) == 1)
		return m_data.find_last_of(chars[0], pos);

	return m_data.find_last_of(chars, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findLastOf(const String& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findLastOf.");

	if (chars.size() == 1)
		return m_data.find_last_of(chars[0], pos);

	return m_data.find_last_of(chars, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::findLastOf(char ch, pos_type pos) const
{
	return m_data.find_last_of(ch, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::UTF8Length() const
{
	return sp::utf8::length(*this);
}

sp::StringToolkit::size_type sp::StringToolkit::UTF8CharCount() const
{
	return sp::utf8::length(*this);
}

sp::StringToolkit sp::StringToolkit::UTF8SubString(size_type start, size_type length) const
{
	return sp::utf8::sub(*this, start, length);
}

sp::StringToolkit sp::StringToolkit::UTF8SubString(sp::StringToolkit::size_type start) const
{
	return sp::utf8::sub(*this, start);
}

sp::StringToolkit sp::StringToolkit::UTF8SubString(size_type start, size_type end, size_type length) const
{
	if (m_data.empty())
		throw sp::Invalid_Operation("Cannot call UTF8Substring on an empty string.");

	const size_type char_count = UTF8CharCount();

	if (start >= char_count || end > char_count || start >= end)
		throw sp::Index_Out_Of_Range("Start or end index out of range in UTF8Substring.");

	if (length == 0)
		throw sp::Empty_String("Length cannot be zero in UTF8Substring.");

	if (start + length > end)
		throw sp::Index_Out_Of_Range("Length exceeds substring bounds in UTF8Substring.");

	return sp::utf8::sub(*this, start, length);
}

void sp::StringToolkit::swap(StringToolkit& other) noexcept
{
	m_data.swap(other.m_data);
}

bool sp::StringToolkit::is_clear() const noexcept
{
	return empty();
}

bool sp::StringToolkit::is_empty() const noexcept
{
	return empty();
}

bool sp::StringToolkit::is_not_empty() const noexcept
{
	return !empty();
}

bool sp::StringToolkit::is_not_clear() const noexcept
{
	return !empty();
}

bool sp::StringToolkit::is_whitespace() const noexcept
{

	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isspace(c); });
}

bool sp::StringToolkit::is_not_whitespace() const noexcept
{
	return !is_whitespace();
}

bool sp::StringToolkit::is_digit() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isdigit(c); });
}

bool sp::StringToolkit::is_not_digit() const noexcept
{
	return !is_digit();
}

bool sp::StringToolkit::is_alpha() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isalpha(c); });
}

bool sp::StringToolkit::is_not_alpha() const noexcept
{
	return !is_alpha();
}

bool sp::StringToolkit::is_alnum() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isalnum(c); });
}

bool sp::StringToolkit::is_not_alnum() const noexcept
{
	return !is_alnum();
}

bool sp::StringToolkit::is_lower() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::islower(c); });
}

bool sp::StringToolkit::is_not_lower() const noexcept
{
	return !is_lower();
}

bool sp::StringToolkit::is_upper() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isupper(c); });
}

bool sp::StringToolkit::is_not_upper() const noexcept
{
	return !is_upper();
}

bool sp::StringToolkit::is_ascii() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return c <= 127; });
}

bool sp::StringToolkit::is_not_ascii() const noexcept
{
	return !is_ascii();
}

bool sp::StringToolkit::is_printable() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isprint(c); });
}

bool sp::StringToolkit::is_not_printable() const noexcept
{
	return !is_printable();
}

bool sp::StringToolkit::is_graphical() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isgraph(c); });
}

bool sp::StringToolkit::is_not_graphical() const noexcept
{
	return !is_graphical();
}

bool sp::StringToolkit::is_punctuation() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::ispunct(c); });
}

bool sp::StringToolkit::is_not_punctuation() const noexcept
{
	return !is_punctuation();
}

bool sp::StringToolkit::is_control() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::iscntrl(c); });
}

bool sp::StringToolkit::is_not_control() const noexcept
{
	return !is_control();
}

bool sp::StringToolkit::is_hex() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isxdigit(c); });
}

bool sp::StringToolkit::is_not_hex() const noexcept
{
	return !is_hex();
}

bool sp::StringToolkit::is_base64() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) {
		return (std::isalnum(c) || c == '+' || c == '/' || c == '=');
		});
}

bool sp::StringToolkit::is_not_base64() const noexcept
{
	return !is_base64();
}

bool sp::StringToolkit::is_utf8() const noexcept
{
	return sp::utf8::is_valid(*this);
}

bool sp::StringToolkit::is_not_utf8() const noexcept
{
	return !is_utf8();
}

bool sp::StringToolkit::is_regex_string() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) {
		return (std::isalnum(c) || std::ispunct(c) || std::isspace(c));
		});
}

bool sp::StringToolkit::is_not_regex_string() const noexcept
{
	return !is_regex_string();
}

void sp::StringToolkit::reset(const String& _data)
{
	m_data = _data;
}

void sp::StringToolkit::reset(String&& _data) noexcept
{
	m_data = std::move(_data);
}

void sp::StringToolkit::reset(const char* _data) noexcept
{
	m_data = _data;
}

void sp::StringToolkit::reset(const StringToolkit& _data)
{
	m_data = _data.m_data;
}

void sp::StringToolkit::reset(StringToolkit&& _data) noexcept
{
	m_data = std::move(_data.m_data);
}

void sp::StringToolkit::reset()
{
	m_data.clear();
}

void sp::StringToolkit::reverse()
{
	m_data = sp::utf8::reverse(*this).s_str();
}

sp::StringToolkit::size_type sp::StringToolkit::count(char ch) const
{
	if (m_data.empty())
		throw sp::Empty_String("Cannot count characters in an empty string.");

	size_type count = 0;

	while (true) {
		size_type pos = m_data.find(ch, count);
		if (pos == npos) {
			break;
		}
		++count;
	}

	return count;
}

sp::StringToolkit::size_type sp::StringToolkit::count(const StringToolkit& str) const
{
	if (m_data.empty())
		throw sp::Empty_String("Cannot count substrings in an empty string.");

	if (str.empty())
		throw sp::Invalid_Argument("The substring cannot be empty in count.");

	if (str.size() > m_data.size())
		throw sp::Invalid_Argument("The substring cannot be larger than the string in count.");

	size_type count = 0;

	while (true) {
		size_type pos = m_data.find(str.m_data, count);
		if (pos == npos) {
			break;
		}
		++count;
	}

	return count;
}

sp::StringToolkit::size_type sp::StringToolkit::count(String_view str) const
{
	if (m_data.empty())
		throw sp::Empty_String("Cannot count substrings in an empty string.");

	if (str.empty())
		throw sp::Invalid_Argument("The substring cannot be empty in count.");

	if (str.size() > m_data.size())
		throw sp::Invalid_Argument("The substring cannot be larger than the string in count.");

	if (str.size() == 1)
		return count(str[0]);

	size_type count = 0;

	while (true) {
		size_type pos = m_data.find(str, count);
		if (pos == npos) {
			break;
		}
		++count;
	}

	return count;
}

void sp::StringToolkit::join(StringToolkit data)
{
	if (data.empty())
		throw sp::Invalid_Argument("The string to join cannot be empty.");

	if (m_data.empty())
		throw sp::Empty_String("Cannot join to an empty string.");

	size_t total_len = 0;
	for (const auto& part : m_data) {
		total_len += 1;
	}
	total_len += data.size() * (m_data.size() - 1);

	StringToolkit result;
	result.reserve(total_len);

	auto it = m_data.begin();
	result += *it;
	++it;

	for (; it != m_data.end(); ++it) {
		result += data;
		result += *it;
	}

	m_data = std::move(result.m_data);
}

void sp::StringToolkit::join(const String& data)
{
	if (data.empty())
		throw sp::Invalid_Argument("The string to join cannot be empty.");

	if (m_data.empty())
		throw sp::Empty_String("Cannot join to an empty string.");

	size_t total_len = 0;
	for (const auto& part : m_data) {
		total_len += 1;
	}
	total_len += data.size() * (m_data.size() - 1);

	StringToolkit result;
	result.reserve(total_len);

	auto it = m_data.begin();
	result += *it;
	++it;

	for (; it != m_data.end(); ++it) {
		result += data;
		result += *it;
	}

	m_data = std::move(result.m_data);
}

void sp::StringToolkit::join(const char* data)
{
	if (data == nullptr || *data == '\0')
		throw sp::Invalid_Argument("The string to join cannot be null or empty.");

	if (m_data.empty())
		throw sp::Empty_String("Cannot join to an empty string.");

	size_t total_len = 0;
	for (const auto& part : m_data) {
		total_len += 1;
	}
	total_len += strlen(data) * (m_data.size() - 1);

	StringToolkit result;
	result.reserve(total_len);

	auto it = m_data.begin();
	result += *it;
	++it;

	for (; it != m_data.end(); ++it) {
		result += data;
		result += *it;
	}

	m_data = std::move(result.m_data);
}

void sp::StringToolkit::join(char data)
{
	if (m_data.empty())
		throw sp::Empty_String("Cannot join to an empty string.");

	size_t total_len = 0;
	for (const auto& part : m_data) {
		total_len += 1;
	}
	total_len += 1 * (m_data.size() - 1);

	StringToolkit result;
	result.reserve(total_len);

	auto it = m_data.begin();
	result += *it;
	++it;

	for (; it != m_data.end(); ++it) {
		result += data;
		result += *it;
	}

	m_data = std::move(result.m_data);
}

sp::StringToolkit sp::StringToolkit::create()
{
    return StringToolkit{ "" };
}

istream& operator>>(istream& is, sp::StringToolkit& str)
{
    String data;

    is >> data;
    str = data;

    return is;
}

ostream& operator<<(ostream& os, const sp::StringToolkit& str)
{
	return os << str.s_str();
}

std::istream& sp::getline(std::istream& is, StringToolkit& str, char delim)
{
	std::getline(is, str.m_data, delim);
	return is;
}

sp::StringToolkit sp::operator""_sp(const char* data, size_t size)
{
	return StringToolkit(data, size);
}

sp::Invalid_Format::Invalid_Format(const String& msg) : String_Error(msg) {}

sp::Invalid_Format::Invalid_Format(const char* msg) : String_Error(msg ? msg : "One or more errors occurred during the formatting operation.") {}

sp::StringToolkit::size_type sp::StringToolkit::find_first_not_of(const StringToolkit& chars, pos_type pos) const noexcept
{
	return m_data.find_first_not_of(chars.m_data, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::find_first_not_of(const char* chars, pos_type pos) const noexcept
{
	return m_data.find_first_not_of(chars, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::find_first_not_of(char ch, pos_type pos) const noexcept
{
	return m_data.find_first_not_of(ch, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::find_last_not_of(const StringToolkit& chars, pos_type pos) const noexcept
{
	return m_data.find_last_not_of(chars.m_data, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::find_last_not_of(const char* chars, pos_type pos) const noexcept
{
	return m_data.find_last_not_of(chars, pos);
}

sp::StringToolkit::size_type sp::StringToolkit::find_last_not_of(char ch, pos_type pos) const noexcept
{
	return m_data.find_last_not_of(ch, pos);
}

void sp::swap(StringToolkit& lhs, StringToolkit& rhs) noexcept
{
	lhs.swap(rhs);
}
