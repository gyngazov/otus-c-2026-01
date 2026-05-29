# lkm_debug

Linux Kernel Module (LKM) с userspace-утилитой и готовыми примерами отладки/трассировки через:

| Tool | Что показывает |
|---|---|
| **strace** | системные вызовы userspace <-> модуль |
| **ftrace** | трассировку kernel-функций и call graph |
| **perf** | профилирование CPU, счётчики, flame graph |
| **bpftrace** | eBPF probes, latency histogram, счётчики вызовов |

---

## Интерфейсы LKM

Здесь есть два основных интерфейса модуля:

1. `/proc/lkm_debug`
   - обычное чтение `cat /proc/lkm_debug` запускает внутри ядра медленную функцию `lkm_slow_work()`;
   - удобно для быстрых экспериментов с `ftrace`, `perf`, `bpftrace`.

2. `/dev/lkm_debug`
   - device, через который userspace-утилита делает `ioctl()` и `write()`;
   - удобно для `strace` и для сценариев userspace -> kernelspace.

В модуле есть специально сделанный «медленный путь» `lkm_slow_work(iterations)`, который не решает прикладную задачу, а нужен для того, чтобы его было хорошо видно в профилировщиках и трассировщиках.

---

## Установка зависимостей

```bash
sudo apt install build-essential linux-headers-$(uname -r) linux-perf bpftrace strace
```

