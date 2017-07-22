// some information on HS6200 RE:
// https://www.deviationtx.com/forum/protocol-development/5433-mould-king-33043-super-f-quad-hs6200-rf-chip
// https://www.deviationtx.com/forum/protocol-development/7150-eachine-e012

#include "hs6200_engine.h"
#include <QDebug>

//#define DEBUG_HS6200
#define DEST_IP "127.0.0.1" // gnuradio udp source address
#define DEST_PORT 1234      // gnuradio udp source port

HS6200_Engine::HS6200_Engine(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Send UDP packets for gnuradio to" << DEST_IP << ":" << DEST_PORT;
}

void HS6200_Engine::enableCRC(bool enabled)
{
    crc_enabled = enabled;
}

void HS6200_Engine::setTXAddr(const uint8_t* addr, uint8_t len)
{
    // precompute address crc
    uint8_t i;
    crc_init = 0xffff;
    for (i = 0; i<len; i++)
        crc_init = crc_update(crc_init, addr[len - 1 - i], 8);
    memcpy(tx_addr, addr, len);
    tx_addr_len = len;
}

void HS6200_Engine::writePayload(const uint8_t* msg, uint8_t len)
{
    const uint8_t scramble[] = {
        0x80,0xf5,0x3b,0x0d,0x6d,0x2a,0xf9,0xbc,
        0x51,0x8e,0x4c,0xfd,0xc1,0x65,0xd0 };

    uint8_t payload[64];
    int i;
    const uint8_t no_ack = 1; // never ask for an ack
    static uint8_t pid;
    uint8_t pos = 0;

    // preamble
    payload[pos++] = tx_addr[tx_addr_len - 1] & 0x80 ? 0xaa : 0x55;

    // address
    for (i = tx_addr_len - 1; i >= 0; i--)
        payload[pos++] = tx_addr[i];

    // guard bytes
    payload[pos++] = tx_addr[0];
    payload[pos++] = tx_addr[0];

    // packet control field
    payload[pos++] = ((len & 0x3f) << 2) | (pid & 0x03);
    payload[pos] = (no_ack & 0x01) << 7;
    pid++;

    // scrambled payload
    if (len > 0) {
        payload[pos++] |= (msg[0] ^ scramble[0]) >> 1;
        for (i = 1; i<len; i++)
            payload[pos++] = ((msg[i - 1] ^ scramble[i - 1]) << 7) | ((msg[i] ^ scramble[i]) >> 1);
        payload[pos] = (msg[len - 1] ^ scramble[len - 1]) << 7;
    }

    // crc
    if (crc_enabled) {
        uint16_t crc = calc_crc(&payload[1 + tx_addr_len + 2], len + 2);
        uint8_t hcrc = crc >> 8;
        uint8_t lcrc = crc & 0xff;
        payload[pos++] |= (hcrc >> 1);
        payload[pos++] = (hcrc << 7) | (lcrc >> 1);
        payload[pos++] = lcrc << 7;
    }

#ifdef DEBUG_HS6200
    printf("\n");
    for(i=0; i<pos; i++)
        printf("%02x ", (uint8_t)payload[i]);
#endif

    // send payload to gnuradio
    QByteArray data((const char*)payload, pos);
    QHostAddress addr(DEST_IP);
    client.writeDatagram(data, addr, DEST_PORT);
}

uint16_t HS6200_Engine::crc_update(uint16_t crc, uint8_t byte, uint8_t bits)
{
    const uint16_t polynomial = 0x1021;
    crc = crc ^ (byte << 8);
    while (bits--)
        if ((crc & 0x8000) == 0x8000)
            crc = (crc << 1) ^ polynomial;
        else
            crc = crc << 1;
    return crc;
}

uint16_t HS6200_Engine::calc_crc(uint8_t* msg, uint8_t len)
{
    uint8_t pos;
    uint16_t crc = crc_init;

    // pcf + payload
    for (pos = 0; pos < len - 1; pos++)
        crc = crc_update(crc, msg[pos], 8);
    // last remaining bit
    crc = crc_update(crc, msg[pos + 1], 1);
    return crc;
}
