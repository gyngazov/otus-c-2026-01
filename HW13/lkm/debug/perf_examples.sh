#!/usr/bin/env bash
# perf_examples.sh — примеры профилирования через perf
#
# Что такое perf:
#   perf — основной инструмент Linux для анализа производительности.
#   Он умеет считать аппаратные события CPU, собирать профили выполнения,
#   строить call graph и помогать находить «горячие» функции.
#
# Для данного LKM perf полезен, чтобы увидеть:
#   - сколько CPU потребляет lkm_slow_work();
#   - какие функции попадают в top / report;
#   - как выглядит вызов модуля на фоне остальных функций ядра.
#
# Предварительные условия:
#   - модуль загружен;
#   - userspace-утилита собрана;
#   - установлен perf (можно собрать из исходников).

# Установка perf (сбор из исходников):
# sudo apt update
# sudo apt install -y git make gcc flex bison \
#   libelf-dev libdw-dev libunwind-dev \
#   libslang2-dev libperl-dev libpython3-dev \
#   libzstd-dev libcap-dev libnuma-dev \
#   libbabeltrace-dev binutils-dev systemtap-sdt-dev pkg-config \
#   python3 python3-dev python-is-python3 \
#   libtraceevent-dev libpfm4-dev pkg-config \
#   python3-setuptools llvm clang libcapstone-dev \
#   libdebuginfod-dev
# Переход в каталог с исходниками ядра
# cd tools/perf
# make clean
# make -j"$(nproc)" PYTHON=python3
# sudo cp perf /usr/local/bin/
# perf --version

set -uo pipefail

TOOL="${1:-../userspace/ioctl_test}"

# Проверка зависимостей
_require() {
    command -v "$1" >/dev/null 2>&1 || { echo "SKIP: '$1' не найден, пропускаем раздел"; return 1; }
}

_require_file() {
    [[ -x "$1" ]] || { echo "SKIP: '$1' не найден или не исполняем. Собери: make -C ../userspace"; return 1; }
}

_require_proc() {
    [[ -r /proc/lkm_debug ]] || { echo "SKIP: /proc/lkm_debug недоступен. Загрузи модуль: sudo insmod ../module/lkm_debug.ko"; return 1; }
}

# Определяем доступность hardware PMU (Performance Monitoring Unit).
# Запускаем perf stat без флагов (точно так, как будет в секции 1)
# и смотрим stderr: при TOPDOWN/Invalid event/not supported — PMU недоступен.
HW_PMU="unknown"
_detect_pmu() {
    if [[ "${HW_PMU}" != "unknown" ]]; then return; fi
    local err
    err=$(perf stat -- true 2>&1 >/dev/null) || true
    if echo "${err}" | grep -qiE "TOPDOWN|Invalid event|not supported"; then
        HW_PMU="no"
    else
        HW_PMU="yes"
    fi
}

echo "=== 1. perf stat — базовые счётчики ==="
#
# perf stat даёт агрегированную статистику выполнения команды.
#
# На физическом железе — hardware PMU: cycles, instructions, и т.д.
# На VM без vPMU — software-события:
#   task-clock       : время CPU (мс);
#   context-switches : переключения контекста;
#   cpu-migrations   : перемещения потока между CPU;
#   page-faults      : страничные ошибки целиком (minor+major).
if _require perf && _require_file "${TOOL}"; then
    _detect_pmu
    if [[ "${HW_PMU}" == "yes" ]]; then
        echo "  [hardware PMU доступен]"
        perf stat "${TOOL}" slow
    else
        echo "  [VM/vCPU без hardware PMU — используем software-события]"
        perf stat -e task-clock,context-switches,cpu-migrations,page-faults \
            "${TOOL}" slow
    fi
fi

echo
echo "=== 2. perf stat — расширенные события ==="
#
# На hardware: аппаратные PMU-счётчики (cycles, instructions, cache-*, branch-*).
# На VM: software-события + minor/major page faults.
#   minor-faults : обработка страниц без I/O (анонимные страницы, стек);
#   major-faults : обработка страниц с диска или свопа.
if _require perf && _require_file "${TOOL}"; then
    _detect_pmu
    if [[ "${HW_PMU}" == "yes" ]]; then
        echo "  [hardware PMU — расширенные аппаратные события]"
        perf stat -e cycles,instructions,cache-references,cache-misses,branch-misses \
            "${TOOL}" slow
    else
        echo "  [VM fallback — software-события + minor/major page faults]"
        perf stat \
            -e task-clock,context-switches,cpu-migrations \
            -e page-faults,minor-faults,major-faults \
            "${TOOL}" slow
    fi
