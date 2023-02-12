#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>

#include <HuffmanDictionary.hpp>
#include "HuffmanNode.hpp"
#include "decoder/ByteLoader.hpp"
#include "decoder/ByteDecoder.hpp"
#include "encoder/ByteWriter.hpp"
#include "encoder/ByteEncoder.hpp"


namespace huffman
{

namespace
{

using frequency_queue = std::vector<HuffmanNode>;

void get_frequencies(std::array<size_t, 256>& array, const char* src, size_t src_size)
{
	for(size_t i = 0; i < src_size; i++)
	{
		unsigned char index = static_cast<unsigned char>(src[i]);

		array[index]++;
	}
}

void get_frequencies(std::array<size_t, 256>& array, const HuffmanNode& node)
{
	if(node.is_byte_node())
	{
		unsigned char index = static_cast<unsigned char>(node.byte());

		array[index] += node.frequency();
	}
	else
	{
		get_frequencies(array, *node.left());
		get_frequencies(array, *node.right());
	}
}

HuffmanNode makeTreeNode(frequency_queue& frequencies)
{
	HuffmanNode child_left = std::move(frequencies[0]);
	HuffmanNode child_right = std::move(frequencies[1]);

	frequencies.erase(frequencies.begin(), frequencies.begin()+2);

	return {std::move(child_left), std::move(child_right)};
}

HuffmanNode make_huffman_tree(frequency_queue& frequencies)
{
	if(frequencies.size() == 0)
	{
		return {0, 0};
	}

	while(frequencies.size() > 1)
	{
		auto new_node = makeTreeNode(frequencies);

		frequencies.emplace(std::find_if(frequencies.begin(),
									frequencies.end(),
									[&](const HuffmanNode& n)
									{ return n.frequency() >= new_node.frequency(); }),
									std::move(new_node));
	}

	HuffmanNode result = std::move(frequencies.front());

	return result;
}

} // unnamed namespace

HuffmanDictionary::HuffmanDictionary(const char* src, size_t src_size)
{
	create(src, src_size);
}

HuffmanDictionary::HuffmanDictionary(const HuffmanNode& root)
{
	m_root = root;
}

void HuffmanDictionary::create(const char* src, size_t src_size)
{
	m_root = {0, 0};
	create_part(src, src_size);
}

const HuffmanNode& HuffmanDictionary::data() const
{
	return m_root;
}

void HuffmanDictionary::create_part(const char* src, size_t src_size)
{
	std::array<size_t, 256> byte_frequencies{};

	// Get frequencies from the source
	get_frequencies(byte_frequencies, src, src_size);

	// Get frequencies from the already existing tree
	get_frequencies(byte_frequencies, m_root);

	// Convert the array to priority_queue
	frequency_queue frequencies;
	for(size_t i = 0; i < byte_frequencies.size(); i++)
	{
		size_t freq = byte_frequencies[i];
		// Trim bytes that do not appear
		if(freq > 0)
		{
			frequencies.emplace(std::find_if(frequencies.begin(),
								frequencies.end(),
								[&](const HuffmanNode& n)
								{ return n.frequency() >= freq; }),
								static_cast<char>(i), freq);
		}
	}

	// Make the new root
	m_root = make_huffman_tree(frequencies);
}

size_t HuffmanDictionary::size() const
{
	return m_root.frequency();
}

bool HuffmanDictionary::empty() const
{
	return size() == 0;
}

std::pair<size_t, size_t> HuffmanDictionary::encode(const char* src, size_t src_size, char* dst, size_t dst_size, size_t offset)
{
	encoder::ByteWriter writer(dst, dst_size, offset);
	encoder::ByteEncoder encoder(writer, m_root);
	for(size_t si = 0; si < src_size; si++)
	{
		bool has_space = encoder.encode(src[si]);
		if(!has_space)
		{
			return {si, offset};
		}

		offset = encoder.bitsWritten();
	}

	return {src_size, offset};
}

std::pair<size_t, size_t> HuffmanDictionary::decode(const char* src, size_t src_size, char* dst, size_t dst_size, size_t offset)
{
	decoder::ByteLoader loader(src, src_size, offset);
	decoder::ByteDecoder decoder(loader, m_root);

	for(size_t di = 0; di < dst_size; di++)
	{
		auto[byte, is_set] = decoder.decode();
		if(!is_set)
		{
			return {offset, di};
		}

		dst[di] = byte;
		offset = decoder.bitsProcessed();
	}

	return {offset, dst_size};
}

} // namespace huffman
