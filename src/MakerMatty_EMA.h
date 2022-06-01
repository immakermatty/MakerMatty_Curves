/**
 * Author	: @makermatty (maker.matejsuchanek.cz)
 * Date		: 1-6-2022
 *
 * Generic Exponential Moving Average Class
 */

class EMA {

public:
    EMA()
        : m_n(1)
    {
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
    }

    EMA(int n, float value)
        : m_n(n)
    {
        this->setValue(value);
    }

    float update(float val)
    {
        m_values[2] = m_values[1];
        m_values[1] = m_values[0];

        const float k = 2.0 / (m_n + 1);
        return m_values[0] = val * k + m_values[1] * (1.0 - k);

        // Serial.println(String(m_values[0]) + ", " + String(m_values[1]) + ", " + String(m_values[2]));
    }

    float getValue() const
    {
        return m_values[0];
    }

    void setValue(const float value)
    {
        m_values[0] = value;
        m_values[1] = value;
        m_values[2] = value;
    }

    float getSlope() const
    {
        return m_values[0] - m_values[1];
    }

    float getCurve() const
    {
        return (m_values[0] - m_values[1]) - (m_values[1] - m_values[2]);
    }

private:
    int m_n;
    float m_values[3];
};