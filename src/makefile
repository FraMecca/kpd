ALL = parse_args.c main.c util.c

all:
		gcc $(ALL) -Wall  -o ../build/main -lgc
clear_o:
		rm ../build/*.o
clear_all: clear_o
		rm ../build/*
debug:
		gcc $(ALL) -Wall -g -o ../build/main -lgc
asan:
		gcc $(ALL) -fsanitize=address -Wall -g -o ../build/main -lgc
