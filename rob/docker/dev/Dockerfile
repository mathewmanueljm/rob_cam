FROM	alejandrocolomar/libalx:dev	AS build
WORKDIR	/tmp
RUN	git clone --single-branch --branch rob				\
			https://github.com/alejandro-colomar/rob_cam.git
RUN	make	-C rob_cam/rob/	-j 2

FROM	debian:testing
WORKDIR	/app
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			telnet \
			deborphan \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get purge $(deborphan) --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
COPY	--from=build /tmp/rob_cam/rob/rob .
RUN	chmod +x ./rob
CMD	["./rob"]
