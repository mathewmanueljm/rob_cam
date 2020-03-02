/******************************************************************************
 *	Copyright (C) 2020	Alejandro Colomar Andrés		      *
 *	SPDX-License-Identifier:	GPL-2.0-only			      *
 ******************************************************************************/


/******************************************************************************
 ******* include **************************************************************
 ******************************************************************************/
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
//#include <sys/syscall.h>
#include <unistd.h>

#include <linux/membarrier.h>

#define ALX_NO_PREFIX
#include <libalx/alx/robot/ur/ur.h>
#include <libalx/base/compiler/size.h>
#include <libalx/base/compiler/unused.h>
#include <libalx/base/errno/error.h>
#include <libalx/base/linux/membarrier.h>
#include <libalx/base/signal/sigterm.h>
#include <libalx/base/socket/tcp/server.h>
#include <libalx/base/stdio/printf/sbprintf.h>
#include <libalx/base/stdlib/getenv/getenv_i.h>
#include <libalx/base/stdlib/getenv/getenv_s.h>
#include <libalx/base/sys/types.h>
#include <libalx/extra/telnet-tcp/client/client.h>


/******************************************************************************
 ******* define ***************************************************************
 ******************************************************************************/
#define ENV_ROBOT_TYPE		"ROBOT_TYPE"
#define ENV_ROBOT_ADDR		"ROBOT_ADDR"
#define ENV_ROBOT_PORT		"ROBOT_PORT"
#define ENV_ROBOT_USER		"ROBOT_USER"
#define ENV_ROBOT_PASSWD	"ROBOT_PASSWD"
#define ENV_ROBOT_STATUS_FNAME	"ROBOT_STATUS_FNAME"
#define ENV_ROB_PORT		"ROB_PORT"
#define ENV_ROB_CAMS_MAX	"ROB_CAMS_MAX"
#define ENV_DELAY_LOGIN		"DELAY_LOGIN"
#define ENV_DELAY_US		"DELAY_US"


/******************************************************************************
 ******* enum *****************************************************************
 ******************************************************************************/
enum	Robot_Steps {
	ROBOT_STEP_IDLE,

	ROBOT_STEP_INFO,

	ROBOT_STEPS
};


/******************************************************************************
 ******* struct / union *******************************************************
 ******************************************************************************/
struct	Robot_Step_Info {
	//	pose;
	char	cmd[_POSIX_ARG_MAX];
};

struct	Robot_Status {
	FILE	*fp;
	int	step;
	struct Robot_Step_Info	info[ROBOT_STEPS];
};


/******************************************************************************
 ******* static variables *****************************************************
 ******************************************************************************/
/* environment variables */
static	char			robot_addr[_POSIX_ARG_MAX];
static	char			robot_port[_POSIX_ARG_MAX];
static	char			robot_user[_POSIX_ARG_MAX];
static	char			robot_passwd[_POSIX_ARG_MAX];
static	char			robot_status_fname[FILENAME_MAX];
static	char			rob_port[_POSIX_ARG_MAX];
static	int			rob_cams_max;
static	int			delay_login;
static	int			delay_us;
/* pid */
static	pid_t			pid;
/* robot */
static	struct Robot_Status	robot_status;
#if 0
static	FILE			*robot;		/* telnet */
#else
struct Alx_UR			*robot;
#endif
/* cam */
static	int			tcp;


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
static
int	rob_init		(void);
static
int	rob_deinit		(void);

static
int	env_init		(void);
static
int	robot_init		(void);
static
int	robot_deinit		(void);
static
int	tcp_init		(void);
static
int	tcp_deinit		(void);

static
int	robot_status_init	(void);
static
int	robot_status_deinit	(void);
static
int	robot_status_update	(int step);
static
void	robot_status_reset	(void);

static
int	cam_session		(int cam);

static
int	robot_steps		(char *cam_data);
static
int	robot_step_info		(char *str);


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
int	main	(void)
{
	int			cam;
	struct sockaddr_storage	cam_addr = {0};
	socklen_t		cam_addr_len;
	int			status;

	status	= 1;
	if (rob_init())
		goto err;

	cam_addr_len	= sizeof(cam_addr);
	do {
		cam = accept(tcp, (struct sockaddr *)&cam_addr, &cam_addr_len);
		if (cam < 0) {
			usleep(delay_us);
			goto retry;
		}

		status	= cam_session(cam);
		close(cam);
		if (status < 0)
			break;
	retry:
		asm volatile ("" : : : "memory");
	} while (!sigterm);

	status	= 0;
	if (rob_deinit())
		status	+= 32;
err:
	fprintf(stderr, "rob#%"PRIpid": ERROR: main(): %i\n", pid, status);
	perrorx(NULL);

	return	status;
}


/******************************************************************************
 ******* static functions (definitions) ***************************************
 ******************************************************************************/
static
int	rob_init		(void)
{
	int	status;

	pid	= getpid();
	status	= -1;
	if (sigterm_init())
		goto err0;
	status--;
	if (env_init())
		goto err0;
	status--;
	if (robot_init())
		goto err0;
	status--;
	if (tcp_init())
		goto err;
	return	0;
err:
	robot_deinit();
err0:
	fprintf(stderr, "rob#%"PRIpid": ERROR: rob_init(): %i\n", pid, status);
	return	status;
}

