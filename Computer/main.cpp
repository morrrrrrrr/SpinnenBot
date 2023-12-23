/*
* The main program for the computer side
*/

#include <iostream>
#include <fstream>
#include "hexapod.hpp"

int main() {
    std::cout << "Hello, World!\n";

    Hexapod hexapod;
    
    std::ofstream file("data.json", std::ios::out | std::ios::trunc);
    file << hexapod.save().dump(2);
    file.close();
}