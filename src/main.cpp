
/*
Program 13 - Huffman
Napisać program do kompresji plików metodą Huffmanna.
Program uruchamiany jest z linii poleceń z wykorzystaniem następujących przełączników:
-i plik wejściowy
-o plik wyjściowy
-t tryb: k – kompresja, d – dekompresja
-s plik ze słownikiem (tworzonym w czasie kompresji, używanym w czasie
dekompresji)
*/
#include <boost/json/parse_options.hpp>
#include <boost/json/storage_ptr.hpp>
#include <boost/json/stream_parser.hpp>
#include <boost/json/value.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iomanip>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <getopt.h>
#include <HuffmanDictionary.hpp>
#include <boost/json/src.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#ifndef VERSION
#define VERSION "unknown"
#endif

namespace pt = boost::property_tree;
using namespace huffman;

static struct
{
	const char shortopt;
	const char* longopt;
	int arg_type;
	const char* argname;
	const char* description;
}
options[] =
{
	{'h', "help",			no_argument,		NULL,		"display this message and exit"},
	{'i', "input",			required_argument,	"FILE",		"input file (only one)"},
	{'o', "output",			required_argument,	"FILE",		"output file (only one)"},
	{'m', "mode",			required_argument,	"MODE",		"'c' or 'k' for compression, 'd' for decompression"},
	{'t', NULL,				required_argument,	"",			"same as --mode (added to meet project's specifications)"},
	{'d', "dictionary",		required_argument,	"FILE",		"file to read/write the dictionary"},
	{'s', NULL,				required_argument,	"FILE",		"same as --dictionary (added to meet project's specifications)"},
	{'v', "version",		no_argument,		NULL,		"show the program's version"},
};

static struct
{
	enum{not_specified, compression, decompression} mode = not_specified;
	std::string dictionary_path{};
	std::string input_path{};
	std::string output_path{};

	bool is_initialized() const
	{
		return mode != not_specified && !dictionary_path.empty() && !input_path.empty() && !output_path.empty();
	}
}
parsed_options{};

/**
 * @brief display a help message
 * @param[in]	prog_name	argv[0]
 */
static void print_help(const char* prog_name)
{
	auto print_element = [](const char shortopt, const char* longopt, int arg_type, const char* argname, const char* description)
	{
		(void) arg_type;
		std::cout << std::setfill(' ')
			<< std::right << std::setw(4) << (shortopt ? std::string("-") + shortopt : "")
			<< std::left << std::setw(25) << (longopt ? (shortopt ? "," : " ") + std::string(" --") + longopt + (argname ? std::string("=") + argname : "") : "")
			<< description
			<< std::endl;
	};

	std::cout
		<< "Usage: " << prog_name << " options..." << std::endl
		<< "Compress files using Huffman compression." << std::endl
		<< std::endl
		<< "Mandatory arguments to long options are mandatory for short options too." << std::endl;

	for(size_t i = 0; i < sizeof(options)/sizeof(*options); i++)
	{
		auto& opt = options[i];
		print_element(opt.shortopt, opt.longopt, opt.arg_type, opt.argname, opt.description);
	}
}

/**
 * @brief parse arguments given on the command line
 * @param[in]	argc	argument count
 * @param[in]	argv	array of arguments
 * @returns		0 on success, exits on failure
 */
