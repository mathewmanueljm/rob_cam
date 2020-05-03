###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

FROM	debian@sha256:21a1267db36931b0a89f1fece2e54afa78438951fcaadd529837b7def15d04de \
			AS build
RUN	apt-get update							&& \
	apt-get upgrade -V --yes					&& \
	apt-get install -V \
			gcc \
			gcc-10 \
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
	    --branch version-0.5.6-a						\
	    https://github.com/alejandro-colomar/rob_cam.git		&& \
	make	-C rob_cam/rob/	-j 2

FROM	debian@sha256:21a1267db36931b0a89f1fece2e54afa78438951fcaadd529837b7def15d04de
WORKDIR	/app
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			telnet \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
COPY	--from=build /tmp/rob_cam/rob/rob ./
RUN	chmod +x ./rob
CMD	["./rob"]

# docker container run --tty --interactive --rm --name rob --network rob_cam_network --env ROBOT_TYPE=ur --env ROBOT_ADDR=robot --env ROBOT_PORT=30002 --env ROBOT_USER= --env ROBOT_PASSWD= --env ROBOT_STATUS_FNAME=status.log --env ROB_PORT=13100 --env ROB_CAMS_MAX=10 --env DELAY_LOGIN=1000000 --env DELAY_US=100000 alejandrocolomar/rob_cam:rob  

