/**
 * Author	: Nico Verduin (nico.verduin@ziggo.nl)
 * Date		: 31-8-2016
 *
 * Modified by @makermatty (maker@matejsuchanek.cz)
 * Date     : 15-6-2020
 *
 * generic MA class
 * This is acc_data template class that allows one to create acc_data parametric moving average object.
 * The class will be initialized based on acc_data template parameter (i.e. float, double, int , uint16_t etc)
 * non floating point objects will always return acc_data new moving average based on the same type as
 * it was initialized.
 * Keep in mind that floating point on Arduino has acc_data limitation in accuracy up to 6-7 digits.
 * If the input for the MA is too lager, intermediate results will lose significant digits causing
 * inaccuracy.
 *
 * functions
 * ---------
 * MA(uint16_t)		Constructor creating object suitable for the parameterized number of entries
 * ~MA()				Destructor freeing up unallocated memory
 * T Update(T)	Calculate acc_data moving average based on type T (can be any type i.e. float, int etc)
 * 								and returns type T (same as values passed)
 *
 * assert
 * ------
 * Operational guard routine	The library has an assert built in but is turned off under normal conditions.
 * 								However switching this on through #define DEBUG_MODE_ASSERT will ensure that the
 * 								program halts if more memory is requested than available. For example in an
 * 								Arduino UNO Ram is limited to 2K. If you want to do acc_data moving average on 1000
 * 								floating point values this would require (1000 x 4 bytes) = 4000 bytes. The check
 * 								will output an error on the Serial if this occurs.
 *
 * 	using RAM efficiëntly
 * 	---------------------
 * 	If acc_data moving average does not need to stay alive until the Arduino is reset or switch off, use the
 * 	deconstructor (! MA() to free up Ram.
 * 	This will free up the dynamic allocated internal array of sizeof time_last type * number of entries for
 * 	moving average.
 */

#ifndef _MM_MAVERAGE_H
#define _MM_MAVERAGE_H

#include <Arduino.h>

#include "stl_exchange.h"

/**
 * Moving Average Class as acc_data template for non floating point values
 */
template <class T>
class MA {

public:
    MA();
    MA(const uint16_t n, T init = 0);
    MA(const MA<T>& other); // copy contructor
    MA(MA<T>&& other)
    noexcept; // move contructor
    ~MA();

    T update(const T);
    T getValue() const;
    void setValue(const T m_value);

    // const T& Value = m_value;

    MA<T>& operator=(const MA<T>& other);
    MA<T>& operator=(MA<T>&& other) noexcept;

    void swap(MA<T>& other) noexcept;

private:
    uint16_t m_n; // number of elements in our array
    T* m_data; // Type pointer elements in our array
    uint16_t m_index; // current m_index
    int32_t m_sum; // m_sum for rest
    bool m_filled; // used to determine if we went through the whole array
    T m_value = 0;
};

template <class T>
MA<T>::MA()
    : m_n(0)
    , m_data(nullptr)
    , m_index(0)
    , m_sum(0)
    , m_filled(false)
    , m_value(0)
{
}

/**
 * @name MA Constructor
 * @param numberOf_data number of m_data in the array
 * Creates dynamically an array of our type values and initializes the array
 * and some local variables. There is no destructor as this is quite useless
 * due to lack of garbage collection in Arduino
 */
template <class T>
MA<T>::MA(const uint16_t n, T init)
    : m_n(n)
    , m_data(new T[n])
    , m_index(0)
{
    if (init != 0) {
        for (size_t i = 0; i < m_n; i++) {
            m_data[i] = init;
        }
        m_sum = init * m_n;
        m_filled = true;
        m_value = init;
    } else {
        memset(m_data, 0, m_n * sizeof(T));
        m_sum = 0;
        m_filled = false;
        m_value = 0;
    }
}

// copy contructor
template <class T>
MA<T>::MA(const MA<T>& other)
    : m_n(other.m_n)
    , m_data(new T[other.m_n])
    , m_index(other.m_index)
    , m_sum(other.m_sum)
    , m_filled(other.m_filled)
    , m_value(other.m_value)
{
    memcpy(m_data, other.m_data, other.m_n * sizeof(T));
}