При необходимости для flame graph также понадобятся скрипты из проекта [Brendan Gregg FlameGraph](https://github.com/brendangregg/FlameGraph).

---

## Сборка

### 1. Собрать модуль ядра

```bash
make
```

### 2. Собрать userspace-утилиту

```bash
cd userspace && make && cd ..
```

---

## Загрузка и проверка

### Загрузить модуль

```bash
sudo insmod lkm_debug.ko
dmesg | tail -5
```

Ожидается сообщение вида:

```text
lkm_debug: module loaded. /dev/lkm_debug  /proc/lkm_debug
```

### Минимальный smoke test

```bash
cat /proc/lkm_debug
sudo ./userspace/ioctl_test ping
sudo ./userspace/ioctl_test slow
echo "hello" | sudo tee /dev/lkm_debug
```

### Проверить логи ядра

```bash
dmesg | tail -20
```

---

## Работа с `/proc/lkm_debug`

Команда:

```bash
cat /proc/lkm_debug
```

Что произойдёт:

- ядро вызовет `lkm_proc_show()`;
- внутри неё будет вызвана `lkm_slow_work(100000)`;
- в stdout вернутся строки `calling slow_work()` и `done`.

Этот путь особенно удобен, когда нужно многократно и быстро триггерить код модуля без отдельной программы.

---

## Работа с `/dev/lkm_debug`

### Простой ioctl

```bash
sudo ./userspace/ioctl_test ping
```

### Медленный ioctl

```bash
sudo ./userspace/ioctl_test slow
```

### Передача строки через write

```bash
sudo ./userspace/ioctl_test write "hello kernel"
```

или

```bash
echo "hello kernel" | sudo tee /dev/lkm_debug
```

---

## Примеры `strace`

`strace` показывает системные вызовы процесса в userspace. Он особенно полезен, чтобы увидеть `openat()`, `ioctl()`, `write()`, `close()` и измерить время syscall'ов.

### Все системные вызовы

```bash
strace -f sudo ./userspace/ioctl_test ping
```

### Только интересующие syscalls

```bash
strace -e trace=ioctl,openat,close sudo ./userspace/ioctl_test slow
```

### Сводная статистика по syscall

```bash
strace -c sudo ./userspace/ioctl_test slow
```

### Временные метки и длительность

```bash
strace -T -tt sudo ./userspace/ioctl_test ping
```

Готовый файл с примерами: `debug/strace_examples.sh`

---

## Примеры `ftrace`

`ftrace` показывает уже не syscalls, а функции ядра. Это один из лучших способов увидеть, как именно выполняется код LKM.

### function_graph для `lkm_slow_work`

```bash
DEBUG=/sys/kernel/debug/tracing
sudo bash -c "echo function_graph > $DEBUG/current_tracer"
sudo bash -c "echo lkm_slow_work > $DEBUG/set_graph_function"
sudo bash -c "echo 1 > $DEBUG/tracing_on"
cat /proc/lkm_debug
sudo bash -c "echo 0 > $DEBUG/tracing_on"
sudo cat $DEBUG/trace | head -60
```

### function tracer с фильтром `lkm_*`

```bash
sudo bash -c "echo function > $DEBUG/current_tracer"
sudo bash -c "echo 'lkm_*' > $DEBUG/set_ftrace_filter"
sudo bash -c "echo > $DEBUG/trace"
sudo bash -c "echo 1 > $DEBUG/tracing_on"
cat /proc/lkm_debug
sudo bash -c "echo 0 > $DEBUG/tracing_on"
sudo cat $DEBUG/trace
```

### Живой поток событий

```bash
sudo cat $DEBUG/trace_pipe
```

В другом терминале:

```bash
cat /proc/lkm_debug
```

Готовый файл с примерами: `debug/ftrace_examples.sh`

---

## Примеры `perf`

`perf` нужен для анализа производительности и CPU hot path.

### Базовые счётчики

```bash
sudo perf stat ./userspace/ioctl_test slow
```

### Расширенные аппаратные события

```bash
sudo perf stat -e cycles,instructions,cache-references,cache-misses,branch-misses \
     ./userspace/ioctl_test slow
```

### Профиль + call graph

```bash
sudo perf record -g --call-graph dwarf -o /tmp/lkm.data \
     ./userspace/ioctl_test slow
sudo perf report -i /tmp/lkm.data --stdio | head -40
```

### Системный профилинг ядра

```bash
sudo perf record -g -a --kernel -e cpu-clock:k -- \
     sh -c 'cat /proc/lkm_debug; cat /proc/lkm_debug; cat /proc/lkm_debug'
```

### Живой просмотр hot functions

```bash
sudo perf top -s symbol
```

Готовый файл с примерами: `debug/perf_examples.sh`

---

## Примеры `bpftrace`

`bpftrace` позволяет без пересборки модуля цепляться к функциям ядра и tracepoint'ам.

### Все функции `lkm_*`

```bash
sudo bpftrace -e 'kprobe:lkm_* { printf("%s called by PID %d\n", probe, pid); }'
```

### Гистограмма latency для `lkm_slow_work`

```bash
sudo bpftrace -e '
  kprobe:lkm_slow_work    { @s[tid] = nsecs; }
  kretprobe:lkm_slow_work /@s[tid]/ {
    @lat_us = hist((nsecs-@s[tid])/1000);
    delete(@s[tid]);
  }'
```

### Трассировка ioctl syscalls

```bash
sudo bpftrace -e '
  tracepoint:syscalls:sys_enter_ioctl
  /comm == "ioctl_test"/ {
    printf("PID %d  cmd=0x%x\n", pid, args->cmd);
  }'
```

### Готовый сценарий

```bash
sudo bpftrace debug/lkm_trace.bt
```

Готовые примеры: `debug/bpftrace_examples.sh`  
Готовый сценарий: `debug/lkm_trace.bt`

---

## Выгрузка модуля

```bash
sudo rmmod lkm_debug
dmesg | tail -3
```

---

## Что можно изучать на основе этого проекта

- жизненный цикл LKM: `init` / `exit`;
- интерфейсы ядра: `procfs`, `misc device`, `ioctl`, `write`;
- безопасную передачу данных из userspace в kernelspace через `copy_from_user()`;
- различие между:
  - `strace` — системные вызовы userspace;
  - `ftrace` — функции ядра;
  - `perf` — профилирование производительности;
  - `bpftrace` — динамическая eBPF-трассировка.

---

## Требования

- требуется поддержка `CONFIG_FTRACE`, `CONFIG_BPF`, `CONFIG_DEBUG_FS`
