#include <iostream>
#include "Matrix.h"
#include <cassert>

/// Главная функция программы
int main() {
	// Создаем матрицу с значением по умолчанию 0.0
	Matrix<0> matrix;

	// Заполняем главную диагональ матрицы (от [0,0] до [9,9]) значениями от 0 до 9
	// а второстепенную диагональ (от [0,9] до [9,0]) значениями от 9 до 0
	for (int i = 0; i < 10; ++i) {
		matrix[i][i] = i;
		matrix[i][9 - i] = 9 - i;
	}

	//Выводим фрагмент матрицы от [1,1] до [8,8]
	for (int i = 1; i <= 8; ++i) {
		for (int j = 1; j <= 8; ++j) {
			std::cout << matrix.at(i, j) << " ";
		}
		std::cout << std::endl;
	}

	// Выводим количество занятых ячеек
	std::cout << "Size: " << matrix.size() << std::endl;

	// Выводим все занятые ячейки вместе со своими позициями
	std::cout << "Print:" << std::endl;
	for (auto const& [x, y, value] : matrix) {
		std::cout << '[' << x << "][" << y << "] = " << value << std::endl;
	}

	//Реализованная каноническая форма =
	((matrix[100][100] = 314) = 0) = 217;
	assert(matrix.at(100, 100) == 217);

	return 0;
}