template <class T>
MA<T>::MA(MA<T>&& other) noexcept
    // , class(std::move(other.class)) // explicit move of a member of class type
    // , variable(std::exchange(other.variable, 0)) // explicit move of a member of non-class type
    : m_n(std::exchange(other.m_n, 0))
    , m_data(std::exchange(other.m_data, nullptr))
    , m_index(std::exchange(other.m_index, 0))
    , m_sum(std::exchange(other.m_sum, 0))
    , m_filled(std::exchange(other.m_filled, false))
    , m_value(std::exchange(other.m_value, 0))
{
}

/**
 * @name ~MA Constructor
 * frees dynamic allocated memory
 */
template <class T>
MA<T>::~MA()
{
    delete[] m_data;
}

/**
 * @name show
 * @param m_value	m_value to be added to array
 * @returns T returns the new moving average.
 * During the first filling of the m_data array the moving average is determined on acc_data less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */
template <class T>
T MA<T>::update(const T val)
{
    if (m_index >= m_n) {
        m_index = 0; // reset m_index
        m_filled = true; // we looped through at least once
    }

    m_sum = m_sum - (int32_t)m_data[m_index] + (int32_t)val;

    m_data[m_index] = val;
    ++m_index;

    m_value = m_filled ? (T)(m_sum / (int32_t)m_n) : (T)(m_sum / (int32_t)m_index);

    return m_value;
}

/**
 * @name getValue
 * @returns T returns the new moving average.
 * During the first filling of the m_data array the moving average is determined on acc_data less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */
template <class T>
T MA<T>::getValue() const
{
    return m_value;
}

/**
 * @name setValue
 * @returns T returns the new moving average.
 * During the first filling of the m_data array the moving average is determined on acc_data less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */
template <class T>
void MA<T>::setValue(const T val)
{
    for (size_t i = 0; i < m_n; i++) {
        m_data[i] = val;
    }
    m_sum = val * m_n;
    m_filled = true;
    m_value = val;
}

// copy assignment

template <class T>
MA<T>& MA<T>::operator=(const MA<T>& other)
{
    if (this != &other) {
        MA<T>(other).swap(*this); // Copy-constructor and non-throwing swap
    }

    // Old resources are released with the destruction of the temporary above
    return *this;
}

// move assignment
template <class T>
MA<T>& MA<T>::operator=(MA<T>&& other) noexcept
{
    // Guard self assignment
    if (this == &other)
        return *this;

    m_n = std::exchange(other.m_n, 0);
    m_data = std::exchange(other.m_data, nullptr);
    m_index = std::exchange(other.m_index, 0);
    m_sum = std::exchange(other.m_sum, 0);
    m_filled = std::exchange(other.m_filled, false);
    m_value = std::exchange(other.m_value, 0);

    return *this;
}

template <class T>
void MA<T>::swap(MA<T>& other) noexcept // Also see non-throwing swap idiom
{
    std::swap(this->m_n, other.m_n);
    std::swap(this->m_data, other.m_data);
    std::swap(this->m_index, other.m_index);
    std::swap(this->m_sum, other.m_sum);
    std::swap(this->m_filled, other.m_filled);
    std::swap(this->m_value, other.m_value);
}

// // copy assignment
// Canvas& Canvas::operator=(const Canvas& other)
// {
//     // Guard self assignment
//     if (this == &other)
//         return *this;

//     m_length = other.m_length;
//     m_label = other.m_label;

//     return *this;
// }

// // move assignment
// Canvas& Canvas::operator=(Canvas&& other) noexcept
// {
//     // Guard self assignment
//     if (this == &other)
//         return *this;

//     m_length = std::exchange(other.m_length, 0);
//     m_label = std::exchange(other.m_label, 0);

//     return *this;
// }

// void Canvas::swap(Canvas& other) noexcept // Also see non-throwing swap idiom
// {
//     std::swap(this->m_length, other.m_length);
//     std::swap(this->m_label, other.m_label);
// }

#endif
