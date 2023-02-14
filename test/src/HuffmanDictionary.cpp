#include <huffman/HuffmanDictionary.hpp>
#include <gtest/gtest.h>

/**
 Useful:
#include <vector>
#include <iostream>
std::vector<unsigned char> b = {0x7F, 0xB7, 0x8D, 0x24, 0x01, 0x00, 0x55, 0xA5, 0xAA, 0x02};
int main()
{
	bool carry = 0;
	for(auto it = b.rbegin(); it != b.rend(); it++)
	{
		bool tmp_carry = *it & 1;
		*it = carry << 7 | *it >> 1;
		carry = tmp_carry;
	}
	for(auto c : b)
	{
		printf("0x%.2X, ", c);
	}
	std::cout << std::endl;
}
*/

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

TEST(HuffmanDictionary, decode_tree)
{
	HuffmanNode root_node{
		{'a', 7},
		{
			{'b', 3}, {'c', 1}
		}
	};

	std::string src = "\x7F\x15";
	HuffmanDictionary dictionary(root_node);
	std::string dst(dictionary.size(), 0);

	auto[src_read, dst_written] = dictionary.decode(src.data(), src.size(), dst.data(), dst.size(), 0);

	EXPECT_EQ(src_read, 15);
	EXPECT_EQ(dst_written, 11);
	EXPECT_EQ(dst, "aaaaaaabbbc");
}

TEST(HuffmanDictionary, decode_tree_partially)
{
	HuffmanNode root_node{
		{'a', 7},
		{
			{'b', 3}, {'c', 1}
		}
	};

	std::string src = "\x7F\x15";
	HuffmanDictionary dictionary(root_node);
	std::string dst(dictionary.size(), 0);

	auto[src_read, dst_written] = dictionary.decode(src.data(), 1, dst.data(), dst.size(), 0);

	EXPECT_EQ(src_read, 7);
	EXPECT_EQ(dst_written, 7);
	EXPECT_STREQ(dst.c_str(), "aaaaaaa");
}

TEST(HuffmanDictionary, encode_tree)
{
	HuffmanNode root_node{
		{'a', 7},
		{
			{'b', 3}, {'c', 1}
		}
	};

	std::string src = "aaaaaaabbbc";
	HuffmanDictionary dictionary(root_node);
	std::string dst(dictionary.size(), 0);

	auto[src_read, dst_written] = dictionary.encode(src.data(), src.size(), dst.data(), dst.size(), 0);

	EXPECT_EQ(src_read, 11);
	EXPECT_EQ(dst_written, 15);
	EXPECT_STREQ(dst.c_str(), "\x7F\x15");
}

TEST(HuffmanDictionary, encode_tree_partially)
{
	HuffmanNode root_node{
		{'a', 7},
		{
			{'b', 3}, {'c', 1}
		}
	};

	std::string src = "aaaaaaabbbc";
	HuffmanDictionary dictionary(root_node);
	std::string dst(dictionary.size(), 0);

	auto[src_read, dst_written] = dictionary.encode(src.data(), src.size(), dst.data(), 1, 0);

	EXPECT_EQ(src_read, 7);
	EXPECT_EQ(dst_written, 7);
	EXPECT_STREQ(dst.c_str(), "\x7F");
}

TEST(HuffmanDictionary, create_part_tree)
{
	HuffmanNode test_tree{
		{ {'c', 1}, {'b', 3} },
		{'a', 7},
	};
	std::string test_data[] = {"aaaaaaa", "bbbc"};
	HuffmanDictionary dictionary(test_data[0].data(), test_data[0].size());

	dictionary.create_part(test_data[1].data(), test_data[1].size());

	compare_trees(dictionary.data(), test_tree);
}

TEST(HuffmanDictionary, create_from_existing_tree)
{
	HuffmanNode root_node{
		{'a', 7},
		{
			{'b', 3},
			{'c', 1}
		}
	};

	HuffmanDictionary dictionary(root_node);

	compare_trees(dictionary.data(), root_node);
}

TEST(HuffmanDictionary, create_empty)
{
	HuffmanDictionary dictionary;

	dictionary.create(nullptr, 0);

	EXPECT_TRUE(dictionary.empty());
	EXPECT_EQ(dictionary.size(), 0);
}

