#ifndef _CSENSORRESULTS_H_
#define _CSENSORRESULTS_H_

class CSensorResults
{
  private:
    uint16_t* pm25Results;
    uint16_t* pm10Results;
    uint8_t* temperatureResults;
    uint8_t* humidityResults;

    uint8_t pos;
    const uint8_t arraySize;

    uint16_t CalcAvg(uint8_t *arr)
    {
      uint8_t calculatedElements = 0;
      uint32_t sum = 0;
      for(uint8_t i = 0; i < pos; i++)
      {
        if(arr[i] > 0)
        {
          sum += arr[i];
          calculatedElements++;
        }
      }
      return (uint16_t)(sum / calculatedElements);
    }

    uint16_t CalcAvg(uint16_t *arr)
    {
      uint8_t calculatedElements = 0;
      uint32_t sum = 0;
      for(uint8_t i = 0; i < pos; i++)
      {
        if(arr[i] > 0)
        {
          sum += arr[i];
          calculatedElements++;
        }
      }
      return (uint16_t)(sum / calculatedElements);
    }
    
  public:
    CSensorResults(uint8_t aSize) : arraySize(aSize) 
    { 
      pm25Results = new uint16_t[arraySize];
      pm10Results = new uint16_t[arraySize];
      temperatureResults = new uint8_t[arraySize];
      humidityResults = new uint8_t[arraySize];
      Clear();
      pos = 0;
    } 
    ~CSensorResults()
    { 
      delete[] pm25Results;
      delete[] pm10Results;
      delete[] temperatureResults;
      delete[] humidityResults;
    } 

    bool Add(uint16_t pm25, uint16_t pm10, uint8_t temp, uint8_t humid)
    {
      if(pos < arraySize)
      {
        pm25Results[pos] = pm25;
        pm10Results[pos] = pm10;
        temperatureResults[pos] = temp;
        humidityResults[pos] = humid;
        pos++;
        return true;
      }
      else
      {
        return false;
      }
    }
    void Clear()
    {
      memset(pm25Results, 0, sizeof(uint16_t) * arraySize);
      memset(pm10Results, 0, sizeof(uint16_t) * arraySize);
      memset(temperatureResults, 0, sizeof(uint8_t) * arraySize);
      memset(humidityResults, 0, sizeof(uint8_t) * arraySize);
      pos = 0;
    }

    uint16_t GetPM25Avg()
    {
      return CalcAvg(pm25Results);
    }

    uint16_t GetPM10Avg()
    {
      return CalcAvg(pm10Results);
    }

    uint16_t GetTempAvg()
    {
      return CalcAvg(temperatureResults);
    }

    uint16_t GetHumidAvg()
    {
      return CalcAvg(humidityResults);
    }
};

#endif

