#include "Twine.h"
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

sp::Twine::Twine() {}
sp::Twine::Twine(const String& _data)           : m_data(_data) {}
sp::Twine::Twine(String&& _data)                : m_data(std::move(_data)) {}
sp::Twine::Twine(const char* _data)                  : m_data(_data) {}
sp::Twine::Twine(std::initializer_list<char> _data)
{
    for (auto& c : _data)
    {
        m_data += c;
    }
}
sp::Twine::Twine(std::vector<char> _data)
{
    for (auto& c : _data)
    {
        m_data += c;
    }
}
sp::Twine::Twine(std::set<char> _data)
{
	for (auto& c : _data)
	{
		m_data += c;
	}
}
sp::Twine::Twine(const Twine& _data)           : m_data(_data.m_data) {}
sp::Twine::Twine(String _data, size_type size) : m_data(_data.substr(0, size)) {}

sp::Twine sp::Twine::create_from_this()                       const { return sp::Twine(m_data); }

sp::Twine& sp::Twine::operator=(const String& _data)     { m_data = _data; return *this; }
sp::Twine& sp::Twine::operator=(String&& _data) noexcept { m_data = std::move(_data); return *this; }
sp::Twine& sp::Twine::operator=(const char* _data)            { m_data = (_data ? _data : ""); return *this; }

const String& sp::Twine::s_str() const& noexcept { return m_data; }
String&& sp::Twine::s_str() &&   noexcept { return std::move(m_data); }
const char* sp::Twine::c_str()   const noexcept { return m_data.c_str(); }
const char* sp::Twine::data()    const noexcept { return m_data.data(); }
char* sp::Twine::data()          noexcept { return &m_data[0]; }
String sp::Twine::operator*()    const { return m_data; }
String& sp::Twine::get_changable_string()   { return m_data; }

sp::Twine::operator std::string()     const& { return m_data; }
sp::Twine::operator std::string() &&  { return std::move(m_data); }
sp::Twine::operator const char* ()    const { return c_str(); }
sp::Twine::operator bool()            const { return !empty(); }

bool sp::Twine::empty()                          const noexcept { return m_data.empty(); }
sp::Twine::size_type sp::Twine::size()      const noexcept { return m_data.size(); }
sp::Twine::size_type sp::Twine::length()    const noexcept { return m_data.length(); }
sp::Twine::size_type sp::Twine::capacity()  const noexcept { return m_data.capacity(); }
void sp::Twine::reserve(size_type new_cap)       { m_data.reserve(new_cap); }
void sp::Twine::shrink_to_fit()                  { m_data.shrink_to_fit(); }

void sp::Twine::clear()                                                            noexcept { m_data.clear(); }

sp::Twine& sp::Twine::insert(pos_type pos, const Twine& str)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, str.m_data);
	return *this;
}

sp::Twine& sp::Twine::insert(pos_type pos, const String& str)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, str);
	return *this;
}

sp::Twine& sp::Twine::insert(pos_type pos, const char* str)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, str ? str : "");
	return *this;
}

sp::Twine& sp::Twine::insert(pos_type pos, char ch)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in insert.");

	m_data.insert(pos, 1, ch);
	return *this;
}

sp::Twine& sp::Twine::erase(pos_type pos, pos_type count)
{
	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in erase.");

	m_data.erase(pos, count);
	return *this;
}

sp::Twine& sp::Twine::assign(const Twine& str)
{
	m_data = str.m_data;
	return *this;
}

sp::Twine& sp::Twine::assign(const String& str)
{
	m_data = str;
	return *this;
}

sp::Twine& sp::Twine::assign(const char* str)
{
	m_data = (str ? str : "");
	return *this;
}

sp::Twine& sp::Twine::assign(pos_type count, char ch)
{
	m_data.assign(count, ch);
	return *this;
}

void sp::Twine::resize(size_type count)
{
	m_data.resize(count);
}

void sp::Twine::resize(size_type count, char fill)
{
	m_data.resize(count, fill);
}

int sp::Twine::compare(const Twine& rhs) const
{
	return m_data.compare(rhs.m_data);
}

int sp::Twine::compare(const String& rhs) const
{
	return m_data.compare(rhs);
}

int sp::Twine::compare(const char* rhs) const
{
	return m_data.compare(rhs ? rhs : "");
}

sp::Twine::pos_type sp::Twine::rfind(const Twine& s, pos_type pos) const noexcept
{
	return m_data.rfind(s.m_data, pos);
}

