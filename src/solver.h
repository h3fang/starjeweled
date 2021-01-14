#pragma once

#include <vector>
#include <map>
#include <chrono>

using std::vector, std::map, std::chrono::high_resolution_clock;

struct Solution {
    int i1, j1;
    int i2, j2;
    char eliminations;
    bool operator<(const Solution& rhs) const {
        if (eliminations != rhs.eliminations) {
            return eliminations > rhs.eliminations;
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
    map<Solution, high_resolution_clock::time_point> solutions;

public:
    Solver();
    int &board(int i, int j);
    void calc_solutions();
    Solution get_best_solution() const;

private:
    const int PADDING = 3;
    vector<vector<int>> b;

    char check_right(int m, int n) const;
    char check_top(int m, int n) const;
};
