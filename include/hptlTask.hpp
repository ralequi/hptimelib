/**
  Language: C++

  License: MIT License
  (c) Naudit HPCN S.L. 2018-2021

  Author: Rafael Leira
  E-Mail: rafael.leira@naudit.es

  Description: High Performance Timing Library, that uses RTC counter.

  Thread Safe: Yes

  Platform Dependencies: Linux-like and Intel Processor
*/
#ifndef __HPTLTASK__HPP__
#define __HPTLTASK__HPP__

// CPP Main Lib
#include "hptl.hpp"

namespace hptl {
/**
 * This class is designed to keep track of tasks that should be executed exactly in amount of time.
 * It can sleep as necessary to ensure that the whole task process is executed with the exact
 * periodicity
 */
class hptlTask {
   public:
	/**
	 * Setups a new hptlTask with default clock
	 *
	 * @param task_period the time in nanoseconds that the task should long
	 */
	hptlTask (uint64_t task_period = 0) {
		this->setPeriod (task_period);
	}

	/**
	 * Setups a new hptlTask with default clock
	 *
	 * @param task_period the time in nanoseconds that the task should long
	 * @param base the hptl clock to be used
	 */
	hptlTask (uint64_t task_period, const hptl &base) {
		_clock = base;

		this->setPeriod (task_period);
	}

	/**
	 * Sets the period in nanoseconds
	 *
	 * @param ns the new task period
	 */
	void setPeriod (uint64_t ns) {
		this->task_period = ns;
		this->reset ();
	}

	/**
	 * Resets the hptlTask timer
	 *
	 */
	void reset () {
		last_measurement = _clock.getTime_ns ();
	}

	/**
	 * Call this method between calls to the periodically executed task.
	 * This method will ensure that the calls are called with the correct periodicity indenpendently
	 * on how long the task takes to be executed.
	 *
	 */
	void wait () {
		auto curr_time = _clock.getTime_ns ();

		if (curr_time < (last_measurement + task_period)) {
			_clock.wait ((last_measurement + task_period) - curr_time);
		}

		this->reset ();
	}

	/**
	 * Returns the internal hptl clock
	 *
	 * @return hptl&
	 */
	hptl &getClock () {
		return _clock;
	}

   private:
	hptl _clock;
	uint64_t last_measurement;
	uint64_t task_period;
};
}  // namespace hptl

#endif
