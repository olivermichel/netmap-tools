
#define NETMAP_WITH_LIBS
#include <net/netmap_user.h>

#include <stdexcept>

namespace netmap {
	
	class iface
	{
		class _ring
		{
		public:
			explicit _ring(const iface& iface_, netmap_ring* ring_) 
				: _iface(iface_),
				  _ring_ptr(ring_) { }

			unsigned count_slots() const
			{
				return _ring_ptr->num_slots;
			}

			unsigned buffer_size() const
			{
				return _ring_ptr->nr_buf_size;
			}
			
			char* buf()
			{
				if (_ring_ptr->head != _ring_ptr->tail) {
					// while (_ring_ptr->head == _ring_ptr->tail) { }
					_head = _ring_ptr->head;
					unsigned next_head = nm_ring_next(_ring_ptr, _head);
					struct netmap_slot* slot = _ring_ptr->slot + _head;
					return NETMAP_BUF(_ring_ptr, slot->buf_idx);
				}
				return nullptr;
			}

			void advance(unsigned len_ = 0)
			{
				if (len_)
					(_ring_ptr->slot + _head)->len = len_;

				_ring_ptr->cur = _ring_ptr->head = _head;
			}

		private:
			const iface& _iface;
			netmap_ring* _ring_ptr = nullptr;
			unsigned _head = 0;
		};

		class _ring_proxy
		{
		public:
			enum class dir { rx = 0, tx = 1 };
	
			explicit _ring_proxy(const iface& iface_, dir dir_)
				: _dir(dir_), 
				  _iface(iface_) { }

			_ring operator[](unsigned i_) 
			{
				switch (_dir) {
					case dir::rx:
						if (i_ >= _iface.count_rx_rings())
							throw std::logic_error("netmap::iface: invalid rx ring id " + i_);
						return _ring(_iface, NETMAP_RXRING(_iface._nmd->nifp, i_));
					case dir::tx:
						if (i_ >= _iface.count_tx_rings())
							throw std::logic_error("netmap::iface: invalid tx ring id " + i_);
						return _ring(_iface, NETMAP_TXRING(_iface._nmd->nifp, i_));
				}
			}

		private:
			dir _dir;
			const iface& _iface;
		};

	public:
		explicit iface(const std::string& iface_name_)
			: _nmd(_open(iface_name_)),
			  tx_rings(*this, _ring_proxy::dir::tx),
			  rx_rings(*this, _ring_proxy::dir::rx) { }

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

		_ring_proxy tx_rings;
		_ring_proxy rx_rings;

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

