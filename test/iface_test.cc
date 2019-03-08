
#include <catch.h>
#include <cstring>
#include <netmap_tools.h>

TEST_CASE("iface", "[iface]")
{
	REQUIRE(std::getenv("TEST_IFACE") != nullptr);
	std::string iface_name = std::string(std::getenv("TEST_IFACE"));

	netmap::iface test_iface(iface_name);

	SECTION("iface")
	{
		CHECK_THROWS(netmap::iface("invalid_iface"));
		CHECK_NOTHROW(netmap::iface(iface_name));
	}

	SECTION("count_tx_rings")
	{
		REQUIRE(test_iface.tx_rings.count() > 0);
	}

	SECTION("count_rx_rings")
	{
		REQUIRE(test_iface.rx_rings.count() > 0);
	}

	SECTION("tx_rings.operator[]")
	{
		for (unsigned i = 0; i < test_iface.tx_rings.count(); i++)
			CHECK_NOTHROW(test_iface.tx_rings[i]);

		CHECK_THROWS(test_iface.tx_rings[test_iface.tx_rings.count()]);
	}

	SECTION("rx_rings.operator[]")
	{
		for (unsigned i = 0; i < test_iface.rx_rings.count(); i++)
			CHECK_NOTHROW(test_iface.rx_rings[i]);
	
		CHECK_THROWS(test_iface.rx_rings[test_iface.rx_rings.count()]);
	}

	SECTION("fd")
	{
		CHECK(test_iface.fd() > 0);
	}

	SECTION("_ring")
	{
		auto tx_ring = test_iface.tx_rings[0];
		auto rx_ring = test_iface.rx_rings[0];

		SECTION("count_slots")
		{
			CHECK(tx_ring.count_slots() > 0);
			CHECK(rx_ring.count_slots() > 0);
		}

		SECTION("buffer_size")
		{
			CHECK(tx_ring.buffer_size() > 0);
			CHECK(rx_ring.buffer_size() > 0);
		}
	}
}

