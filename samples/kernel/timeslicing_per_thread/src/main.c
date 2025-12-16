/*
 * Copyright (c) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Per-Thread Time Slicing Demonstration
 *
 * This sample demonstrates per-thread time slicing in the Zephyr kernel.
 * Each thread can have its own custom time slice duration and callback
 * function that is invoked when the time slice expires.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define NUM_THREADS 3
#define THREAD_PRIORITY 5

/* Thread stacks */
K_THREAD_STACK_ARRAY_DEFINE(thread_stacks, NUM_THREADS, STACK_SIZE);
static struct k_thread threads[NUM_THREADS];

/* Track slice expirations per thread */
static volatile int slice_expirations[NUM_THREADS];
static volatile int work_iterations[NUM_THREADS];

/**
 * @brief Time slice expiration callback
 *
 * This callback is invoked when a thread's time slice expires.
 * It allows the application to track slice expirations or implement
 * custom behavior.
 *
 * @param thread The thread whose time slice expired
 * @param data User data (thread ID in this example)
 */
static void slice_expired_callback(struct k_thread *thread, void *data)
{
	int thread_id = (int)(uintptr_t)data;
	slice_expirations[thread_id]++;

	printk("  [Thread %d slice expired, count=%d]\n",
	       thread_id, slice_expirations[thread_id]);
}

/**
 * @brief Thread entry point
 *
 * Each thread performs work while tracking how many time slices it uses.
 *
 * @param p1 Thread ID
 * @param p2 Unused
 * @param p3 Unused
 */
static void thread_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	int thread_id = (int)(uintptr_t)p1;

	printk("Thread %d started\n", thread_id);

	/* Perform CPU-intensive work */
	while (work_iterations[thread_id] < 15) {
		/* Busy work */
		for (volatile int i = 0; i < 50000; i++) {
			/* Just burn CPU cycles */
		}
		work_iterations[thread_id]++;
		
		/* Periodically yield to be polite, but time slicing
		 * will preempt us anyway
		 */
		if (work_iterations[thread_id] % 5 == 0) {
			k_yield();
		}
	}

	printk("Thread %d completed: %d iterations, %d slices\n", 
	       thread_id, work_iterations[thread_id], slice_expirations[thread_id]);
}

/**
 * @brief Main function
 *
 * Creates threads with different per-thread time slice settings.
 */
int main(void)
{
	printk("\n=== Zephyr Per-Thread Time Slicing Demonstration ===\n\n");
	printk("This demo shows per-thread time slicing with custom slice durations.\n");
	printk("Each thread has a different time slice and a callback function.\n\n");

	/* Define different time slices for each thread (in ticks) */
	const int32_t slice_ticks[] = {
		50,  /* Thread 0: 50 ticks */
		100, /* Thread 1: 100 ticks */
		150  /* Thread 2: 150 ticks */
	};

	/* Create threads with different per-thread time slices */
	for (int i = 0; i < NUM_THREADS; i++) {
		slice_expirations[i] = 0;
		work_iterations[i] = 0;
		
		/* Create the thread */
		k_thread_create(&threads[i], 
				thread_stacks[i],
				STACK_SIZE,
				thread_entry,
				(void *)(uintptr_t)i, NULL, NULL,
				THREAD_PRIORITY, 0, K_NO_WAIT);
		
		/* Set per-thread time slice with callback */
		k_thread_time_slice_set(&threads[i], 
					slice_ticks[i],
					slice_expired_callback,
					(void *)(uintptr_t)i);
		
		printk("Configured Thread %d with time slice of %d ticks\n", 
		       i, slice_ticks[i]);
	}

	printk("\nThreads running with per-thread time slicing...\n\n");

	/* Wait for all threads to complete */
	for (int i = 0; i < NUM_THREADS; i++) {
		k_thread_join(&threads[i], K_FOREVER);
	}

	printk("\n=== Final Results ===\n");
	for (int i = 0; i < NUM_THREADS; i++) {
		printk("Thread %d: Time slice=%d ticks, Expirations=%d, Work iterations=%d\n",
		       i, slice_ticks[i], slice_expirations[i], work_iterations[i]);
	}

	printk("\nPer-thread time slicing demonstration completed successfully!\n");
	printk("Note: Threads with smaller time slices experienced more slice expirations.\n");

	return 0;
}
