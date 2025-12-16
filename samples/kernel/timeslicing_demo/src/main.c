/*
 * Copyright (c) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Time Slicing Demonstration
 *
 * This sample demonstrates time-sliced scheduling in the Zephyr kernel.
 * It creates multiple threads with equal priority that will be scheduled
 * in a round-robin fashion using time slicing.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE 1024
#define NUM_THREADS 3
#define THREAD_PRIORITY 5

/* Thread stacks */
K_THREAD_STACK_ARRAY_DEFINE(thread_stacks, NUM_THREADS, STACK_SIZE);
static struct k_thread threads[NUM_THREADS];

/* Counter for each thread */
static volatile int thread_counters[NUM_THREADS];

/**
 * @brief Thread entry point
 *
 * Each thread increments its counter and prints a message periodically.
 * With time slicing enabled, all threads should get fair CPU time.
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
	int iterations = 0;

	printk("Thread %d started\n", thread_id);

	while (iterations < 20) {
		/* Busy work to consume CPU time */
		for (volatile int i = 0; i < 100000; i++) {
			thread_counters[thread_id]++;
		}

		iterations++;
		
		/* Print status periodically */
		if (iterations % 5 == 0) {
			printk("Thread %d: iteration %d, counter=%d\n", 
			       thread_id, iterations, thread_counters[thread_id]);
		}
	}

	printk("Thread %d completed with counter=%d\n", 
	       thread_id, thread_counters[thread_id]);
}

/**
 * @brief Main function
 *
 * Creates multiple threads with equal priority and demonstrates
 * time-sliced scheduling.
 */
int main(void)
{
	printk("\n=== Zephyr Time Slicing Demonstration ===\n\n");
	printk("This demo shows time-sliced scheduling with %d threads\n", NUM_THREADS);
	printk("All threads have equal priority (%d) and will be scheduled\n", THREAD_PRIORITY);
	printk("in a round-robin fashion with time slicing enabled.\n\n");

	/* Create threads with equal priority */
	for (int i = 0; i < NUM_THREADS; i++) {
		thread_counters[i] = 0;
		
		k_thread_create(&threads[i], 
				thread_stacks[i],
				STACK_SIZE,
				thread_entry,
				(void *)(uintptr_t)i, NULL, NULL,
				THREAD_PRIORITY, 0, K_NO_WAIT);
	}

	/* Wait for all threads to complete */
	for (int i = 0; i < NUM_THREADS; i++) {
		k_thread_join(&threads[i], K_FOREVER);
	}

	printk("\n=== Final Results ===\n");
	for (int i = 0; i < NUM_THREADS; i++) {
		printk("Thread %d final counter: %d\n", i, thread_counters[i]);
	}

	printk("\nTime slicing demonstration completed successfully!\n");
	printk("Note: All threads received approximately equal CPU time\n");
	printk("due to the time-sliced round-robin scheduling.\n");

	return 0;
}
