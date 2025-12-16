# Time-Sliced Scheduling in Zephyr Kernel

## Overview

This document provides a comprehensive overview of the time-sliced scheduling implementation in the Zephyr RTOS kernel.

## Executive Summary

Time-sliced scheduling is **fully implemented and operational** in the Zephyr kernel. The implementation provides:

- ✅ Round-robin scheduling for equal-priority preemptible threads
- ✅ Configurable time slice durations (default: 20ms)
- ✅ Per-thread time slicing with custom callbacks
- ✅ Multi-CPU/SMP support
- ✅ Priority ceiling to exempt high-priority threads
- ✅ Runtime configuration APIs
- ✅ Comprehensive test coverage
- ✅ Complete documentation

## Implementation Details

### Core Files

1. **kernel/timeslicing.c** (131 lines)
   - Main time slicing implementation
   - Manages time slice timeouts per CPU
   - Implements `z_time_slice()` called from timer interrupt
   - Handles both global and per-thread time slicing

2. **kernel/timeout.c**
   - Integrates time slicing with timer system
   - Calls `z_time_slice()` from `sys_clock_announce()`

3. **kernel/include/ksched.h**
   - Public API declarations
   - Helper functions like `thread_is_sliceable()`

### Key Functions

#### Public APIs

```c
/* Set global time slicing parameters */
void k_sched_time_slice_set(int32_t slice_ms, int max_prio);

/* Set per-thread time slice with callback (requires CONFIG_TIMESLICE_PER_THREAD) */
void k_thread_time_slice_set(struct k_thread *thread, 
                             int32_t thread_slice_ticks,
                             k_thread_timeslice_fn_t expired, 
                             void *data);
```

#### Internal Functions

```c
/* Called from timer interrupt to handle time slice expiration */
void z_time_slice(void);

/* Reset time slice timer for current thread */
void z_reset_time_slice(struct k_thread *curr);

/* Check if thread is subject to time slicing */
bool thread_is_sliceable(struct k_thread *thread);

/* Move current thread to end of priority queue */
void move_current_to_end_of_prio_q(void);
```

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `CONFIG_TIMESLICING` | y | Enable time slicing feature |
| `CONFIG_TIMESLICE_SIZE` | 20 | Default time slice size in milliseconds |
| `CONFIG_TIMESLICE_PRIORITY` | 0 | Priority ceiling (threads with prio < this are exempt) |
| `CONFIG_TIMESLICE_PER_THREAD` | n | Enable per-thread time slicing |

### Dependencies

Time slicing requires:
- `CONFIG_SYS_CLOCK_EXISTS=y` (system timer support)
- `CONFIG_NUM_PREEMPT_PRIORITIES != 0` (preemptible threads)

## How It Works

### Time Slice Flow

```
1. Timer interrupt occurs
   ↓
2. sys_clock_announce() called in kernel/timeout.c
   ↓
3. Process expired timeouts
   ↓
4. z_time_slice() called (#ifdef CONFIG_TIMESLICING)
   ↓
5. Check if slice expired for current CPU
   ↓
6. If expired and thread is sliceable:
   - Call per-thread callback (if configured)
   - Move thread to end of priority queue
   - Reset time slice timer
   ↓
7. Next thread at same priority gets scheduled
```

### Sliceable Thread Criteria

A thread is subject to time slicing if ALL of the following are true:
1. Thread is preemptible (`thread->base.prio >= 0`)
2. Time slicing is configured (global or per-thread)
3. Thread priority is NOT higher than slice_max_prio
4. Thread is not prevented from running
5. Thread is not the idle thread

### Multi-CPU Support

- Each CPU maintains its own time slice timeout
- Per-CPU slice expiration flags
- IPI (Inter-Processor Interrupt) sent when handling slice expiration for remote CPU

## Usage Examples

### Basic Time Slicing

```c
/* Enable time slicing with 100ms slices for priority 0 and lower */
k_sched_time_slice_set(100, 0);

/* Create threads with equal priority - they will time slice */
k_thread_create(&thread1, stack1, STACK_SIZE, 
                thread_fn, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
k_thread_create(&thread2, stack2, STACK_SIZE,
                thread_fn, NULL, NULL, NULL, 5, 0, K_NO_WAIT);
```

