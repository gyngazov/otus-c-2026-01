# 1. Navigate to the ftrace directory
cd /sys/kernel/tracing

# 2. Reset the buffer and select the function tracer
echo 0 > tracing_on
echo nop > current_tracer
echo function_graph > current_tracer

# 3. Restrict tracing to your terminal shell's PID (which will execute the binary)
echo $$ > set_ftrace_pid
#echo -n __rcu_read_lock > set_ftrace_filter

# 4. Execute your binary and trace it
echo 1 > tracing_on
/bin/ls
echo 0 > tracing_on

# 5. View the recorded trace
cat trace | less
