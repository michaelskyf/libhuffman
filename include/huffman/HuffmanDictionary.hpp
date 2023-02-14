#pragma once

#include <memory>

#include "huffman/HuffmanNode.hpp"

namespace huffman
{

class HuffmanDictionary
{
public:
	HuffmanDictionary() = default;
	HuffmanDictionary(const HuffmanNode& root);
	HuffmanDictionary(const char* data, size_t size);
	~HuffmanDictionary() = default;

	/**
	 * @brief				create a new dictionary from the given data
	 * @param[in]	data	pointer to data
	 * @param[in]	size	size of data
	 * @throws				std::bad_alloc
	 */
	void create(const char* data, size_t size);

	/**
	 * @brief				create a new dictionary from the given data (partially)
	 * @param[in]	data	pointer to data
	 * @param[in]	size	size of data
	 * @throws				std::bad_alloc
	 */
	void create_part(const char* data, size_t size);

	/**
	 * @brief				get sum of all frequencies in the dictionary
	 * @returns 			0 if the tree is not initialized, otherwise sum of all frequencies in the tree
	 * @throws				nothing
	 */
	size_t size() const;

	/**
	 * @brief				check if the dictionary is initialized ( same as size() == 0 )
	 * @returns				false if initialized, otherwise true
	 * @throws				nothing
	 */
	bool empty() const;

	/**
	 * @brief				return a pointer to the root node
	 * @returns				nullptr if the tree is not initialized, otherwise a pointer to the root node
	 * @throws				nothing
	 */
	const HuffmanNode& data() const;

	/**
	 * @brief						create a new dictionary (if not already initialized) and encode the data according to it
	 * @param[in]		src			source
	 * @param[in]		src_size	source size
	 * @param[out]		dst			destination
	 * @param[in]		dst_size	destination size
	 * @param[in]		bits_set	numer of bits used in byte
	 * @returns						number of bytes read from src (first) and number of bits written to dst (the last byte may be partially written) (second)
	 * @throws						std::bad_alloc
	 */
	std::pair<size_t, size_t> encode(const char* src, size_t src_size, char* dst, size_t dst_size, size_t bits_set);

	/**
	 * @brief						decode given data using the dictionary
	 * @param[in]		src			source
	 * @param[in]		src_size	source size
	 * @param[out]		dst			destination
	 * @param[in]		dst_size	destination size
	 * @param[in]		bits_set	numer of bits used in byte
	 * @returns						number of bits read from src (first) and number of bytes written to dst (second)
	 * @throws						std::bad_alloc
	 */
	std::pair<size_t, size_t> decode(const char* src, size_t src_size, char* dst, size_t dst_size, size_t bits_set);

private:
	HuffmanNode m_root{0, 0};
};

} // namespace huffman
