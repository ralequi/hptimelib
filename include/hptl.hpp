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
#ifndef __HPTLIB__HPP__
#define __HPTLIB__HPP__

// C Lib
#include "hptl.h"
// Config file
#include <system_error>

#include "hptl_config.h"

namespace hptl {
class hptl {
   public:
	/**
	 * Setups a new HPTL clock with default config
	 *
	 */
	hptl () : hptl (NULL) {
	}

	/**
	 * Setups a new HPTL clock with the given config
	 *
	 * @param conf is an HPTL configure file.
	 * 			   If NULL, default configuration is used.
	 */
	hptl (hptl_config *conf) {
		if (!this->reconfigure (conf))
			throw std::runtime_error ("Invalid HPTL config");
	}

	/**
	 * Reconfigure internal HPTL clock
	 *
	 * @param conf configuration. If NULL, default configuration is used.
	 * @return true Everything is ok
	 * @return false There is an issue with the configure structure
	 */
	bool reconfigure (hptl_config *conf) {
		return hptl_initclk (&this->_clock, conf) == 0;
	}

	/**
	 * Clock (RE-)Syncronization with RTSC
	 **/
	void sync () {
		hptl_syncclk (&this->_clock);
	}

	/**
	 * Gets current time
	 **/
	hptl_t getTime () {
		return hptl_getTime (&this->_clock);
	}

	/**
	 * Get the Time in nanosecons from  01 Jan 1970
	 *
	 * @return uint64_t
	 */
	uint64_t getTime_ns () {
		return hptl_clkntimestamp (&this->_clock, getTime ());
	}

	/**
	 * Gets current time. The return would be in the format of the fatest function available in the
	 * system. Acuracy nor precision are not guaranteed
	 **/
	hptl_t getFastestTime () {
		return hptl_getFastestTime (&this->_clock);
	}

	/**
	 * Convert a certain number of nanoseconds into clock-cycles
	 * @param ns the ns to convert into clock-cycles
	 **/
	uint64_t ns2cycles (uint64_t ns) {
		return hptl_ns2cycles (&this->_clock, ns);
	}

	/**
	 * Return the resolution in terms of ns
	 **/
	uint64_t getclkres () {
		return hptl_getclkres (&this->_clock);
	}

	/**
	 * Wait certain ns actively.
	 * @param ns the number of nanoseconds to wait
	 **/
	void wait (uint64_t ns) {
		hptl_wait (&this->_clock, ns);
	}

	/**
	 * Wait certain cycles actively.
	 * @param cycles the number of cpu-cycles to wait
	 **/
	void wait_cycles (uint64_t cycles) {
		hptl_wait_cycles (cycles);
	}

   private:
	hptl_clock _clock;
};
}  // namespace hptl

#endif
