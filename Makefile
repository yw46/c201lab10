CC = g++
CFLAGS = -Wall -Wextra -Wconversion -O3
#CFLAGS = -Wall -Wextra -Wconversion -g 
LIBS = -lpthread

# Note: -O3 in CFLAGS turns on code optimization: the compiler will
# make changes to our code to make it faster but produce the same
# result.  If you need to debug your code, uncomment the second
# CFLAGS line.  This disables optimization, and turns on the -g
# debugger flag.  Change this back once you get your code working.

all: primes

clean:
	rm -f *.o primes

primes: primes.c
	$(CC) $(CFLAGS) -o primes primes.c $(LIBS)
