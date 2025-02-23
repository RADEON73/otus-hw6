#pragma once
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <iterator>
#include <iosfwd>
#include <utility>

/// @brief Шаблонный класс для реализации бесконечной разреженной матрицы.
/// @tparam DefaultValue Значение по умолчанию для свободных ячеек матрицы.
template <auto DefaultValue>
class Matrix
{
	using Index = std::pair<int, int>; ///< Тип для хранения индексов ячейки (x, y).
	using T = decltype(DefaultValue); ///< Тип значения по умолчанию.

	/// @brief Хэш-функция для std::pair<int, int>.
	struct pair_hash
	{
		/// @brief Оператор вызова для вычисления хэша.
		/// @tparam T1 Тип первого элемента пары.
		/// @tparam T2 Тип второго элемента пары.
		/// @param p Пара, для которой вычисляется хэш.
		/// @return Хэш-значение для пары.
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2>& p) const {
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ (hash2 << 1); ///< Комбинируем хэши.
		}
	};

public:
	/// @brief Прокси-класс для доступа к элементам матрицы.
	class Proxy
	{
	public:
		/// @brief Конструктор прокси-объекта.
		/// @param matrix Ссылка на матрицу.
		/// @param x Координата x (строка).
		Proxy(Matrix& matrix, int x) : matrix_(matrix)
		{
			index.first = x;
		}

		/// @brief Оператор присваивания значения ячейке.
		/// @param value Значение, которое нужно присвоить ячейке.
		/// @return Ссылка на текущий прокси-объект.
		Proxy& operator=(T value) {
			// Если значение равно значению по умолчанию, удаляем ячейку, иначе сохраняем значение
			value == DefaultValue ? matrix_.erase(index) : matrix_.set(index, value);
			return *this;
		}

		/// @brief Оператор приведения к типу T (для чтения значения ячейки).
		/// @return Значение ячейки.
		explicit operator T() const {
			return matrix_.get(index);
		}

		/// @brief Оператор [] для доступа к следующему уровню (координата y).
		/// @param y Координата y (столбец).
		/// @return Ссылка на текущий прокси-объект.
		Proxy& operator[](int y) {
			index.second = y;
			return *this;
		}

		/// @brief Перегрузка оператора вывода для прокси-объекта.
		/// @param os Поток вывода.
		/// @param proxy Прокси-объект.
		/// @return Поток вывода.
		friend std::ostream& operator<<(std::ostream& os, const Proxy& proxy) {
			os << static_cast<decltype(DefaultValue)>(proxy);
			return os;
		}

	private:
		Matrix& matrix_; ///< Ссылка на матрицу.
		Index index{ 0, 0 }; ///< Индексы ячейки (x, y).
	};

	/// @brief Итератор для прохода по занятым ячейкам матрицы.
	class Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag; ///< Категория итератора.
		using value_type = std::tuple<int, int, T>; ///< Тип значения, возвращаемого итератором.
		using difference_type = std::ptrdiff_t; ///< Тип для хранения разницы между итераторами.
		using pointer = value_type*; ///< Указатель на значение.
		using reference = value_type&; ///< Ссылка на значение.

		/// @brief Конструктор итератора.
		/// @param it Итератор для внутреннего контейнера.
		explicit Iterator(typename std::unordered_map<std::pair<int, int>, T, pair_hash>::iterator it) : it_(it) {}

		/// @brief Оператор разыменования.
		/// @return Кортеж (x, y, value) для текущей ячейки.
		value_type operator*() const {
			return std::make_tuple(it_->first.first, it_->first.second, it_->second);
		}

		/// @brief Оператор инкремента.
		/// @return Ссылка на текущий итератор.
		Iterator& operator++() {
			++it_;
			return *this;
		}

		/// @brief Оператор сравнения итераторов.
		/// @param other Другой итератор.
		/// @return true, если итераторы не равны, иначе false.
		bool operator!=(const Iterator& other) const {
			return it_ != other.it_;
		}

	private:
		typename std::unordered_map<std::pair<int, int>, T, pair_hash>::iterator it_; ///< Внутренний итератор.
	};

	/// @brief Оператор [] для доступа к строке матрицы.
	/// @param x Координата x (строка).
	/// @return Прокси-объект для доступа к элементам строки.
	Proxy operator[](int x) {
		return Proxy(*this, x);
	}

	/// @brief Получить значение ячейки.
	/// @param index Индексы ячейки (x, y).
	/// @return Значение ячейки или значение по умолчанию, если ячейка свободна.
	T get(const Index& index) const {
		auto it = data_.find(index);
		return it != data_.end() ? it->second : DefaultValue;
	}

	/// @brief Установить значение ячейки.
	/// @param index Индексы ячейки (x, y).
	/// @param value Значение, которое нужно установить.
	void set(const Index& index, T value) {
		data_[index] = value;
	}

	/// @brief Удалить ячейку.
	/// @param index Индексы ячейки (x, y).
	void erase(const Index& index) {
		data_.erase(index);
	}

	/// @brief Получить количество занятых ячеек.
	/// @return Количество занятых ячеек.
	size_t size() const {
		return data_.size();
	}

	/// @brief Получить итератор на начало занятых ячеек.
	/// @return Итератор на первую занятую ячейку.
	Iterator begin() {
		return Iterator(data_.begin());
	}

	/// @brief Получить итератор на конец занятых ячеек.
	/// @return Итератор на конец контейнера.
	Iterator end() {
		return Iterator(data_.end());
	}

	/// @brief Получить константное значение ячейки.
	/// @param x Координата x (строка).
	/// @param y Координата y (столбец).
	/// @return Значение ячейки или значение по умолчанию, если ячейка свободна.
	T at(int x, int y) const {
		auto it = data_.find({ x, y });
		return it != data_.end() ? it->second : DefaultValue;
	}

private:
	std::unordered_map<Index, T, pair_hash> data_; ///< Контейнер для хранения занятых ячеек.
};