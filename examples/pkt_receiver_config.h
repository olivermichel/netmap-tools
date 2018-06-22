
#include <cxxopts/cxxopts.h>
#include <chrono>

namespace pkt_receiver {
	
	bool run = true;
	unsigned long long count = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	
	struct config
	{
		std::string iface_name;
		bool verbose = false;
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
		cxxopts::Options opts("pkt_receiver", " - ");

		opts.add_options()
			("i,interface", "sets the sender interface [required]", cxxopts::value<std::string>(), "IFACE")
			("v,verbose", "print intermediate output (optional)")
			("h,help", "print this help message");

		return opts;
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

		config.verbose = (bool) parsed_opts.count("v");

		return config;
	}
}