static int parse_args(int argc, char* argv[])
{
	int option;
	int option_index = 0;
	int should_parse = 1;

	std::vector<struct option> long_options;

	for(size_t i = 0; i < sizeof(options)/sizeof(*options); i++)
	{
		auto& fullopt = options[i];

		long_options.push_back({fullopt.longopt ? fullopt.longopt : "", fullopt.arg_type, nullptr, fullopt.shortopt});
	}
	long_options.push_back({});

	while(should_parse)
	{
		option = getopt_long(argc, argv, "vhi:o:m:t:d:s:", long_options.data(), &option_index);
		switch(option)
		{
			case 0:
			break;

			case 'h':
				print_help(*argv);
				exit(EXIT_SUCCESS);
			break;

			case 'v':
				std::cout << *argv << ": version " VERSION << std::endl;
				exit(EXIT_SUCCESS);
			break;

			case 'i':
				parsed_options.input_path = optarg;
			break;

			case 'o':
				parsed_options.output_path = optarg;
			break;

			case 't':
			case 'm':
				if(*optarg == 'c' || *optarg == 'k')
				{
					parsed_options.mode = parsed_options.compression;
				}
				else if(*optarg == 'd')
				{
					parsed_options.mode = parsed_options.decompression;
				}
				else
				{
					std::cout << "Invalid mode" << std::endl;
					print_help(argv[0]);
					exit(EXIT_FAILURE);
				}
			break;

			case 's':
			case 'd':
				parsed_options.dictionary_path = optarg;
			break;

			case '?':
				std::cerr << "Try '" << *argv << " --help' for more information." << std::endl;
				exit(EXIT_FAILURE);
			break;

			default:
				should_parse = 0;
			break;
		}
	}

	if(parsed_options.is_initialized() == false)
	{
		if(parsed_options.dictionary_path.empty())
		{
			std::cerr << "Error: No dictionary path." << std::endl;
		}
		if(parsed_options.input_path.empty())
		{
			std::cerr << "Error: No input path." << std::endl;
		}
		if(parsed_options.output_path.empty())
		{
			std::cerr << "Error: No output path." << std::endl;
		}
		if(parsed_options.mode == parsed_options.not_specified)
		{
			std::cerr << "Error: Mode not specified." << std::endl;
		}

		print_help(argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}

/**
 * @brief read the huffman tree from a file
 * @param[in]	path	name of file from where the tree would be read
 * @returns		pointer to a new tree on success, nullptr on failure
 */
std::unique_ptr<HuffmanNode> read_huffman_tree_json(const char* path)
{
	pt::ptree root;
	pt::read_json(path, root);
	std::unique_ptr<HuffmanNode> result{};

	std::function<std::unique_ptr<HuffmanNode>(const pt::ptree&)> recursive_get =
	[&](const pt::ptree& json_node)
	{
		auto new_node = std::make_unique<HuffmanNode>();

		try
		{
			new_node->m_frequency = json_node.get<size_t>("frequency");
		}
		catch(...)
		{
			return std::unique_ptr<HuffmanNode>{};
		}

		try
		{
			new_node->m_left = recursive_get(json_node.get_child("left"));
			new_node->m_right = recursive_get(json_node.get_child("right"));

			if(!(new_node->m_left && new_node->m_right))
			{
				throw std::runtime_error("");
			}
		}
		catch(...)
		{
			try
			{
				new_node->m_character = json_node.get<unsigned char>("character");
			}
			catch(...)
			{
				return std::unique_ptr<HuffmanNode>{};
			}
		}

		return new_node;
	};

	try
	{
		auto real_root = root.get_child("root");
		result = recursive_get(real_root);
	}
	catch(...)
	{

	}

	return result;
}

/**
 * @brief write the huffman tree to a file
 * @param[in]	path	name of file where the tree would be written
 * @param[in]	root	root of the tree
 * @returns		true when suceeded, false on fail
 */
bool write_huffman_tree_json(const char* path, const HuffmanNode& root)
{
	pt::ptree json_root, json_root_node;
	std::ofstream file{path};

	std::function<void(pt::ptree&, const HuffmanNode&)> recursive_put =
	[&](pt::ptree& json_node, const HuffmanNode& node)
	{

		json_node.put("frequency", node.m_frequency);

		if(node.is_character())
		{
			json_node.put("character", static_cast<unsigned char>(node.m_character));
		}
		else if(node.m_left && node.m_right)
		{
			pt::ptree left, right;

			recursive_put(left, *node.m_left);
			recursive_put(right, *node.m_right);

			json_node.add_child("left", left);
			json_node.add_child("right", right);
		}
	};

	recursive_put(json_root_node, root);

	json_root.add_child("root", json_root_node);

	pt::write_json(file, json_root);

	return true;
}

/**
 * @brief compresses a file
 */
void compress()
{
		char read_buffer[1024], write_buffer[1024];
		std::ifstream input_file(parsed_options.input_path);
		std::ofstream output_file(parsed_options.output_path);
		HuffmanDictionary dictionary;
		size_t read_bytes;

		if(!input_file || !output_file)
		{
			std::cerr << "Failed to open input or output file" << std::endl;
			exit(EXIT_FAILURE);
		}

		while(true)
		{
			input_file.read(read_buffer, sizeof(read_buffer));
			if(!(read_bytes = input_file.gcount()))
			{
				break;
			}

			dictionary.create_part(read_buffer, read_bytes);
		}

		if(!dictionary.is_initialized())
		{
			std::cerr << "Failed to initialize dictionary" << std::endl;
			exit(EXIT_FAILURE);
		}

		input_file.clear();
		input_file.seekg(std::ios_base::beg);

		size_t offset = 0;
		size_t buffer_fill;
		while(true)
		{
			input_file.read(read_buffer, sizeof(read_buffer));
			if(!(read_bytes = input_file.gcount()))
			{
				break;
			}

			char* r_buf = read_buffer;
			while(read_bytes)
			{
				auto result = dictionary.encode(r_buf, read_bytes, write_buffer, sizeof(write_buffer), *write_buffer, offset);
				offset = result.second;
				buffer_fill = offset/8;
				offset -= (offset/8)*8;

				if(!output_file.write(write_buffer, buffer_fill))
				{
					std::cerr << "Failed to write to output file" << std::endl;
					exit(EXIT_FAILURE);
				}

				read_bytes -= result.first;
				r_buf += result.first;
			}

			if(offset)
			{
				memmove(write_buffer, write_buffer+buffer_fill, 1);
			}
		}

		if(offset)
		{
			if(!output_file.write(write_buffer, 1))
			{
				std::cerr << "Failed to write to output file" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if(!write_huffman_tree_json(parsed_options.dictionary_path.c_str(), *dictionary.data()))
		{
			std::cerr << "Failed to write dictionary" << std::endl;
			exit(EXIT_FAILURE);
		}

		output_file.close();
		input_file.close();
}

/**
 * @brief decompresses a file
 */
void decompress()
{
	std::ifstream input_file(parsed_options.input_path);
	std::ofstream output_file(parsed_options.output_path);
	HuffmanDictionary dictionary;
	auto htptr = read_huffman_tree_json(parsed_options.dictionary_path.c_str());
	if(htptr)
	{
		dictionary = {*htptr};
		htptr.reset();
	}
	else
	{
		std::cerr << "Failed to load dictionary" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(!input_file || !output_file)
	{
		std::cerr << "Failed to open input or output file" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string read_buf;
	std::string write_buf(3, 0);
	size_t offset = 0;
	size_t chars_left = dictionary.size();

	while(chars_left)
	{
		std::string tmp(3, 0);
		input_file.read(tmp.data(), tmp.size());
		if(input_file.gcount() == 0)
		{
			break;
		}

		read_buf.append(tmp);

		while(true)
		{
			auto[src_read, dst_written] = dictionary.decode(read_buf.data(), read_buf.size(), write_buf.data(), std::min(write_buf.size(), chars_left), offset);
			if(dst_written == 0)
			{
				break;
			}

			offset = src_read % 8;

			read_buf.erase(0, src_read/8);

			if(!output_file.write(write_buf.data(), dst_written))
			{
				std::cerr << "Error while writing output file. Aborting" << std::endl;
				return;
			}

			chars_left -= dst_written;
		}
	}
}

int main(int argc, char* argv[])
{
	parse_args(argc, argv);

	if(parsed_options.mode == parsed_options.compression)
	{
		compress();
	}
	else
	{
		decompress();
	}
}
