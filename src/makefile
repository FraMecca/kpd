ALL = parse_args.c main.c util_mpd_handler.c util_datastructs.c 

all:
		gcc $(ALL) -Wall  -o ../build/main -lgc -lm -lmpdclient
clear_o:
		rm ../build/*.o
clear_all: clear_o
		rm ../build/*
debug:
		gcc $(ALL) -Wall -g -o ../build/main -lgc -lm -lmpdclient
asan:
		gcc $(ALL) -fsanitize=address -Wall -g -o ../build/main -lgc -lm -lmpdclient
gprof:
		gcc $(ALL) -Wall  -o ../build/main -lgc -lm -lmpdclient -pg
		../build/main