sp::Twine::pos_type sp::Twine::rfind(const char* s, pos_type pos) const noexcept
{
	return m_data.rfind(s ? s : "", pos);
}

sp::Twine::pos_type sp::Twine::rfind(char ch, pos_type pos) const noexcept
{
	return m_data.rfind(ch, pos);
}

sp::Twine& sp::Twine::append(const String& s)                            { m_data.append(s); return *this; }
sp::Twine& sp::Twine::append(const char* s, sp::Twine::size_type count)  { m_data.append(s, count); return *this; }
sp::Twine& sp::Twine::append(const char* s)                                   { m_data.append(s); return *this; }
sp::Twine& sp::Twine::append(sp::Twine::size_type count, char ch)        { m_data.append(count, ch); return *this; }

sp::Twine& sp::Twine::operator+=(const Twine& rhs)   { m_data += rhs.m_data; return *this; }
sp::Twine& sp::Twine::operator+=(const String& rhs)  { m_data += rhs; return *this; }
sp::Twine& sp::Twine::operator+=(const char* rhs)         { m_data += rhs; return *this; }
sp::Twine& sp::Twine::operator+=(char ch)                 { m_data += ch; return *this; }

sp::Twine sp::Twine::operator+(const Twine& rhs) const { return Twine(m_data + rhs.m_data); }
sp::Twine sp::Twine::operator+(const String& rhs) const { return Twine(m_data + rhs); }
sp::Twine sp::Twine::operator+(const char* rhs) const { return Twine(m_data + (rhs ? rhs : "")); }

sp::Twine sp::operator+(const char* lhs, const sp::Twine& rhs) { return sp::Twine((lhs ? lhs : "") + rhs.s_str()); }
sp::Twine sp::operator+(const String& lhs, const sp::Twine& rhs) { return sp::Twine(lhs + rhs.s_str()); }

sp::Twine sp::Twine::operator*(sp::Twine::size_type n) const
{
    String _Data{};

	for (size_type i = 0; i < n; ++i)
	{
		_Data += m_data;
	}

    return Twine(_Data);
}

sp::Twine& sp::Twine::operator*=(sp::Twine::size_type n)
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

sp::Twine sp::Twine::operator-(size_type n) const
{
	Twine result = substr(n);

	return result;
}

sp::Twine& sp::Twine::operator-=(size_type n)
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

sp::Twine& sp::Twine::operator--()
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

sp::Twine sp::Twine::operator&(const sp::Twine& rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs.m_data.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] &= rhs.m_data[i];
    }
    return result;
}
sp::Twine sp::Twine::operator&(const String& rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] &= rhs[i];
    }
    return result;
}
sp::Twine sp::Twine::operator&(const char* rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type rhs_len = std::strlen(rhs);
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs_len);
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] &= rhs[i];
    }
    return result;
}

sp::Twine sp::Twine::operator|(const sp::Twine& rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs.m_data.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] |= rhs.m_data[i];
    }
    return result;
}

sp::Twine sp::Twine::operator|(const String& rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] |= rhs[i];
    }
    return result;
}

sp::Twine sp::Twine::operator|(const char* rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type rhs_len = std::strlen(rhs);
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs_len);
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] |= rhs[i];
    }
    return result;
}


sp::Twine sp::Twine::operator^(const sp::Twine& rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs.m_data.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] ^= rhs.m_data[i];
    }
    return result;
}

sp::Twine sp::Twine::operator^(const String& rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] ^= rhs[i];
    }
    return result;
}

sp::Twine sp::Twine::operator^(const char* rhs) const
{
    sp::Twine result{ *this };
    sp::Twine::size_type rhs_len = std::strlen(rhs);
    sp::Twine::size_type min_size = (std::min)(result.m_data.size(), rhs_len);
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        result.m_data[i] ^= rhs[i];
    }
    return result;
}

sp::Twine& sp::Twine::operator&=(const sp::Twine& rhs)
{
    sp::Twine::size_type min_size = (std::min)(this->m_data.size(), rhs.m_data.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        m_data[i] &= rhs.m_data[i];
    }
    return *this;
}

sp::Twine& sp::Twine::operator|=(const sp::Twine& rhs)
{
    sp::Twine::size_type min_size = (std::min)(this->m_data.size(), rhs.m_data.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        m_data[i] |= rhs.m_data[i];
    }
    return *this;
}

