/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2015 Florian Ziesche
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __FLOOR_TIMER_HPP__
#define __FLOOR_TIMER_HPP__

#include <floor/core/platform.hpp>

class FLOOR_API floor_timer {
public:
	floor_timer() { add("start", false); }
	~floor_timer() {}
	
	void add(const string& name, const bool print_diff = true) {
		entries.emplace_back(name, SDL_GetPerformanceCounter());
		if(print_diff) {
			print_entry(entries.end() - 1);
		}
	}
	
	void end(const bool print_all = false) const {
		if(print_all) {
			for(auto entry = entries.cbegin(); entry != entries.cend(); entry++) {
				print_entry(entry);
			}
		}
		const auto total_diff = entries.back().second - entries.front().second;
		log_undecorated("[TOTAL] %_s # %_ms # %_",
						double(total_diff) / double(ticks_per_second),
						double(total_diff) / double(ticks_per_second / 1000ull),
						total_diff);
	}
	
protected:
	const unsigned long long int ticks_per_second { SDL_GetPerformanceFrequency() };
	vector<pair<const string, const unsigned long long int>> entries;
	
	void print_entry(const decltype(entries)::const_iterator& iter) const {
		// don't print the start entry
		if(iter == entries.cbegin()) return;
		
		const auto prev_iter = iter - 1;
		const auto diff = iter->second - prev_iter->second;
		log_undecorated("[%_] %_ms # %_",
						iter->first,
						double(diff) / double(ticks_per_second / 1000ull),
						diff);
	}
	
};

// TODO: merge with other timer class
// timer needs a monotonic/steady clock, try using high_resolution_clock as default if it is steady, otherwise use steady_clock
template <class clock_type = typename conditional<chrono::high_resolution_clock::is_steady,
												  chrono::high_resolution_clock,
												  chrono::steady_clock>::type>
//! simple timer class based on std chrono functionality and capable of using arbitrary duration types (defaults to ms)
class FLOOR_API floor_timer2_t {
public:
	//! "starts" the timer: returns the current time_point of a monotonic clock
	static chrono::time_point<clock_type> start() {
		return clock_type::now();
	}
	
	//! "stops" the timer: substracts the start time_point from the current time_point of a monotonic clock
	//! and casts the result to a specified duration type (or milliseconds by default)
	template <typename duration_type = chrono::milliseconds>
	static uint64_t stop(const chrono::time_point<clock_type>& start_time) {
		return (uint64_t)chrono::duration_cast<duration_type>(clock_type::now() - start_time).count();
	}
	
protected:
	// static class
	floor_timer2_t(const floor_timer2_t&) = delete;
	~floor_timer2_t() = delete;
	floor_timer2_t& operator=(const floor_timer2_t&) = delete;
	
};
//! floor_timer_t with default clock_type (this is for easier use, so <> doesn't have to be used all the time)
using floor_timer2 = floor_timer2_t<>;

#endif
