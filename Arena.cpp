#include "RobotBase.h"
#include "RadarObj.h"
#include <iostream>
#include <vector>
#include <string>
// #include <filesystem>
#include <dirent.h>
#include <dlfcn.h>
#include <random>
#include <map>


constexpr int BOARD_ROWS = 10;
constexpr int BOARD_COLS = 10;
constexpr int NUM_FLAMETHROWERS = 3;
constexpr int NUM_PITS = 2;
constexpr int NUM_MOUNDS = 3;

struct RobotInfo {
    RobotBase* robot;
    void* handle;
    bool alive;
};

class Arena {
    void update_board() {
        // Clear board except obstacles
        for (int r = 0; r < BOARD_ROWS; ++r)
            for (int c = 0; c < BOARD_COLS; ++c)
                if (board[r][c] != 'F' && board[r][c] != 'P' && board[r][c] != 'M')
                    board[r][c] = '.';
        // Place robots
        for (const auto& info : robots) {
            if (info.alive) {
                int row, col;
                info.robot->get_current_location(row, col);
                // Use each robot's unique avatar symbol
                board[row][col] = info.robot->m_character;
            }
        }
    }

    void print_stats() {
        for (const auto& info : robots) {
            if (info.alive) {
                std::cout << info.robot->print_stats() << std::endl;
            }
        }
    }
private:
    char board[BOARD_ROWS][BOARD_COLS];
    std::vector<RobotInfo> robots;
    std::map<std::string, char> robotChars;

    void place_obstacles(char type, int count) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis_row(0, BOARD_ROWS-1);
        std::uniform_int_distribution<> dis_col(0, BOARD_COLS-1);
        int placed = 0;
        while (placed < count) {
            int r = dis_row(gen);
            int c = dis_col(gen);
            if (board[r][c] == '.') {
                board[r][c] = type;
                placed++;
            }
        }
    }

    void print_board() {
        std::cout << "\n    ";
        for (int c = 0; c < BOARD_COLS; ++c) {
            if (c < 10) std::cout << ' ' << c << ' ';
            else std::cout << c << ' ';
        }
        std::cout << '\n';
        for (int r = 0; r < BOARD_ROWS; ++r) {
            std::cout << (r < 10 ? "  " : " ") << r << " ";
            for (int c = 0; c < BOARD_COLS; ++c) std::cout << board[r][c] << "  ";
            std::cout << '\n';
        }
    }

    void load_robots() {
        // Explicitly load known robots if their .so files exist
        const char* robotLibs[] = {"libRobot_Speedy.so", "libRatboy.so", "libFlame_e_o.so"};
        for (const char* fname : robotLibs) {
            FILE* f = fopen(fname, "r");
            if (!f) continue;
            fclose(f);
            void* handle = dlopen(fname, RTLD_LAZY);
            if (!handle) continue;
            RobotFactory create_robot = (RobotFactory)dlsym(handle, "create_robot");
            if (!create_robot) { dlclose(handle); continue; }
            RobotBase* robot = create_robot();
            robot->set_boundaries(BOARD_ROWS, BOARD_COLS);
            // Place robot randomly
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis_row(0, BOARD_ROWS-1);
            std::uniform_int_distribution<> dis_col(0, BOARD_COLS-1);
            int r, c;
            do {
                r = dis_row(gen);
                c = dis_col(gen);
            } while (board[r][c] != '.');
            robot->move_to(r, c);
            robots.push_back({robot, handle, true});
            robotChars[robot->m_name] = robot->m_character;
            std::cout << "Loaded robot: " << robot->m_name << " at (" << r << "," << c << ")\n";
        }
    }

    std::vector<RadarObj> get_radar_results(int row, int col, int direction) {
        std::vector<RadarObj> results;
        // Scan up to 5 cells in the given direction
        for (int dist = 1; dist <= 5; ++dist) {
            int nr = row + directions[direction].first * dist;
            int nc = col + directions[direction].second * dist;
            if (nr < 0 || nr >= BOARD_ROWS || nc < 0 || nc >= BOARD_COLS) break;
            char cell = board[nr][nc];
            if (cell != '.') results.push_back(RadarObj(cell, nr, nc));
        }
        return results;
    }

    void handle_obstacle(RobotInfo& info) {
        int row, col;
        info.robot->get_current_location(row, col);
        char cell = board[row][col];
        if (cell == 'F') {
            info.robot->take_damage(10); // Flamethrower damage
            std::cout << info.robot->m_name << " stepped on a flamethrower!\n";
        } else if (cell == 'P') {
            info.robot->disable_movement();
            std::cout << info.robot->m_name << " fell into a pit!\n";
        }
        // Mounds are handled in movement logic
    }

public:
    Arena() {
        for (int r = 0; r < BOARD_ROWS; ++r)
            for (int c = 0; c < BOARD_COLS; ++c)
                board[r][c] = '.';
        place_obstacles('F', NUM_FLAMETHROWERS);
        place_obstacles('P', NUM_PITS);
        place_obstacles('M', NUM_MOUNDS);
        load_robots();
        update_board();
    }

    void run() {
        int round = 0;
        bool game_over = false;
        while (!game_over) {
            std::cout << "\n=========== starting round " << round << " ===========\n";
            update_board();
            print_board();
            print_stats();
            int alive_count = 0;
            for (auto& info : robots) {
                if (!info.alive) continue;
                int health = info.robot->get_health();
                if (health <= 0) {
                    info.alive = false;
                    continue;
                }
                int row, col;
                info.robot->get_current_location(row, col);
                int radar_dir;
                info.robot->get_radar_direction(radar_dir);
                auto radar_results = get_radar_results(row, col, radar_dir);
                info.robot->process_radar_results(radar_results);
                int move_dir, move_dist;
                info.robot->get_move_direction(move_dir, move_dist);
                // Movement: check for mounds
                int new_row = row + directions[move_dir].first * move_dist;
                int new_col = col + directions[move_dir].second * move_dist;
                if (new_row < 0 || new_row >= BOARD_ROWS || new_col < 0 || new_col >= BOARD_COLS || board[new_row][new_col] == 'M') {
                    std::cout << info.robot->m_name << " cannot move to (" << new_row << "," << new_col << ")\n";
                } else {
                    info.robot->move_to(new_row, new_col);
                }
                handle_obstacle(info);
                int shot_row, shot_col;
                if (info.robot->get_shot_location(shot_row, shot_col)) {
                    // Find robot at shot location
                    for (auto& target : robots) {
                        if (!target.alive) continue;
                        int tr, tc;
                        target.robot->get_current_location(tr, tc);
                        if (tr == shot_row && tc == shot_col) {
                            target.robot->take_damage(12); // Example damage
                            std::cout << info.robot->m_name << " shoots " << target.robot->m_name << "!\n";
                        }
                    }
                }
                alive_count++;
            }
            // Check for winner
            if (alive_count <= 1) {
                for (const auto& info : robots) {
                    if (info.alive) {
                        std::cout << "\nWinner: " << info.robot->m_name << "\n";
                    }
                }
                game_over = true;
            }
            round++;
        }
    }

    ~Arena() {
        for (auto& info : robots) {
            if (info.robot) delete info.robot;
            if (info.handle) dlclose(info.handle);
        }
    }
};

int main() {
    Arena arena;
    arena.run();
    return 0;
}
