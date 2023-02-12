#include <HuffmanDictionary.hpp>
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
