FROM	alejandrocolomar/libalx:dev
RUN	git clone https://github.com/alejandro-colomar/rob_cam_test.git	&& \
	make	-C rob_cam_test	-j 2

    2  apt-get update
    3  apt-get upgrade
    6  cat > cam.c
    7  gcc -Wall -Werror -Wextra -O3 -march=native -flto -fuse-linker-plugin -static $(pkg-config --static --cflags libalx-base) cam.c -o cam $(pkg-config --static --libs libalx-base)

