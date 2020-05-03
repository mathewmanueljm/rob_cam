
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
			$< -o $@					\
			`pkg-config --static --libs libalx-robot`	\
			`pkg-config --static --libs libalx-telnet-tcp`	\
			`pkg-config --static --libs libalx-base`

clean:
	rm -f rob
