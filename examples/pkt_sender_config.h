
#include <cxxopts/cxxopts.h>
#include <chrono>

namespace pkt_sender {
	
	bool run = true;
	unsigned long long count = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	
	struct config
	{
		std::string iface_name;
		unsigned char src_addr[6];
		unsigned char dst_addr[6];
		unsigned verbosity = 0;
	};

	void signal_handler(int sig_)
	{
		run = false;
	}

	void _print_help(cxxopts::Options& opts_, int exit_code_ = 0) 
	{
		std::ostream& os = (exit_code_ ? std::cerr : std::cout);
		os << opts_.help({""}) << std::endl;
		exit(exit_code_);
	}

	cxxopts::Options _set_options()
	{
		cxxopts::Options opts("pkt_sender", " - ");

		opts.add_options()
			("i,interface", "sender interface [required]", cxxopts::value<std::string>(), "IFACE")
			("s,source", "source mac address [required]", cxxopts::value<std::string>(), "SRC")
			("d,destination", "destination mac address [required]", cxxopts::value<std::string>(),
			        "DEST")
			("v,verbose", "print intermediate output [optional]")
			("h,help", "print this help message");

		return opts;
	}

	void parse_mac_addr(const char* str_, unsigned char* dst_buf_)
	{
		int temp[6] = {0};

		if (std::sscanf(str_, "%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4],
				&temp[5]) != 6)
			throw std::invalid_argument("parse_mac_addr: invalid format");

		for (unsigned i = 0; i < 6; i++)
			dst_buf_[i] = (unsigned char) temp[i];
	}

	config _parse_config(cxxopts::Options opts_, int argc_, char** argv_)
	{
		config config {};
		auto parsed_opts = opts_.parse(argc_, argv_);

		if (parsed_opts.count("h"))
			_print_help(opts_);

		if (parsed_opts.count("i"))
			config.iface_name = parsed_opts["i"].as<std::string>();
		else
			_print_help(opts_);

		if (parsed_opts.count("s")) {
			parse_mac_addr(parsed_opts["s"].as<std::string>().c_str(), config.src_addr);
		} else {
			_print_help(opts_);
		}

		if (parsed_opts.count("d")) {
			parse_mac_addr(parsed_opts["d"].as<std::string>().c_str(), config.dst_addr);
		} else {
			_print_help(opts_);
		}

		config.verbosity = (unsigned) parsed_opts.count("v");

		return config;
	}
}

