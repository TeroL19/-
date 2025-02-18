#pragma once

struct QuadraticEquation {
    double a, b, c;
};

void read_quadratic_equation(QuadraticEquation* equation);

double calculate_discriminant(double a, double b, double c);

bool has_real_solutions(double discriminant);

void print_solutions(double x1, double x2, bool is_single_root);

void handle_linear_case(double b, double c);

void handle_no_real_solutions();

void calculate_roots(double a, double b, double discriminant);

void solve_quadratic(const QuadraticEquation& equation);