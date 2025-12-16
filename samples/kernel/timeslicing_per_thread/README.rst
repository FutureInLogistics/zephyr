.. _timeslicing_per_thread:

Per-Thread Time Slicing Demonstration
######################################

Overview
********

This sample demonstrates the per-thread time slicing feature in the Zephyr kernel.
Unlike global time slicing, per-thread time slicing allows each thread to have its
own custom time slice duration and expiration callback function.

The sample creates multiple threads, each configured with a different time slice
duration, and shows how the scheduler respects these individual settings. Each
thread also registers a callback function that gets invoked when its time slice
expires, allowing the application to track or respond to slice expirations.

Requirements
************

This sample requires:

- A board with sufficient memory to create the demonstration threads
- System timer support (``CONFIG_SYS_CLOCK_EXISTS``)
- Per-thread time slicing support (``CONFIG_TIMESLICE_PER_THREAD``)

Building and Running
********************

This application can be built and executed on any supported board as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/kernel/timeslicing_per_thread
   :board: qemu_x86
   :goals: build run
   :compact:

Sample Output
*************

.. code-block:: console

    === Zephyr Per-Thread Time Slicing Demonstration ===

    This demo shows per-thread time slicing with custom slice durations.
    Each thread has a different time slice and a callback function.

    Configured Thread 0 with time slice of 50 ticks
    Configured Thread 1 with time slice of 100 ticks
    Configured Thread 2 with time slice of 150 ticks

    Threads running with per-thread time slicing...

    Thread 0 started
    Thread 1 started
    Thread 2 started
      [Thread 0 slice expired, count=1]
      [Thread 1 slice expired, count=1]
      [Thread 0 slice expired, count=2]
      [Thread 2 slice expired, count=1]
      [Thread 0 slice expired, count=3]
      ...
    Thread 0 completed: 15 iterations, 8 slices
    Thread 1 completed: 15 iterations, 5 slices
    Thread 2 completed: 15 iterations, 4 slices

    === Final Results ===
    Thread 0: Time slice=50 ticks, Expirations=8, Work iterations=15
    Thread 1: Time slice=100 ticks, Expirations=5, Work iterations=15
    Thread 2: Time slice=150 ticks, Expirations=4, Work iterations=15

    Per-thread time slicing demonstration completed successfully!
    Note: Threads with smaller time slices experienced more slice expirations.

How It Works
************

The sample demonstrates several key features:

1. **Custom Time Slice Duration**: Each thread is configured with a different time
   slice duration using :c:func:`k_thread_time_slice_set`. Thread 0 gets 50 ticks,
   Thread 1 gets 100 ticks, and Thread 2 gets 150 ticks.

2. **Slice Expiration Callbacks**: Each thread registers a callback function that
   is invoked whenever its time slice expires. This allows the application to:
   
   - Track how often each thread's slice expires
   - Implement custom behavior on slice expiration
   - Adjust thread behavior dynamically

3. **Fair Scheduling with Custom Slices**: Despite having different time slice
   durations, all threads still get fair access to the CPU. Threads with shorter
   slices are preempted more frequently but continue to make progress.

Configuration Options
*********************

- ``CONFIG_TIMESLICING``: Enable time slicing (must be enabled)
- ``CONFIG_TIMESLICE_PER_THREAD``: Enable per-thread time slicing (must be enabled)
- ``CONFIG_TIMESLICE_SIZE``: Default time slice size (used if not set per-thread)
- ``CONFIG_TIMESLICE_PRIORITY``: Priority ceiling for time slicing

API Functions
*************

- :c:func:`k_thread_time_slice_set`: Set per-thread time slice duration and callback
- :c:func:`k_sched_time_slice_set`: Set global time slice settings

See Also
********

- :ref:`scheduling_v2` - Kernel scheduling documentation
- :ref:`timeslicing_demo` - Basic time slicing demonstration
