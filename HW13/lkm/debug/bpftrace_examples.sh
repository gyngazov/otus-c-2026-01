#!/usr/bin/env bash
# bpftrace_examples.sh — набор примеров для bpftrace / eBPF
#
# Что такое bpftrace:
#   bpftrace — высокоуровневая оболочка над eBPF, позволяющая быстро писать
#   одноразовые и исследовательские трассировки без пересборки ядра и модуля.
#
# Для данного проекта bpftrace особенно интересен, потому что позволяет:
#   - вешаться на вход/выход kernel-функций модуля;
#   - считать количество вызовов;
#   - измерять latency;
#   - строить histogram;
#   - анализировать syscalls userspace параллельно с kernel-событиями.
#
# Предварительные условия:
#   - модуль загружен;
#   - установлен bpftrace;
#   - ядро поддерживает BPF/kprobes/tracepoints.

echo "=== bpftrace one-liners and scripts ==="
echo

echo "--- 1. Трассировка всех kernel-функций lkm_* ---"
#
# kprobe:lkm_* цепляется ко всем функциям ядра, имя которых подходит
# под шаблон lkm_*. Это удобный способ быстро увидеть, какие функции модуля
# вообще вызываются в ответ на действия пользователя.
echo 'Run:  bpftrace -e '"'"'kprobe:lkm_* { printf("%s called by PID %d\n", probe, pid); }'"'"
echo

echo "--- 2. Подсчёт вызовов lkm_slow_work в секунду ---"
#
# count() накапливает число срабатываний kprobe.
# interval:s:1 раз в секунду печатает значение и обнуляет счётчик.
#
# Это простой способ оценить интенсивность нагрузки.
cat <<'BPFEOF'
 bpftrace -e '
kprobe:lkm_slow_work
{
    @calls = count();
}
interval:s:1
{
    printf("lkm_slow_work calls/sec: %d\n", @calls);
    clear(@calls);
}'
BPFEOF
echo

echo "--- 3. Измерение latency функции lkm_slow_work ---"
#
# Идея классическая:
#   - на входе сохраняем timestamp в @start[tid];
#   - на выходе считаем разницу;
#   - складываем latency в histogram.
#
# Результат особенно полезен, если варьировать iterations и смотреть,
# как смещается распределение времени выполнения.
cat <<'BPFEOF'
 bpftrace -e '
kprobe:lkm_slow_work  { @start[tid] = nsecs; }
kretprobe:lkm_slow_work
/@start[tid]/
{
    @latency_us = hist((nsecs - @start[tid]) / 1000);
    delete(@start[tid]);
}'
BPFEOF
echo

echo "--- 4. Трассировка ioctl вызовов по имени процесса ---"
#
# Здесь используется tracepoint sys_enter_ioctl.
# Фильтр /comm == "ioctl_test"/ позволяет отсеять остальные процессы.
#
# Это удобный мост между userspace и kernelspace: видно, когда именно
# программа отправляет ioctl-команды.
cat <<'BPFEOF'
 bpftrace -e '
tracepoint:syscalls:sys_enter_ioctl
/comm == "ioctl_test"/
{
    printf("PID %d ioctl fd=%d cmd=0x%x\n", pid, args->fd, args->cmd);
}'
BPFEOF
echo

echo "--- 5. Наблюдение за lkm_write() ---"
#
# kprobe:lkm_write позволяет увидеть вызовы write()-обработчика модуля.
# arg2 в данном случае соответствует длине передаваемого буфера.
#
# Полезно, если тестировать разные сообщения через:
#    ./userspace/ioctl_test write "..."
cat <<'BPFEOF'
 bpftrace -e '
kprobe:lkm_write
{
    printf("write called by PID %d (%s), len=%lu\n", pid, comm, arg2);
}'
BPFEOF
echo

echo "--- 6. Отслеживание kmalloc / kfree ---"
#
# В текущем учебном модуле выделения памяти почти нет, но пример оставлен
# как шаблон для дальнейших экспериментов. Если позже в модуле появятся
# kmalloc/kfree, этот шаблон можно адаптировать под анализ аллокаций.
cat <<'BPFEOF'
bpftrace -e '
kprobe:__kmalloc
/ustack/ /* filter: originating from userspace-triggered path */
{
    printf("kmalloc size=%lu caller=%s\n", arg0, func);
}
kprobe:kfree
{
    @kfree_calls = count();
}'
BPFEOF
echo

echo "--- 7. Запуск готового script file ---"
#
# Вместо длинного one-liner можно запускать bpftrace-скрипт из файла.
# В этом проекте такой файл уже подготовлен: debug/lkm_trace.bt
#
# Это особенно удобно для повторяемых экспериментов.
echo "   bpftrace debug/lkm_trace.bt"
echo
