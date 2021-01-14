#include "solver.h"

Solver::Solver() :
    b(N+2*PADDING, vector<int>(N+2*PADDING, -1)) {
}

int &Solver::board(int i, int j) {
    return b[i + PADDING][j + PADDING];
}

void Solver::calc_solutions() {
    map<Solution, high_resolution_clock::time_point> s;
    const auto now = high_resolution_clock::now();
    for (int m = N-1+PADDING; m >= PADDING; --m ) {
        for (int n = PADDING; n < N+PADDING; ++n) {
            // swap with the right cell
            if (char e = check_right(m, n); e > 0) {
                s[{m - PADDING, n - PADDING, m - PADDING, n + 1 - PADDING, e}] = now;
            }
            // swap with the top cell
            if (char e = check_top(m, n); e > 0) {
                s[{m - PADDING, n - PADDING, m - 1 - PADDING, n - PADDING, e}] = now;
            }
        }
    }

    for (auto& [k, v] : s) {
        if (solutions.find(k) != solutions.end()) {
            v = solutions[k];
        }
    }
    solutions = s;
}

Solution Solver::get_best_solution() const {
    if (solutions.empty()) {
        return {-1, -1, -1, -1, -1};
    }
    auto s = (*solutions.begin()).first;
    const auto now = high_resolution_clock::now();
    using namespace std::chrono_literals;
    for (const auto &[k, v] : solutions) {
        if (now - v >= 1.0s) {
            s = k;
            break;
        }
    }
    return s;
}

char Solver::check_right(int m, int n) const {
    const int c = b[m][n], cr = b[m][n+1];
    if (cr == -1) {
        return 0;
    }

    char e1 = 1, e2 = 1;
    if (c == b[m-1][n+1] && c == b[m-2][n+1]) {
        e1 += 2;
    }
    if (c == b[m+1][n+1] && c == b[m+2][n+1]) {
        e1 += 2;
    }
    if (c == b[m-1][n+1] && c == b[m+1][n+1]) {
        if (e1 == 1) {
            e1 += 2;
        }
        else if (e1 == 3) {
            e1 += 1;
        }
    }
    if (c == b[m][n+2] && c == b[m][n+3]) {
        e1 += 2;
    }

    if (cr == b[m-1][n] && cr == b[m-2][n]) {
        e2 += 2;
    }
    if (cr == b[m+1][n] && cr == b[m+2][n]) {
        e2 += 2;
    }
    if (cr == b[m-1][n] && cr == b[m+1][n]) {
        if (e2 == 1) {
            e2 += 2;
        }
        else if (e2 == 3) {
            e2 += 1;
        }
    }
    if (cr == b[m][n-1] && cr == b[m][n-2]) {
        e2 += 2;
    }
    return ((e1 >= 3) ? e1 : 0) + ((e2 >= 3) ? e2 : 0);
}

char Solver::check_top(int m, int n) const {
    const int c = b[m][n], ct = b[m-1][n];
    if (ct == -1) {
        return 0;
    }

    char e1 = 1, e2 = 1;
    if (c == b[m-1][n-1] && c == b[m-1][n-2]) {
        e1 += 2;
    }
    if (c == b[m-1][n+1] && c == b[m-1][n+2]) {
        e1 += 2;
    }
    if (c == b[m-1][n+1] && c == b[m-1][n-1]) {
        if (e1 == 1) {
            e1 += 2;
        }
        else if (e1 == 3) {
            e1 += 1;
        }
    }
    if (c == b[m-2][n] && c == b[m-3][n]) {
        e1 += 2;
    }

    if (ct == b[m][n-1] && ct == b[m][n-2]) {
        e2 += 2;
    }
    if (ct == b[m][n+1] && ct == b[m][n+2]) {
        e2 += 2;
    }
    if (ct == b[m][n-1] && ct == b[m][n+1]) {
        if (e2 == 1) {
            e2 += 2;
        }
        else if (e2 == 3) {
            e2 += 1;
        }
    }
    if (ct == b[m+1][n] && ct == b[m+2][n]) {
        e2 += 2;
    }
    return ((e1 >= 3) ? e1 : 0) + ((e2 >= 3) ? e2 : 0);
}
