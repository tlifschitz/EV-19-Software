#ifndef __PERFORMANCE_COUNTER_H__
#define __PERFORMANCE_COUNTER_H__

#include "EV19_Peripherals.h"

/*
 *
 * PERFORMANCE COUNTER PERIPHERAL
 *
 * A performance-counter unit is just a block of
 * big counters for timing "sections" in your software.
 *
 * This block lets you accurately measure execution-time
 * taken by blocks C-code.  Simple, efficient, minimally-intrusive
 * macros allow you to mark the start and end of blocks-of-interest
 * in your program.  Each block-of-interest is called a "section."
 *
 * This peripheral has a measurement start *top feature that
 * lets you measure each section as a fraction of some larger
 * program (or enclosing task).
 *
 * This block to keep track of as many sections as you like (the default is 3).
 * You change the number of sections in the GUI.
 *
 * This block will contain two counters for every section:
 *   * Time:        A 64-bit time (clock-tick) counter.
 *   * Occurrences: A 32-bit event counter.
 *
 * If you had some function, and you wanted to know how much of your
 * execution time it was taking, you would do this:
 *
 *         // Use section-counter #3 to measure this function:
 *         #define INTERESTING_FUNCTION_SECTION 3
 *
 *         int my_interesting_function (int a, int b, ....)
 *         {
 *            int result;
 *            PERF_BEGIN (PERF_UNIT_BASE, INTERESTING_FUNCTION_SECTION);
 *
 *               ..body of subroutine...
 *
 *            PERF_END (PERF_UNIT_BASE, INTERESTING_FUNCTION_SECTION);
 *            return result;
 *         }
 *
 *
 * Then you would need to turn measurement on & off only for the
 * "interesting" part of your program, like this:
 *
 *         #include "altera_avalon_performance_counter.h"
 *         #include "system.h"
 *
 *         int main(){
 *
 *            // Reset the counters before every run
 *            PERF_RESET (PERF_UNIT_BASE);
 *
 *            // First, do things that we don't want to measure:
 *            //
 *            get_user_input_that_might_take_arbitrarily_long();
 *
 *            // Now our program starts in earnest.  Begin measuring:
 *            //
 *            PERF_START_MEASURING (PERF_UNIT_BASE);
 *
 *            while (things_to_do()) {
 *               my_interesting_function (a, b);
 *               my_boring_function (c,d);
 *               my_time_consuming_function (e,f);
 *            }
 *
 *            PERF_STOP_MEASURING (PERF_UNIT_BASE);
 *
 *            clean_up_and_print_results();
 *            return 0;
 *        }
 *
 * You can simultaneously measure as many sections as you like
 * (HOW_MANY_COUNTERS, less one for the "global" counter #0).
 *
 * You read the results using these functions:
 *
 *          unsigned long long perf_get_section_time
 *              (void* hw_base_address, int which_section);
 *
 *          unsigned long long perf_get_num_starts
 *              (void* hw_base_address, int which_section);
 *
 * Using straightforward arithmetic, you can compute the total
 * "hard" runtime, in real seconds, taken by each section
 * (using ALT_CPU_FREQ).
 *
 * You can also compute the actual percent-time-taken by each section
 * by dividing the section-time by the overall measurement time.
 * (the overall measurement time is returned by perf_get_section_time
 * for section #0).
 *
 *
 * THE REGISTER MAP
 *
 *
 *             31           7     6     5     4     3     2     1     0
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_0     |            Global Time Counter [31: 0]                    |  0
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_0     |            Global Time Counter [63:32]                    |  4
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Ev_0      |            Global Measurement-Start Counter               |  8
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 *   -       |                  --reserved--                             |  C
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_1     |            Section 1 Time Counter [31: 0]                 | 10
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_1     |            Section 1 Time Counter [63:32]                 | 14
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Ev_1      |            Section 1 Start Counter                        | 18
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 *   -       |                  --reserved--                             | 1C
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_2     |            Section 2 Time Counter [31: 0]                 | 20
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_2     |            Section 2 Time Counter [63:32]                 | 24
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Ev_2      |            Section 2 Start Counter                        | 28
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 *   -       |                  --reserved--                             | 2C
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 *           |                                                           |
 *           ~                      ...                                  ~
 *
 *           ~                      ...                                  ~
 *           |                                                           |
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_N     |            Section N Time Counter [31: 0]                 | n0
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Tlo_N     |            Section N Time Counter [63:32]                 | n4
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 * Ev_N      |            Section N Start Counter                        | n8
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 *   -       |                  --reserved--                             | nC
 *           +---+-/../--+-----+-----+-----+-----+-----+-----+-----+-----+
 *
 *
 * THE GLOBAL COUNTERS
 *
 * This unit uses section #0 as a special "global" section, which
 * counts the total time during which measurements are being taken.
 * None of the other section-counters are allowed to run at all
 * (not even the other event counters) when the global time-counter
 * is stopped.
 *
 * Special macros (PERF_START_MEASURING, PERF_STOP_MEASURING) are defined
 * to control the global counters.  Users should not manipulate
 * the global counters directly through PERF_BEGIN and PERF_END.
 *
 * PERF_BEGIN and PERF_END
 *
 * These macros are very efficient, typically requiring only
 * two or three machine instructions.
 *
 *
 */


#define PERF_START(n) PERF_COUNTERS[n]->counterHi = 0

#define PERF_STOP(n)   PERF_COUNTERS[n]->counterLo = 0

#define PERF_START_MEASURING PERF_START(0)

#define PERF_STOP_MEASURING  PERF_STOP(0)

#define PERF_RESET PERF_COUNTERS[0]->counterLo = 1

#define PERF_GET_SECTION_TIME(n) ((uint64_t)((((uint64_t)PERF_COUNTERS[n]->counterHi)<<32)&0xFFFFFFFF00000000) | (((uint64_t)PERF_COUNTERS[n]->counterLo)&0x00000000FFFFFFFF))

#define PERF_GET_OCCURRENCIES(n) PERF_COUNTERS[n]->occurrencies


#endif /* _PERFORMANCECOUNTER_H_ */
