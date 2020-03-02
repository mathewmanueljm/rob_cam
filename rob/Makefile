
CFLAGS_W	= -Wall -Wextra -Werror -Wno-implicit-fallthrough
CFLAGS_O	= -O3 -march=native -flto -fuse-linker-plugin -static
CFLAGS		= $(CFLAGS_W) $(CFLAGS_O)

.PHONY: all
all: rob

rob: rob.c
	gcc $(CFLAGS) 							\
			`pkg-config --static --cflags libalx-base`	\
			`pkg-config --static --cflags libalx-telnet-tcp`\
			`pkg-config --static --cflags libalx-robot`	\
			rob.c -o rob					\
			`pkg-config --static --libs libalx-robot`	\
			`pkg-config --static --libs libalx-telnet-tcp`	\
			`pkg-config --static --libs libalx-base`

ur_simulator: ur_simulator.c
	gcc $(CFLAGS) 							\
			`pkg-config --static --cflags libalx-base`	\
			`pkg-config --static --cflags libalx-robot`	\
			ur_simulator.c -o ur_simulator			\
			`pkg-config --static --libs libalx-robot`	\
			`pkg-config --static --libs libalx-base`