TEST(HuffmanDictionary, default_constructor)
{
	HuffmanDictionary dictionary;

	EXPECT_TRUE(dictionary.empty());
	EXPECT_EQ(dictionary.size(), 0);
}

TEST(HuffmanDictionary, constructor)
{
	std::string test_data = "A" "BB" "CCC" "DDDD" "EEEEE" "FFFFFF" "GGGGGGG";
	HuffmanDictionary dictionary(test_data.data(), test_data.size());

	EXPECT_FALSE(dictionary.empty());
	EXPECT_EQ(dictionary.size(), 28);
}

TEST(HuffmanDictionary, create_part)
{
	std::string test_data[] = {"A" "BB" "CCC" "DDDD",  "EEEEE" "FFFFFF" "GGGGGGG"};
	HuffmanDictionary dictionary(test_data[0].data(), test_data[0].size());

	dictionary.create_part(test_data[1].data(), test_data[1].size());

	EXPECT_FALSE(dictionary.empty());
	EXPECT_EQ(dictionary.size(), 28);
}

TEST(HuffmanDictionary, decode_empty)
{
	HuffmanDictionary dictionary;
	auto[src_read, dst_written] = dictionary.decode(nullptr, 0, nullptr, 0, 0);

	EXPECT_EQ(src_read, 0);
	EXPECT_EQ(dst_written, 0);
	
	EXPECT_TRUE(dictionary.empty());
	EXPECT_EQ(dictionary.size(), 0);
}

TEST(HuffmanDictionary, decode)
{
	const std::string test_string = "A" "BB" "CCC" "DDDD" "EEEEE" "FFFFFF" "GGGGGGG";
	const std::string encoded =
	{'\x7F', '\xB7', '\x8D', '\x24', '\x01', '\x00', '\x55', '\xA5', '\xAA', '\x02'};
	std::string decoded(test_string.size(), 0);

	HuffmanDictionary dictionary(test_string.c_str(), test_string.size());

	auto[src_read, decoded_size] = dictionary.decode(encoded.c_str(), encoded.size(),
												decoded.data(), std::min(dictionary.size(), decoded.size()), 0);

	EXPECT_EQ(test_string.size(), decoded_size);
	EXPECT_EQ(9*8+2, src_read);
	EXPECT_EQ(test_string, decoded);
}

TEST(HuffmanDictionary, encode_empty)
{
	HuffmanDictionary dictionary;
	auto[src_read, dst_written] = dictionary.encode(nullptr, 0, nullptr, 0, 0);

	EXPECT_EQ(src_read, 0);
	EXPECT_EQ(dst_written, 0);

	EXPECT_TRUE(dictionary.empty());
	EXPECT_EQ(dictionary.size(), 0);
}

TEST(HuffmanDictionary, encode)
{
	HuffmanDictionary dictionary;
	const std::string test_string = "A" "BB" "CCC" "DDDD" "EEEEE" "FFFFFF" "GGGGGGG";
	const std::string correctly_encoded_string =
	{'\x7F', '\xB7', '\x8D', '\x24', '\x01', '\x00', '\x55', '\xA5', '\xAA', '\x02'};
	std::string buffer(correctly_encoded_string.size(), 0);

	dictionary.create(test_string.data(), test_string.size());
	size_t offset = dictionary.encode(test_string.data(), test_string.size(), buffer.data(), buffer.size(), 0).second;

	EXPECT_EQ(offset, (correctly_encoded_string.size()-1)*8+2); // test_string.size()-1 full characters and 2 additional bits
	EXPECT_EQ(buffer, correctly_encoded_string);
}

TEST(HuffmanDictionary, encode_and_decode)
{
	HuffmanDictionary dictionary;
	const std::string test_string = "A" "BB" "CCC" "DDDD" "EEEEE" "FFFFFF" "GGGGGGG";
	std::string result(test_string.size(), 0);
	char buffer[1024];

	dictionary.create(test_string.data(), test_string.size());

	dictionary.encode(test_string.data(), test_string.size(), buffer, sizeof(buffer), 0);
	dictionary.decode(buffer, sizeof(buffer), result.data(), result.size(), 0);

	EXPECT_EQ(result, test_string);
}
