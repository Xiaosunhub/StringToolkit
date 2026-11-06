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

#include "json.h"
#include "method.h"
#include "utf8.h"
#include <charconv>
#include <cmath>

using sp::cppString;
using sp::json::Type;
using sp::json::Value;

Value::Value() = default;

Value::Value(std::nullptr_t) {}

Value::Value(bool value)
	: m_type(Type::Bool), m_bool(value) {}

Value::Value(const char* value)
	: m_type(Type::String), m_string(value) {}

Value::Value(const cppString& value)
	: m_type(Type::String), m_string(value) {}

Value::Value(const array_t& value)
	: m_type(Type::Array), m_array(value) {}

Value::Value(const object_t& value)
	: m_type(Type::Object), m_object(value) {}

bool Value::is_integer() const noexcept
{
	if (m_type != Type::Number)
		return false;

	constexpr double LO = static_cast<double>(std::numeric_limits<long long>::min());
	constexpr double HI = static_cast<double>(std::numeric_limits<long long>::max());

	return m_number >= LO && m_number <= HI && std::trunc(m_number) == m_number;
}

bool Value::as_bool() const
{
	if (m_type != Type::Bool)
		throw sp::Invalid_Operation("json::Value is not a bool.");

	return m_bool;
}

double Value::as_number() const
{
	if (m_type != Type::Number)
		throw sp::Invalid_Operation("json::Value is not a number.");

	return m_number;
}

long long Value::as_int() const
{
	if (!is_integer())
		throw sp::Invalid_Operation("json::Value is not an integer.");

	return static_cast<long long>(m_number);
}

const cppString& Value::as_string() const
{
	if (m_type != Type::String)
		throw sp::Invalid_Operation("json::Value is not a string.");

	return m_string;
}

const Value::array_t& Value::as_array() const
{
	if (m_type != Type::Array)
		throw sp::Invalid_Operation("json::Value is not an array.");

	return m_array;
}

const Value::object_t& Value::as_object() const
{
	if (m_type != Type::Object)
		throw sp::Invalid_Operation("json::Value is not an object.");

	return m_object;
}

Value::array_t& Value::array_items()
{
	if (m_type == Type::Null)
	{
		m_type = Type::Array;
		m_array.clear();
	}

	if (m_type != Type::Array)
		throw sp::Invalid_Operation("json::Value is not an array.");

	return m_array;
}

Value::object_t& Value::object_items()
{
	if (m_type == Type::Null)
	{
		m_type = Type::Object;
		m_object.clear();
	}

	if (m_type != Type::Object)
		throw sp::Invalid_Operation("json::Value is not an object.");

	return m_object;
}

const Value& Value::operator[](std::size_t index) const
{
	if (m_type != Type::Array)
		throw sp::Invalid_Operation("json::Value is not an array.");

	if (index >= m_array.size())
		throw sp::Index_Out_Of_Range("json::Value array index out of range.");

	return m_array[index];
}

const Value& Value::operator[](const cppString& key) const
{
	static const Value NULL_VALUE{};

	if (m_type != Type::Object)
		throw sp::Invalid_Operation("json::Value is not an object.");

	for (const auto& [k, v] : m_object)
	{
		if (k == key)
			return v;
	}

	return NULL_VALUE;
}

bool Value::contains(const cppString& key) const noexcept
{
	if (m_type != Type::Object)
		return false;

	for (const auto& [k, v] : m_object)
	{
		if (k == key)
			return true;
	}

	return false;
}

std::size_t Value::size() const
{
	switch (m_type)
	{
	case Type::Array:  return m_array.size();
	case Type::Object: return m_object.size();
	case Type::String: return sp::utf8::length(m_string);
	default:           return 0;
	}
}

bool Value::operator==(const Value& other) const
{
	if (m_type != other.m_type)
		return false;

	switch (m_type)
	{
	case Type::Null:   return true;
	case Type::Bool:   return m_bool == other.m_bool;
	case Type::Number: return m_number == other.m_number;
	case Type::String: return m_string == other.m_string;
	case Type::Array:  return m_array == other.m_array;
	case Type::Object: return m_object == other.m_object;
	}

	return false;
}

