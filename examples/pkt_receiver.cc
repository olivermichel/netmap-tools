
#include "pkt_receiver_config.h"

#include <netmap_tools.h>
#include <cstring>
#include <netinet/if_ether.h>
#include <poll.h>
#include <signal.h>

int main(int argc_, char** argv_)
{
	pkt_receiver::config config = pkt_receiver::_parse_config(pkt_receiver::_set_options(), argc_, argv_);
	netmap::iface iface(config.iface_name);
	signal(SIGINT, pkt_receiver::signal_handler);

	struct pollfd fds { .fd = iface.fd(), .events = POLLIN };

	pkt_receiver::start = std::chrono::high_resolution_clock::now();

	unsigned int len = 0;
	char* buf = nullptr;

	while (pkt_receiver::run && poll(&fds, 1, -1)) {
		while(iface.rx_rings[0].avail()) {
			buf = iface.rx_rings[0].next_buf(len);
			iface.rx_rings[0].advance();
			pkt_receiver::count++;

			if (config.verbosity > 0)
				std::cout << pkt_receiver::count << std::endl;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - pkt_receiver::start);
	double time_s = (double) dur.count() / 1000000;
	std::cout << pkt_receiver::count / time_s << std::endl;

	return 0;
}