fi

echo
echo "=== 3. perf record + report — CPU profile и call graph ==="
#
# perf record собирает профиль, а perf report показывает его в удобном виде.
#
# -g --call-graph dwarf : собирать граф вызовов через DWARF unwind.
#   На VM с малым числом семплов (< 20) вывод может быть скудным —
#   это нормально для синтетической нагрузки.
#
# Результат сохраняется в /tmp/perf_lkm.data.
if _require perf && _require_file "${TOOL}"; then
    perf record -g --call-graph dwarf -o /tmp/perf_lkm.data -- "${TOOL}" slow
    echo "--- top functions ---"
    perf report -i /tmp/perf_lkm.data --stdio | head -40
fi

echo
echo "=== 4. perf record — только kernel-функции ==="
#
# Здесь интерес смещён на активность ядра.
#
# -a              : system-wide (все CPU)
# -e cpu-clock:k  : событие cpu-clock только для kernel context (:k)
# -o FILE         : файл должен идти ПОСЛЕ опций, ДО команды (через --)
#
# В качестве нагрузки несколько раз читается /proc/lkm_debug.
if _require perf && _require_proc; then
    perf record -g -a -e cpu-clock:k -o /tmp/perf_kernel.data -- \
        sh -c 'for i in 1 2 3; do cat /proc/lkm_debug; done' || true
    echo "  Saved to /tmp/perf_kernel.data. View with:  perf report -i /tmp/perf_kernel.data --stdio"
fi

echo
echo "=== 5. perf record — hot path систем-wide + отчёт ==="
#
# perf top — интерактивный TUI-инструмент; в неинтерактивном режиме
# (скрипт, CI, pipe) он неизбежно печатает справку "Mapped keys".
# Вместо этого используем эквивалентный подход:
#   1) perf record -a собирает семплы всех CPU за 5 с;
#   2) в фоне циклически читается /proc/lkm_debug;
#   3) perf report показывает топ-символы статически.
#
# Это полный эквивалент perf top без TUI-проблем.
if _require perf && _require_proc; then
    echo "  Collecting 5s system-wide profile while reading /proc/lkm_debug ..."
    (
        for i in $(seq 1 100); do
            cat /proc/lkm_debug >/dev/null 2>&1 || break
            sleep 0.05
        done
    ) &
    BG=$!
    perf record -a -g -e cpu-clock -o /tmp/perf_top.data -- sleep 5 2>/dev/null || true
    wait "${BG}" 2>/dev/null || true
    echo "--- top symbols (system-wide, 5s) ---"
    perf report -i /tmp/perf_top.data --stdio --no-children -s symbol | \
        grep -v '^#' | grep -v '^$' | head -25 || true
fi

echo
echo "=== 6. Генерация flame graph ==="
#
# Сам perf не строит flame graph напрямую, но умеет собрать данные,
# которые затем можно конвертировать через инструменты от Brendan Gregg.
#
# Общая схема:
#   1) perf record собирает семплы со стеком;
#   2) perf script превращает их в текстовое представление;
#   3) stackcollapse-perf.pl схлопывает стеки;
#   4) flamegraph.pl рисует SVG.
#
# Быстрый способ получить flame graph прямо сейчас:
if _require perf && _require_proc; then
    FLAMEDIR="/tmp/FlameGraph"
    if [[ -d "${FLAMEDIR}" ]]; then
        perf record -F 99 -g -a -o /tmp/perf_flame.data -- \
            sh -c 'for i in $(seq 1 10); do cat /proc/lkm_debug >/dev/null; done' || true
        perf script -i /tmp/perf_flame.data | \
            "${FLAMEDIR}/stackcollapse-perf.pl" > /tmp/out.folded
        "${FLAMEDIR}/flamegraph.pl" /tmp/out.folded > /tmp/flamegraph.svg
        echo "  Flame graph saved: /tmp/flamegraph.svg"
    else
        echo "  FlameGraph tools not found at ${FLAMEDIR}."
        echo "  Install with:"
        echo "    git clone https://github.com/brendangregg/FlameGraph ${FLAMEDIR}"
        echo "  Then re-run this script."
        echo
        echo "  Manual steps:"
        echo "    1)  perf record -F 99 -g -a -- sleep 5  (while running workload)"
        echo "    2)  perf script | stackcollapse-perf.pl > out.folded"
        echo "    3)  flamegraph.pl out.folded > flamegraph.svg"
    fi
fi
