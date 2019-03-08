
#include <iostream>
#include <netmap_tools.h>

int main(int argc_, char** argv_)
{
    if (argc_ < 2) {
        std::cerr << "usage: netmap_info <iface_name...>" << std::endl;
        return 1;
    }

    for (auto iface_index = 1; iface_index < argc_; iface_index++) {

        try {
            std::string iface_name = std::string(argv_[iface_index]);

            netmap::iface iface(iface_name);

            std::cout << iface_name << ":" << std::endl;
            std::cout << "  rx rings: " << iface.rx_rings.count() << std::endl;

            for (auto i = 0; i < iface.rx_rings.count(); i++) {
                if (iface.rx_rings[i].buffer_size() > 0) {
                    std::cout << "    " << i << ": " << iface.rx_rings[i].count_slots()
                        << " slots / " << iface.rx_rings[i].buffer_size() << " bytes " << std::endl;
                }
            }

            std::cout << "  tx rings: " << iface.tx_rings.count() << std::endl;

            for (auto i = 0; i < iface.tx_rings.count(); i++) {
                if (iface.tx_rings[i].buffer_size() > 0) {
                    std::cout << "    " << i << ": " << iface.tx_rings[i].count_slots()
                        << " slots / " << iface.tx_rings[i].buffer_size() << " bytes " << std::endl;
                }
            }

            if (iface_index < argc_ - 1) std::cout << std::endl;

        } catch (const std::exception &e) {
            std::cerr << "could not open interface: " << argv_[iface_index] << std::endl;
            std::cerr << " - " << e.what() << std::endl;
        }
    }

    return 0;
}
