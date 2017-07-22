#ifndef E012_PROTOCOL_H
#define E012_PROTOCOL_H

#include <QObject>
#include <qtimer.h>
#include "hs6200_engine.h"
#include "gamepadmonitor.h"

#define NUM_RF_CHANNELS 4
#define ADDRESS_LENGTH 5
#define PACKET_SIZE 15

class E012_Protocol : public QObject
{
    Q_OBJECT

public:
    E012_Protocol(GamepadMonitor *monitor);

private:
    void initialize();
    void initialize_txid();
    uint8_t scale_input(double chanval, double srcMin, double srcMax, uint8_t destMin, uint8_t destMax);
    void send_packet(uint8_t bind);
    enum Phase{
        BIND,
        CONTROL
    } phase;
    GamepadMonitor* gamepad;
    QTimer tickTimer;
    QTimer logTimer;
    HS6200_Engine hs6200;
    uint16_t bind_counter;
    uint8_t tx_addr[ADDRESS_LENGTH];
    uint8_t rf_chans[NUM_RF_CHANNELS];
    uint8_t packet[PACKET_SIZE];

private slots:
    void step();
    void log_sticks();
};

#endif // E012_PROTOCOL_H
