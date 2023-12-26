/*
* The main program for the computer side
*/

#include <iostream>
#include <fstream>
#include "hexapod.hpp"
#include "connection.hpp"
#include "input.hpp"

void save_to_file(Hexapod& hex, const std::string& filename) {
    if (filename == "default_data.json") {
        std::cerr << "Error: Tried overwriting the default data! use another filename!\n";
        return;
    }

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

    Renderer::init({800, 600});

    SerialConnection::setPortName("COM1");
    if (SerialConnection::openConnection()) std::cout << "Sucess\n";

    Hexapod hexapod;

    load_from_file(hexapod, "new_data.json");

    // hexapod.calculateRestingPositions(100);

    Input input;

    save_to_file(hexapod, "new_data2.json");

    unsigned long long t0 = clock();
    while (true) {
        float dt = (clock() - t0) / 1000.0f;
        t0 = clock();

        input.update();

        Vector2f lin_input_vector({
            static_cast<float>(input.getKey(sf::Keyboard::D) - input.getKey(sf::Keyboard::A)),
            static_cast<float>(input.getKey(sf::Keyboard::W) - input.getKey(sf::Keyboard::S))
        });
        float rot_input = static_cast<float>(input.getKey(sf::Keyboard::E) - input.getKey(sf::Keyboard::Q));

        if (lin_input_vector.at(0) || lin_input_vector.at(1))
            hexapod.update(dt, lin_input_vector.magnitude(), lin_input_vector.inverted(), rot_input);
        else if (rot_input)
            hexapod.update(dt, 0, {0, 1}, rot_input);

        hexapod.calculateServoAngles();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Renderer::stop();
}