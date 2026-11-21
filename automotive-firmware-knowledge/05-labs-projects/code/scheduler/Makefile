CC     ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -Wconversion -O2

sched_demo: sched.c main_host.c sched.h
	$(CC) $(CFLAGS) sched.c main_host.c -o $@

run: sched_demo
	./sched_demo

clean:
	rm -f sched_demo

.PHONY: run clean