sp::Twine& sp::Twine::operator^=(const sp::Twine& rhs)
{
    sp::Twine::size_type min_size = (std::min)(this->m_data.size(), rhs.m_data.size());
    for (sp::Twine::size_type i = 0; i < min_size; ++i) {
        m_data[i] ^= rhs.m_data[i];
    }
    return *this;
}

 char& sp::Twine::operator[](sp::Twine::size_type pos) 
{
	if (pos >= m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in operator[].");

    return m_data[pos]; 
}
 const char& sp::Twine::operator[](sp::Twine::size_type pos) const 
{
	if (pos >= m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in operator[].");

    return m_data[pos]; 
}

char& sp::Twine::at(sp::Twine::size_type pos) { return m_data.at(pos); }
const char& sp::Twine::at(sp::Twine::size_type pos) const { return m_data.at(pos); }

char& sp::Twine::front() { return m_data.front(); }
const char& sp::Twine::front() const { return m_data.front(); }
char& sp::Twine::back() { return m_data.back(); }
const char& sp::Twine::back() const { return m_data.back(); }

sp::Twine::iterator sp::Twine::begin()                   noexcept { return m_data.begin(); }
sp::Twine::const_iterator sp::Twine::begin()             const noexcept { return m_data.begin(); }
sp::Twine::const_iterator sp::Twine::cbegin()            const noexcept { return m_data.cbegin(); }
sp::Twine::iterator sp::Twine::end()                     noexcept { return m_data.end(); }
sp::Twine::const_iterator sp::Twine::end()               const noexcept { return m_data.end(); }
sp::Twine::const_iterator sp::Twine::cend()              const noexcept { return m_data.cend(); }
sp::Twine::reverse_iterator sp::Twine::rbegin()          noexcept { return m_data.rbegin(); }
sp::Twine::const_reverse_iterator sp::Twine::rbegin()    const noexcept { return m_data.rbegin(); }
sp::Twine::const_reverse_iterator sp::Twine::crbegin()   const noexcept { return m_data.crbegin(); }
sp::Twine::reverse_iterator sp::Twine::rend()            noexcept { return m_data.rend(); }
sp::Twine::const_reverse_iterator sp::Twine::rend()      const noexcept { return m_data.rend(); }
sp::Twine::const_reverse_iterator sp::Twine::crend()     const noexcept { return m_data.crend(); }


sp::Twine sp::Twine::substr(
    sp::Twine::pos_type pos,
    sp::Twine::pos_type count
) const
{
    return sp::Twine(m_data.substr(pos, count));
}

sp::Twine sp::Twine::substr_between(
    sp::Twine::pos_type start_pos,
    sp::Twine::pos_type end_pos,
    sp::Twine::pos_type count
) const 
{
    using pos_t = sp::Twine::pos_type;
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
        return sp::Twine{};

    return sp::Twine(m_data.substr(real_start, take_len));
}

sp::Twine::pos_type sp::Twine::find(const sp::Twine& s, sp::Twine::pos_type pos) const noexcept
{
    return m_data.find(s.m_data, pos);
}

sp::Twine::pos_type sp::Twine::find(const char* s, sp::Twine::pos_type pos)           const noexcept
{
    return m_data.find(s, pos);
}

sp::Twine::pos_type sp::Twine::find(char ch, sp::Twine::pos_type pos)                 const noexcept
{
    return m_data.find(ch, pos);
}

 sp::Twine& sp::Twine::replace(pos_type pos, pos_type count, const Twine& str)
{
	sp::Twine::size_type data_len = m_data.size();
	if (pos > data_len)
		throw sp::Index_Out_Of_Range("Index out of range in replace.");

	m_data.replace(pos, count, str.m_data);

	return *this;
}

 sp::Twine& sp::Twine::replace(pos_type pos, pos_type count, const char* s)
{
	sp::Twine::size_type data_len = m_data.size();

	if (pos > data_len)
		throw sp::Index_Out_Of_Range("Index out of range in replace.");

	m_data.replace(pos, count, s);

	return *this;
}

 sp::Twine& sp::Twine::replace(pos_type pos, pos_type count, const String& str)
{
	sp::Twine::size_type data_len = m_data.size();

	if (pos > data_len)
		throw sp::Index_Out_Of_Range("Index out of range in replace.");

	m_data.replace(pos, count, str);

	return *this;
}

sp::Twine& sp::Twine::toUpper()
{
    std::transform(
        m_data.begin(),
        m_data.end(),
        m_data.begin(),
        [](unsigned char c) { return std::toupper(c); }
    );

    return *this;
}

sp::Twine& sp::Twine::toLower()
{
	std::transform(
		m_data.begin(),
		m_data.end(),
		m_data.begin(),
		[](unsigned char c) { return std::tolower(c); }
	);

	return *this;
}

sp::Twine sp::Twine::copyUpper() const
{
	sp::Twine result{ *this };
    result.toUpper();

    return result;
}

sp::Twine sp::Twine::copyLower() const
{
	sp::Twine result{ *this };
	result.toLower();

    return result;
}

sp::Twine& sp::Twine::trimLeft()
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

sp::Twine& sp::Twine::trimRight()
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

sp::Twine& sp::Twine::trim()
{
	trimLeft();
	trimRight();
	return *this;
}

sp::Twine& sp::Twine::trim(char ch)
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



sp::Twine sp::Twine::copyTrim() const
{
    sp::Twine result{ *this };
    result.trim();

    return result;
}

sp::Twine sp::Twine::copyTrim(char ch) const
{
    sp::Twine result{ *this };
    result.trim(ch);

    return result;
}

 sp::Twine& sp::Twine::replaceAll(const Twine& from, const Twine& to)
{
	if (from.m_data.empty())
		throw sp::Invalid_Argument("The 'from' string cannot be empty in replaceAll.");

	pos_type start_pos = 0;

	while ((start_pos = m_data.find(from.m_data, start_pos)) != sp::Twine::npos)
	{
		m_data.replace(start_pos, from.m_data.length(), to.m_data);
		start_pos += to.m_data.length();
	}

    return *this;
}

sp::Twine sp::Twine::copyReplaceAll(const Twine& from, const Twine& to)
{
	sp::Twine result{ *this };
	result.replaceAll(from, to);

    return result;
}

 bool sp::Twine::startsWith(const Twine& prefix) const
{
	if (prefix.length() > m_data.length())
		throw sp::Invalid_Argument("The prefix string cannot be longer than the original string in startsWith.");

	return m_data.compare(0, prefix.length(), prefix.m_data) == 0;
}

 bool sp::Twine::startsWith(const char* prefix) const
{
	if (prefix == nullptr)
		throw sp::Invalid_Argument("The prefix string cannot be null in startsWith.");
	size_type prefix_length = std::strlen(prefix);
	if (prefix_length > m_data.length())
		throw sp::Invalid_Argument("The prefix string cannot be longer than the original string in startsWith.");
	return m_data.compare(0, prefix_length, prefix) == 0;
}

 bool sp::Twine::startsWith(const String& prefix) const
{
	if (prefix.length() > m_data.length())
		throw sp::Invalid_Argument("The prefix string cannot be longer than the original string in startsWith.");
	return m_data.compare(0, prefix.length(), prefix) == 0;
}

 bool sp::Twine::startsWith(char ch) const
{
	if (m_data.empty())
		throw sp::Invalid_Argument("The original string cannot be empty in startsWith.");
	return m_data[0] == ch;
}

 bool sp::Twine::endsWith(const Twine& suffix) const
{
	if (suffix.length() > m_data.length())
		throw sp::Invalid_Argument("The suffix string cannot be longer than the original string in endsWith.");

	return m_data.compare(m_data.length() - suffix.length(), suffix.length(), suffix.m_data) == 0;
}

bool sp::Twine::endsWith(const char* suffix) const
{
	if (suffix == nullptr)
		throw sp::Invalid_Argument("The suffix string cannot be null in endsWith.");
	size_type suffix_length = std::strlen(suffix);
	if (suffix_length > m_data.length())
		throw sp::Invalid_Argument("The suffix string cannot be longer than the original string in endsWith.");
	return m_data.compare(m_data.length() - suffix_length, suffix_length, suffix) == 0;
}

 bool sp::Twine::endsWith(const String& suffix) const
{
	if (suffix.length() > m_data.length())
		throw sp::Invalid_Argument("The suffix string cannot be longer than the original string in endsWith.");
	return m_data.compare(m_data.length() - suffix.length(), suffix.length(), suffix) == 0;

}

 bool sp::Twine::endsWith(char ch) const
{
	if (m_data.empty())
		throw sp::Invalid_Argument("The original string cannot be empty in endsWith.");
	return m_data[m_data.length() - 1] == ch;
}

bool sp::Twine::contains(const Twine& substring) const
{
	return m_data.find(substring.m_data) != sp::Twine::npos;
}

bool sp::Twine::contains(const char* substring) const
{
	return m_data.find(substring) != sp::Twine::npos;
}

bool sp::Twine::contains(const String& substring) const
{
    return m_data.find(substring) != sp::Twine::npos;
}

bool sp::Twine::contains(char ch) const
{
    return m_data.find(ch) != sp::Twine::npos;
}

bool sp::Twine::contains(const std::initializer_list<char>& chars) const
{
	return std::any_of(chars.begin(), chars.end(), [this](char ch) {
		return m_data.find(ch) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::vector<char>& chars) const
{
	return std::any_of(chars.begin(), chars.end(), [this](char ch) {
		return m_data.find(ch) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::set<char>& chars) const
{
	return std::any_of(chars.begin(), chars.end(), [this](char ch) {
		return m_data.find(ch) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::initializer_list<String>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String& substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::vector<String>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String& substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::set<String>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String& substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::initializer_list<Twine>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const Twine& substring) {
		return m_data.find(substring.m_data) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::vector<Twine>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const Twine& substring) {
		return m_data.find(substring.m_data) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::set<Twine>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const Twine& substring) {
		return m_data.find(substring.m_data) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::initializer_list<const char*>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const char* substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::vector<const char*>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const char* substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::set<const char*>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const char* substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::initializer_list<String_view>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String_view& substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::vector<String_view>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String_view& substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

bool sp::Twine::contains(const std::set<String_view>& substrings) const
{
	return std::any_of(substrings.begin(), substrings.end(), [this](const String_view& substring) {
		return m_data.find(substring) != sp::Twine::npos;
		});
}

void sp::Twine::push_back(char c)
{
    m_data.push_back(c);
}

void sp::Twine::pop_back()
{
    m_data.pop_back();
}

bool sp::Twine::matches(const Twine& pattern) const
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

bool sp::Twine::matches(const char* pattern) const
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

bool sp::Twine::matches(const String& pattern) const
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

void sp::Twine::regexReplace(const Twine& pattern, const Twine& replacement)
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

void sp::Twine::regexReplace(const char* pattern, const char* replacement)
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

void sp::Twine::regexReplace(const String& pattern, const String& replacement)
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

sp::Twine& sp::Twine::fromWide(const wString& wideStr)
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

std::wstring sp::Twine::toWide() const
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

std::vector<sp::Twine> sp::Twine::split(const Twine& delimiter) const
{
	if (delimiter.empty())
		throw sp::Invalid_Argument("Delimiter cannot be empty in split.");

	if (m_data.empty())
		return {};

	if (m_data.find(delimiter.m_data) == sp::Twine::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::Twine> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter.m_data);

	while (end != sp::Twine::npos) {
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

std::vector<sp::Twine> sp::Twine::split(const char* delimiter) const
{
	if (delimiter == nullptr || *delimiter == '\0')
		throw sp::Invalid_Argument("Delimiter cannot be null or empty in split.");

	if (m_data.empty())
		return {};

	if (m_data.find(delimiter) == sp::Twine::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::Twine> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter);

	while (end != sp::Twine::npos) {
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

std::vector<sp::Twine> sp::Twine::split(const String& delimiter) const
{
	if (delimiter.empty())
		throw sp::Invalid_Argument("Delimiter cannot be empty in split.");

	if (m_data.empty())
		return {};

	if (m_data.find(delimiter) == sp::Twine::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::Twine> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter);

	while (end != sp::Twine::npos) {
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

std::vector<sp::Twine> sp::Twine::split(char delimiter) const
{
	if (m_data.empty())
		return {};

	if (m_data.find(delimiter) == sp::Twine::npos)
		return { *this }; // Return the whole string if delimiter not found

	std::vector<sp::Twine> tokens;
	pos_type start = 0;
	pos_type end = m_data.find(delimiter);

	while (end != sp::Twine::npos) {
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

sp::Twine::size_type sp::Twine::findFirstOf(const Twine& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findFirstOf.");

	if (chars.size() == 1)
		return m_data.find(chars[0], pos);

	return m_data.find_first_of(chars.m_data, pos);
}

sp::Twine::size_type sp::Twine::findFirstOf(const char* chars, pos_type pos) const
{
	if (chars == nullptr || *chars == '\0')
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findFirstOf.");

	if (std::strlen(chars) == 1)
		return m_data.find(chars[0], pos);

	return m_data.find_first_of(chars, pos);
}

sp::Twine::size_type sp::Twine::findFirstOf(const String& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findFirstOf.");

	if (chars.size() == 1)
		return m_data.find(chars[0], pos);

	return m_data.find_first_of(chars, pos);
}

sp::Twine::size_type sp::Twine::findFirstOf(char ch, pos_type pos) const
{
	return m_data.find(ch, pos);
}

sp::Twine::size_type sp::Twine::findLastOf(const Twine& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findLastOf.");

	if (chars.size() == 1)
		return m_data.find_last_of(chars[0], pos);

	return m_data.find_last_of(chars.m_data, pos);
}

sp::Twine::size_type sp::Twine::findLastOf(const char* chars, pos_type pos) const
{
	if (chars == nullptr || *chars == '\0')
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findLastOf.");

	if (std::strlen(chars) == 1)
		return m_data.find_last_of(chars[0], pos);

	return m_data.find_last_of(chars, pos);
}

sp::Twine::size_type sp::Twine::findLastOf(const String& chars, pos_type pos) const
{
	if (chars.empty())
		return npos;

	if (pos > m_data.size())
		throw sp::Index_Out_Of_Range("Index out of range in findLastOf.");

	if (chars.size() == 1)
		return m_data.find_last_of(chars[0], pos);

	return m_data.find_last_of(chars, pos);
}

sp::Twine::size_type sp::Twine::findLastOf(char ch, pos_type pos) const
{
	return m_data.find_last_of(ch, pos);
}

sp::Twine::size_type sp::Twine::UTF8Length() const
{
	return sp::utf8::length(*this);
}

sp::Twine::size_type sp::Twine::UTF8CharCount() const
{
	return sp::utf8::length(*this);
}

sp::Twine sp::Twine::UTF8SubString(size_type start, size_type length) const
{
	return sp::utf8::sub(*this, start, length);
}

sp::Twine sp::Twine::UTF8SubString(sp::Twine::size_type start) const
{
	return sp::utf8::sub(*this, start);
}

sp::Twine sp::Twine::UTF8SubString(size_type start, size_type end, size_type length) const
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

void sp::Twine::swap(Twine& other) noexcept
{
	m_data.swap(other.m_data);
}

bool sp::Twine::is_clear() const noexcept
{
	return empty();
}

bool sp::Twine::is_empty() const noexcept
{
	return empty();
}

bool sp::Twine::is_not_empty() const noexcept
{
	return !empty();
}

bool sp::Twine::is_not_clear() const noexcept
{
	return !empty();
}

bool sp::Twine::is_whitespace() const noexcept
{

	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isspace(c); });
}

bool sp::Twine::is_not_whitespace() const noexcept
{
	return !is_whitespace();
}

bool sp::Twine::is_digit() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isdigit(c); });
}

bool sp::Twine::is_not_digit() const noexcept
{
	return !is_digit();
}

bool sp::Twine::is_alpha() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isalpha(c); });
}

bool sp::Twine::is_not_alpha() const noexcept
{
	return !is_alpha();
}

bool sp::Twine::is_alnum() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isalnum(c); });
}

bool sp::Twine::is_not_alnum() const noexcept
{
	return !is_alnum();
}

bool sp::Twine::is_lower() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::islower(c); });
}

bool sp::Twine::is_not_lower() const noexcept
{
	return !is_lower();
}

bool sp::Twine::is_upper() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isupper(c); });
}

