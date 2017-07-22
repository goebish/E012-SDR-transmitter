#include <QCoreApplication>
#include "e012_protocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    GamepadMonitor gamepad_monitor;
    E012_Protocol e012(&gamepad_monitor);
    return application.exec();
}
