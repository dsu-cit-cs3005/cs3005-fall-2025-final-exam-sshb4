#include "RobotBase.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

//speed is prioritized over armor. uses railgun. is supposed to avoid stuff.

class Robot_Speedy : public RobotBase {
private:
    // Track known obstacles and robots
    std::vector<RadarObj> known_obstacles;
    std::vector<RadarObj> known_robots;

    // Helper: Is cell an obstacle?
    bool is_obstacle(int row, int col) const {
        return std::any_of(known_obstacles.begin(), known_obstacles.end(), [&](const RadarObj& obj) {
            return obj.m_row == row && obj.m_col == col;
        });
    }
    // Helper: Is cell a robot?
    bool is_robot(int row, int col) const {
        return std::any_of(known_robots.begin(), known_robots.end(), [&](const RadarObj& obj) {
            return obj.m_row == row && obj.m_col == col;
        });
    }
    // Helper: Pick a safe direction
    int pick_safe_direction(int current_row, int current_col) {
        for (int dir = 1; dir <= 8; ++dir) {
            int new_row = current_row + directions[dir].first;
            int new_col = current_col + directions[dir].second;
            if (new_row >= 0 && new_row < m_board_row_max && new_col >= 0 && new_col < m_board_col_max) {
                if (!is_obstacle(new_row, new_col) && !is_robot(new_row, new_col)) {
                    return dir;
                }
            }
        }
        return 1; // Default to Up if no safe direction
    }

public:
    Robot_Speedy() : RobotBase(5, 2, railgun) {
        m_name = "Robot_Speedy";
        m_character = 'S';
        std::srand(std::time(nullptr));
    }

    // Radar direction: look for threats
    virtual void get_radar_direction(int& radar_direction) override {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        radar_direction = pick_safe_direction(current_row, current_col);
    }

    // Process radar results
    virtual void process_radar_results(const std::vector<RadarObj>& radar_results) override {
        known_obstacles.clear();
        known_robots.clear();
        for (const auto& obj : radar_results) {
            if (obj.m_type == 'M' || obj.m_type == 'P' || obj.m_type == 'F') {
                known_obstacles.push_back(obj);
            } else if (obj.m_type == 'R') {
                known_robots.push_back(obj);
            }
        }
    }

    // Movement: pick safe direction and max distance
    virtual void get_move_direction(int &direction, int &distance) override {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        // Try all directions and distances to find a valid move
        for (int d = 1; d <= 8; ++d) {
            for (int dist = get_move_speed(); dist >= 1; --dist) {
                int new_row = current_row + directions[d].first * dist;
                int new_col = current_col + directions[d].second * dist;
                if (new_row >= 0 && new_row < m_board_row_max && new_col >= 0 && new_col < m_board_col_max) {
                    if (!is_obstacle(new_row, new_col) && !is_robot(new_row, new_col)) {
                        direction = d;
                        distance = dist;
                        return;
                    }
                }
            }
        }
        // If no valid move, stay in place
        direction = 1; // Up
        distance = 0;
    }

    // Shooting: only shoot if a robot is in line of sight (railgun)
    virtual bool get_shot_location(int& shot_row, int& shot_col) override {
        int current_row, current_col;
        get_current_location(current_row, current_col);
        // Scan for robots in the same row or column
        for (const auto& obj : known_robots) {
            if (obj.m_row == current_row || obj.m_col == current_col) {
                shot_row = obj.m_row;
                shot_col = obj.m_col;
                return true;
            }
        }
        return false;
    }
};

// Factory function for dynamic loading
extern "C" RobotBase* create_robot() {
    return new Robot_Speedy();
}