### Per-Thread Time Slicing

```c
void my_slice_expired(struct k_thread *thread, void *data)
{
    printk("Thread %p slice expired\n", thread);
}

/* Set custom time slice for specific thread */
k_thread_time_slice_set(&my_thread, 50, my_slice_expired, &my_thread);
```

## Test Coverage

Comprehensive tests exist in `tests/kernel/sched/schedule_api/`:

1. **test_slice_scheduling.c**
   - Tests basic time slicing with multiple threads
   - Verifies round-robin behavior
   - Tests time slice enable/disable

2. **test_slice_perthread.c**
   - Tests per-thread time slicing
   - Verifies callback invocation
   - Tests thread abortion from callback

3. **test_sched_timeslice_reset.c**
   - Tests time slice reset functionality
   - Verifies slice timer resets on thread changes

4. **test_sched_timeslice_and_lock.c**
   - Tests interaction with scheduler locking
   - Verifies time slicing respects locks

## Demonstration Samples

Two new samples have been added to showcase time slicing:

### 1. samples/kernel/timeslicing_demo/

Basic demonstration of time slicing with multiple equal-priority threads.

**Features:**
- Creates 3 threads with equal priority
- Shows fair CPU sharing via round-robin scheduling
- Measures and displays execution counts

**Build:**
```bash
west build -b qemu_x86 samples/kernel/timeslicing_demo
west build -t run
```

### 2. samples/kernel/timeslicing_per_thread/

Advanced demonstration of per-thread time slicing with custom callbacks.

**Features:**
- Creates 3 threads with different time slice durations
- Demonstrates slice expiration callbacks
- Shows how threads with different slices coexist

**Build:**
```bash
west build -b qemu_x86 samples/kernel/timeslicing_per_thread
west build -t run
```

## Performance Considerations

### Overhead

- Time slice check occurs every timer interrupt
- Minimal overhead when time slice hasn't expired
- O(1) complexity for moving thread to end of queue
- Spinlock held briefly during time slice operations

### Tuning

**Larger time slices:**
- Pros: Less context switching overhead, better cache performance
- Cons: Less responsive to priority changes, potential latency issues

**Smaller time slices:**
- Pros: More fair CPU distribution, better responsiveness
- Cons: More context switching overhead, worse cache performance

**Recommended:** 
- Start with default 20ms
- Tune based on application requirements
- Use per-thread slicing for mixed workloads

## Known Limitations

1. **Not Guaranteed Equal Time**: Time slicing ensures threads don't run for more than one slice, but doesn't guarantee equal time distribution (due to other scheduling events)

2. **Timer Resolution**: Time slice granularity limited by system timer tick rate

3. **Cooperative Threads**: Cooperative threads (priority < 0) are never subject to time slicing

4. **Meta-IRQ Threads**: Meta-IRQ threads always run to completion regardless of time slicing

## Compatibility

- **Architecture Independent**: Works on all Zephyr-supported architectures
- **SMP Support**: Full multi-CPU support included
- **Userspace Compatible**: Works with userspace threads
- **Tickless Mode**: Compatible with tickless idle

## References

### Documentation
- `doc/kernel/services/scheduling/index.rst` - Main scheduling documentation
- `doc/kernel/services/scheduling/timeslicing.svg` - Time slicing diagram

### Source Code
- `kernel/timeslicing.c` - Implementation
- `kernel/include/ksched.h` - API definitions
- `include/zephyr/kernel.h` - Public API

### Tests
- `tests/kernel/sched/schedule_api/` - Test suite

## Conclusion

The Zephyr kernel provides a complete, production-ready time-sliced scheduling implementation with:

- ✅ Robust core implementation
- ✅ Flexible configuration options
- ✅ Advanced per-thread features
- ✅ Comprehensive test coverage
- ✅ Complete documentation
- ✅ Example applications

The feature is enabled by default and ready for use in real-world applications requiring fair scheduling of equal-priority threads.
