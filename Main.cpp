#include "Classes.hpp"

int main() {
    setlocale(LC_ALL, "Russian");

    try {
        Teacher teacher("Халидов Искандер Анасович");

        vector<QuadraticEquation> equations = read_equations_from_file("Equations.txt");

        vector<Student> students;
        students.emplace_back("Илья", GOOD);
        students.emplace_back("Артем", GOOD);
        students.emplace_back("Иннокентий", AVERAGE);
        students.emplace_back("Дима", AVERAGE);
        students.emplace_back("Виктор", AVERAGE);
        students.emplace_back("Василий", BAD);
        students.emplace_back("Константин", BAD);

        teacher.start_test(equations, students);
        teacher.check_all_solutions();
        teacher.publish_grades();
    }

    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}