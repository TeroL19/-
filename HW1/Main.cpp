#include "solver.hpp"

#include <locale.h>

int main() {
    setlocale(LC_ALL, "Russian");
    QuadraticEquation eq;
    read_quadratic_equation(&eq);
    solve_quadratic(eq);
    return 0;
}
