# netmap-tools

## Interface Library Receiver Example

```cpp
netmap::iface iface("enp130s0f0");

unsigned len = 0;
char* buf = nullptr;
struct pollfd fds { .fd = iface.fd(), .events = POLLIN };

while (poll(&fds, 1, -1)) {
  while (iface.rx_rings[0].avail()) {
    buf = iface.rx_rings[0].next_buf(len);
    // do something with buf of length len
    iface.rx_rings[0].advance();
  }
}
```
