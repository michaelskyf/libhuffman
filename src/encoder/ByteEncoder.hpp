#pragma once

#include <array>
#include "huffman/HuffmanNode.hpp"
#include "encoder/ByteWriter.hpp"

namespace huffman::encoder
{

class ByteEncoder
{
public:
	ByteEncoder(const ByteWriter& writer, const HuffmanNode& root_node);

	bool encode(char byte);

	size_t bitsWritten() const;
	size_t maxBits() const;

private:
	ByteWriter m_writer;
	std::array<std::pair<uint64_t, size_t>, 256> m_lookup_table{};
};

} // namespace huffman::encoder