#!/bin/bash

KT=/sys/kernel/tracing
AOUT=a.out
LOG=1.log

echo function > $KT/current_tracer
echo my_ioctl > $KT/set_ftrace_filter
echo 1 > $KT/tracing_on
./$AOUT
echo 0 > $KT/tracing_on
cat $KT/trace > $LOG

echo > $KT/trace
echo > $KT/set_ftrace_filter
echo nop > $KT/current_tracer
echo > $KT/set_ftrace_pid