namespace
{
	cppString number_to_string(double value)
	{
		// Integers print without a decimal point.
		constexpr double LO = static_cast<double>(std::numeric_limits<long long>::min());
		constexpr double HI = static_cast<double>(std::numeric_limits<long long>::max());

		if (value >= LO && value <= HI && std::trunc(value) == value)
			return cppString{ std::to_string(static_cast<long long>(value)) };

		char buf[64];
		const auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);

		if (ec != std::errc())
			throw sp::Invalid_Conversion("Cannot serialize JSON number.");

		return cppString{ String(buf, static_cast<std::size_t>(ptr - buf)) };
	}

	void dump_into(const Value& v, int indent, int depth, String& out)
	{
		auto pad = [&](int d)
		{
			if (indent >= 0)
				out.append(static_cast<std::size_t>(d * indent), ' ');
		};

		switch (v.type())
		{
		case Type::Null:
			out += "null";
			return;
		case Type::Bool:
			out += v.as_bool() ? "true" : "false";
			return;
		case Type::Number:
			out += number_to_string(v.as_number()).s_str();
			return;
		case Type::String:
			out += '"';
			out += sp::json::escape(v.as_string()).s_str();
			out += '"';
			return;
		case Type::Array:
		{
			const auto& items = v.as_array();

			if (items.empty())
			{
				out += "[]";
				return;
			}

			out += '[';

			for (std::size_t i = 0; i < items.size(); ++i)
			{
				if (i > 0)
					out += ',';

				if (indent >= 0)
				{
					out += '\n';
					pad(depth + 1);
				}

				dump_into(items[i], indent, depth + 1, out);
			}

			if (indent >= 0)
			{
				out += '\n';
				pad(depth);
			}

			out += ']';
			return;
		}
		case Type::Object:
		{
			const auto& items = v.as_object();

			if (items.empty())
			{
				out += "{}";
				return;
			}

			out += '{';

			for (std::size_t i = 0; i < items.size(); ++i)
			{
				if (i > 0)
					out += ',';

				if (indent >= 0)
				{
					out += '\n';
					pad(depth + 1);
				}

				out += '"';
				out += sp::json::escape(items[i].first).s_str();
				out += "\":";
				out += indent >= 0 ? " " : "";

				dump_into(items[i].second, indent, depth + 1, out);
			}

			if (indent >= 0)
			{
				out += '\n';
				pad(depth);
			}

			out += '}';
			return;
		}
		}
	}
}

cppString Value::dump(int indent) const
{
	String out;
	dump_into(*this, indent, 0, out);
	return cppString(std::move(out));
}

namespace
{
	// Recursive-descent parser over the raw bytes.
	class Parser
	{
	public:
		explicit Parser(const String& src) : m_src(src) {}

		Value run()
		{
			skip_ws();
			Value v = parse_value();
			skip_ws();

			if (m_pos != m_src.size())
				fail("Trailing characters after the JSON document");

			return v;
		}

	private:
		const String& m_src;
		std::size_t m_pos = 0;

		[[noreturn]] void fail(const String& what) const
		{
			throw sp::Invalid_Conversion(
				"JSON parse error at byte " + std::to_string(m_pos) + ": " + what);
		}

		void skip_ws() noexcept
		{
			while (m_pos < m_src.size()
				&& (m_src[m_pos] == ' ' || m_src[m_pos] == '\t'
					|| m_src[m_pos] == '\n' || m_src[m_pos] == '\r'))
			{
				++m_pos;
			}
		}

		char peek() const noexcept
		{
			return m_pos < m_src.size() ? m_src[m_pos] : '\0';
		}

		bool consume(char c) noexcept
		{
			if (peek() == c)
			{
				++m_pos;
				return true;
			}

			return false;
		}

		void expect(char c)
		{
			if (!consume(c))
				fail(String("Expected '") + c + "'");
		}

