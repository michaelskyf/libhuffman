#include <array>
#include "huffman/HuffmanNode.hpp"
#include "ByteEncoder.hpp"
#include "ByteWriter.hpp"
#include "encoder/ByteEncoder.hpp"

namespace
{

uint64_t reverse_code(uint64_t code, size_t depth)
{
	uint64_t new_code = 0;
	while(depth--)
	{
		new_code |= (code & 1) << depth;
		code >>= 1;
	}

	return new_code;
}

void make_lookup_table(const huffman::HuffmanNode& node, std::array<std::pair<uint64_t, size_t>, 256>& lookup_table, uint64_t code, size_t depth)
{
	if(node.is_byte_node())
	{
		lookup_table[static_cast<unsigned char>(node.byte())] = std::make_pair(reverse_code(code, depth), depth);
	}
	else
	{
		make_lookup_table(*node.left(), lookup_table, (code << 1) | 1, depth+1);
		make_lookup_table(*node.right(), lookup_table, code << 1, depth+1);
	}
}

} // namespace

namespace huffman::encoder
{

ByteEncoder::ByteEncoder(const ByteWriter& writer, const HuffmanNode& root_node)
	: m_writer{writer}
{
	// TODO: init lookup table
	make_lookup_table(root_node, m_lookup_table, 0, 0);
}

bool ByteEncoder::encode(char byte)
{
	unsigned char index = static_cast<unsigned char>(byte);
	auto[code, length] = m_lookup_table[index];
	return m_writer.write(code, length);
}

size_t ByteEncoder::bitsWritten() const
{
	return m_writer.bitsWritten();
}

size_t ByteEncoder::maxBits() const
{
	return m_writer.maxBits();
}

} // namespace huffman::encoder