#ifndef HS6200_ENGINE_H
#define HS6200_ENGINE_H

#include <QObject>
#include <QUdpSocket>

class HS6200_Engine : public QObject
{
public:
    HS6200_Engine(QObject *parent = 0);
    void setTXAddr(const uint8_t* addr, uint8_t len);
    void writePayload(const uint8_t* msg, uint8_t len);
    void enableCRC(bool enabled);

private:
    uint16_t crc_update(uint16_t crc, uint8_t byte, uint8_t bits);
    uint16_t calc_crc(uint8_t* msg, uint8_t len);
    QUdpSocket client;
    bool crc_enabled;
    uint16_t crc_init;
    uint8_t tx_addr[5];
    uint8_t tx_addr_len;
};

#endif // HS6200_ENGINE_H
