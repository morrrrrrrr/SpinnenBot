
#include "SFML/Graphics.hpp"
#include "vector.hpp"
#include "constants.hpp"
#include <vector>
#include <algorithm>
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

    std::vector<Vector3f> m_points;
    std::vector<Connection> m_connections;

    Vector2u m_screen_size;

    Renderer() {

    }

    // this function runs on a different thread
    static void thread_function(const Vector2u& window_size) {
        Renderer& rend = Renderer::getInstance();

        // create the window
        sf::RenderWindow window(sf::VideoMode(window_size.at(0), window_size.at(1)), "Render", sf::Style::Close | sf::Style::Titlebar);

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

            // copy connections vector, to fix error when connections are reset, while sorting
            std::vector<Connection> connections(rend.m_connections);

            std::sort(connections.begin(), connections.end(), [&rend](const Connection& a, const Connection& b) -> bool {
                // calculate the z pos of the middle of connection a and b
                float a_z = (rend.m_points[a.a].at(2) + rend.m_points[a.b].at(2)) / 2;
                float b_z = (rend.m_points[b.a].at(2) + rend.m_points[b.b].at(2)) / 2;

                // return, which line should be drawn first -> the line more behind
                return a_z > b_z;
            });

            // draw lines here:
            for (const Connection& connection : connections) {
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

    static void init(const Vector2u& window_size) {
        Renderer& rend = Renderer::getInstance();
        rend.m_is_running = true;
        rend.m_screen_size = window_size;

        rend.m_running_thread = std::thread(
            thread_function,
            std::ref(window_size) // thread arguments
        );
    }

    static void stop() {
        Renderer& rend = Renderer::getInstance();

        // idiot safety (for me)
        if (!rend.m_is_running) return;

        rend.m_is_running = false;
        rend.m_running_thread.join();
    }

    static void setPoints(const std::vector<Vector3f>& points) {
        Renderer::getInstance().m_points = points;
    }
    static void setConnections(const std::vector<Connection>& connections) {
        Renderer::getInstance().m_connections = connections;
    }

    static Vector2u getScreenSize() {
        return Renderer::getInstance().m_screen_size;
    }
};

#endif
