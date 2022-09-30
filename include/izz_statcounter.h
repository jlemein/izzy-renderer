//
// Created by jeffrey on 30-9-22.
//
#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

#ifndef NDEBUG
  #define IZZ_STAT_COUNT(X) StatCounter::instance()->count(#X);
  #define IZZ_STAT_NEXTFRAME() StatCounter::instance()->startFrame();
#else
  #define IZZ_STAT_COUNT(X)
  #define IZZ_STAT_NEXTFRAME()
#endif // NDEBUG

namespace izz {

/**
 * Statistic containing value for previous and current measurement period.
 * The current measurement period tends to get incremented a lot. For visualization
 * purposes it's better to use value for the last measurement period.
 * @tparam T type of the measured value. By default int.
 */
template <typename T=int>
struct Stat {
  T current;
  T last;
};

/**
 * @brief Singleton class that can be used to track performance or statistics in the application.
 *
 * @details
 * The StatCounter tracks the frame duration
 */
class StatCounter {
 public:
  /// @brief measurement period in milliseconds. Statistics will get averaged over this frequency.
  static const inline int MEASUREMENT_PERIOD_MS = 1000;

  /**
   * @returns the singleton instance of StatCounter.
   */
  static StatCounter* instance();

 private:
  static StatCounter* m_instance;
  static std::once_flag flag_;

  StatCounter() {
    // reset the timers
    clock_gettime(CLOCK_REALTIME, &m_start);
    m_current = m_start;
  }

  std::unordered_map<std::string, Stat<int>> m_counters;

  timespec m_start;                 /// start time of the measurement period.
  timespec m_current;               /// current time of the measurement period.
                                    /// Current time is always greater or equal than start time.

  Stat<int> m_frameCount = {0, 1};  /// Frame counter statistic. @see \Stat<T>.
  double m_lastDuration = 0.0;      /// Duration of the last measurement period (in milliseconds).

  /**
   * Retrieves the difference in time between the two time arguments in milliseconds.
   * @param t0 First time measurement.
   * @param t1 Second time measurement. t1 should be greater or equal than t0 (i.e. t1 >= t0).
   * @return time difference in milliseconds.
   */
  inline double getDuration(timespec& t0, timespec& t1) {
    timespec diff;
    diff.tv_sec = t1.tv_sec - t0.tv_sec;
    diff.tv_nsec = t1.tv_nsec - t0.tv_nsec;

    return static_cast<double>(diff.tv_sec * 1000L)
           + static_cast<double>(diff.tv_nsec) / 1000000.0;
  }

 public:
  /**
   * Indicate that a new frame has started.
   * This function is usually called by the render system when it starts a new frame.
   */
  inline void startFrame() {
    clock_gettime(CLOCK_REALTIME, &m_current);
    ++m_frameCount.current;

    // check if the measurement period is over. If so, the values are copied from current to last
    // to store the values and make them possible to be retrieved using the getters.
    double duration = getDuration(m_start, m_current);
    if (duration > MEASUREMENT_PERIOD_MS) {
      for (auto& counter : m_counters) {
        counter.second.last = counter.second.current;
        counter.second.current = 0;
      }
      m_lastDuration = duration;
      m_frameCount.last = m_frameCount.current;
      m_frameCount.current = 0;
      m_start = m_current;
    }
  }

  /**
   * Increment the count of the statistic key.
   * @param key Key chosen by user to track statistic.
   * @param increment The amount to increment the value with. By default 1.
   */
  void count(std::string key, int increment = 1);

  /**
   * Retrieves the count for the specified statistic key. The count corresponds to the last measurement period.
   * @param key The statistics key.
   * @return the actual count for the last measurement period.
   */
  inline int getCount(std::string key) {
    return m_counters[key].last;
  }

  /**
   * Retrieves the count for the specified statistic key, averaged for one frame.
   * @param key The statistics key.
   * @return the count per frame.
   */
  int getCountPerFrame(std::string key) {
    return m_counters[key].last / m_frameCount.last;
  }

  /**
   * @returns the last measurement duration in milliseconds.
   */
  inline double getMeasuredDuration() {
    return m_lastDuration;
  }

  /**
   * @returns the frame duration (in milliseconds) of the last measurement period.
   */
  inline double getFrameDuration() {
    return m_lastDuration / m_frameCount.last;  // to milliseconds
  }

  /**
   * @returns the frames per second for the last measurement period.
   */
  inline double getFps() {
    if (m_lastDuration == 0) {
      return 0.0;
    }
    return 1000.0 * m_frameCount.last / m_lastDuration;
  }
};

}  // namespace izz