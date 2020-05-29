/******************************************************************************
 *	Copyright (C) 2020	Alejandro Colomar Andrés		      *
 *	SPDX-License-Identifier:	GPL-2.0-only			      *
 ******************************************************************************/


/******************************************************************************
 ******* include **************************************************************
 ******************************************************************************/
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define ALX_NO_PREFIX
#include <libalx/base/compiler.h>
#include <libalx/base/errno.h>
#include <libalx/base/linux.h>
#include <libalx/base/signal.h>
#include <libalx/base/stdio.h>
#include <libalx/base/stdlib.h>
#include <libalx/base/sys.h>
#include <libalx/extra/cv/cv.h>


/******************************************************************************
 ******* define ***************************************************************
 ******************************************************************************/
#define ENV_ROB_ADDR		"ROB_ADDR"
#define ENV_ROB_PORT		"ROB_PORT"
#define ENV_CAMERA_IDX		"CAMERA_IDX"
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
static	char			rob_addr[_POSIX_ARG_MAX];
static	char			rob_port[_POSIX_ARG_MAX];
static	int			camera_idx;
static	int			delay_us;
/* pid */
static	pid_t			pid;
/* camera */
static	cam_s			*camera;
/* rob */
static	int			rob;


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
static
int	cam_init	(img_s **img);
static
int	cam_deinit	(img_s *img);

static
int	env_init	(void);
static
int	rob_init	(void);
static
int	rob_deinit	(void);
static
int	rob_reinit	(void);
static
int	camera_init	(void);
static
void	camera_deinit	(void);
static
int	cv_init		(img_s **img);
static
void	cv_deinit	(img_s *img);

static
int	session		(int i, img_s *img);
static
int	wait_rob	(void);
static
int	proc_cv		(uint8_t *restrict blue11, img_s *restrict img);


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
int	main	(void)
{
	img_s	*img;
	int	status;

	status	= 1;
	if (cam_init(&img))
		goto err0;

	mb();
	for (int i = 0; !sigterm; i++) {
		if (sigpipe) {
			if (rob_reinit())
				goto err;
		}
		status	= session(i, img);
		if (status)
			goto err;
		usleep(delay_us);
		mb();
	}

	status	= 0;
err:
	cam_deinit(img);
err0:
	fprintf(stderr, "cam#%"PRIpid": OUT: %i\n", pid, status);
	perrorx(NULL);

	return	status;
}


/******************************************************************************
 ******* static functions (definitions) ***************************************
 ******************************************************************************/
static
int	cam_init	(img_s **img)
{
	int	status;

	pid	= getpid();
	status	= -1;
	if (sigpipe_init())
		goto err0;
	status--;
	if (sigterm_init())
		goto err0;
	status--;
	if (env_init())
		goto err0;
	status--;
	if (rob_init())
		goto err0;
	status--;
	if (camera_init())
		goto err1;
	status--;
	if (cv_init(img))
		goto err2;
	return	0;
err2:
	camera_deinit();
err1:
	rob_deinit();
err0:
	fprintf(stderr, "cam#%"PRIpid": ERROR: cam_init(): %i\n", pid, status);
	return	status;
}

static
int	cam_deinit	(img_s *img)
{
	int	status;

	status	= 0;
	cv_deinit(img);
	camera_deinit();
	if (rob_deinit())
		status -= 4;

	return	status;
}


static
int	env_init	(void)
{
	int	status;

	status	= -1;
	if (getenv_s(rob_addr, ARRAY_SIZE(rob_addr), ENV_ROB_ADDR))
		goto err;
	status--;
	if (getenv_s(rob_port, ARRAY_SIZE(rob_port), ENV_ROB_PORT))
		goto err;
	status--;
	if (getenv_i32(&camera_idx, ENV_CAMERA_IDX))
		goto err;
	status--;
	if (getenv_i32(&delay_us, ENV_DELAY_US))
		goto err;

	return	0;
err:
	fprintf(stderr, "cam#%"PRIpid": ERROR: env_init(): %i\n", pid, status);
	return	status;
}

static
int	rob_init	(void)
{

	rob	= tcp_client_open(rob_addr, rob_port);
	return	rob < 0;
}

static
int	rob_deinit	(void)
{
	return	close(rob);
}

static
int	rob_reinit	(void)
{

	rob_deinit();
	return	rob_init();
}

static
int	camera_init	(void)
{
	int	status;

	status	= -1;
	if (alx_cv_alloc_cam(&camera))
		goto err;
	alx_cv_init_cam(camera, NULL, camera_idx, 0);
	return	0;
err:
	fprintf(stderr, "cam#%"PRIpid": ERROR: camera_init(): %i\n",pid,status);
	return	status;
}

static
void	camera_deinit	(void)
{

	alx_cv_deinit_cam(camera);
	alx_cv_free_cam(camera);
}

static
int	cv_init		(img_s **img)
{
	int	status;

	status	= -1;
	if (alx_cv_init_img(img))
		goto err;
	return	0;
err:
	fprintf(stderr, "cam#%"PRIpid": ERROR: cv_init(): %i\n", pid, status);
	return	status;
}

static
void	cv_deinit	(img_s *img)
{

	alx_cv_deinit_img(img);
}


static
int	session		(int i, img_s *img)
{
	uint8_t		blue11;
	char		buf[BUFSIZ];
	ptrdiff_t	len;
	ssize_t		n;
	int		status;
	clock_t		time_0;
	clock_t		time_1;
	double		time_tot;

	time_0 = clock();

	status	= -1;
	if (wait_rob())
		goto err;
	status--;
	if (proc_cv(&blue11, img))
		goto err;
	status--;
	if (sbprintf(buf, &len, "cam#%"PRIpid":session#%i:  img[B][1][1] = %"PRIu8"",
							pid, i, blue11))
		goto err;
	printf("%s\n", buf);
	status--;
	n	= send(rob, buf, len, 0);
	if (n < 0)
		goto err;

	time_1 = clock();
	time_tot = ((double) time_1 - time_0) / CLOCKS_PER_SEC;
	printf("session time:	%5.3lf s;\n", time_tot);
	return	0;
err:
	fprintf(stderr, "cam#%"PRIpid": ERROR: session(): %i\n", pid, status);
	return	status;
}

static
int	wait_rob	(void)
{
	char	buf[BUFSIZ];
	ssize_t	n;
	int	status;

	status	= -1;
	n	= recv(rob, buf, ARRAY_SIZE(buf) - 1, 0);
	if (n < 0)
		goto err;
	status--;
	if (!n)
		goto err;
	return	0;
err:
	fprintf(stderr, "cam#%"PRIpid": ERROR: wait_rob(): %i\n", pid, status);
	return	status;
}

static
int	proc_cv		(uint8_t *restrict blue11, img_s *restrict img)
{
	clock_t	time_0;
	clock_t	time_1;
	double	time_tot;
	int	status;

	time_0 = clock();

	status	= -1;
	if (alx_cv_cam_read(img, camera))
		goto err;
	status--;
	if (alx_cv_component(img, ALX_CV_CMP_BGR_B))
		goto err;
	status--;
	if (alx_cv_pixel_get_u8(img, blue11, 1, 1))
		goto err;

	time_1 = clock();
	time_tot = ((double) time_1 - time_0) / CLOCKS_PER_SEC;
	printf("cv time:	%5.3lf s;\n", time_tot);

	return	0;
err:
	fprintf(stderr, "cam#%"PRIpid": ERROR: proc_cv(): %i\n", pid, status);
	return	status;
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

