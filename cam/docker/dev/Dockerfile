FROM	alejandrocolomar/libalx:dev	AS build
WORKDIR	/tmp
RUN	git clone --single-branch --branch cam				\
			https://github.com/alejandro-colomar/rob_cam.git
RUN	make	-C rob_cam/cam/	-j 2

FROM	debian:testing
WORKDIR	/app
COPY	--from=build /tmp/rob_cam/cam/cam .
RUN	chmod +x ./cam
CMD	["./cam"]
