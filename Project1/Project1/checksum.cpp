#include "checksum.h"

namespace
{
	struct Crc32Table
	{
		std::uint32_t entries[256]{};

		constexpr Crc32Table() noexcept
		{
			for (std::uint32_t i = 0; i < 256; ++i)
			{
				std::uint32_t c = i;

				for (int k = 0; k < 8; ++k)
					c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);

				entries[i] = c;
			}
		}
	};

	constexpr Crc32Table CRC32_TABLE{};

	String hex_string(std::uint64_t value, int digits)
	{
		String out(static_cast<std::size_t>(digits), '0');

		for (int i = digits - 1; i >= 0; --i)
		{
			out[static_cast<std::size_t>(i)] = "0123456789abcdef"[value & 0xF];
			value >>= 4;
		}

		return out;
	}
}

std::uint32_t sp::checksum::crc32(const Twine& data) noexcept
{
	std::uint32_t crc = 0xFFFFFFFFu;

	for (const char ch : data.s_str())
	{
		const unsigned char byte = static_cast<unsigned char>(ch);
		crc = CRC32_TABLE.entries[(crc ^ byte) & 0xFFu] ^ (crc >> 8);
	}

	return crc ^ 0xFFFFFFFFu;
}

sp::Twine sp::checksum::crc32_hex(const Twine& data)
{
	return Twine{ hex_string(crc32(data), 8) };
}

std::uint64_t sp::checksum::fnv1a64(const Twine& data) noexcept
{
	std::uint64_t hash = 0xCBF29CE484222325ull;

	for (const char ch : data.s_str())
	{
		hash ^= static_cast<unsigned char>(ch);
		hash *= 0x100000001B3ull;
	}

	return hash;
}

sp::Twine sp::checksum::fnv1a64_hex(const Twine& data)
{
	return Twine{ hex_string(fnv1a64(data), 16) };
}

std::uint32_t sp::checksum::adler32(const Twine& data) noexcept
{
	constexpr std::uint32_t MOD_ADLER = 65521u;

	std::uint32_t a = 1;
	std::uint32_t b = 0;

	for (const char ch : data.s_str())
	{
		a = (a + static_cast<unsigned char>(ch)) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

sp::Twine sp::checksum::adler32_hex(const Twine& data)
{
	return Twine{ hex_string(adler32(data), 8) };
}