static
int	rob_deinit		(void)
{
	int	status;

	status	= 0;
	if (tcp_deinit())
		status -= 1;
	if (robot_deinit())
		status -= 2;
	if (robot_status_deinit())
		status -= 4;

	return	status;
}


static
int	env_init		(void)
{
	int	status;

	status	= -1;
	if (getenv_s(robot_addr, ARRAY_SIZE(robot_addr), ENV_ROBOT_ADDR))
		goto err;
	status--;
	if (getenv_s(robot_port, ARRAY_SIZE(robot_port), ENV_ROBOT_PORT))
		goto err;
	status--;
	if (getenv_s(robot_user, ARRAY_SIZE(robot_user), ENV_ROBOT_USER))
		goto err;
	status--;
	if (getenv_s(robot_passwd, ARRAY_SIZE(robot_passwd), ENV_ROBOT_PASSWD))
		goto err;
	status--;
	if (getenv_s(robot_status_fname, ARRAY_SIZE(robot_status_fname),
							ENV_ROBOT_STATUS_FNAME))
		goto err;
	status--;
	if (getenv_s(rob_port, ARRAY_SIZE(rob_port), ENV_ROB_PORT))
		goto err;
	status--;
	if (getenv_i32(&rob_cams_max, ENV_ROB_CAMS_MAX))
		goto err;
	status--;
	if (getenv_i32(&delay_login, ENV_DELAY_LOGIN))
		goto err;
	status--;
	if (getenv_i32(&delay_us, ENV_DELAY_US))
		goto err;

	return	0;
err:
	fprintf(stderr, "rob#%"PRIpid": ERROR: env_init(): %i\n", pid, status);
	return	status;
}

static
int	robot_init		(void)
{
	int	status;

	status	= -1;
#if 0
	if (telnet_open_client(&robot, robot_addr, robot_port, "w"))
		goto err0;
	status--;
	if (telnet_login(robot, robot_user, robot_passwd, delay_login))
		goto err;
#else
	if (ur_init(&robot, robot_addr, robot_port, delay_login))
		goto err0;
#endif
	status--;
	if (robot_status_init())
		goto err;

	return	0;
err:
	robot_deinit();
err0:
	fprintf(stderr, "rob#%"PRIpid": ERROR: robot_init(): %i\n", pid, status);
	return	status;
}

static
int	robot_deinit		(void)
{

#if 0
	return	pclose(robot);
#else
	return	ur_deinit(robot);
#endif
}

static
int	tcp_init		(void)
{

	tcp	= tcp_server_open(rob_port, rob_cams_max);
	return	tcp < 0;
}

static
int	tcp_deinit		(void)
{
	return	close(tcp);
}


static
int	robot_status_init	(void)
{
	FILE	*fp;

	fp	= fopen(robot_status_fname, "r");
	if (!fp)
		return	0;

	if (fread(&robot_status, 1, sizeof(robot_status), fp) <= 0)
		goto eread;
	robot_status.fp	= fp;

	return	0;
eread:
	memset(&robot_status, 0, sizeof(robot_status));
	return	-2;
}

static
int	robot_status_deinit	(void)
{

	if (robot_status.fp)
		return	fclose(robot_status.fp);
	return	0;
}

static
int	robot_status_update	(int step)
{

	if (!step) {
		robot_status_reset();
		return	0;
	}

	robot_status.step	= step;

	if (!robot_status.fp) {
		robot_status.fp	= fopen(robot_status_fname, "w");
		if (!robot_status.fp)
			return	-1;
	} else {
		rewind(robot_status.fp);
	}

	if (fwrite(&robot_status, sizeof(robot_status), 1,robot_status.fp) != 1)
		return	-2;

	return	0;
}

static
void	robot_status_reset	(void)
{

	if (robot_status.fp)
		fclose(robot_status.fp);
	remove(robot_status_fname);
	memset(&robot_status, 0, sizeof(robot_status));
}


static
int	cam_session		(int cam)
{
	static	int i = 0;
	char	cam_data[BUFSIZ];
	ssize_t	n;

	i++;
	while (true) {
		n = read(cam, cam_data, ARRAY_SIZE(cam_data) - 1);
		if (n < 0)
			return	1;
		cam_data[n]	= 0;
		if (!n)
			return	1;
		if (robot_steps(cam_data))
			return	-1;
		usleep(delay_us);
	}
}


static
int	robot_steps		(char *cam_data)
{

	switch (robot_status.step) {
	case ROBOT_STEP_IDLE:
		robot_status_update(ROBOT_STEP_INFO);
	case ROBOT_STEP_INFO:
		if (robot_step_info(cam_data))
			goto err;
		robot_status_update(ROBOT_STEP_IDLE);
	}

	return	0;
err:
	fprintf(stderr, "rob#%"PRIpid": ERROR: robot_steps(): %i\n",
							pid, robot_status.step);
	return	robot_status.step;
}

static
int	robot_step_info		(char *str)
{

#if 0
	return	telnet_send(robot, str);
#else
	return	ur_puts(robot, str, delay_us, stdout);
#endif
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

