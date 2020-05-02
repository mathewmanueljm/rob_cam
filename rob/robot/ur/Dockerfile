###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

FROM	debian@sha256:e6a6f2625ec46aa6ce5c537208565cde16138e7963c341ff2a3ecbf9a6736060 \
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
RUN	git clone https://github.com/alejandro-colomar/libalx.git	&& \
	make	base telnet-tcp robot	-C libalx	-j 8		&& \
	make	install-base		-C libalx			&& \
	make	install-telnet-tcp	-C libalx			&& \
	make	install-robot		-C libalx			&& \
	rm -rf	libalx
RUN	git clone https://github.com/alejandro-colomar/rob_cam.git	&& \
	make	-C rob_cam/rob/robot/ur/	-j 2

FROM	debian@sha256:31c7d1766dad20666e72712f71a340726964797750b4e53d4ae561fb3c9e4623
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

