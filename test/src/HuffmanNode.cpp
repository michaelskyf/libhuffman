#include <HuffmanNode.hpp>
#include <gtest/gtest.h>

using namespace huffman;

namespace
{

void compare_trees(const HuffmanNode& lhs, const HuffmanNode& rhs)
{
	EXPECT_EQ(lhs.is_byte_node(), rhs.is_byte_node());
	if(lhs.is_byte_node() != rhs.is_byte_node())
	{
		return;
	}

	bool is_byte_node = lhs.is_byte_node();

	if(is_byte_node)
	{
		EXPECT_EQ(lhs.byte(), rhs.byte());
		EXPECT_EQ(lhs.frequency(), rhs.frequency());
	}
	else
	{
		compare_trees(*lhs.left(), *rhs.left());
		compare_trees(*lhs.right(), *rhs.right());
	}
}

} // namespace

TEST(HuffmanNode, copy_constructor_character_node)
{
	HuffmanNode char_node{'x', 7};
	HuffmanNode new_char_node{char_node};

	compare_trees(new_char_node, char_node);
}

TEST(HuffmanNode, copy_constructor_tree_node)
{
	HuffmanNode char_node{
		{'a', 7},
		{
			{'b', 3},
			{'c', 1}
		}
	};

	HuffmanNode new_char_node{char_node};

	compare_trees(new_char_node, char_node);
}

TEST(HuffmanNode, copy_assignment_constructor_character_node)
{
	HuffmanNode char_node{'x', 7};
	HuffmanNode new_char_node{0, 0};

	new_char_node = char_node;

	compare_trees(new_char_node, char_node);
}

TEST(HuffmanNode, copy_assignment_constructor_tree_node)
{
	HuffmanNode char_node{
		{'a', 7},
		{
			{'b', 3},
			{'c', 1}
		}
	};
	
	HuffmanNode new_char_node{0, 0};

	new_char_node = char_node;

	compare_trees(new_char_node, char_node);
}

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