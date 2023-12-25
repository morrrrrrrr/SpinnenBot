
#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <windows.h>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

class SerialException : public std::exception {
private:
    std::string errorMessage;

public:
    SerialException(const std::string& message) : errorMessage("Serial Error: " + message) {}

    // Override the what() function to provide a custom error message
    const char* what() const noexcept override {
        return errorMessage.c_str();
    }
};

class SerialPort {
    std::string _port;
    int _bRate;
    bool _isOpen, _pauseReadThread;

    std::stringstream _readBuffer;
    std::thread _readThread;

    DCB _dcbSerialParams;
    HANDLE _hSerial;
public:
    SerialPort(const std::string& port, int bRate = 9600) : _port(port), _bRate(bRate) {
        _isOpen = false;
        _readBuffer.clear();
    }

    // Deconstructor: cleanup
    ~SerialPort() {
        if (_isOpen)
            close();
    }

    SerialPort(const SerialPort&) = delete;  // Deleted copy constructor
    SerialPort& operator=(const SerialPort&) = delete;  // Deleted copy assignment operator

    void readFunctionThread(bool& open, bool& pause) {
        while (open) {
            while (pause && open) { } // pause the thread while bigger read operations are happening
            DWORD pos;
            char c;
            
            if (ReadFile(_hSerial, &c, 1, &pos, nullptr)) {
                if (pos) { // read a byte
                    _readBuffer << c;
                }
                else { // read empty
                    
                }
            }
        }
    }

    bool open() {
        try {
            if (_isOpen) {
                throw SerialException("Tried to open Port that was already open");
            }

            _hSerial = CreateFileA(
                _port.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                0,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                0
            );

            if (_hSerial == INVALID_HANDLE_VALUE) {
                throw SerialException("Error opening serial port.");
            }

            _dcbSerialParams = { 0 };
            _dcbSerialParams.DCBlength = sizeof(_dcbSerialParams);
            _dcbSerialParams.BaudRate = (DWORD)_bRate;
            _dcbSerialParams.ByteSize = 8;
            _dcbSerialParams.StopBits = ONESTOPBIT;
            _dcbSerialParams.Parity   = NOPARITY;

            if (!SetCommState(_hSerial, &_dcbSerialParams)) {
                CloseHandle(_hSerial);
                throw SerialException("Error setting serial port state.");
            }

            COMMTIMEOUTS timeouts = { 0 };
            timeouts.ReadIntervalTimeout = 10000;
            timeouts.ReadTotalTimeoutConstant = 1000;
            timeouts.ReadTotalTimeoutMultiplier = 0;
            timeouts.WriteTotalTimeoutConstant = 100;
            timeouts.WriteTotalTimeoutMultiplier = 0;

            if (!SetCommTimeouts(_hSerial, &timeouts)) {
                CloseHandle(_hSerial);
                throw SerialException("Error setting serial port timeouts.");
            }
            
            _isOpen = true;
            _pauseReadThread = false;

            // initialize read thread
            _readBuffer.clear();

            _readThread = std::thread(readFunctionThread, this, std::ref(this->_isOpen), std::ref(this->_pauseReadThread));
        }
        catch (const std::exception& exception) {
            std::cerr << "Failed to open SerialPort: " << exception.what() << "\n";
            return false;
        }
        return true;
    }

    void close() {
        if (!_isOpen) {
            std::cerr << "Tried to close Port that was already closed\n";
            return;
        }

        _isOpen = false;
        
        _readThread.join();

        CloseHandle(_hSerial);
    }
    
    std::size_t available() {
        if (!_isOpen) {
            std::cerr << "Accessed SerialPort.available() while the port was closed\n";
            return 0;
        } 

        return _readBuffer.str().size();
    }

    char readChar() {
        if (available())
            return _readBuffer.get();
        
        return 0;
    }

    std::string readString() {
        std::string str;
        _readBuffer >> str;
        return str;
    }

    std::string readString(int length) {
        while (available() < length) {
            // wait for the read buffer to fill up until it is at least `length` long
        }
        char c;
        int len = 0;
        std::string output;
        while ((c = readChar()) && len < length) {
            len++;
            output = output + c;
        }
        return output;
    }

    void writeString(const std::string& str) {
        if (!_isOpen)
            return;

        DWORD pos;
        int isWritten = WriteFile(_hSerial, (LPCVOID)str.c_str(), str.size(), &pos, nullptr);
        if (!isWritten) {
            throw SerialException("Error writing to serial port: " + std::to_string(GetLastError()));
        }
    }

    bool isOpen() {
        return _isOpen;
    }
    
    friend SerialPort& operator<<(SerialPort& port, const std::string& str);
    friend std::string& operator>>(SerialPort& port, std::string& str);
    friend char& operator>>(SerialPort& port, char& c);
}; // class port

SerialPort& operator<<(SerialPort& port, const std::string& str) {
    port.writeString(str);
    return port;
}

std::string& operator>>(SerialPort& port, std::string& str) {
    port._readBuffer >> str;
    return str;
}

char& operator>>(SerialPort& port, char& c) {
    c = port.readChar();
    return c;
}


#endif