		void expect_literal(const char* word)
		{
			for (const char* p = word; *p; ++p)
			{
				if (m_pos >= m_src.size() || m_src[m_pos] != *p)
					fail(String("Invalid literal, expected '") + word + "'");

				++m_pos;
			}
		}

		Value parse_value()
		{
			switch (peek())
			{
			case 'n': expect_literal("null");  return Value{ nullptr };
			case 't': expect_literal("true");  return Value{ true };
			case 'f': expect_literal("false"); return Value{ false };
			case '"': return Value{ cppString(parse_string()) };
			case '[': return parse_array();
			case '{': return parse_object();
			default:
				if (peek() == '-' || (peek() >= '0' && peek() <= '9'))
					return parse_number();

				fail(m_pos < m_src.size() ? "Unexpected character" : "Unexpected end of input");
			}
		}

		Value parse_number()
		{
			const std::size_t start = m_pos;

			consume('-');

			if (consume('0'))
			{
				if (peek() >= '0' && peek() <= '9')
					fail("Leading zeros are not allowed in numbers");
			}
			else if (peek() >= '1' && peek() <= '9')
			{
				while (peek() >= '0' && peek() <= '9')
					++m_pos;
			}
			else
			{
				fail("Invalid number");
			}

			if (consume('.'))
			{
				if (!(peek() >= '0' && peek() <= '9'))
					fail("Fraction needs at least one digit");

				while (peek() >= '0' && peek() <= '9')
					++m_pos;
			}

			if (peek() == 'e' || peek() == 'E')
			{
				++m_pos;

				if (peek() == '+' || peek() == '-')
					++m_pos;

				if (!(peek() >= '0' && peek() <= '9'))
					fail("Exponent needs at least one digit");

				while (peek() >= '0' && peek() <= '9')
					++m_pos;
			}

			const String text = m_src.substr(start, m_pos - start);
			double value = 0.0;
			const auto [ptr, ec] = std::from_chars(
				text.data(), text.data() + text.size(), value);

			if (ec != std::errc() || ptr != text.data() + text.size())
				fail("Number out of range");

			return Value{ value };
		}

		String parse_string()
		{
			expect('"');
			String raw;

			while (true)
			{
				if (m_pos >= m_src.size())
					fail("Unterminated string");

				const char c = m_src[m_pos];

				if (c == '"')
				{
					++m_pos;
					break;
				}

				if (c == '\\')
				{
					// Keep the escape verbatim; json::unescape decodes it,
					// including \uXXXX surrogate pairs.
					raw += c;
					++m_pos;

					if (m_pos >= m_src.size())
						fail("Unterminated escape sequence");

					raw += m_src[m_pos];
					++m_pos;

					if (raw.back() == 'u')
					{
						for (int i = 0; i < 4; ++i)
						{
							if (m_pos >= m_src.size())
								fail("Unterminated \\u escape");

							raw += m_src[m_pos];
							++m_pos;
						}
					}

					continue;
				}

				if (static_cast<unsigned char>(c) < 0x20)
					fail("Unescaped control character in string");

				raw += c;
				++m_pos;
			}

			return sp::json::unescape(cppString(std::move(raw))).s_str();
		}

		Value parse_array()
		{
			expect('[');
			skip_ws();

			Value::array_t items;

			if (consume(']'))
				return Value{ items };

			while (true)
			{
				skip_ws();
				items.push_back(parse_value());
				skip_ws();

				if (consume(']'))
					break;

				expect(',');
			}

			return Value{ items };
		}

		Value parse_object()
		{
			expect('{');
			skip_ws();

			Value::object_t items;

			if (consume('}'))
				return Value{ items };

			while (true)
			{
				skip_ws();

				if (peek() != '"')
					fail("Object keys must be strings");

				cppString key{ parse_string() };
				skip_ws();
				expect(':');
				skip_ws();
				items.emplace_back(std::move(key), parse_value());
				skip_ws();

				if (consume('}'))
					break;

				expect(',');
			}

			return Value{ items };
		}
	};
}

Value sp::json::parse(const cppString& text)
{
	return Parser(text.s_str()).run();
}
