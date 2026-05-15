# Makefile для userspace-утилиты ioctl_test
#
# Здесь всё максимально просто:
#   - CC      : компилятор C
#   - CFLAGS  : флаги сборки
#   - TARGETS : список исполняемых файлов, которые нужно собрать
#
# Почему включены -Wall -Wextra -g:
#   -Wall / -Wextra помогают заранее увидеть подозрительные места в коде;
#   -g добавляет отладочную информацию, полезную для gdb/strace/perf.

CC = gcc
CFLAGS = -Wall -Wextra -g
TARGETS = ioctl_test

# Цель по умолчанию: собрать все исполняемые файлы из TARGETS.
all: $(TARGETS)

# Шаблонное правило:
#   из файла foo.c собирается бинарник foo.
# $@ — имя цели
# $< — первый prerequisite
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Удаление собранных бинарников.
clean:
	rm -f $(TARGETS)
