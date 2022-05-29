#pragma once

/*
* Implements a simple linear recursive exponential filter.
* See: http://www.statistics.com/glossary&term_id=756 */
template<class T> class ExponentialFilter
{
  // Weight for new values, as a percentage ([0..100])
  T m_WeightNew;

  // Current filtered value.
  T m_Current;

public:
  ExponentialFilter(T WeightNew, T Initial)
    : m_WeightNew(WeightNew), m_Current(Initial*100)
  { }

  void Filter(T New)
  {
    m_Current = (100 * m_WeightNew * New + (100 - m_WeightNew) * m_Current + 50)/100;
  }

  void SetWeight(T NewWeight)
  {
    m_WeightNew = NewWeight;
  }

  T GetWeight() const { return m_WeightNew; }

  T Current() const { return (m_Current + 50)/100; }

  void SetCurrent(T NewValue)
  {
    m_Current = NewValue*100;
  }
};

// Specialization for floating point math.
template<> class ExponentialFilter<float>
{
  float m_fWeightNew;
  float m_fCurrent;

public:
  ExponentialFilter(float fWeightNew, float fInitial)
    : m_fWeightNew(fWeightNew/100.0), m_fCurrent(fInitial)
  { }


  void Filter(float fNew)
  {
    m_fCurrent = m_fWeightNew * fNew + (1.0 - m_fWeightNew) * m_fCurrent;
  }

  void SetWeight(float NewWeight)
  {
    m_fWeightNew = NewWeight/100.0;
  }

  float GetWeight() const { return m_fWeightNew*100.0; }

  float Current() const { return m_fCurrent; }

  void SetCurrent(float fNewValue)
  {
    m_fCurrent = fNewValue;
  }
};
