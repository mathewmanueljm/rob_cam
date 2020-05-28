###############################################################################
#        Copyright (C) 2020        Sebastian Francisco Colomar Bauza          #
#        Copyright (C) 2020        Alejandro Colomar Andrés                   #
#        SPDX-License-Identifier:  GPL-2.0-only                               #
###############################################################################

##	alpine:latest
FROM	alpine@sha256:39eda93d15866957feaee28f8fc5adb545276a64147445c64992ef69804dbf01 \
			AS dns

RUN	apk add	--no-cache --upgrade bind

CMD	["named", "-c", "/etc/bind/named.conf", "-g"]

###############################################################################
