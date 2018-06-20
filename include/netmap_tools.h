
#define NETMAP_WITH_LIBS
#include <net/netmap_user.h>

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
			
			unsigned count_slots() const
			{
				return _ring->num_slots;
			}

			unsigned buffer_size() const
			{
				return _ring->nr_buf_size;
			}

			~ring()
			{
				_ring = nullptr;
			}

		private:
			netmap_ring* _ring;
		};

		class tx_ring : public ring
		{
		public:
			tx_ring(netmap_ring* ring_)
				: ring(ring_) { }
		};

		class rx_ring : public ring
		{
		public:
			rx_ring(netmap_ring* ring_)
				: ring(ring_) { }
		};

		class _ring_proxy
		{
		public:
			explicit _ring_proxy(const iface& iface_)
				: _iface(iface_) { }
		protected:
			const iface& _iface;
		};

		class _tx_ring_proxy : public _ring_proxy
		{
		public:
			explicit _tx_ring_proxy(const iface& iface_)
				: _ring_proxy(iface_) { }	
			
			tx_ring operator[](unsigned i_)
			{
				if (i_ >= _iface.count_tx_rings())
					throw std::logic_error("netmap::iface: invalid tx ring id " + i_);

				return tx_ring(NETMAP_TXRING(_iface._nmd->nifp, i_));
			}
		};

		class _rx_ring_proxy : public _ring_proxy
		{
		public:
			explicit _rx_ring_proxy(const iface& iface_) 
				: _ring_proxy(iface_) { }

			rx_ring operator[](unsigned i_)
			{
				if (i_ >= _iface.count_rx_rings())
					throw std::logic_error("netmap::iface: invalid rx ring id " + i_);
				
				return rx_ring(NETMAP_RXRING(_iface._nmd->nifp, i_));
			}
		};
	
	public:
		explicit iface(const std::string& iface_name_)
			: _nmd(_open(iface_name_)), tx_rings(*this), rx_rings(*this) { }

		inline unsigned count_rx_rings() const
		{
			return _nmd->req.nr_rx_rings;
		}

		inline unsigned count_tx_rings() const
		{
			return _nmd->req.nr_tx_rings;
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
		struct nm_desc* _open(const std::string& iface_name_)
		{
			struct nm_desc base_nmd { };
			bzero(&base_nmd, sizeof(base_nmd));
			struct nm_desc* nmd = nm_open(("netmap:" + iface_name_).c_str(), nullptr, 0, &base_nmd);

			if (!nmd)
				throw std::runtime_error("netmap::iface: could not open device " + iface_name_);

			struct netmap_if* nifp = nmd->nifp;
			struct nmreq* req = &nmd->req;
			return nmd;
		}

		struct nm_desc* _nmd = nullptr;
	};
}

