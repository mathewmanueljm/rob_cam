
CFLAGS_W	= -Wall -Wextra -Werror -Wno-implicit-fallthrough
CFLAGS_O	= -O3 -march=native -flto -fuse-linker-plugin -static
CFLAGS		= $(CFLAGS_W) $(CFLAGS_O)

all: rob

rob: rob.c
	gcc $(CFLAGS) `pkg-config --static --cflags libalx-base libalx-telnet-tcp` \
			rob.c -o rob					\
			`pkg-config --static --libs libalx-telnet-tcp libalx-base`

