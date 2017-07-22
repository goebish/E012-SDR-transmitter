#include "e012_protocol.h"
#include <QDebug>

//#define DEBUG_E012
#define LOG_INTERVAL 500
#define PACKET_INTERVAL 4 // 4ms between packets
#define BIND_COUNT 500
#define RF_BIND_CHANNEL 0x3c

E012_Protocol::E012_Protocol(GamepadMonitor *input)
{
    gamepad = input;
    initialize();
    connect(&logTimer, SIGNAL(timeout()), this, SLOT(log_sticks()));
    tickTimer.setTimerType(Qt::PreciseTimer);
    connect(&tickTimer, SIGNAL(timeout()), this, SLOT(step()));
    tickTimer.start(PACKET_INTERVAL);
}

void E012_Protocol::initialize()
{
    logTimer.stop();
    hs6200.enableCRC(true);
    initialize_txid();
    // bind address
    hs6200.setTXAddr((uint8_t*)"\x55\x42\x9c\x8f\xc9", ADDRESS_LENGTH);
    bind_counter = BIND_COUNT;
    phase = BIND;
    qDebug() << "Sending bind packets";
}

void E012_Protocol::initialize_txid()
{
    uint8_t i;
    // tx address
    tx_addr[0] = 0xd1;
    tx_addr[1] = 0xcf;
    tx_addr[2] = 0xb6;
    tx_addr[3] = 0x5d;
    tx_addr[4] = 0x8d;
    // rf channels 
    // hack: use only 1 out of 4 freqs, same one as bind
    // so we don't have to hop
    for (i = 0; i<NUM_RF_CHANNELS; i++) {
        rf_chans[i] = RF_BIND_CHANNEL; // use same as bind ...
    }
}

uint8_t E012_Protocol::scale_input(double chanval, double srcMin, double srcMax, uint8_t destMin, uint8_t destMax)
{
    int32_t range = (int32_t)destMax - (int32_t)destMin;
    double chan_range = srcMax - srcMin;
    return (range * (chanval - srcMin)) / chan_range + destMin;
}

void E012_Protocol::send_packet(uint8_t bind)
{
    static uint8_t current_chan = 0;

    packet[0] = tx_addr[1];
    if (bind) {
        packet[1] = 0xaa;
        memcpy(&packet[2], rf_chans, NUM_RF_CHANNELS);
        memcpy(&packet[6], tx_addr, ADDRESS_LENGTH);
    }
    else {
        packet[1] = 0x01
            /*| GET_FLAG(CHANNEL_RTH, 0x04)
            | GET_FLAG(CHANNEL_HEADLESS, 0x10)
            | GET_FLAG(CHANNEL_FLIP, 0x40)*/;
        
        packet[2] = scale_input(gamepad->aileron(), -1.0, 1.0, 0, 200); // aileron
        packet[3] = scale_input(gamepad->elevator(), -1.0, 1.0, 0, 200); // elevator
        packet[4] = scale_input(gamepad->rudder(), -1.0, 1.0, 0, 200); // rudder
        packet[5] = scale_input(gamepad->throttle(), 0.0, 1.0, 0, 200); // throttle

        packet[6] = 0xaa;
        packet[7] = 0x02; // rate (0-2)
        packet[8] = 0x00;
        packet[9] = 0x00;
        packet[10] = 0x00;
    }
    packet[11] = 0x00;
    packet[12] = 0x00;
    packet[13] = 0x56;
    packet[14] = tx_addr[2];

#ifdef DEBUG_E012
    printf("\n");
    for (int i = 0; i < 15; i++)
        printf("%02x ", packet[i]);
#endif

    hs6200.writePayload(packet, PACKET_SIZE);
}

void E012_Protocol::step()
{
    if (gamepad->rebind() && bind_counter == 0)
        initialize();
    
    switch (phase) {
        case BIND:
            if (bind_counter == 0) {
                hs6200.setTXAddr(tx_addr, 5);
                phase = CONTROL;
                qDebug() << "Sending control packets";
                logTimer.start(LOG_INTERVAL);
            }
            else {
                send_packet(1);
                bind_counter--;
            }
            break;
        case CONTROL:
            send_packet(0);
            break;
    }
}

void E012_Protocol::log_sticks()
{
    qDebug() << "throttle:" << packet[5]
             << "aileron:"  << packet[2]
             << "elevator:" << packet[3]
             << "rudder:"   << packet[4];
}