
#include <catch.h>

#include <netmap_tools.h>

TEST_CASE("iface", "[iface]")
{
	SECTION("iface")
	{
		CHECK_THROWS(netmap::iface("invalid_iface"));
	}
}
