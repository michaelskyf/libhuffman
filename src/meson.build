source_files = files(
	'HuffmanDictionary.cpp',
	'HuffmanNode.cpp',
)

subdir('encoder')

libhuffman = static_library(
    'huffman',
	source_files,
	include_directories : inc,
    install: true
)

libhuffman_dep = declare_dependency(
  include_directories : inc,
  link_with : libhuffman
)
