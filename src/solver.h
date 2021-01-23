#pragma once

#include <vector>
#include <map>
#include <set>
#include <chrono>

using std::vector, std::map, std::set, std::pair, std::chrono::high_resolution_clock;

struct Move {
    int i1 = 0, j1 = 0;
    int i2 = 0, j2 = 0;

    bool operator>(const Move& rhs) const {
        if (i1 != rhs.i1) {
            return i1 > rhs.i1;
        }
        else if (i2 != rhs.i2) {
            return i2 > rhs.i2;
        }
        else if (j1 != rhs.j1) {
            return j1 > rhs.j1;
        }
        else {
            return j2 > rhs.j2;
        }
    }
};

struct Solution {
    Move move;
    int eliminated = 0;
    high_resolution_clock::time_point timestamp;

    bool operator>(const Solution& rhs) const {
        if (eliminated != rhs.eliminated) {
            return eliminated > rhs.eliminated;
        }
        else {
            return move > rhs.move;
        }
    }
};

class Solver
{
public:
    const int N = 8;
    map<Move, Solution, std::greater<Move>> solutions;

public:
    Solver(int age_threshold_ms);
    int &cell(int i, int j);
    void calc_solutions();
    Move get_best_move() const;

private:
    const char PADDING = 3;
    const std::chrono::milliseconds solution_age_threshold;
    vector<vector<int>> board;

    set<pair<int, int>> check_right(vector<vector<int>> &b, int m, int n) const;
    set<pair<int, int>> check_top(vector<vector<int>> &b, int m, int n) const;
    int simulate(const Move& m);
};
