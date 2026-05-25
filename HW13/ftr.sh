#!/bin/bash

cd /sys/kernel/tracing

echo function > current_tracer
# echo sys_* > set_ftrace_filter
# echo _raw_spin_lock >> set_ftrace_filter
echo 1 > tracing_on
/home/xtr/dev/otus-c-2026-01/HW13/a.out
echo 0 > tracing_on
cat trace

echo > trace
echo > set_ftrace_filter
echo nop > current_tracer
echo > set_ftrace_pid