#pragma once

#include <huffman/HuffmanNode.hpp>
#include "ByteLoader.hpp"

namespace huffman::decoder
{

class ByteDecoder
{
public:

	ByteDecoder(const ByteLoader& loader, const HuffmanNode& root_node)
		: m_loader{loader}, m_root_node{root_node}
	{

	}

	std::pair<char, bool> decode()
	{
		const HuffmanNode* current_node = &m_root_node;

		while(!m_loader.empty() && !current_node->is_byte_node())
		{
			if(m_loader.value() & 1)
			{
				current_node = current_node->left();
			}
			else
			{
				current_node = current_node->right();
			}

			m_loader >>= 1;
		}

		return {current_node->byte(), current_node->is_byte_node()};
	}

	size_t bitsProcessed() const
	{
		return m_loader.bitsProcessed();
	}

	size_t maxBits() const
	{
		return m_loader.maxBits();
	}

private:
	ByteLoader m_loader;
	const HuffmanNode& m_root_node;
};

} // namespace huffman::decoder