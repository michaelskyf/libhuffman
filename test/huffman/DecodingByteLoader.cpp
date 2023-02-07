#include "src/DecodingByteLoader.hpp"
#include <gtest/gtest.h>

TEST(DecodingByteLoader, empty)
{
	auto object = DecodingByteLoader(nullptr, 0, 0);

	EXPECT_TRUE(object.empty());
	EXPECT_EQ(object.bitsProcessed(), 0);
	EXPECT_EQ(object.totalBits(), 0);
}

TEST(DecodingByteLoader, empty_offset)
{
	auto object = DecodingByteLoader(nullptr, 0, 12);

	EXPECT_TRUE(object.empty());
	EXPECT_EQ(object.bitsProcessed(), 0);
	EXPECT_EQ(object.totalBits(), 0);
}

TEST(DecodingByteLoader, one_byte)
{
	char byte = '\xB3';
	auto object = DecodingByteLoader(&byte, 1, 3);

	EXPECT_FALSE(object.empty());
	EXPECT_EQ(object.value() & 0b111, (byte >> 3) & 0b111);
	EXPECT_EQ(object.bitsProcessed(), 3);
	EXPECT_EQ(object.totalBits(), 8);
}

TEST(DecodingByteLoader, one_byte_shift)
{
	char byte = '\xB3';
	auto object = DecodingByteLoader(&byte, 1, 0);

	object >>= 3;

	EXPECT_FALSE(object.empty());
	EXPECT_EQ(object.value() & 0b111, (byte >> 3) & 0b111);
	EXPECT_EQ(object.bitsProcessed(), 3);
	EXPECT_EQ(object.totalBits(), 8);
}

TEST(DecodingByteLoader, one_byte_big_offset)
{
	char byte = '\xB3';
	auto object = DecodingByteLoader(&byte, 1, 9);

	EXPECT_TRUE(object.empty());
	EXPECT_EQ(object.bitsProcessed(), 8);
	EXPECT_EQ(object.totalBits(), 8);
}

TEST(DecodingByteLoader, one_byte_big_shift)
{
	char byte = '\xB3';
	auto object = DecodingByteLoader(&byte, 1, 0);

	object >>= 9;

	EXPECT_TRUE(object.empty());
	EXPECT_EQ(object.bitsProcessed(), 8);
	EXPECT_EQ(object.totalBits(), 8);
}

TEST(DecodingByteLoader, byte_array_shift)
{
	std::string bytes{ '\xAA', '\xBB', '\xAA', '\xBB' };
	auto object = DecodingByteLoader(bytes.data(), bytes.size(), 0);

	object >>= 16;

	EXPECT_FALSE(object.empty());
	EXPECT_EQ(object.bitsProcessed(), 16);
	EXPECT_EQ(object.totalBits(), bytes.size()*8);
	EXPECT_EQ('\xAA', object.value());
}

TEST(DecodingByteLoader, byte_array_offset)
{
	std::string bytes{ '\xAA', '\xBB', '\xAA', '\xBB' };
	auto object = DecodingByteLoader(bytes.data(), bytes.size(), 16);

	EXPECT_FALSE(object.empty());
	EXPECT_EQ(object.bitsProcessed(), 16);
	EXPECT_EQ(object.totalBits(), bytes.size()*8);
	EXPECT_EQ('\xAA', object.value());
}