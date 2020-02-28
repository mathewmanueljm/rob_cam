/******************************************************************************
 *	Copyright (C) 2020	Alejandro Colomar Andrés		      *
 *	SPDX-License-Identifier:	GPL-2.0-only			      *
 ******************************************************************************/


/******************************************************************************
 ******* include **************************************************************
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

#define ALX_NO_PREFIX
#include <libalx/base/compiler/size.h>
#include <libalx/base/errno/error.h>
#include <libalx/base/socket/tcp/server.h>
#include <libalx/base/stdio/printf/sbprintf.h>
#include "libalx/extra/telnet-tcp/client/client.h"


/******************************************************************************
 ******* define ***************************************************************
 ******************************************************************************/
#define ROBOT_ADDR	"robot"
#define ROBOT_PORT	"23"
#define ROBOT_USER	"user"
#define ROBOT_PASSWD	""

#define ROB_PORT	"13100"
#define CLIENTS_MAX	(50)

#define DELAY_LOGIN	(1000 * 1000)
#define DELAY_US	(10 * 1000)


/******************************************************************************
 ******* enum *****************************************************************
 ******************************************************************************/


/******************************************************************************
 ******* struct / union *******************************************************
 ******************************************************************************/


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
void	cam_session	(int cam, FILE *telnet);


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
int	main		(void)
{
	FILE			*telnet;
	int			tcp;
	int			cam;
	struct sockaddr_storage	cam_addr = {0};
	socklen_t		cam_addr_len;
	int			status;

	status	= EXIT_FAILURE;

	if (telnet_open_client(&telnet, ROBOT_ADDR, ROBOT_PORT, "w"))
		goto out0;
	if (telnet_login(telnet, ROBOT_USER, ROBOT_PASSWD, DELAY_LOGIN))
		goto out1;

	tcp	= tcp_server_open(ROB_PORT, CLIENTS_MAX);
	if (tcp < 0)
		goto out1;

	if (telnet_send(telnet, "ls -la"))
		goto out;
	if (telnet_send(telnet, "date"))
		goto out;
	if (telnet_send(telnet, "stat ."))
		goto out;

	cam_addr_len	= sizeof(cam_addr);
	while (true) {
		cam = accept(tcp, (struct sockaddr *)&cam_addr, &cam_addr_len);
		if (cam < 0) {
			usleep(DELAY_US);
			continue;
		}

		cam_session(cam, telnet);
		close(cam);
	}

	status	= EXIT_SUCCESS;
out:
	if (close(tcp))
		status	= EXIT_FAILURE;
out1:
	if (pclose(telnet))
		status	= EXIT_FAILURE;
out0:

	return	status;
}


/******************************************************************************
 ******* static functions (definitions) ***************************************
 ******************************************************************************/
void	cam_session	(int cam, FILE *telnet)
{
	static	int i = 0;
	char	buf[BUFSIZ];
	ssize_t	n;

	i++;
	while (true) {
		n = read(cam, buf, ARRAY_SIZE(buf) - 1);
		if (n < 0)
			return;
		buf[n]	= 0;
		if (!n)
			return;
		if (telnet_send(telnet, buf))
			return;
		usleep(DELAY_US);
	}
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

