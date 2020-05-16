#include "P2PSocket.h"
#include "P2PMessage.h"
#include "P2PConnection.h"

#include <stdexcept>


#include <socket/Packet.h>

/* For checksum calculation */
#include <mining/hashplugin.h>

#include <util/logger.h>

#define P2P_MAX_PAYLOAD     (32 * 1024 * 1024)


struct p2pheader
{
    uint32_t    magic;
    char        cmd[12];
    int32_t     payload_size;
    uint32_t    checksum;
};


void P2PSocket::send(const P2PMessage &message)
{
    static HashPluginRef SHA256d = get_hashplugin("sha256d");
    p2pheader header;

    memset(&header, 0, sizeof(header));

    header.magic = this->_parent.networkMagic();
    strcpy(header.cmd, message.cmd().c_str());
    header.payload_size = message.payload().size();

    Hash256 checksum = SHA256d->hash(message.payload());
    memcpy(&header.checksum, checksum.raw(), sizeof(header.checksum));

    mlog(DEBUG, " >>> [%s] (%d, %08x)", header.cmd, header.payload_size, header.checksum);
    if (header.payload_size)
        mlog(DEBUG, " >>> %s", message.payload().asHex().c_str());

    this->Socket::send(ByteString::from(header) + message.payload());
}


void P2PSocket::onReceive(const Packet &packet)
{
    static HashPluginRef SHA256d = get_hashplugin("sha256d");

    _buffer += packet;

    while (_buffer.size() >= sizeof(p2pheader))
    {
        p2pheader hdr;
        memcpy(&hdr, _buffer.data(), sizeof(hdr));

        if (hdr.magic != _parent.networkMagic())
        {
            throw std::runtime_error("Error in P2P connection: Invalid network magic received (" + ByteString::from(hdr.magic).asHex() + " != " + ByteString::from(_parent.networkMagic()).asHex() + ")");
        }

        if (hdr.payload_size < 0 || hdr.payload_size >= P2P_MAX_PAYLOAD)
            throw std::runtime_error("Error in P2P connection: Invalid packet size " + std::to_string(hdr.payload_size));

        size_t packet_size = sizeof(hdr) + hdr.payload_size;

        if (_buffer.size() < packet_size)
            return;

        ConstByteStringRef payload(_buffer.data() + sizeof(p2pheader), hdr.payload_size);
        Hash256 checksum = SHA256d->hash(payload);

        if (memcmp(checksum.bytes().data, &hdr.checksum, sizeof(hdr.checksum)))
            throw std::runtime_error("Error in P2P connection: Checksum error");

        mlog(DEBUG, " <<< [%s] (%d, %08x)", hdr.cmd, hdr.payload_size, hdr.checksum);
        if (hdr.payload_size)
            mlog(DEBUG, " <<< %s", payload.asHex().c_str());

        _parent.processMessage(P2PMessage::parse({ hdr.cmd, std::move(payload) }));

        if (_buffer.size() == packet_size)
        {
            _buffer.clear();
            return;
        }

        /*
         * WARNING/FIXME: Copying (parts of) a ByteString to itself is undefined.
         */
        this->_buffer = ByteString(_buffer.data() + packet_size, _buffer.size() - packet_size);
    }
}