bool sp::Twine::is_not_upper() const noexcept
{
	return !is_upper();
}

bool sp::Twine::is_ascii() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return c <= 127; });
}

bool sp::Twine::is_not_ascii() const noexcept
{
	return !is_ascii();
}

bool sp::Twine::is_printable() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isprint(c); });
}

bool sp::Twine::is_not_printable() const noexcept
{
	return !is_printable();
}

bool sp::Twine::is_graphical() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isgraph(c); });
}

bool sp::Twine::is_not_graphical() const noexcept
{
	return !is_graphical();
}

bool sp::Twine::is_punctuation() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::ispunct(c); });
}

bool sp::Twine::is_not_punctuation() const noexcept
{
	return !is_punctuation();
}

bool sp::Twine::is_control() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::iscntrl(c); });
}

bool sp::Twine::is_not_control() const noexcept
{
	return !is_control();
}

bool sp::Twine::is_hex() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) { return std::isxdigit(c); });
}

bool sp::Twine::is_not_hex() const noexcept
{
	return !is_hex();
}

bool sp::Twine::is_base64() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) {
		return (std::isalnum(c) || c == '+' || c == '/' || c == '=');
		});
}

bool sp::Twine::is_not_base64() const noexcept
{
	return !is_base64();
}

bool sp::Twine::is_utf8() const noexcept
{
	return sp::utf8::is_valid(*this);
}

