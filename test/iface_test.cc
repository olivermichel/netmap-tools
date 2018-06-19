
#include <catch.h>

#include <netmap_tools.h>

TEST_CASE("iface", "[iface]")
{
	SECTION("iface")
	{
		CHECK_NOTHROW(netmap::iface("enp130s0f0"));
		CHECK_THROWS(netmap::iface("invalid_iface"));
	}
}
