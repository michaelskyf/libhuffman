#include <cstddef>
#include <cstdint>
#include <algorithm>

#include "ByteWriter.hpp"
#include "encoder/ByteWriter.hpp"

namespace
{

std::pair<char, size_t> encode_byte(char byte, size_t bits_set, uint64_t& code, size_t& length)
{
	char real_byte = static_cast<char>(byte & ~(0xff << bits_set));
	size_t bits_written = std::min(8-bits_set, length);

	real_byte |= static_cast<char>(code << bits_set);
	length -= bits_written;
	code >>= bits_written;

	return {real_byte, bits_set + bits_written};
}

} // namespace

namespace huffman::encoder
{

ByteWriter::ByteWriter(char* dst, size_t dst_size, size_t offset)
	: m_dst{dst},
	  m_bits_written{offset},
	  m_shift{offset%8},
	  m_total_bits{dst_size*8}
{
	dst += offset / 8;
	if(offset > m_total_bits)
	{
		m_bits_written = m_total_bits;
	}
}

size_t ByteWriter::bitsWritten() const
{
	return m_bits_written;
}

bool ByteWriter::empty() const
{
	return m_bits_written == m_total_bits;
}

size_t ByteWriter::maxBits() const
{
	return m_total_bits;
}

bool ByteWriter::write(uint64_t code, size_t length)
{
	if(length > m_total_bits - m_bits_written)
	{
		return false;
	}

	m_bits_written += length;

	while(length)
	{
		auto[byte, new_shift] = encode_byte(*m_dst, m_shift, code, length);

		*m_dst = byte;
		m_shift = new_shift % 8;

		if(m_shift == 0)
		{
			m_dst++;
		}
	}

	return true;
}

} // namespace huffman::encoder