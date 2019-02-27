

#define CATCH_CONFIG_RUNNER
#include <catch.h>

#include <cstdlib>
#include <iostream>

int main(int argc_, char** argv_)
{
	std::string iface_name;
/*
	if (const char* env_iface_name = std::getenv("TEST_IFACE")) {
		iface_name = env_iface_name;
	} else {
		std::cerr << "TEST_IFACE environment variable was not provided" << std::endl;
		return 1;
	}
*/
	Catch::Session catch_session;
	int return_code = catch_session.run(argc_, argv_);
	return return_code;
}

