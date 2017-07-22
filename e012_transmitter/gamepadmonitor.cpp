#include "gamepadmonitor.h"
#include <QDebug>

GamepadMonitor::GamepadMonitor(QObject *parent)
    : QObject(parent)
{ 
    m_throttle = m_aileron = m_elevator = m_rudder = 0.0;
    m_rebind = false;

    qDebug() << "Waiting for gamepad, press start to (re)bind";

    connect(QGamepadManager::instance(), SIGNAL(gamepadConnected(int)), 
        this, SLOT(gamepadConnected(int)));

    connect(QGamepadManager::instance(), SIGNAL(gamepadDisconnected(int)), 
        this, SLOT(gamepadDisconnected(int)));

    connect(QGamepadManager::instance(), SIGNAL(gamepadButtonPressEvent(int, QGamepadManager::GamepadButton,double)), 
        this, SLOT(gamepadButtonPressed(int, QGamepadManager::GamepadButton, double)));

    connect(QGamepadManager::instance(), SIGNAL(gamepadButtonReleaseEvent(int, QGamepadManager::GamepadButton)), 
        this, SLOT(gamepadButtonReleased(int, QGamepadManager::GamepadButton)));

    connect(QGamepadManager::instance(), SIGNAL(gamepadAxisEvent(int, QGamepadManager::GamepadAxis, double)), 
        this, SLOT(gamepadAxisChanged(int, QGamepadManager::GamepadAxis, double)));
}

// not triggered on Linux ?
void GamepadMonitor::gamepadConnected(int deviceId)
{
    Q_UNUSED(deviceId)
    qDebug() << "Gamepad connected";
}

// not triggered on Linux -> no failsafe!
void GamepadMonitor::gamepadDisconnected(int deviceId)
{
    Q_UNUSED(deviceId)
    qDebug() << "Gamepad disconnected";
    m_throttle = 0.0;
}

void GamepadMonitor::gamepadButtonPressed(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    Q_UNUSED(deviceId)
    switch (button) {
        // use right analog trigger for throttle
        case QGamepadManager::GamepadButton::ButtonR2:
            m_throttle = value;
            break;
        case QGamepadManager::GamepadButton::ButtonStart:
            m_rebind = true;
            break;
        default:
            break;
    }
}

void GamepadMonitor::gamepadButtonReleased(int deviceId, QGamepadManager::GamepadButton button)
{
    Q_UNUSED(deviceId)
    switch (button) {
    case QGamepadManager::GamepadButton::ButtonR2:
        m_throttle = 0.0;
        break;
    case QGamepadManager::GamepadButton::ButtonStart:
        m_rebind = false;
        break;
    default:
        break;
    }
}

void GamepadMonitor::gamepadAxisChanged(int deviceId, QGamepadManager::GamepadAxis axis, double value)
{
    Q_UNUSED(deviceId)
    switch (axis) {
        case QGamepadManager::GamepadAxis::AxisLeftX:
            m_rudder = value;
            break;
        case QGamepadManager::GamepadAxis::AxisRightX:
            m_aileron = value;
            break;
        case QGamepadManager::GamepadAxis::AxisRightY:
            m_elevator = -value;
            break;
        default:
            break;
    }
}
