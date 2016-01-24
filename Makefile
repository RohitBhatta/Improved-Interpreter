CFLAGS=-g -std=c99 -O0 -Werror -Wall

p2 : p2.o Makefile
	gcc $(CFLAGS) -o p2 p2.o

%.o : %.c Makefile
	gcc $(CFLAGS) -MD -c $*.c

%.o : %.S Makefile
	gcc $(CFLAGS) -MD -c $*.S

TESTS=$(sort $(wildcard *.test))
RUNS=$(patsubst %.test,%.run,$(TESTS))

test : $(RUNS)

$(RUNS) : %.run : %.test Makefile p2
	@echo -n "[$*] \"`cat $*.test`\" ... "
	@-./p2 "`head -1 $*.test`" > $*.out
	@((diff -b $*.out $*.ok > /dev/null) && echo "pass") || (echo "fail" ; echo "--- expected ---"; cat $*.ok; echo "--- found ---" ; cat $*.out)

clean :
	rm -f *.d
	rm -f *.o
	rm -f p2

-include *.d
