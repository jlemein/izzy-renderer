//
// Created by jeffrey on 30-9-22.
//
#include <izz_statcounter.h>
using namespace izz;

StatCounter* StatCounter::m_instance = nullptr;
std::once_flag StatCounter::flag_;

StatCounter *StatCounter::instance() {
  std::call_once(flag_, []{
    m_instance = new StatCounter;
  });
  return m_instance;
}

void StatCounter::count(std::string key, int increment) {
  if (m_counters.contains(key)) {
    m_counters[key].current += increment;
  } else {
    m_counters[key].current = 0;
  }
}