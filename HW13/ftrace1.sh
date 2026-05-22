#!/usr/bin/env bash
# ftrace_examples.sh — примеры трассировки функций ядра через ftrace
#
# Что такое ftrace:
#   ftrace — встроенный в ядро Linux механизм трассировки.
#   В отличие от strace, он показывает уже не syscalls userspace, а то,
#   какие функции ядра реально выполняются.
#
# Для этого проекта ftrace полезен, чтобы:
#   - увидеть вызовы lkm_debug_init(), lkm_proc_show(), lkm_slow_work();
#   - посмотреть порядок вызовов функций;
#   - оценить вложенность через function_graph tracer.
#
# Требования:
#   - root-права;
#   - смонтированный debugfs (часто уже смонтирован дистрибутивом):
#       mount -t debugfs none /sys/kernel/debug

# ftrace_examples.sh — запуск от root: ./ftrace_examples.sh

# Параметры вывода ftrace:
# Формат строки:
#   TASK-PID   CPU#  FLAGS   TIMESTAMP: FUNCTION <- CALLER
#
# TASK    — имя процесса (здесь: cat)
# PID     — идентификатор процесса
# CPU#    — ядро, на котором выполнялась функция
# FLAGS   — флаги иркрежима/прерываний
# TIMESTAMP — время с момента загрузки (cekundy.nanos)
# FUNCTION <- CALLER: вызванная функция <- откуда вызвана

# Сценарии и ожидаемый вывод:
# cat /proc/...       lkm_proc_show -> lkm_slow_work
# ioctl PING          lkm_open -> lkm_ioctl -> lkm_release
# ioctl SLOW          lkm_open -> lkm_ioctl -> lkm_slow_work -> lkm_release
# write               lkm_open -> lkm_write -> lkm_release

[ "$(id -u)" != "0" ] && { echo "Run as root: sudo bash $0"; exit 1; }

set -euo pipefail
DEBUG=/sys/kernel/debug/tracing
IOCTL=../userspace/ioctl_test

FUNCS="del_test cre_test"

tr_write() { echo "$1" > "${DEBUG}/$2"; }
tr_clear()  { : > "${DEBUG}/trace"; }     # : > file — самый быстрый способ очистки
tr_off()    { tr_write 0 tracing_on; }
tr_on()     { tr_write 1 tracing_on; }
tr_show()   { grep -v '^#' "${DEBUG}/trace" | head -"${1:-20}"; }

cleanup() {
    tr_off 2>/dev/null || true
    tr_write nop current_tracer 2>/dev/null || true
    : > "${DEBUG}/set_ftrace_filter" 2>/dev/null || true
    echo "ftrace: cleanup done"
}
trap cleanup EXIT

# Загрузка модуля
# if ! lsmod | grep -q lkm_debug; then
#     insmod ../lkm_debug.ko && echo "[info] lkm_debug loaded"
# else
#     echo "[info] lkm_debug already loaded"
# fi

# tr_write 4096 buffer_size_kb
# echo "[info] buffer = 4096 KB"

# # Диагностика перед запуском
# echo
# echo "=== Диагностика ==="
# echo -n "/dev/lkm_debug: "
# ls -la /dev/lkm_debug 2>/dev/null || echo "NOT FOUND"
# echo -n "ioctl_test: "
# ls -la "${IOCTL}" 2>/dev/null || echo "NOT FOUND — соберите: cd .. && make"

# # Проверяем, что ioctl_test вообще работает
# echo "--- тестовый запуск ioctl_test ping ---"
# "${IOCTL}" ping && echo "[ok] ioctl_test ping работает" || echo "[FAIL] ioctl_test не работает"

# # Фильтр: только точные имена
# echo
# echo "=== Фильтр ==="
# FIRST=1
# for FN in ${FUNCS}; do
#     if grep -qw "^${FN}" "${DEBUG}/available_filter_functions" 2>/dev/null; then
#         if [ "${FIRST}" = "1" ]; then
#             echo "${FN}" >  "${DEBUG}/set_ftrace_filter"; FIRST=0
#         else
#             echo "${FN}" >> "${DEBUG}/set_ftrace_filter"
#         fi
#         echo "  [ok] ${FN}"
#     fi
# done
# [ "${FIRST}" = "1" ] && { echo "ERROR: нет инструментированных функций"; exit 1; }

#: > /sys/kernel/debug/tracing/set_ftrace_filter

#echo -n close > /sys/kernel/debug/tracing/set_ftrace_filter
#echo -n creat >> /sys/kernel/debug/tracing/set_ftrace_filter
#echo -n unlink >> /sys/kernel/debug/tracing/set_ftrace_filter
tr_write function_graph current_tracer

# Сценарии
echo
echo "tests"
tr_clear
tr_on
./a.out
tr_off
#tr_show 100
cat /sys/kernel/debug/tracing/trace
cleanup

# echo
# echo "=== 2. ioctl_test ping ==="
# tr_clear; tr_on
# "${IOCTL}" ping >/dev/null 2>&1 || true
# tr_off
# tr_show 10

# echo
# echo "=== 3. ioctl_test slow ==="
# tr_clear; tr_on
# "${IOCTL}" slow >/dev/null 2>&1 || true
# tr_off
# tr_show 10

# echo
# echo "=== 4. ioctl_test write ==="
# tr_clear; tr_on
# "${IOCTL}" write "hello ftrace" >/dev/null 2>&1 || true
# tr_off
# tr_show 10

# # Если все сценарии 2-4 пустые — покажем размер буфера и вывод
# # полный контент trace для диагностики
# echo
# echo "=== Доп. диагностика: полный trace после ioctl_test slow ==="
# tr_clear; tr_on
# "${IOCTL}" slow >/dev/null 2>&1 || true
# tr_off
# echo "--- полный буфер (head 30) ---"
# head -30 "${DEBUG}/trace"

# echo
# echo "=== 5. trace_pipe (live) ==="
# echo "  Терминал 1: echo 1 > ${DEBUG}/tracing_on && cat ${DEBUG}/trace_pipe"
# echo "  Терминал 2: cat /proc/lkm_debug"

# echo; echo "=== Готово ==="
