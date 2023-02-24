#ifndef RTSP_INTERNAL_NET_ADDRESS_H
#define RTSP_INTERNAL_NET_ADDRESS_H

#include <netinet/in.h>

namespace rtsp {
typedef unsigned int netAddressBits;

bool IsMulticastAddress(netAddressBits address);

class AddressString {
public:
    AddressString(struct sockaddr_in const &addr);
    AddressString(struct in_addr const &addr);
    AddressString(netAddressBits addr);

    virtual ~AddressString();

    char const *val() const {
        return fVal;
    }

private:
    void init(netAddressBits addr);

private:
    char *fVal;
};
}

#endif
