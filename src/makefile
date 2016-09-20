ALL = parse_args.c main.c util.c

all:
		gcc $(ALL) -Wall  -o ../build/main
clear_o:
		rm ../build/*.o
clear_all: clear_o
		rm ../build/*
debug:
		#gcc $(ALL) -Wno-pointer-sign -Wall -g -o ../build/main
		gcc $(ALL) -Wall -g -o ../build/main
asan:
		gcc $(ALL) -Wno-pointer-sign -Wall -g -fsanitize=address -lcrypto -o ../build/main
clang:
		clang $(ALL) -Wno-pointer-sign -Wall -g -lcrypto -o ../build/main
