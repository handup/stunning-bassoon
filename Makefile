export PKG_CONFIG_PAH=./raylib/lib/pkgconfig
PKGS=raylib
CFLAGS=-Wall -Wextra -ggdb -pedantic -std=c11 `pkg-config $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)`

probe: main.c
	$(CC) $(CFLAGS) -o probe main.c $(LIBS) -lm
