.. _timeslicing_demo:

Time Slicing Demonstration
###########################

Overview
********

This sample demonstrates the time-sliced scheduling feature in the Zephyr kernel.
Time slicing allows multiple threads with equal priority to share CPU time in a
round-robin fashion, ensuring fair scheduling among threads of the same priority.

The sample creates multiple preemptible threads with the same priority and shows
how the scheduler automatically switches between them at regular time intervals
(time slices), giving each thread an opportunity to execute.

Requirements
************

This sample can run on any board with sufficient memory to create the demonstration
threads. It uses the system timer for time slicing, so the board must support
``CONFIG_SYS_CLOCK_EXISTS``.

Building and Running
********************

This application can be built and executed on any supported board as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/kernel/timeslicing_demo
   :board: qemu_x86
   :goals: build run
   :compact:

Sample Output
*************

.. code-block:: console

    === Zephyr Time Slicing Demonstration ===

    This demo shows time-sliced scheduling with 3 threads
    All threads have equal priority (5) and will be scheduled
    in a round-robin fashion with time slicing enabled.

    Thread 0 started
    Thread 1 started
    Thread 2 started
    Thread 0: iteration 5, counter=500000
    Thread 1: iteration 5, counter=500000
    Thread 2: iteration 5, counter=500000
    Thread 0: iteration 10, counter=1000000
    Thread 1: iteration 10, counter=1000000
    Thread 2: iteration 10, counter=1000000
    Thread 0: iteration 15, counter=1500000
    Thread 1: iteration 15, counter=1500000
    Thread 2: iteration 15, counter=1500000
    Thread 0: iteration 20, counter=2000000
    Thread 0 completed with counter=2000000
    Thread 1: iteration 20, counter=2000000
    Thread 1 completed with counter=2000000
    Thread 2: iteration 20, counter=2000000
    Thread 2 completed with counter=2000000

    === Final Results ===
    Thread 0 final counter: 2000000
    Thread 1 final counter: 2000000
    Thread 2 final counter: 2000000

    Time slicing demonstration completed successfully!
    Note: All threads received approximately equal CPU time
    due to the time-sliced round-robin scheduling.

How It Works
************

The sample creates multiple threads with equal priority that perform CPU-intensive
work (incrementing counters in tight loops). Without time slicing, the first thread
to run would monopolize the CPU. With time slicing enabled:

1. The scheduler assigns each thread a time slice (default 100ms in this sample)
2. When a thread's time slice expires, it's moved to the back of the ready queue
3. The next thread at the same priority gets to run
4. This continues in a round-robin fashion, giving all equal-priority threads
   fair access to the CPU

Configuration Options
*********************

- ``CONFIG_TIMESLICING``: Enable time slicing (enabled by default)
- ``CONFIG_TIMESLICE_SIZE``: Time slice size in milliseconds (default 100ms in this sample)
- ``CONFIG_TIMESLICE_PRIORITY``: Priority ceiling for time slicing (default 0)

See Also
********

- :ref:`scheduling_v2` - Kernel scheduling documentation
- :c:func:`k_sched_time_slice_set` - Runtime time slice configuration
- :c:func:`k_thread_time_slice_set` - Per-thread time slice configuration
