/** 
 * Author	: @makermatty (maker.matejsuchanek.cz)
 * Date		: 15-6-2020
 * 
 * Generic Exponential Moving Average Class
 */

class EMA
{
private:
    int n;
    float value[3];

public:
    EMA()
    {
        n = 1;
        value[0] = 0;
        value[1] = 0;
        value[2] = 0;
    }

    EMA(int n, float val)
    {
        n = n;
        value[0] = val;
        value[1] = val;
        value[2] = val;
    }

    void update(float val)
    {
        value[2] = value[1];
        value[1] = value[0];

        float k = 2.0 / (n + 1);
        value[0] = val * k + value[1] * (1.0 - k);

        // Serial.println(String(value[0]) + ", " + String(value[1]) + ", " + String(value[2]));
    }

    float getValue() const
    {
        return value[0];
    }

    float getSlope() const
    {
        return value[0] - value[1];
    }

    float getCurve() const
    {
        return (value[0] - value[1]) - (value[1] - value[2]);
    }
};