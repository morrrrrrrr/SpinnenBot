
#include "constants.hpp"
#include "renderer.hpp"
#include "nlohmann/json.hpp"
#include "../leg.hpp"
#include <cmath>

#ifndef ROBOT_RENDERER_HPP
#define ROBOT_RENDERER_HPP

// The camera in this example is a static camera
// converts 3d points into 2d points and sends them to the renderer
class Camera {
    // offset of the points that are going to be rendered
    Vector3f m_offset    { 0, -40,  500 };
    // rotation of the points that are going to be rendered
    Vector3f m_rotation  { 0,  0,  0 };
    // world screen size
    Vector3f m_3d_screen { 1,  1,  0 };

    Vector3f convertScreenCoordinate(const Vector3f& point) {
        const Vector2u screen_size = Renderer::getScreenSize();

        Vector3f p = point + m_3d_screen / 2.0f;

        p = Vector3f{
            p.at(0) * screen_size.at(0) / m_3d_screen.at(0),
            screen_size.at(1) - (p.at(1) * screen_size.at(1) / m_3d_screen.at(1)),
            p.at(2)
        };

        return p;
    }

    void applyTransformations(std::vector<Vector3f>& points) {
        // TODO: apply rotation

        auto multiply_row = [](const Vector3f& row, const float& f) -> float {
            float ans = 0;
            for (int i = 0; i < 3; i++) {
                ans += row.at(i) * f;
            }
            return ans;
        };
        
        auto deg_to_rad = [](const float& deg) -> float { return deg * M_PI / 180.0f; };

        float a = deg_to_rad(m_rotation.at(0));
        float b = deg_to_rad(m_rotation.at(1));
        float c = deg_to_rad(m_rotation.at(2));

        for (Vector3f& point : points) {
            point = {
                multiply_row(
                    {std::cos(b)*std::cos(c), std::sin(a)*std::sin(b)*std::cos(c) - std::cos(a)*std::sin(c), std::cos(a)*std::sin(b)*std::cos(c) + std::sin(a)*std::sin(c)}, 
                    point.at(0)
                ),
                multiply_row(
                    {std::cos(b)*std::sin(c), std::sin(a)*std::sin(b)*std::sin(c) + std::cos(a)*std::cos(c), std::cos(a)*std::sin(b)*std::sin(c) - std::sin(a)*std::cos(c)}, 
                    point.at(1)
                ),
                multiply_row(
                    {-std::sin(b), std::sin(a)*std::cos(b), std::cos(a)*std::cos(b)}, 
                    point.at(2)
                )
            };

            point += m_offset;
        }
    }

    std::vector<Vector3f> project(const std::vector<Vector3f>& points) {
        std::vector<Vector3f> projected;

        for (const Vector3f& point : points) {
            // project point and add to projected
            projected.emplace_back(Vector3f{
                point.at(0) / point.at(2),
                point.at(1) / point.at(2),
                point.at(2) // keep z the same
            });
        }

        return projected;
    }

    static Camera& getInstance() {
        static Camera camera;
        return camera;
    }

public:

    // Projects points onto the camera screen, then sends them as 2d points to the renderer
    static void toScreen(std::vector<Vector3f> points) {
        Camera& cam = getInstance();
        
        cam.applyTransformations(points);

        std::vector<Vector3f> projected = cam.project(points);

        // convert each point to screen coordinates
        for (Vector3f& point : projected) {
            point = cam.convertScreenCoordinate(point);
        }

        // send points to renderer
        Renderer::getInstance().setPoints(projected);
    }
};

// converts servo angles into 3d points
class RobotRenderer {

public:

    static void sendConnections(const vector::Vector<6, bool>& leg_grounded) {
        // {a, b, width, color}
        std::vector<Connection> connections;
        // for the legs: 6 * 4 points
        // connections between 0, 1, 2, 3 | 4, 5, 6, 7 |
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 3; j++) {
                connections.push_back(Connection{
                    i * 4 + j,       // a
                    i * 4 + j + 1,   // b
                    3,              // width
                    leg_grounded.at(i) ? sf::Color::White : sf::Color::Red // color
                });
            }
            // connections.push_back(Connection{
            //     i * 4,       // a
            //     i * 4 + 3,   // b
            //     10,              // width
            //     sf::Color::Red // color
            // });
        }

        for (int i = 0; i < 5; i++) {
            connections.push_back(Connection{
                i * 4,
                i * 4 + 4,
                5,
                sf::Color::Blue
            });
        }
        connections.push_back(Connection{
            0,
            20,
            5,
            sf::Color::Blue
        });

        Renderer::getInstance().setConnections(connections);
    }

    static void sendAngles(
        const nlohmann::json& leg_data, const std::array<Leg, 6>& legs, const std::array<float, 18>& angles, const std::array<Vector3f, 6>& leg_positions,
        const vector::Vector<6, bool>& leg_grounded
    ) {
        std::vector<Vector3f> points;

        for (int i = 0; i < 6; i++) {
            Vector2f offset = static_cast<Vector2f>(leg_data[i].at("origin_offset").get<std::array<float, 2>>());

            float a0 = (angles[i * 3]) * M_PI / 180;
            const Vector3f& axis_lengths = legs[i].getAxisLengths();

            Vector3f p0{
                offset.at(0) + std::cos(a0) * axis_lengths.at(0),
                0,
                offset.at(1) + std::sin(a0) * axis_lengths.at(0)
            };

            float a1 = angles[i * 3 + 1]  * M_PI / 180;
            float a2 = angles[i * 3 + 2]  * M_PI / 180;

            Vector2f p1_v2f{
                std::cos(a1) * axis_lengths.at(1),
                std::sin(a1) * axis_lengths.at(1)
            };
            Vector2f p2_v2f = Vector2f{
                std::cos(a2) * axis_lengths.at(2),
                std::sin(a2) * axis_lengths.at(2) + p1_v2f.at(1)
            };

            Vector3f p1 = Vector3f{
                p0.at(0),
                p1_v2f.at(1),
                p0.at(2),
            } + p0.normalized() * p1_v2f.at(0);

            Vector3f p2 = Vector3f{
                p1.at(0),
                p2_v2f.at(1),
                p1.at(2),
            } + p0.normalized() * p2_v2f.at(0);

            points.push_back(Vector3f{offset.at(0), 0, offset.at(1)});
            points.push_back(p0);
            points.push_back(p1);
            // points.push_back(p2);
            points.push_back(leg_positions[i]); // p2
        }

        Camera::toScreen(points);
        sendConnections(leg_grounded);
    }
};

#endif
