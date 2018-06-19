
#include <catch.h>

#include <netmap_tools.h>

TEST_CASE("iface", "[iface]")
{
	REQUIRE(std::getenv("TEST_IFACE") != nullptr);
	std::string iface_name = std::string(std::getenv("TEST_IFACE"));
	netmap::iface test_iface(iface_name);
	unsigned tx_rings_count = 0, rx_rings_count = 0;

	SECTION("iface")
	{
		CHECK_THROWS(netmap::iface("invalid_iface"));
		CHECK_NOTHROW(netmap::iface(iface_name));
	}

	SECTION("count_tx_rings")
	{
		REQUIRE(test_iface.count_tx_rings() > 0);
	}

	SECTION("count_rx_rings")
	{
		REQUIRE(test_iface.count_rx_rings() > 0);
	}

	SECTION("tx_rings.operator[]")
	{
		for (unsigned i = 0; i < test_iface.count_tx_rings(); i++)
			CHECK_NOTHROW(test_iface.tx_rings[i]);

		CHECK_THROWS(test_iface.tx_rings[test_iface.count_tx_rings()]);
	}

	SECTION("rx_rings.operator[]")
	{
		for (unsigned i = 0; i < test_iface.count_rx_rings(); i++)
			CHECK_NOTHROW(test_iface.rx_rings[i]);
	
		CHECK_THROWS(test_iface.rx_rings[test_iface.count_rx_rings()]);
	}

	SECTION("fd")
	{
		CHECK(test_iface.fd() > 0);
	}
}

