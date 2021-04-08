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
    T* data; // Type pointer elements in our array
    uint16_t index; // current index
    int32_t sum; // sum for rest
    bool filled; // used to determine if we went through the whole array
    uint16_t n; // number of elements in our array
    T value = 0;

public:
    MA(uint16_t len, T init = 0); // constructor
    ~MA(); // destructor
    T update(const T); // Calculate new moving Average
    T getValue();

    const T& Value = value;
};

/**
 * @name MA Constructor
 * @param numberOf_data number of data in the array
 * Creates dynamically an array of our type values and initializes the array
 * and some local variables. There is no destructor as this is quite useless
 * due to lack of garbage collection in Arduino
 */
template <class T>
MA<T>::MA(uint16_t len, T init)
{
    // allocate an array of size T
    n = len >= 1 ? len : 1;

    data = new T[n];
    assert(data != NULL);

    filled = false; // this becomes true when the array is minimally filled once
    index = 0; // start with index 0
    sum = 0;

    if (init) {
        for (uint16_t i = 0; i < n; i++) {
            data[i] = init;
            sum += (uint32_t)data[i];
        }
        filled = true;
    }
}

/**
 * @name ~MA Constructor
 * frees dynamic allocated memory
 */
template <class T>
MA<T>::~MA()
{
    delete[] data;
    data = NULL;
}

/**
 * @name show
 * @param value	value to be added to array
 * @returns T returns the new moving average.
 * During the first filling of the data array the moving average is determined on acc_data less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */
template <class T>
T MA<T>::update(const T val)
{
    if (index >= n) {
        index = 0; // reset index
        filled = true; // we looped through at least once
    }

    sum = sum - (int32_t)data[index] + (int32_t)val;

    data[index] = val;
    index++;

    value = filled ? (T)(sum / (int32_t)n) : (T)(sum / (int32_t)index);

    return value;
}

/**
 * @name getValue
 * @returns T returns the new moving average.
 * During the first filling of the data array the moving average is determined on acc_data less
 * amount of entries. This function keeps track of this situation and calculates the correct
 * average on the number of items listed.
 */
template <class T>
T MA<T>::getValue()
{
    return value;
}

#endif