bool sp::Twine::is_not_utf8() const noexcept
{
	return !is_utf8();
}

bool sp::Twine::is_regex_string() const noexcept
{
	return std::all_of(m_data.begin(), m_data.end(), [](unsigned char c) {
		return (std::isalnum(c) || std::ispunct(c) || std::isspace(c));
		});
}

bool sp::Twine::is_not_regex_string() const noexcept
{
	return !is_regex_string();
}

void sp::Twine::reset(const String& _data)
{
	m_data = _data;
}

void sp::Twine::reset(String&& _data) noexcept
{
	m_data = std::move(_data);
}

void sp::Twine::reset(const char* _data) noexcept
{
	m_data = _data;
}

void sp::Twine::reset(const Twine& _data)
{
	m_data = _data.m_data;
}

void sp::Twine::reset(Twine&& _data) noexcept
{
	m_data = std::move(_data.m_data);
}

void sp::Twine::reset()
{
	m_data.clear();
}

void sp::Twine::reverse()
{
	m_data = sp::utf8::reverse(*this).s_str();
}

sp::Twine::size_type sp::Twine::count(char ch) const
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

sp::Twine::size_type sp::Twine::count(const Twine& str) const
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

sp::Twine::size_type sp::Twine::count(String_view str) const
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

void sp::Twine::join(Twine data)
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

	Twine result;
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

