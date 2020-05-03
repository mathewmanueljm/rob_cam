#!/bin/sh
################################################################################
##	Copyright (C) 2020	  Alejandro Colomar Andrés		      ##
##	SPDX-License-Identifier:  GPL-2.0-only				      ##
################################################################################
##
## Prepare the repo for release
## ============================
##
##  - Remove the files that shouldn't go into the release
##  - Update version numbers
##
################################################################################


################################################################################
##	functions							      ##
################################################################################
update_version()
{
	local	version=$1

	sed "/--branch master/s/master/v${version}/"			\
			-i ./cam/Dockerfile
	sed "/--branch master/s/master/v${version}/"			\
			-i ./rob/Dockerfile
	sed "/--branch master/s/master/v${version}/"			\
			-i ./rob/robot/ur/Dockerfile

	sed "/alejandrocolomar\/rob_cam:cam/s/:cam/:cam_${version}/" \
			-i ./docker/docker-compose.yaml
	sed "/alejandrocolomar\/rob_cam:rob/s/:rob/:rob_${version}/" \
			-i ./docker/docker-compose.yaml
	sed "/alejandrocolomar\/rob_cam:ur-sim/s/:ur-sim/:ur-sim_${version}/" \
			-i ./docker/docker-compose.yaml

	sed "/alejandrocolomar\/rob_cam:cam/s/:cam/:cam_${version}/" \
			-i ./kubernetes/kube-compose.yaml
	sed "/alejandrocolomar\/rob_cam:rob/s/:rob/:rob_${version}/" \
			-i ./kubernetes/kube-compose.yaml
	sed "/alejandrocolomar\/rob_cam:ur-sim/s/:ur-sim/:ur-sim_${version}/" \
			-i ./kubernetes/kube-compose.yaml
}


################################################################################
##	main								      ##
################################################################################
main()
{
	local	version=$1

	update_version	${version}
}


################################################################################
##	run								      ##
################################################################################
main	$1


################################################################################
##	end of file							      ##
################################################################################
