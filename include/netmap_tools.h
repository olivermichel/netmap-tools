
#define NETMAP_WITH_LIBS
#include <net/netmap_user.h>
#include <vector>
#include <stdexcept>

namespace netmap {

    class iface
    {
        class ring
        {
        public:
            ring() = delete;
            explicit ring(netmap_ring* ring_)
                    : _ring(ring_) { }

            ring(const ring&) = delete;
            ring& operator=(const ring&) = delete;

            ring(ring&&) = default;
            ring& operator=(ring&&) = default;

            inline unsigned count_slots() const
            {
                return _ring->num_slots;
            }

            inline unsigned buffer_size() const
            {
                return _ring->nr_buf_size;
            }

            inline unsigned head() const
            {
                return _ring->head;
            }

            inline unsigned tail() const
            {
                return _ring->tail;
            }

            inline unsigned current() const
            {
                return _ring->cur;
            }

            inline bool avail() const
            {
                return _ring->cur != _ring->tail;

            }

            ~ring()
            {
                _ring = nullptr;
            }

        protected:
            netmap_ring* _ring;
        };

        class tx_ring : public ring
        {
        public:
            explicit tx_ring(netmap_ring* ring_)
                    : ring(ring_) { }

            char* next_buf()
            {
                return _ring->cur != _ring->tail ?
                       NETMAP_BUF(_ring, _ring->slot[_ring->cur].buf_idx) : nullptr;
            }

            void advance(unsigned pkt_len_)
            {
                unsigned i = _ring->cur;
                _ring->slot[i].len = (uint16_t) pkt_len_;
                _ring->head = _ring->cur = nm_ring_next(_ring, i);
            }
        };

        class rx_ring : public ring
        {
        public:
            explicit rx_ring(netmap_ring* ring_)
                    : ring(ring_) { }

            char* next_buf(unsigned& len_)
            {
                if (_ring->cur != _ring->tail) {
                    len_ = _ring->slot[_ring->cur].len;
                    return NETMAP_BUF(_ring, _ring->slot[_ring->cur].buf_idx);
                } else return nullptr;
            }

            void advance()
            {
                unsigned i = _ring->cur;
                _ring->head = _ring->cur = nm_ring_next(_ring, i);
            }
        };

        class _ring_proxy
        {
        public:
            _ring_proxy() = default;

            explicit _ring_proxy(iface* iface_)
                    : _iface(iface_) { }

            inline const std::vector<unsigned>& fd_rings() const
            {
                return _fd_rings;
            }

            inline void set_iface(iface* iface_)
            {
                _iface = iface_;
            }

        protected:
            iface* _iface = nullptr;
            std::vector<unsigned> _fd_rings;
        };

        class _tx_ring_proxy : public _ring_proxy
        {
        public:

            _tx_ring_proxy() = default;

//			explicit _tx_ring_proxy(iface* iface_)
//				: _ring_proxy(iface_)
//			{
//			    if (_iface->_nmd == nullptr)
//			        std::cout << "_tx_ring: is null" << std::endl;
//			}

            tx_ring operator[](unsigned i_)
            {
                if (i_ >= _iface->tx_rings.count())
                    throw std::logic_error("netmap::iface: invalid tx ring id "
                                           + std::to_string(i_));

                return tx_ring(NETMAP_TXRING(_iface->_nmd->nifp, i_));
            }

            inline unsigned count() const
            {
                return _iface->_nmd->req.nr_tx_rings;
            }

            inline void synchronize()
            {
                ioctl(_iface->_nmd->fd, NIOCTXSYNC);
            }

//			void _init()
//            {
//                for (unsigned i = 0; i < _iface->tx_rings.count(); i++)
//					if (rx_ring(NETMAP_TXRING(_iface->_nmd->nifp, i)).buffer_size() > 0)
//						_fd_rings.push_back(i);
//            }
        };

        class _rx_ring_proxy : public _ring_proxy
        {
        public:
            _rx_ring_proxy() = default;

//		    explicit _rx_ring_proxy(iface* iface_)
//				: _ring_proxy(iface_)
//			{
//                if (_iface->_nmd == nullptr)
//                    std::cout << "_rx_ring: is null" << std::endl;
//			}

            rx_ring operator[](unsigned i_)
            {
                if (i_ >= _iface->rx_rings.count())
                    throw std::logic_error("netmap::iface: invalid rx ring id "
                                           + std::to_string(i_));

                return rx_ring(NETMAP_RXRING(_iface->_nmd->nifp, i_));
            }

            inline unsigned count() const
            {
                return _iface->_nmd->req.nr_rx_rings;
            }

            inline void synchronize()
            {
                ioctl(_iface->_nmd->fd, NIOCRXSYNC);
            }
//
//			void _init()
//            {
//                for (unsigned i = 0; i < _iface->rx_rings.count(); i++)
//                    if (rx_ring(NETMAP_RXRING(_iface->_nmd->nifp, i)).buffer_size() > 0)
//                        _fd_rings.push_back(i);
//            }
        };

    public:

        static unsigned count_tx_rings(const std::string& iface_name_)
        {
            auto nmd = _open(iface_name_);
            auto nr_tx_rings = nmd->req.nr_tx_rings;
            _close(nmd);
            return nr_tx_rings;
        }

        static unsigned count_rx_rings(const std::string& iface_name_)
        {
            auto nmd = _open(iface_name_);
            auto nr_rx_rings = nmd->req.nr_rx_rings;
            _close(nmd);
            return nr_rx_rings;
        }

        explicit iface(const std::string& iface_name_)
        {
            _nmd = iface::_open(iface_name_);

            if (_nmd == nullptr)
                std::cerr << "iface: is null" << std::endl;

            tx_rings.set_iface(this);
            rx_rings.set_iface(this);
        }

        int fd() const
        {
            return _nmd->fd;
        }

        ~iface()
        {
            nm_close(_nmd);
        }

        _tx_ring_proxy tx_rings;
        _rx_ring_proxy rx_rings;

    private:

        static nm_desc* _open(const std::string& iface_name_)
        {
//			std::cout << "iface::_open()" << std::endl;
//
            struct nm_desc base_nmd { };
            bzero(&base_nmd, sizeof(base_nmd));
            struct nm_desc* nmd = nm_open(("netmap:" + iface_name_).c_str(), nullptr, 0, &base_nmd);

            if (nmd == nullptr)
                throw std::runtime_error("netmap::iface: could not open device " + iface_name_);

            return nmd;
        }

        static void _close(nm_desc* nmd_)
        {
            nm_close(nmd_);
        }

        struct nm_desc* _nmd = nullptr;
    };
}