void sp::Twine::join(const String& data)
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

	Twine result;
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

void sp::Twine::join(const char* data)
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

	Twine result;
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

void sp::Twine::join(char data)
{
	if (m_data.empty())
		throw sp::Empty_String("Cannot join to an empty string.");

	size_t total_len = 0;
	for (const auto& part : m_data) {
		total_len += 1;
	}
	total_len += 1 * (m_data.size() - 1);

	Twine result;
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

sp::Twine sp::Twine::create()
{
    return Twine{ "" };
}

istream& operator>>(istream& is, sp::Twine& str)
{
    String data;

    is >> data;
    str = data;

    return is;
}

ostream& operator<<(ostream& os, const sp::Twine& str)
{
	return os << str.s_str();
}

std::istream& sp::getline(std::istream& is, Twine& str, char delim)
{
	std::getline(is, str.m_data, delim);
	return is;
}

sp::Twine sp::operator""_sp(const char* data, size_t size)
{
	return Twine(data, size);
}

sp::Invalid_Format::Invalid_Format(const String& msg) : String_Error(msg) {}

sp::Invalid_Format::Invalid_Format(const char* msg) : String_Error(msg ? msg : "One or more errors occurred during the formatting operation.") {}

sp::Twine::size_type sp::Twine::find_first_not_of(const Twine& chars, pos_type pos) const noexcept
{
	return m_data.find_first_not_of(chars.m_data, pos);
}

sp::Twine::size_type sp::Twine::find_first_not_of(const char* chars, pos_type pos) const noexcept
{
	return m_data.find_first_not_of(chars, pos);
}

sp::Twine::size_type sp::Twine::find_first_not_of(char ch, pos_type pos) const noexcept
{
	return m_data.find_first_not_of(ch, pos);
}

sp::Twine::size_type sp::Twine::find_last_not_of(const Twine& chars, pos_type pos) const noexcept
{
	return m_data.find_last_not_of(chars.m_data, pos);
}

sp::Twine::size_type sp::Twine::find_last_not_of(const char* chars, pos_type pos) const noexcept
{
	return m_data.find_last_not_of(chars, pos);
}

sp::Twine::size_type sp::Twine::find_last_not_of(char ch, pos_type pos) const noexcept
{
	return m_data.find_last_not_of(ch, pos);
}

void sp::swap(Twine& lhs, Twine& rhs) noexcept
{
	lhs.swap(rhs);
}
