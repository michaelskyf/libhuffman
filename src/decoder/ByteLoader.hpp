#pragma once

#include <algorithm>

namespace huffman::decoder
{

class ByteLoader 
{
public:

	ByteLoader(const char* src, size_t src_size, size_t offset)
	: m_src{src},
	  m_shift{0},
	  m_bits_processed{0},
	  m_total_bits{src_size*8}
	{
		*this >>= offset;
	}

	bool empty() const
	{
		return m_bits_processed >= m_total_bits;
	}

	char value() const
	{
		return empty() ? '\0' : static_cast<char>(*m_src >> m_shift);
	}

	size_t bitsProcessed() const
	{
		return m_bits_processed;
	}

	char operator>>=(size_t right_shift)
	{
		if(right_shift < m_total_bits - m_bits_processed)
		{
			m_bits_processed += right_shift;
		}
		else
		{
			m_bits_processed = m_total_bits;
		}

		m_src += (m_shift + right_shift) / 8;
		m_shift = (m_shift + right_shift) % 8;

		return value();
	}

	size_t maxBits() const
	{
		return m_total_bits;
	}

private:
	const char* m_src;
	size_t m_shift;
	size_t m_bits_processed;
	const size_t m_total_bits;
};

} // namespace huffman::decoder