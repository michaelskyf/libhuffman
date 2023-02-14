#include "huffman/HuffmanNode.hpp"
#include "encoder/ByteWriter.hpp"
#include <encoder/ByteEncoder.hpp>
#include <gtest/gtest.h>

using namespace huffman::encoder;
using namespace huffman;

TEST(ByteEncoder, single_node)
{
	char buffer[1024];
	HuffmanNode node{0, 0};
	ByteWriter writer(buffer, sizeof(buffer), 0);
	ByteEncoder encoder(writer, node);

	bool was_successful = encoder.encode('x');

	EXPECT_TRUE(was_successful); // Single node doen't require any buffer to write
	EXPECT_EQ(encoder.bitsWritten(), 0);
	EXPECT_EQ(encoder.maxBits(), sizeof(buffer)*8);
}

TEST(ByteEncoder, tree)
{
	char buffer[1024];
	HuffmanNode node{
		{'A', 1},
		{'B', 1}
	};

	ByteWriter writer(buffer, sizeof(buffer), 0);
	ByteEncoder encoder(writer, node);

	bool was_successful = encoder.encode('A');

	EXPECT_TRUE(was_successful);
	EXPECT_EQ(encoder.bitsWritten(), 1);
	EXPECT_EQ(encoder.maxBits(), sizeof(buffer)*8);
}

TEST(ByteEncoder, tree_no_memory)
{
	HuffmanNode node{
		{'A', 1},
		{'B', 1}
	};

	ByteWriter writer(nullptr, 0, 0);
	ByteEncoder encoder(writer, node);

	bool was_successful = encoder.encode('A');

	EXPECT_FALSE(was_successful);
	EXPECT_EQ(encoder.bitsWritten(), 0);
	EXPECT_EQ(encoder.maxBits(), 0);
}

TEST(ByteEncoder, tree_not_enough_memory)
{
	char buffer[1];
	HuffmanNode node{
		{'A', 1},
		{'B', 1}
	};

	ByteWriter writer(buffer, sizeof(buffer), 7);
	ByteEncoder encoder(writer, node);

	EXPECT_TRUE(encoder.encode('A'));
	EXPECT_FALSE(encoder.encode('B'));

	EXPECT_EQ(encoder.bitsWritten(), 8);
	EXPECT_EQ(encoder.maxBits(), sizeof(buffer)*8);
}