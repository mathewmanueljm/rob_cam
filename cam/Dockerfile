###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

FROM	debian:testing	AS build
RUN	apt-get update							&& \
	apt-get upgrade -V --yes					&& \
	apt-get install -V \
			gcc-10 \
			g++-10 \
			make \
			git \
			pkg-config \
			libbsd-dev \
			libgsl-dev \
			libopencv-dev \
			deborphan \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get purge $(deborphan) --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
WORKDIR	/tmp
RUN	git clone https://github.com/alejandro-colomar/libalx.git	&& \
	make	base cv				-C libalx	-j 8	&& \
	make	install-base install-cv		-C libalx
RUN	git clone --single-branch --branch cam \
		https://github.com/alejandro-colomar/rob_cam.git	&& \
	make	-C rob_cam/cam/	-j 2

FROM	debian:testing
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			make \
			libc6 \
			libstdc++6 \
			libbsd0 \
			libgsl23 \
			libgslcblas0 \
			libopencv-core4.2 \
			libopencv-videoio4.2 \
			libopencv-dev \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
WORKDIR	/tmp
COPY	--from=build /tmp/libalx ./libalx
RUN	make	install-base install-cv		-C libalx
WORKDIR	/app
COPY	--from=build /tmp/rob_cam/cam/cam ./
RUN	chmod +x ./cam
CMD	["./cam"]

# docker container run --tty --interactive --rm --name cam --network rob_cam_network --device="/dev/video0:/dev/video0" --env ROB_ADDR=rob --env ROB_PORT=13100 --env CAMERA_IDX=0 --env DELAY_US=10000 alejandrocolomar/rob_cam:cam  

