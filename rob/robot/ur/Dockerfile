###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

FROM	debian:stable	AS build
RUN	apt-get update							&& \
	apt-get upgrade -V --yes					&& \
	apt-get install -V \
			gcc \
			make \
			git \
			pkg-config \
			libbsd-dev \
			deborphan \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get purge $(deborphan) --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
WORKDIR	/tmp
RUN	git clone https://github.com/alejandro-colomar/libalx.git	&& \
	make	base telnet-tcp robot	-C libalx	-j 8		&& \
	make	install-base		-C libalx			&& \
	make	install-telnet-tcp	-C libalx			&& \
	make	install-robot		-C libalx			&& \
	rm -rf	libalx
RUN	git clone --single-branch --branch rob \
		https://github.com/alejandro-colomar/rob_cam.git	&& \
	make	-C rob_cam/rob/robot/ur/	-j 2

FROM	debian:stable
WORKDIR	/app
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			telnet \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
COPY	--from=build /tmp/rob_cam/rob/robot/ur/ur-sim ./
RUN	chmod +x ./ur-sim
CMD	["./ur-sim"]

# docker container run --tty --network rob_cam_network --name robot --rm alejandrocolomar/rob_cam:ur-sim 

