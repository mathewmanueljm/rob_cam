###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

##	debian:latest
FROM	debian@sha256:18f374fa88f9230a5c860f986dc83d3d03a056708164ff30c98d0e789e90946d \
			AS build
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
RUN	git clone							\
	    --single-branch						\
	    --branch v1-b18.3						\
	    https://github.com/alejandro-colomar/libalx.git		&& \
	make	base telnet-tcp robot		-C libalx	-j 8	&& \
	make	install-base			-C libalx		&& \
	make	install-telnet-tcp		-C libalx		&& \
	make	install-robot			-C libalx		&& \
	rm -rf	libalx
RUN	git clone							\
	    --single-branch						\
	    --branch version-0.5					\
	    https://github.com/alejandro-colomar/rob_cam.git		&& \
	make	bin/rob			-C rob_cam

##	debian:latest
FROM	debian@sha256:18f374fa88f9230a5c860f986dc83d3d03a056708164ff30c98d0e789e90946d
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			telnet \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
COPY	--from=build /tmp/rob_cam/bin/rob /usr/local/bin/rob
CMD	["rob"]

# docker container run --tty --interactive --rm --name rob --network rob_cam_network --env ROBOT_TYPE=ur --env ROBOT_ADDR=robot --env ROBOT_PORT=30002 --env ROBOT_USER= --env ROBOT_PASSWD= --env ROBOT_STATUS_FNAME=status.log --env ROB_PORT=13100 --env ROB_CAMS_MAX=10 --env DELAY_LOGIN=1000000 --env DELAY_US=100000 alejandrocolomar/rob_cam:rob_0.5.6  

