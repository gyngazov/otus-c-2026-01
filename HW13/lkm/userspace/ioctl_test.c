/*
 * ioctl_test.c — userspace-утилита для работы с /dev/lkm_debug
 *
 * Назначение программы:
 *   Это маленький тестовый инструмент из userspace, который позволяет
 *   удобно дёргать интерфейс kernel-модуля без написания сложного кода.
 *
 * Через него можно:
 *   1) отправить простой ioctl PING;
 *   2) отправить ioctl SLOW, который запускает медленный путь
 *      в модуле и хорошо виден в perf/ftrace/bpftrace;
 *   3) записать строку в /dev/lkm_debug через write().
 *
 * Почему этот файл важен:
 *   - для strace он показывает системные вызовы userspace;
 *   - для perf/ftrace/bpftrace он создаёт удобную нагрузку на модуль;
 *   - его можно вызывать прямо из shell и скриптов.
 *
 * Сборка:
 *   gcc -o ioctl_test ioctl_test.c
 *
 * Примеры запуска:
 *   ./ioctl_test ping
 *   ./ioctl_test slow
 *   ./ioctl_test write "hello kernel"
 */

#include <stdio.h>      /* printf, fprintf, perror */
#include <stdlib.h>     /* exit */
#include <string.h>     /* strcmp, strlen */
#include <fcntl.h>      /* open, O_RDWR */
#include <unistd.h>     /* close, write */
#include <sys/ioctl.h>  /* ioctl, _IO */

/*
 * DEVICE:
 *   путь к устройству, которое создаёт модуль ядра.
 *   После загрузки модуля ожидается наличие файла /dev/lkm_debug.
 *
 * IOCTL_*:
 *   эти значения должны совпадать с определениями в lkm_debug.c,
 *   иначе userspace и kernelspace будут «говорить на разных языках».
 */
#define DEVICE   "/dev/lkm_debug"
#define IOCTL_MAGIC 'D'
#define IOCTL_PING  _IO(IOCTL_MAGIC, 1)
#define IOCTL_SLOW  _IO(IOCTL_MAGIC, 2)

/*
 * usage()
 *
 * Печатает краткую инструкцию по использованию программы и завершает её.
 * Вызывается, если пользователь запустил утилиту без аргументов или
 * передал неизвестную команду.
 */
static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s <ping|slow|write <msg>>\n", prog);
	exit(1);
}

int main(int argc, char *argv[])
{
	/*
	 * argc < 2 означает, что после имени программы не передали команду.
	 * Например, пользователь просто запустил:
	 *   ./ioctl_test
	 */
	if (argc < 2)
		usage(argv[0]);

	/*
	 * Открываем устройство на чтение/запись.
	 *
	 * Почему O_RDWR:
	 *   хотя в этом примере мы в основном используем ioctl() и write(),
	 *   двусторонний режим — самый простой и универсальный для тестов.
	 *
	 * Если open() не удался, скорее всего:
	 *   - модуль не загружен;
	 *   - /dev/lkm_debug не создан;
	 *   - не хватает прав доступа.
	 */
	int fd = open(DEVICE, O_RDWR);
	if (fd < 0) {
		perror("open " DEVICE);
		return 1;
	}

	/*
	 * Разбор первой команды.
	 * argv[1] — это подкоманда: ping / slow / write.
	 */
	if (strcmp(argv[1], "ping") == 0) {
		/*
		 * ioctl(fd, IOCTL_PING)
		 *
		 * Отправляем в модуль простую команду без дополнительных данных.
		 * Она нужна для smoke test: проверить, что ioctl-путь вообще работает.
		 *
		 * Это особенно полезно для strace:
		 *   strace -e trace=ioctl ./ioctl_test ping
		 */
		if (ioctl(fd, IOCTL_PING) < 0)
			perror("ioctl PING");
		else
			printf("PING ok\n");

	} else if (strcmp(argv[1], "slow") == 0) {
		/*
		 * ioctl(fd, IOCTL_SLOW)
		 *
		 * Эта команда запускает в ядре медленную функцию lkm_slow_work().
		 * Именно этот путь удобно профилировать:
		 *   - perf stat / perf record;
		 *   - ftrace function_graph;
		 *   - bpftrace kprobe/kretprobe.
		 */
		if (ioctl(fd, IOCTL_SLOW) < 0)
			perror("ioctl SLOW");
		else
			printf("SLOW ok\n");

	} else if (strcmp(argv[1], "write") == 0) {
		/*
		 * Команда write требует второй аргумент — текст сообщения.
		 * Например:
		 *   ./ioctl_test write "hello kernel"
		 */
		if (argc < 3)
			usage(argv[0]);

		/*
		 * strlen() считает длину строки без завершающего '\0'.
		 * Именно это количество байтов будет передано в write().
		 */
		size_t len = strlen(argv[2]);

		/*
		 * write(fd, argv[2], len)
		 *
		 * Передаём строку в модуль через файловый интерфейс устройства.
		 * Внутри ядра её принимает lkm_write(), которая использует
		 * copy_from_user() и пишет содержимое в dmesg.
		 */
		if (write(fd, argv[2], len) < 0)
			perror("write");
		else
			printf("write ok\n");

	} else {
		/*
		 * Неизвестная команда — выводим usage и завершаемся.
		 */
		usage(argv[0]);
	}

	/*
	 * Закрываем файловый дескриптор устройства.
	 * Это вызовет release()-обработчик в модуле, если он определён.
	 */
	close(fd);
	return 0;
}
