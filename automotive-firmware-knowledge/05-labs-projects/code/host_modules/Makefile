CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Wconversion -Wsign-conversion -O2 -g

all: test_modules

test_modules: modules.c test_modules.c modules.h
	$(CC) $(CFLAGS) modules.c test_modules.c -o $@

test: test_modules
	./test_modules

# Coverage: make coverage && xem modules.c.gcov
coverage:
	$(CC) -std=c99 -O0 --coverage modules.c test_modules.c -o test_cov && ./test_cov && gcov -b modules.c

# Static analysis (cần cppcheck)
lint:
	cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem modules.c

clean:
	rm -f test_modules test_cov *.gcda *.gcno *.gcov

.PHONY: all test coverage lint clean
