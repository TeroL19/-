#pragma once
#include <random>
#include <queue>
#include <unordered_map>
#include <iostream>  
#include <vector>  
#include <string>    
#include <cmath>   
#include <clocale>

using namespace std;

enum StudentType { GOOD, AVERAGE, BAD };

enum RootCount { INF, ZERO, ONE, TWO };

struct Solution {
    RootCount num_roots;
    vector<double> roots;
};

struct QuadraticEquation {
    double a, b, c;
};

struct StudentResponse {
    QuadraticEquation equation;
    Solution solution;
    string student_name;
};

class QuadraticEquationSolver {
public:
    static Solution solve(const QuadraticEquation& equation);

private:
    static double calculate_discriminant(double a, double b, double c) {
        return b * b - 4 * a * c;
    }

    static bool has_real_solutions(double discriminant) {
        return discriminant >= 0;
    }

    static Solution create_solution(double x1, double x2, RootCount count);

    static Solution handle_linear_case(double b, double c);

    static Solution handle_no_real_solutions() {
        return create_solution(0, 0, ZERO);
    }

    static Solution calculate_roots(double a, double b, double discriminant);

    static Solution solve_quadratic(const QuadraticEquation& equation);
};

class Teacher;

class Student {
protected:
    string name;
    StudentType type;

public:
    explicit Student(const string& name, StudentType type) : name(name), type(type) {}

    Solution solve_equation(const QuadraticEquation& equation);

    string get_name() {
        return name;
    }

    StudentType get_type() {
        return type;
    }

    void submit_solution(QuadraticEquation equation, Teacher& teacher);

    void solve_equations(vector<QuadraticEquation>& equations, Teacher& teacher);


private:
    void solution_random_change(Solution& solution);
};

class Teacher {
private:
    queue<StudentResponse> mail_queue;
    unordered_map<string, int> gradebook;
    string teacher_name;

public:
    explicit Teacher(const string& name) : teacher_name(name) {}

    void start_test(vector<QuadraticEquation>& equations, vector<Student>& students);

    void receive_response(const StudentResponse& response) {
        mail_queue.push(response);
    }

    Solution solve_equation(QuadraticEquation& equation) {
        return QuadraticEquationSolver::solve(equation);
    }

    void check_all_solutions();

    void publish_grades();

    string get_name() {
        return teacher_name;
    }

private:
    bool compare_solutions(Solution& teacher_solution, Solution& student_solution);
};

void print_solution(const Solution& solution);

vector<QuadraticEquation> read_equations_from_file(const string& filename);