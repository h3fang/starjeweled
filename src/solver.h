#pragma once

#include <vector>
#include <set>
#include <chrono>

using std::vector, std::set, std::pair, std::chrono::high_resolution_clock;

struct Solution {
    int i1 = 0, j1 = 0;
    int i2 = 0, j2 = 0;
    int eliminated = 0;
    high_resolution_clock::time_point timestamp = high_resolution_clock::now();

    bool operator==(const Solution& rhs) const {
        return i1 == rhs.i1 && j1 == rhs.j1 && i2 == rhs.i2 && j2 == rhs.j2;
    }

    bool operator>(const Solution& rhs) const {
        if (eliminated != rhs.eliminated) {
            return eliminated > rhs.eliminated;
        }
        else if (i1 != rhs.i1) {
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

class Solver
{
public:
    const int N = 8;
    vector<Solution> solutions;

public:
    Solver(int age_threshold_ms);
    int &cell(int i, int j);
    void calc_solutions();
    Solution get_best_solution();

private:
    const char PADDING = 3;
    const std::chrono::milliseconds solution_age_threshold;
    vector<vector<int>> board;

    set<pair<int, int>> check_right(vector<vector<int>> &b, int m, int n) const;
    set<pair<int, int>> check_top(vector<vector<int>> &b, int m, int n) const;
    int simulate(const Solution& m);
};
