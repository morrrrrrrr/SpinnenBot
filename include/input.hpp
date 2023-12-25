
#pragma once

#include "SFML/Graphics.hpp"
#include <array>

class Input {
    std::array<bool, sf::Keyboard::KeyCount> m_key_map, m_prev_key_map;
    std::array<bool, sf::Mouse::ButtonCount> m_button_map, m_prev_button_map;
public:
    Input() { }

    void update() {
        m_prev_key_map = m_key_map;
        m_prev_button_map = m_button_map;
        
        for (unsigned i = 0; i < sf::Keyboard::KeyCount; i++) {
            m_key_map[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));
        }

        for (unsigned i = 0; i < sf::Mouse::ButtonCount; i++) {
            m_button_map[i] = sf::Mouse::isButtonPressed(sf::Mouse::Button(i));
        }
    }

    bool getKey(const sf::Keyboard::Key& key) const {
        return m_key_map[key];
    }
    bool getKeyDown(const sf::Keyboard::Key& key) const {
        return m_key_map[key] && !m_prev_key_map[key];
    }
    bool getKeyUp(const sf::Keyboard::Key& key) const {
        return !m_key_map[key] && m_prev_key_map[key];
    }

    bool getButton(const sf::Mouse::Button& button) const {
        return m_button_map[button];
    }
    bool getButtonDown(const sf::Mouse::Button& button) const {
        return m_button_map[button] && !m_prev_button_map[button];
    }
    bool getButtonUp(const sf::Mouse::Button& button) const {
        return !m_button_map[button] && m_prev_button_map[button];
    }
};
