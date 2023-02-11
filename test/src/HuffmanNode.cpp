#include <HuffmanNode.hpp>
#include <gtest/gtest.h>

using namespace huffman;

TEST(HuffmanNode, character_node)
{
	HuffmanNode node{'c', 17};

	EXPECT_TRUE(node.is_byte_node());
	EXPECT_EQ(node.frequency(), 17);
	EXPECT_EQ(node.byte(), 'c');
}

TEST(HuffmanNode, tree_node)
{
	HuffmanNode node{
		{'\0', 7},
		{'\0', 5}
	};

	EXPECT_FALSE(node.is_byte_node());
	EXPECT_EQ(node.frequency(), 12);
	EXPECT_EQ(node.left()->frequency(), 7);
	EXPECT_EQ(node.right()->frequency(), 5);
}