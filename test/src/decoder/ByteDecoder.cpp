#include "HuffmanNode.hpp"
#include <decoder/ByteDecoder.hpp>
#include <gtest/gtest.h>
#include <type_traits>

using namespace huffman::decoder;
using namespace huffman;

TEST(decoder_ByteDecoder, decode_null)
{
	ByteLoader loader(nullptr, 0, 0);
	HuffmanNode root
	{
		{'a', 1},
		{'b', 2},
	};

	ByteDecoder decoder(loader, root);

	auto[byte, is_set] = decoder.decode();

	EXPECT_FALSE(is_set);
	EXPECT_EQ(decoder.bitsProcessed(), 0);
}