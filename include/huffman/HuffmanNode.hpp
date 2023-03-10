#pragma once

#include <memory>

namespace huffman
{

class HuffmanNode
{
public:
	HuffmanNode(char byte, size_t frequency) noexcept;
	HuffmanNode(HuffmanNode&& left, HuffmanNode&& right);

	HuffmanNode(HuffmanNode&& other) noexcept = default;
	HuffmanNode& operator=(HuffmanNode&& other) noexcept = default;

	HuffmanNode(const HuffmanNode& other);
	HuffmanNode& operator=(const HuffmanNode& other) noexcept;

	~HuffmanNode() = default;

	bool is_byte_node() const;
	size_t frequency() const;
	char byte() const;
	const HuffmanNode* left() const;
	const HuffmanNode* right() const;

private:
	size_t m_frequency;
	bool m_is_byte_node;

	char m_byte{};
	std::unique_ptr<HuffmanNode> m_left{}, m_right{};
};

} // namespace huffman
