
#include "SFML/Graphics.hpp"
#include "vector.hpp"
#include "constants.hpp"
#include <vector>
#include <thread>

#ifndef RENDERER_HPP
#define RENDERER_HPP

// This struct defines a line between point a and b
// @param a index of point a
// @param b index of point b
// @param width width of the line
// @param color color of the line
struct Connection {
    int a, b;
    float width;
    sf::Color color;
};

class Renderer {
    bool m_is_running = false;
    std::thread m_running_thread;

    std::vector<Vector2f> m_points;
    std::vector<Connection> m_connections;

    Renderer() {

    }

    // this function runs on a different thread
    static void thread_function(const sf::Vector2u& window_size) {
        Renderer& rend = Renderer::getInstance();

        // create the window
        sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Render", sf::Style::Close | sf::Style::Titlebar);

        window.setFramerateLimit(60);

        const sf::Color background_color = sf::Color::Black;

        auto draw_line = [&](Vector2f p1, Vector2f p2, float width, sf::Color col) -> void {
            Vector2f diff = p2 - p1;
            float length = width + diff.magnitude();
            float angle = std::atan2(diff.at(1), diff.at(0));

            sf::RectangleShape rectangle({length, width});
            rectangle.setOrigin(sf::Vector2f{
                width / 2.0f, width / 2.0f
            });
            rectangle.setRotation(angle * 180 / M_PI);
            rectangle.setPosition(p1.at(0), p1.at(1));
            rectangle.setFillColor(col);

            window.draw(rectangle);
        };

        // enter the main loop
        while (window.isOpen()) {
            // poll event
            sf::Event event;
            while (window.pollEvent(event)) { 
                switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                
                default:
                    break;
                }
            }

            // check if the renderer should still be running:
            if (!rend.m_is_running) window.close();

            window.clear(background_color);

            // draw lines here:
            for (const Connection& connection : rend.m_connections) {
                draw_line(
                    rend.m_points[connection.a],
                    rend.m_points[connection.b],
                    connection.width,
                    connection.color
                );
            }

            window.display();
        }
    }
public:
    static Renderer& getInstance() { 
        static Renderer instance;
        return instance;
    }

    static void init(const sf::Vector2u& window_size) {
        Renderer& rend = Renderer::getInstance();
        rend.m_is_running = true;

        rend.m_running_thread = std::thread(
            thread_function,
            std::ref(window_size) // thread arguments
        );
    }

    static void stop() {
        Renderer& rend = Renderer::getInstance();

        rend.m_is_running = true;
        rend.m_running_thread.join();
    }

    static void setPoints(const std::vector<Vector2f>& points) {
        Renderer::getInstance().m_points = points;
    }
    static void setConnections(const std::vector<Connection>& connections) {
        Renderer::getInstance().m_connections = connections;
    }
};

#endif
