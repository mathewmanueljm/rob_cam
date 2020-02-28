
CFLAGS_W	= -Wall -Wextra -Werror
CFLAGS_O	= -O3 -march=native -flto -fuse-linker-plugin
CFLAGS		= $(CFLAGS_W) $(CFLAGS_O)

all: cam

cam:
	gcc $(CFLAGS) `pkg-config --cflags libalx-base libalx-cv`	\
			cam.c -o cam					\
			`pkg-config --libs libalx-cv libalx-base`

