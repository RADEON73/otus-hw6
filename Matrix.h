#pragma once
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <iterator>

/// @brief ��������� ����� ��� ���������� ����������� ����������� �������.
/// @tparam DefaultValue �������� �� ��������� ��� ��������� ����� �������.
template <auto DefaultValue>
class Matrix
{
    using T = decltype(DefaultValue); ///< ��� �������� �� ���������.
    using Index = std::pair<int, int>; ///< ��� ��� �������� �������� ������ (x, y).

    /// @brief ���-������� ��� std::pair<int, int>.
    struct pair_hash
    {
        /// @brief �������� ������ ��� ���������� ����.
        /// @tparam T1 ��� ������� �������� ����.
        /// @tparam T2 ��� ������� �������� ����.
        /// @param p ����, ��� ������� ����������� ���.
        /// @return ���-�������� ��� ����.
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);
            return hash1 ^ (hash2 << 1); ///< ����������� ����.
        }
    };

public:
    /// @brief ������-����� ��� ������� � ��������� �������.
    class Proxy
    {
    public:
        /// @brief ����������� ������-�������.
        /// @param matrix ������ �� �������.
        /// @param x ���������� x (������).
        Proxy(Matrix& matrix, int x) : matrix_(matrix)
        {
            index.first = x;
        }

        /// @brief �������� ������������ �������� ������.
        /// @param value ��������, ������� ����� ��������� ������.
        /// @return ������ �� ������� ������-������.
        Proxy& operator=(T value) {
            // ���� �������� ����� �������� �� ���������, ������� ������, ����� ��������� ��������
            value == DefaultValue ? matrix_.erase(index) : matrix_.set(index, value);
            return *this;
        }

        /// @brief �������� ���������� � ���� T (��� ������ �������� ������).
        /// @return �������� ������.
        explicit operator T() const {
            return matrix_.get(index);
        }

        /// @brief �������� [] ��� ������� � ���������� ������ (���������� y).
        /// @param y ���������� y (�������).
        /// @return ������ �� ������� ������-������.
        Proxy& operator[](int y) {
            index.second = y;
            return *this;
        }

    private:
        Matrix& matrix_; ///< ������ �� �������.
        Index index{ 0, 0 }; ///< ������� ������ (x, y).
    };

    /// @brief �������� ��� ������� �� ������� ������� �������.
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag; ///< ��������� ���������.
        using value_type = std::tuple<int, int, T>; ///< ��� ��������, ������������� ����������.
        using difference_type = std::ptrdiff_t; ///< ��� ��� �������� ������� ����� �����������.
        using pointer = value_type*; ///< ��������� �� ��������.
        using reference = value_type&; ///< ������ �� ��������.

        /// @brief ����������� ���������.
        /// @param it �������� ��� ����������� ����������.
        explicit Iterator(typename std::unordered_map<std::pair<int, int>, T, pair_hash>::iterator it) : it_(it) {}

        /// @brief �������� �������������.
        /// @return ������ (x, y, value) ��� ������� ������.
        value_type operator*() const {
            return std::make_tuple(it_->first.first, it_->first.second, it_->second);
        }

        /// @brief �������� ����������.
        /// @return ������ �� ������� ��������.
        Iterator& operator++() {
            ++it_;
            return *this;
        }

        /// @brief �������� ��������� ����������.
        /// @param other ������ ��������.
        /// @return true, ���� ��������� �� �����, ����� false.
        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

    private:
        typename std::unordered_map<std::pair<int, int>, T, pair_hash>::iterator it_; ///< ���������� ��������.
    };

    /// @brief �������� [] ��� ������� � ������ �������.
    /// @param x ���������� x (������).
    /// @return ������-������ ��� ������� � ��������� ������.
    Proxy operator[](int x) {
        return Proxy(*this, x);
    }

    /// @brief �������� �������� ������.
    /// @param index ������� ������ (x, y).
    /// @return �������� ������ ��� �������� �� ���������, ���� ������ ��������.
    T get(const Index& index) const {
        auto it = data_.find(index);
        return it != data_.end() ? it->second : DefaultValue;
    }

    /// @brief ���������� �������� ������.
    /// @param index ������� ������ (x, y).
    /// @param value ��������, ������� ����� ����������.
    void set(const Index& index, T value) {
        data_[index] = value;
    }

    /// @brief ������� ������.
    /// @param index ������� ������ (x, y).
    void erase(const Index& index) {
        data_.erase(index);
    }

    /// @brief �������� ���������� ������� �����.
    /// @return ���������� ������� �����.
    size_t size() const {
        return data_.size();
    }

    /// @brief �������� �������� �� ������ ������� �����.
    /// @return �������� �� ������ ������� ������.
    Iterator begin() {
        return Iterator(data_.begin());
    }

    /// @brief �������� �������� �� ����� ������� �����.
    /// @return �������� �� ����� ����������.
    Iterator end() {
        return Iterator(data_.end());
    }

private:
    std::unordered_map<Index, T, pair_hash> data_; ///< ��������� ��� �������� ������� �����.
};