
#include "pkt_sender_config.h"

#include <netmap_tools.h>

int main(int argc_, char** argv_)
{
	pkt_sender::config config = pkt_sender::_parse_config(pkt_sender::_set_options(), argc_, argv_);
	
	return 0;
}

