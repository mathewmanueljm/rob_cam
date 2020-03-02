
CFLAGS_W	= -Wall -Wextra -Werror -Wno-implicit-fallthrough
CFLAGS_O	= -O3 -march=native -flto -fuse-linker-plugin -static
CFLAGS		= $(CFLAGS_W) $(CFLAGS_O)

.PHONY: all
all: ur-sim

ur-sim: ur-sim.c
	gcc $(CFLAGS) 							\
			`pkg-config --static --cflags libalx-base`	\
			`pkg-config --static --cflags libalx-robot`	\
			$< -o $@					\
			`pkg-config --static --libs libalx-robot`	\
			`pkg-config --static --libs libalx-base`

