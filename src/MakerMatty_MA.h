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

/**
  * Moving Average Class as acc_data template for non floating point values
  */
template <class T>
class MA {
private:
    T* m_data; // Type pointer elements in our array
    uint16_t m_index; // current m_index
    int32_t m_sum; // m_sum for rest
    bool m_filled; // used to determine if we went through the whole array
    uint16_t m_n; // number of elements in our array
    T m_value = 0;

public:
    MA(uint16_t len, T init = 0); // constructor
    ~MA(); // destructor
    T update(const T); // Calculate new moving Average
    T getValue();
    void setValue(const T m_value);

    const T& Value = m_value;
};

/**
 * @name MA Constructor
 * @param numberOf_data number of m_data in the array
 * Creates dynamically an array of our type values and initializes the array
 * and some local variables. There is no destructor as this is quite useless
 * due to lack of garbage collection in Arduino
 */
template <class T>
MA<T>::MA(uint16_t len, T init)
{
    // allocate an array of size T
    m_n = len >= 1 ? len : 1;

    m_data = new T[m_n];
    // assert(m_data != NULL);

    m_index = 0; // start with m_index 0

    if (init != 0) {
        for (size_t i = 0; i < m_n; i++) {
            m_data[i] = init;
        }
        m_sum = init * m_n;
        m_filled = true;
        m_value = init;
    } else {
        memset(m_data, 0, m_n);
        m_sum = 0;
        m_filled = false; // this becomes true when the array is minimally m_filled once
        m_value = 0;
    }
}

/**
 * @name ~MA Constructor
 * frees dynamic allocated memory
 */
template <class T>
MA<T>::~MA()
{
    delete[] m_data;
    m_data = NULL;
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
    m_index++;

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
T MA<T>::getValue()
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

#endif