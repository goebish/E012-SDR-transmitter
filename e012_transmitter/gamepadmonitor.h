#ifndef GAMEPADMONITOR_H
#define GAMEPADMONITOR_H

#include <QObject>
#include <QtGamepad/QGamepad>

class GamepadMonitor : public QObject
{
    Q_OBJECT

public:
    explicit GamepadMonitor(QObject *parent = 0);
    double aileron() { return m_aileron; }
    double elevator() { return m_elevator; }
    double throttle() { return m_throttle; }
    double rudder() { return m_rudder; }
    bool rebind() { return m_rebind; }

private:
    double m_aileron, m_elevator, m_throttle, m_rudder;
    bool m_rebind;

private slots:
    void gamepadConnected(int deviceId);
    void gamepadDisconnected(int deviceId);
    void gamepadButtonPressed(int deviceId, QGamepadManager::GamepadButton button, double value);
    void gamepadButtonReleased(int deviceId, QGamepadManager::GamepadButton button);
    void gamepadAxisChanged(int deviceId, QGamepadManager::GamepadAxis, double value);
};

#endif // GAMEPADMONITOR_H
