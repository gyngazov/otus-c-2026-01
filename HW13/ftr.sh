#!/bin/bash

KT=/sys/kernel/tracing
AOUT=a.out
LOG=1.log

echo function > $KT/current_tracer
echo 1 > $KT/tracing_on
./$AOUT
echo 0 > $KT/tracing_on
cat $KT/trace > $LOG

echo > $KT/trace
echo > $KT/set_ftrace_filter
echo nop > $KT/current_tracer
echo > $KT/set_ftrace_pid

echo "10 самых частых вызовов ядра в приложении $AOUT"
grep $AOUT $LOG \
    |awk '{print $5" "$6}' \
    |sort \
    |uniq -c \
    |sort -nr \
    |head \
    |cat -n

rm -f $LOG