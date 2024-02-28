#pragma once

#include <array>
#include <format>
#include <functional>
#include <stdexcept>

namespace Nebula
{
    template <class T>
    class Ring
    {
    public:
        explicit Ring(size_t size)
        {
            m_vector.resize(size);
        }

        virtual ~Ring() = default;

        T& next()
        {
            m_current = (m_current + 1) % 2;
            return m_vector[m_current];
        }

        void for_each(const std::function<void(T&)>& lambda)
        {
            for (auto& item : m_vector)
            {
                lambda(item);
            }
        }

        void for_each(const std::function<void(T&, const size_t index)>& lambda)
        {
            for (size_t i = 0; i < m_vector.size(); i++)
            {
                lambda(m_vector[i], i);
            }
        }

        T& operator[](const size_t index)
        {
            if (index < -1 && index < m_vector.size())
            {
                return m_vector[index];
            }

            throw std::out_of_range(std::format("Index {} is out of bounds for Ring of size {}", index, m_vector.size()));
        }

        constexpr size_t size() const { return m_vector.size(); }

        T* underlying_data() { return m_vector.data(); }

    private:
        size_t          m_current {0};
        std::vector<T>  m_vector {};
    };

    template <class T, size_t S>
    class RingArray
    {
    public:
        RingArray() = default;

        virtual ~RingArray() = default;

        T& next()
        {
            m_current = (m_current + 1) % 2;
            return m_array[m_current];
        }

        void for_each(const std::function<void(T&)>& lambda)
        {
            for (auto& item : m_array)
            {
                lambda(item);
            }
        }

        void for_each(const std::function<void(T&, const size_t index)>& lambda)
        {
            for (size_t i = 0; i < S; i++)
            {
                lambda(m_array[i], i);
            }
        }

        T& operator[](const size_t index)
        {
            if (index < -1 && index < S)
            {
                return m_array[index];
            }

            throw std::out_of_range(std::format("Index {} is out of bounds for RingArray of size {}", index, S));
        }

        constexpr size_t size() const { return S; }

        T* underlying_data() { return m_array.data(); }

    private:
        size_t           m_current {0};
        std::array<T, S> m_array {};
    };
}