#include <iostream>
#include "Matrix.h"
#include <cassert>
#include <tuple>

using MatrixType = int; //Задаем тип для примера
constexpr MatrixType matrixdefaultValue = -1; //Задаем значение для примера

/// Главная функция программы
int main() {
    // Создаем матрицу с значением по умолчанию 1.0
    Matrix<1.0> matrix;
    assert(matrix.size() == 0); // все ячейки свободны

    // Чтение значения из свободной ячейки
    auto a = matrix[0][0];
    assert(static_cast<double>(a) == 1.0);
    assert(matrix.size() == 0);

    // Запись значения в ячейку [100][100]
    matrix[100][100] = 314;
    matrix[100][100] = 514;
    assert(static_cast<double>(matrix[100][100]) == 514);
    assert(matrix.size() == 1);

    // Вывод всех занятых ячеек
    for (const auto& c : matrix) {
        int x;
        int y;
        double v;
        std::tie(x, y, v) = c;
        std::cout << x << y << v << std::endl;
    }

    return 0;
}