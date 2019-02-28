
#include "pkt_sender_config.h"

#include <cstring>
#include <iomanip>
#include <netinet/if_ether.h>
#include <netmap_tools.h>
#include <poll.h>
#include <signal.h>

int main(int argc_, char** argv_)
{
	pkt_sender::config config = pkt_sender::_parse_config(pkt_sender::_set_options(), argc_, argv_);
	signal(SIGINT, pkt_sender::signal_handler);

	char* buf  = nullptr;
	char msg[14] = {0};
	auto eth = (struct ether_header*) msg;
	std::memcpy(eth->ether_dhost, config.dst_addr, 6);
	std::memcpy(eth->ether_shost, config.src_addr, 6);

	netmap::iface iface(config.iface_name);
	struct pollfd fds { .fd = iface.fd(), .events = POLLOUT };

	pkt_sender::start = std::chrono::high_resolution_clock::now();

	while (pkt_sender::run && poll(&fds, 1, -1)) {
		for (unsigned tx_ring_id = 0; tx_ring_id < iface.tx_rings.count(); tx_ring_id++) {
			while (iface.tx_rings[tx_ring_id].avail()) {
				buf = iface.tx_rings[tx_ring_id].next_buf();
				std::memcpy(buf, msg, 14);
				iface.tx_rings[tx_ring_id].advance(14);
				pkt_sender::count++;

				if (config.verbosity > 0 && pkt_sender::count % 100000 == 0)
					std::cout << pkt_sender::count << std::endl;
			}
		}
		iface.tx_rings.synchronize();
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - pkt_sender::start);
	double time_s = (double) dur.count() / 1000000;
	std::cout <<  pkt_sender::count << "," << std::fixed << time_s << ","
		<<  pkt_sender::count / time_s << std::endl;
	return 0;
}
