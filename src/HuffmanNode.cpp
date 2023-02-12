#include "HuffmanNode.hpp"

namespace huffman
{

HuffmanNode::HuffmanNode(char byte, size_t frequency) noexcept
	: m_frequency{frequency}, m_is_byte_node{true}, m_byte{byte}
{

}

HuffmanNode::HuffmanNode(HuffmanNode&& left, HuffmanNode&& right)
	: m_frequency{left.m_frequency + right.m_frequency},
	  m_is_byte_node{false},
	  m_left{std::make_unique<HuffmanNode>(std::move(left))},
	  m_right{std::make_unique<HuffmanNode>(std::move(right))}
{

}

HuffmanNode::HuffmanNode(HuffmanNode&& other) noexcept
	: m_frequency{other.m_frequency},
	  m_is_byte_node{other.m_is_byte_node},
	  m_byte{other.m_byte},
	  m_left{std::move(other.m_left)},
	  m_right{std::move(other.m_right)}
{

}

HuffmanNode::HuffmanNode(const HuffmanNode& other)
	: m_frequency{other.m_frequency},
	  m_is_byte_node{other.m_is_byte_node},
	  m_byte{other.m_byte}
{
	if(!other.is_byte_node())
	{
		m_left = std::make_unique<HuffmanNode>(*other.m_left);
		m_right = std::make_unique<HuffmanNode>(*other.m_right);
	}
}

HuffmanNode& HuffmanNode::operator=(HuffmanNode&& other) noexcept
{
	m_frequency = other.m_frequency;
	m_is_byte_node = other.m_is_byte_node;
	m_byte = other.m_byte;
	m_left = std::move(other.m_left);
	m_right = std::move(other.m_right);

	return *this;
}

HuffmanNode& HuffmanNode::operator=(const HuffmanNode& other) noexcept
{
	m_frequency = other.m_frequency;
	m_is_byte_node = other.m_is_byte_node;
	m_byte = other.m_byte;

	if(!other.is_byte_node())
	{
		m_left = std::make_unique<HuffmanNode>(*other.m_left);
		m_right = std::make_unique<HuffmanNode>(*other.m_right);
	}

	return *this;
}

bool HuffmanNode::is_byte_node() const
{
	return m_is_byte_node;
}

size_t HuffmanNode::frequency() const
{
	return m_frequency;
}

char HuffmanNode::byte() const
{
	return m_byte;
}

const HuffmanNode* HuffmanNode::left() const
{
	return m_left.get();
}

const HuffmanNode* HuffmanNode::right() const
{
	return m_right.get();
}

} // namespace huffman
