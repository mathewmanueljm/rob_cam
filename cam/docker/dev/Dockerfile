FROM	debian:testing	AS build
RUN	apt-get update							&& \
	apt-get upgrade -V --yes					&& \
	apt-get install -V \
			gcc \
			g++ \
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
	make	install-base install-cv		-C libalx		&& \
	rm -rf	libalx
RUN	git clone --single-branch --branch cam \
		https://github.com/alejandro-colomar/rob_cam.git	&& \
	make	-C rob_cam/cam/	-j 2

FROM	debian:testing
WORKDIR	/app
RUN	apt-get update							&& \
	apt-get upgrade --yes						&& \
	apt-get install -V \
			libc6 \
			libstdc++6 \
			libbsd0 \
			libgsl23 \
			libgslcblas0 \
			libopencv-core4.2 \
			libopencv-videoio4.2 \
			deborphan \
			--yes						&& \
	apt-get autoremove --purge --yes				&& \
	apt-get purge $(deborphan) --yes				&& \
	apt-get autoclean						&& \
	apt-get clean
COPY	--from=build /tmp/rob_cam/cam/cam ./
RUN	chmod +x ./cam
CMD	["./cam"]











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

