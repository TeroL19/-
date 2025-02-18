#include "solver.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

void read_quadratic_equation(QuadraticEquation* equation) {
    cout << "Введите квадратное уравнение в формате ax^2 + bx + c: ";
    while (true) {
        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "Ошибка! Уравнение не должно быть пустым." << endl;
            continue;
        }

        stringstream ss(input);
        char x1, caret, exp, x2, op1, op2;

        if (ss >> equation->a >> x1 >> caret >> exp >> op1 >> equation->b >> x2 >> op2 >> equation->c) {
            if (x1 != 'x' || caret != '^' || exp != '2' || x2 != 'x') {
                cout << "Ошибка! Убедитесь, что уравнение записано в формате ax^2 + bx + c. Если какие-то коэффициенты нулевые или единичные - все равно пишите их" << endl;
                continue;
            }
            if (op1 == '-') equation->b = -equation->b;
            if (op2 == '-') equation->c = -equation->c;
            break;
        }
        else {
            cout << "Ошибка! Некорректный ввод. Попробуйте снова." << endl;
        }
    }
}

double calculate_discriminant(double a, double b, double c) {
    return b * b - 4 * a * c;
}

bool has_real_solutions(double discriminant) {
    return discriminant >= 0;
}

void print_solutions(double x1, double x2, bool is_single_root) {
    if (is_single_root) {
        cout << "Уравнение имеет один корень: x = " << x1 << endl;
    }
    else {
        cout << "Уравнение имеет два корня: x1 = " << x1 << ", x2 = " << x2 << endl;
    }
}

void handle_linear_case(double b, double c) {
    if (b == 0) {
        cout << (c == 0 ? "Бесконечно много решений" : "Нет решений") << endl;
    }
    else {
        cout << "Линейное уравнение, один корень: x = " << -c / b << endl;
    }
}

void handle_no_real_solutions() {
    cout << "Действительных корней нет (D < 0)" << endl;
}

void calculate_roots(double a, double b, double discriminant) {
    double sqrt_D = sqrt(discriminant);
    double x1 = (-b + sqrt_D) / (2 * a);
    double x2 = (-b - sqrt_D) / (2 * a);
    print_solutions(x1, x2, discriminant == 0);
}

void solve_quadratic(const QuadraticEquation& equation) {
    double a = equation.a, b = equation.b, c = equation.c;

    if (a == 0) {
        handle_linear_case(b, c);
        return;
    }

    double D = calculate_discriminant(a, b, c);

    if (!has_real_solutions(D)) {
        handle_no_real_solutions();
        return;
    }

    calculate_roots(a, b, D);
}
