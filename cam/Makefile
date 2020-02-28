
CFLAGS_W	= -Wall -Wextra -Werror
CFLAGS_O	= -O3 -march=native -flto -fuse-linker-plugin -static
CFLAGS		= $(CFLAGS_W) $(CFLAGS_O)

all: cam

cam:
	gcc $(CFLAGS) `pkg-config --static --cflags libalx-base`	\
			cam.c -o cam					\
			`pkg-config --static --libs libalx-base`

