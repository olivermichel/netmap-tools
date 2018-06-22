
#include "pkt_sender_config.h"

#include <netmap_tools.h>
#include <cstring>
#include <netinet/if_ether.h>
#include <poll.h>
#include <signal.h>

int main(int argc_, char** argv_)
{
	pkt_sender::config config = pkt_sender::_parse_config(pkt_sender::_set_options(), argc_, argv_);
	netmap::iface iface(config.iface_name);
	signal(SIGINT, pkt_sender::signal_handler);	

	struct pollfd fds;
	fds.fd = iface.fd();
	fds.events = POLLOUT;

	char* buf  = nullptr;
	
	char msg[14] = {0};
	struct ether_header* eth = (struct ether_header*) msg;
	std::memcpy(eth->ether_dhost, config.dst_addr, 6);
	std::memcpy(eth->ether_shost, config.src_addr, 6);

	pkt_sender::start = std::chrono::high_resolution_clock::now();

	while (pkt_sender::run) {
		poll(&fds, 1, -1);
		while(iface.tx_rings[0].avail()) {
			if (buf = iface.tx_rings[0].next_buf()) {
				std::memcpy(buf, msg, 14);	
				iface.tx_rings[0].advance(14);
				pkt_sender::count++;
			}
		}
		ioctl(NIOCTXSYNC, iface.fd());
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - pkt_sender::start);
	double time_s = (double) dur.count() / 1000000;
	std::cout << pkt_sender::count / time_s << std::endl;

	return 0;
}

