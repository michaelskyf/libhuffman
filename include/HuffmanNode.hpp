#pragma once

#include <memory>

namespace huffman
{

class HuffmanNode
{
public:
	HuffmanNode(char byte, size_t frequency) noexcept;
	HuffmanNode(HuffmanNode&& left, HuffmanNode&& right);
	HuffmanNode(HuffmanNode&& other) noexcept;
	HuffmanNode(const HuffmanNode& other);
	HuffmanNode& operator=(HuffmanNode&& other) noexcept;
	HuffmanNode& operator=(const HuffmanNode& other) noexcept;

	bool is_byte_node() const;
	size_t frequency() const;
	char byte() const;
	const HuffmanNode* left() const;
	const HuffmanNode* right() const;

private:
	size_t m_frequency{};
	bool m_is_byte_node{};

	char m_byte{};
	std::unique_ptr<HuffmanNode> m_left{}, m_right{};
};

} // namespace huffman
