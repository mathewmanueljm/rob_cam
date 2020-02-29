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
#include <libalx/base/stdlib/getenv/getenv_i.h>
#include <libalx/base/stdlib/getenv/getenv_s.h>
#include <libalx/base/sys/types.h>
#include "libalx/extra/telnet-tcp/client/client.h"


/******************************************************************************
 ******* define ***************************************************************
 ******************************************************************************/
#define ENV_ROBOT_ADDR		"ROBOT_ADDR"
#define ENV_ROBOT_PORT		"ROBOT_PORT"
#define ENV_ROBOT_USER		"ROBOT_USER"
#define ENV_ROBOT_PASSWD	"ROBOT_PASSWD"
#define ENV_ROB_PORT		"ROB_PORT"
#define ENV_ROB_CAMS_MAX	"ROB_CAMS_MAX"
#define ENV_DELAY_LOGIN		"DELAY_LOGIN"
#define ENV_DELAY_US		"DELAY_US"


/******************************************************************************
 ******* enum *****************************************************************
 ******************************************************************************/


/******************************************************************************
 ******* struct / union *******************************************************
 ******************************************************************************/


/******************************************************************************
 ******* static variables *****************************************************
 ******************************************************************************/
/* environment variables */
static	char	robot_addr[_POSIX_ARG_MAX];
static	char	robot_port[_POSIX_ARG_MAX];
static	char	robot_user[_POSIX_ARG_MAX];
static	char	robot_passwd[_POSIX_ARG_MAX];
static	char	rob_port[_POSIX_ARG_MAX];
static	int	rob_cams_max;
static	int	delay_login;
static	int	delay_us;
/* pid */
static	pid_t	pid;


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
static
int	init_env	(void);
static
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

	pid	= getpid();
	status	= 1;
	if (init_env())
		goto out0;
	status++;
	if (telnet_open_client(&telnet, robot_addr, robot_port, "w"))
		goto out0;
	status++;
	if (telnet_login(telnet, robot_user, robot_passwd, delay_login))
		goto out1;

	status++;
	tcp	= tcp_server_open(rob_port, rob_cams_max);
	if (tcp < 0)
		goto out1;

	status++;
	if (telnet_send(telnet, "ls -la"))
		goto out;
	if (telnet_send(telnet, "date"))
		goto out;
	if (telnet_send(telnet, "stat ."))
		goto out;

	status++;
	cam_addr_len	= sizeof(cam_addr);
	while (true) {
		cam = accept(tcp, (struct sockaddr *)&cam_addr, &cam_addr_len);
		if (cam < 0) {
			usleep(delay_us);
			continue;
		}

		cam_session(cam, telnet);
		close(cam);
	}

	status	= EXIT_SUCCESS;
out:
	if (close(tcp))
		status	+= 32;
out1:
	if (pclose(telnet))
		status	+= 64;
out0:
	fprintf(stderr, "rob#%"PRIpid": ERROR: %i\n", pid, status);
	perrorx(NULL);

	return	status;
}


/******************************************************************************
 ******* static functions (definitions) ***************************************
 ******************************************************************************/
static
int	init_env	(void)
{
	int	status;

	status	= -1;
	if (getenv_s(robot_addr, ARRAY_SIZE(robot_addr), ENV_ROBOT_ADDR))
		return	status;
	status--;
	if (getenv_s(robot_port, ARRAY_SIZE(robot_port), ENV_ROBOT_PORT))
		return	status;
	status--;
	if (getenv_s(robot_user, ARRAY_SIZE(robot_user), ENV_ROBOT_USER))
		return	status;
	status--;
	if (getenv_s(robot_passwd, ARRAY_SIZE(robot_passwd), ENV_ROBOT_PASSWD))
		return	status;
	status--;
	if (getenv_s(rob_port, ARRAY_SIZE(rob_port), ENV_ROB_PORT))
		return	status;
	status--;
	if (getenv_i32(&rob_cams_max, ENV_ROB_CAMS_MAX))
		return	status;
	status--;
	if (getenv_i32(&delay_login, ENV_DELAY_LOGIN))
		return	status;
	status--;
	if (getenv_i32(&delay_us, ENV_DELAY_US))
		return	status;

	return	0;
}

static
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
		usleep(delay_us);
	}
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

