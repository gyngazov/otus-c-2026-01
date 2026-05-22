#!/bin/bash

cd /sys/kernel/tracing

echo function > current_tracer
echo sys_* > set_ftrace_filter
echo 1 > tracing_on
ls
echo 0 > tracing_on
cat trace