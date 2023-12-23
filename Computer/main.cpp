/*
* The main program for the computer side
*/

#include <iostream>
#include <fstream>
#include "hexapod.hpp"

void save_to_file(Hexapod& hex, const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    file << hex.save().dump(2);
    file.close();
}
void load_from_file(Hexapod& hex, const std::string& filename) {
    nlohmann::json data;

    std::ifstream file(filename, std::ios::in);
    if (!file) {
        std::cout << "Error opening" << filename << "\n";
        std::exit(-1);
    }
    file >> data;
    file.close();

    hex.load(data);
}

int main() {
    std::cout << "Hello, World!\n";

    Hexapod hexapod;
    
    load_from_file(hexapod, "data.json");

    hexapod.calculateRestingPositions(100);

    save_to_file(hexapod, "new_data.json");
}