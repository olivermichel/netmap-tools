# netmap-tools

## Install netmap

in /ansible:

    ansible-playbook -vv netmap-local.yml
    
load kernel module and run tests :

    sudo modprobe netmap
    sudo ip link set up dev enp2s0f0
    sudo TEST_IFACE=enp2s0f0 build/debug-gcc8/unit

## Configure ports

for example:

    sudo ip a add 192.168.15.1/24 dev enp2s0f0
    sudo ip a add 192.168.15.2/24 dev enp2s0f1

## Test pkt-gen

on one side:

    sudo pkt-gen -f tx -i netmap:enp2s0f0
    sudo pkt-gen -f tx -i netmap:enp2s0f0 -s 192.168.15.1 -d 192.168.15.2 -S 68:05:ca:39:84:94 -D 68:05:ca:39:84:95
    
on other side:
    
    sudo pkt-gen -f rx -i netmap:enp2s0f1

## Example Applications

on one side:

    sudo build/debug-gcc8/pkt_sender -i enp2s0f0 -s 68:05:ca:39:84:94 -d 68:05:ca:39:84:95

on other side:

    sudo build/debug-gcc8/pkt_receiver -i enp2s0f1

## Interface Library Receiver Example

```cpp
netmap::iface iface("enp2s0f0");

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

## Interface Library Sender Example

```cpp
netmap::iface iface("enp2s0f0");

char* buf = nullptr;
char msg[14] = {0};
struct ether_header* eth = (struct ether_header*) msg;
// set ethernet header

struct pollfd fds { .fd = iface.fd(), .events = POLLIN };

while (poll(&fds, 1, -1)) {
  while (iface.tx_rings[0].avail()) {
    buf = iface.tx_rings[0].next_buf();
    std::memcpy(buf, msg, 14);
    iface.tx_rings[0].advance(14);
  }
}
```
