
#include "serial.hpp"
#include <array>
#include <cmath>

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

class SerialConnection {
    SerialPort m_port;
    std::string m_port_name;

    SerialConnection() : m_port("N/A", 9600), m_port_name("N/A") { }
public:
    // Set the Port Name
    static void setPortName(const std::string& name) { getInstance().m_port_name = name; }

    static SerialConnection& getInstance() {
        static SerialConnection instance;
        return instance;
    }

    static bool openConnection() {
        return getInstance().m_port.open();
    }
    static void closeConnection() {
        getInstance().m_port.close();
    }
    
    // Send Motor angles to connected arduino with set Protocol (see README)
    static void sendAngles(const std::array<float, 18>& angles) {
        SerialConnection& connection = getInstance();

        auto round = [&](float f) -> float {
            return static_cast<float>(static_cast<int>(f * 100.0f) / 100.0f);
        };

        for (int i = 0; i < 18; i++) {
            std::string msg;

            // motor index
            msg += static_cast<char>(i + 'a');

            // round the number for easier communication
            msg += round(angles[i]);

            // send newline to end command
            msg += '\n';
            
            // send msg to serial port
            connection.m_port << msg;
        }
    }
};

#endif
