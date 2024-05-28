CC=gcc# GNU Compiler Collection
CFLAGS=-c -Wall -Wextra -Werror# -std=c11 -c - compile without linking 
LC=lcov -c -d . -o# graphical gcov frontend
# -t <name> sets the name of the report, when measuring code coverage with tests, you can specify the
# name of the test or test suite
# -o <name> sets output file name with intermediate information
# -c specifies that lcov should use existing coverage data
# -d <path> sets the directory in which to look for coverage data, and we pass the current directory “.”
AFLAGS=ar rc# ar - archive
# -c Suppresses the message normally printed when ar creates a new archive file -r
# -r Replaces or adds file to archive. If archive does not exist, ar creates it and prints a message.
GCOVFLAGS=--coverage #-fprofile-arcs -ftest-coverage # must be used to build all source files for which
# we are going to request code coverage reports.

SOURCES=$(wildcard *.c)# allows use regexp
TESTS=$(wildcard ./tests/*test.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))# allows change initial string to provided
TESTOBJ=$(patsubst %.c, %.o, $(TESTS))
LIBSRC=$(wildcard s21_*.c)
LIBOBJ=$(patsubst %.c, %.o, $(LIBSRC))

ifeq ($(shell uname -s),Linux)
OPEN_CMD=xdg-open
OFLAGS=-lcheck -lsubunit -lm -lrt -lpthread -lgcov -o 
endif
ifeq ($(shell uname -s),Darwin) # MacOS
OPEN_CMD=open
OFLAGS=-lcheck -lm -lpthread -o
endif

build: all

all: s21_decimal.a $(SOURCES) test style_check gcov_report

%.o: %.c
	$(CC) $(CFLAGS) $< $(GCOVFLAGS) -o $@

s21_test%.o: $(TESTS)
	$(CC) $(CFLAGS) $< $(GCOVFLAGS) -o $@

test: $(OBJECTS) $(TESTOBJ)
	$(CC) $(OBJECTS) $(TESTOBJ) $(OFLAGS) $@ --coverage
	./test

retest: clean test

s21_decimal.a:
	$(CC) $(CFLAGS) $(LIBSRC)
	$(AFLAGS) s21_decimal.a $(LIBOBJ)
	rm -rf *.o

gcov_report:
	make test
	./test
	$(LC) report.info
	genhtml -o report report.info
	$(OPEN_CMD) report/index.html

style:
	clang-format --style=Google -i *.c *.h

style_check:
	clang-format -style=google -i *.c
	clang-format -style=google -i ./tests/*.c
	clang-format -style=google -i *.h

debug_help:
	cp ./debug/main.c ./main.c
	$(CC) -g main.c s21_arithmetic.c s21_helper.c s21_comparison.c s21_another.c s21_convertors.c

mini_verter:
	@cd ../materials/build && sh run.sh

clean:
	rm -rf *.o *.a *.gcno *.gcda *.info *.html *.css *.png *.out.dSYM main.c *.out
	rm -rf ./tests/*.o ./tests/*.gcda ./tests/*.gcno
	rm -rf test src

clean+: clean
	rm -r report

rebuild: clean all
