###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

##	debian:bullseye
FROM	debian@sha256:a0304760ee6dbca1270ddb92a850b5328057618edd9658cac3e4c913ce38dae1 \
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
	    --branch v1.0-b23						\
	    https://github.com/alejandro-colomar/libalx.git		&& \
	make	base telnet-tcp robot	-C libalx	-j 8		&& \
	make	install-base		-C libalx			&& \
	make	install-telnet-tcp	-C libalx			&& \
	make	install-robot		-C libalx			&& \
	rm -rf	libalx
RUN	git clone							\
	    --single-branch						\
	    --branch version-0.5					\
	    https://github.com/alejandro-colomar/rob_cam.git		&& \
	make	bin/ur-sim		-C rob_cam

##	debian:bullseye
FROM	debian@sha256:a0304760ee6dbca1270ddb92a850b5328057618edd9658cac3e4c913ce38dae1
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			telnet \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
COPY	--from=build /tmp/rob_cam/bin/ur-sim /usr/local/bin/ur-sim
CMD	["ur-sim"]

# docker container run --tty --network rob_cam_network --name robot --rm alejandrocolomar/rob_cam:ur-sim_0.5.6 

