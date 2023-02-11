subdir('decoder')
subdir('encoder')

test_sources = [
    'HuffmanDictionary.cpp',
	'HuffmanNode.cpp'
]

e = executable('huffman', test_sources,
		dependencies : gtest_main_dep,
		include_directories : [inc],
		link_with : [libhuffman])

test('huffman', e)