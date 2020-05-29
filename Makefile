
CFLAGS_W	= -Wall -Wextra -Werror -Wno-implicit-fallthrough
CFLAGS_O	= -O3 -march=native -flto -fuse-linker-plugin
CFLAGS		= $(CFLAGS_W) $(CFLAGS_O)

ALL		= bin/cam bin/rob bin/ur-sim
all: $(ALL)

bin/cam: src/cam/cam.c
	gcc-10 $(CFLAGS)						\
			`pkg-config --cflags libalx-base libalx-cv`	\
			$< -o $@					\
			`pkg-config --libs libalx-cv libalx-base`

bin/rob: src/rob/rob.c
	gcc $(CFLAGS) -static						\
			`pkg-config --static --cflags libalx-base`	\
			`pkg-config --static --cflags libalx-telnet-tcp`\
			`pkg-config --static --cflags libalx-robot`	\
			$< -o $@					\
			`pkg-config --static --libs libalx-robot`	\
			`pkg-config --static --libs libalx-telnet-tcp`	\
			`pkg-config --static --libs libalx-base`

bin/ur-sim: src/robot/ur-sim/ur-sim.c
	gcc-10 $(CFLAGS) -static					\
			`pkg-config --static --cflags libalx-base`	\
			`pkg-config --static --cflags libalx-robot`	\
			$< -o $@					\
			`pkg-config --static --libs libalx-robot`	\
			`pkg-config --static --libs libalx-base`


clean:
	rm -f $(ALL)
