ALL = src/main.c src/util_mpd_handler.c src/util_datastructs.c  src/kpd_search.c
DEBUG = -g -Wall
LFLAGS = -lz -lm -lmpdclient
INSTALL_PATH=/usr/local

all:
		gcc $(ALL) -o build/main $(LFLAGS)
clear_o:
		rm ../build/*.o
clear_all: clear_o
		rm ../build/*
debug:
		gcc $(ALL) -o build/main $(LFLAGS) $(DEBUG)
asan:
		gcc $(ALL) -fsanitize=address $(DEBUG) $(LFLAGS)-o build/main 
gprof:
		gcc $(ALL) -Wall  -o build/main $(LFLAGS) -pg 
install:
	cp build/main $(INSTALL_PATH)/bin/kpd
	cp man.1 $(INSTALL_PATH)/share/man/man1/kpd.1
uninstall:
	rm $(INSTALL_PATH)/bin/kpd
	rm $(INSTALL_PATH)/share/man/man1/kpd.1
