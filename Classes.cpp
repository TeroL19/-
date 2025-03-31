#include "Classes.hpp"

#include <fstream>
#include <sstream>

void print_solution(const Solution& solution) {
    switch (solution.num_roots) {
    case INF:
        cout << "Бесконечно много решений" << endl;
        break;
    case ZERO:
        cout << "Действительных корней нет (D < 0)" << endl;
        break;
    case ONE:
        cout << "Уравнение имеет один корень: x = " << solution.roots[0] << endl;
        break;
    case TWO:
        cout << "Уравнение имеет два корня: x1 = " << solution.roots[0]
            << ", x2 = " << solution.roots[1] << endl;
        break;
    }
}

vector<QuadraticEquation> read_equations_from_file(const string& filename) {
    vector<QuadraticEquation> equations;
    ifstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        QuadraticEquation eq;
        if (iss >> eq.a >> eq.b >> eq.c) {
            equations.push_back(eq);
        }
    }

    return equations;
}

Solution QuadraticEquationSolver::solve(const QuadraticEquation& equation) {
    double a = equation.a, b = equation.b, c = equation.c;

    if (a == 0) {
        return handle_linear_case(b, c);
    }

    double D = calculate_discriminant(a, b, c);

    if (!has_real_solutions(D)) {
        return handle_no_real_solutions();
    }

    return calculate_roots(a, b, D);
}

Solution QuadraticEquationSolver::create_solution(double x1, double x2, RootCount count) {
    Solution s;
    s.num_roots = count;
    if (count == ONE) s.roots.push_back(x1);
    if (count == TWO) {
        s.roots.push_back(x1);
        s.roots.push_back(x2);
    }
    return s;
}

Solution QuadraticEquationSolver::handle_linear_case(double b, double c) {
    if (b == 0) {
        return create_solution(0, 0, c == 0 ? INF : ZERO);
    }
    else {
        Solution s;
        s.num_roots = ONE;
        s.roots.push_back(-c / b);
        return s;
    }
}

Solution QuadraticEquationSolver::calculate_roots(double a, double b, double discriminant) {
    double sqrt_D = sqrt(discriminant);
    double x1 = (-b + sqrt_D) / (2 * a);
    double x2 = (-b - sqrt_D) / (2 * a);
    return create_solution(x1, x2, discriminant == 0 ? ONE : TWO);
}

Solution QuadraticEquationSolver::solve_quadratic(const QuadraticEquation& equation) {
    double a = equation.a, b = equation.b, c = equation.c;

    if (a == 0) {
        return handle_linear_case(b, c);
    }

    double D = calculate_discriminant(a, b, c);

    if (!has_real_solutions(D)) {
        return handle_no_real_solutions();
    }

    return calculate_roots(a, b, D);
}

Solution Student::solve_equation(const QuadraticEquation& equation) {
    switch (this->type) {
    case StudentType::GOOD:
        return QuadraticEquationSolver::solve(equation);

    case StudentType::AVERAGE: {
        Solution solution = QuadraticEquationSolver::solve(equation);
        solution_random_change(solution);
        return solution;
    }

    case StudentType::BAD:
        return Solution{ ONE, {0} };
    }
    return Solution{ ZERO, {} };
}

void Student::submit_solution(QuadraticEquation equation, Teacher& teacher) {
    Solution solution = this->solve_equation(equation);
    teacher.receive_response({ equation, solution, this->get_name() });
}

void Student::solve_equations(vector<QuadraticEquation>& equations, Teacher& teacher) {
    for (auto& equation : equations) {
        submit_solution(equation, teacher);
    }
}

void Student::solution_random_change(Solution& solution) {
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution dist(0.5);

    if (!dist(gen)) return;

    switch (solution.num_roots) {
    case ZERO:
        solution.num_roots = ONE;
        solution.roots = { 0 };
        break;
    case ONE:
        solution.roots[0] += 1;
        break;
    case TWO:
        solution.roots[0] += 1;
        solution.roots[1] -= 1;
        break;
    case INF:
        solution.num_roots = ONE;
        solution.roots = { 0 };
        break;
    }
}

void Teacher::start_test(vector<QuadraticEquation>& equations, vector<Student>& students) {
    for (auto& student : students) {
        student.solve_equations(equations, *this);
        gradebook[student.get_name()] = 0;
    }
}

void Teacher::check_all_solutions() {
    while (!this->mail_queue.empty()) {
        StudentResponse current = this->mail_queue.front();
        QuadraticEquation this_equation = current.equation;
        Solution teacher_solution = solve_equation(this_equation);

        bool is_true_answer = compare_solutions(teacher_solution, current.solution);
        if (is_true_answer)
            gradebook[current.student_name] += 1;
        this->mail_queue.pop();
    }
}

void Teacher::publish_grades() {
    cout << this->teacher_name << " публикует таблицу успеваемости:" << endl << endl;
    for (auto& entry : gradebook) {
        cout << entry.first << " " << entry.second << endl;
    }
}

bool Teacher::compare_solutions(Solution& teacher_solution, Solution& student_solution) {
    if (teacher_solution.num_roots != student_solution.num_roots)
        return false;

    for (size_t i = 0; i < teacher_solution.roots.size(); ++i) {
        if (teacher_solution.roots[i] != student_solution.roots[i])
            return false;
    }
    return true;
}

