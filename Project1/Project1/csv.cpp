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

#include "csv.h"
#include <algorithm>

namespace
{
	sp::cppString escape_field(const sp::cppString& field, char delimiter, bool always)
	{
		const String& src = field.s_str();
		bool quoted = always;

		if (!quoted)
		{
			for (const char c : src)
			{
				if (c == delimiter || c == '"' || c == '\r' || c == '\n')
				{
					quoted = true;
					break;
				}
			}
		}

		if (!quoted)
			return field;

		String out;
		out.reserve(src.size() + 2);
		out += '"';

		for (const char c : src)
		{
			if (c == '"')
				out += '"';

			out += c;
		}

		out += '"';
		return sp::cppString(std::move(out));
	}
}

sp::csv::Document::Document(Row header)
	: m_header(std::move(header)), m_has_header(true)
{
}

const sp::csv::Document::Row& sp::csv::Document::header() const noexcept
{
	static const Row empty{};
	return m_has_header ? m_header : empty;
}

void sp::csv::Document::set_header(Row header)
{
	m_header = std::move(header);
	m_has_header = true;
}

void sp::csv::Document::clear_header() noexcept
{
	m_header.clear();
	m_has_header = false;
}

void sp::csv::Document::add_row(Row row)
{
	m_rows.push_back(std::move(row));
}

std::size_t sp::csv::Document::column_count() const noexcept
{
	std::size_t width = m_has_header ? m_header.size() : 0;

	for (const Row& row : m_rows)
		width = (std::max)(width, row.size());

	return width;
}

const sp::csv::Document::Row& sp::csv::Document::operator[](std::size_t row) const
{
	if (row >= m_rows.size())
		throw sp::Index_Out_Of_Range("CSV row index out of range in Document::operator[].");

	return m_rows[row];
}

sp::cppString sp::csv::Document::at(std::size_t row, std::size_t column) const
{
	if (row >= m_rows.size())
		throw sp::Index_Out_Of_Range("CSV row index out of range in Document::at.");

	if (column >= m_rows[row].size())
		return cppString{};

	return m_rows[row][column];
}

std::size_t sp::csv::Document::column_index(const cppString& column) const noexcept
{
	if (!m_has_header)
		return String::npos;

	for (std::size_t i = 0; i < m_header.size(); ++i)
	{
		if (m_header[i] == column)
			return i;
	}

	return String::npos;
}

sp::cppString sp::csv::Document::get(std::size_t row, const cppString& column) const
{
	if (!m_has_header)
		throw sp::Invalid_Argument("CSV Document has no header in Document::get.");

	const std::size_t index = column_index(column);

	if (index == String::npos)
		throw sp::Invalid_Argument("Unknown CSV column in Document::get: " + column.s_str());

	return at(row, index);
}

char sp::csv::sniff_delimiter(const cppString& text) noexcept
{
	constexpr char candidates[] = { ',', ';', '\t', '|' };
	const String& src = text.s_str();

	std::size_t counts[4] = { 0, 0, 0, 0 };
	bool in_quotes = false;

	for (std::size_t i = 0; i < src.size(); ++i)
	{
		const char c = src[i];

		if (c == '"')
		{
			if (in_quotes && i + 1 < src.size() && src[i + 1] == '"')
			{
				++i;
				continue;
			}

			in_quotes = !in_quotes;
			continue;
		}

		if (in_quotes)
			continue;

		if (c == '\n' || c == '\r')
			break; // only the first record is sampled

		for (int k = 0; k < 4; ++k)
		{
			if (c == candidates[k])
				++counts[k];
		}
	}

	std::size_t best = 0;

	for (int k = 1; k < 4; ++k)
	{
		if (counts[k] > counts[best])
			best = static_cast<std::size_t>(k);
	}

	return counts[best] == 0 ? ',' : candidates[best];
}

sp::csv::Document sp::csv::parse(const cppString& text, char delimiter, bool has_header)
{
	if (delimiter == '\0')
		delimiter = sniff_delimiter(text);

	const String& src = text.s_str();

	std::vector<Document::Row> records;
	Document::Row current;
	String field;
	bool in_quotes = false;

	auto end_field = [&]()
	{
		current.push_back(cppString(std::move(field)));
		field.clear();
	};

	auto end_record = [&]()
	{
		end_field();
		records.push_back(std::move(current));
		current.clear();
	};

	for (std::size_t i = 0; i < src.size(); ++i)
	{
		const char c = src[i];

		if (in_quotes)
		{
			if (c == '"')
			{
				if (i + 1 < src.size() && src[i + 1] == '"')
				{
					field += '"';
					++i;
				}
				else
				{
					in_quotes = false;
				}
			}
			else if (c == '\r')
			{
				if (i + 1 < src.size() && src[i + 1] == '\n')
					++i;

				field += '\n';
			}
			else
			{
				field += c;
			}
		}
		else
		{
			if (c == '"' && field.empty())
			{
				in_quotes = true;
			}
			else if (c == delimiter)
			{
				end_field();
			}
			else if (c == '\r' || c == '\n')
			{
				if (c == '\r' && i + 1 < src.size() && src[i + 1] == '\n')
					++i;

				end_record();
			}
			else
			{
				field += c;
			}
		}
	}

	if (in_quotes)
		throw sp::Invalid_Conversion("Unterminated quoted field in csv::parse.");

	// A trailing line break already closed the last record.
	if (!field.empty() || !current.empty())
		end_record();

	Document doc;

	if (has_header && !records.empty())
	{
		doc.set_header(std::move(records.front()));
		records.erase(records.begin());
	}

	doc.rows() = std::move(records);
	return doc;
}

sp::cppString sp::csv::write(const Document& doc, char delimiter,
	bool always_quote, const cppString& line_ending)
{
	String out;

	auto write_row = [&](const Document::Row& row)
	{
		for (std::size_t i = 0; i < row.size(); ++i)
		{
			if (i > 0)
				out += delimiter;

			out += escape_field(row[i], delimiter, always_quote).s_str();
		}

		out += line_ending.s_str();
	};

	if (doc.has_header())
		write_row(doc.header());

	for (const auto& row : doc.rows())
		write_row(row);

	return cppString(std::move(out));
}

std::vector<sp::csv::Document::Row> sp::csv::parse_rows(const cppString& text, char delimiter)
{
	Document doc = parse(text, delimiter, false);
	return std::move(doc.rows());
}

sp::cppString sp::csv::write_rows(const std::vector<Document::Row>& rows,
	char delimiter, const cppString& line_ending)
{
	Document doc;
	doc.rows() = rows;
	return write(doc, delimiter, false, line_ending);
}
