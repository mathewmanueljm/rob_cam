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
#include <libalx/base/compiler/size.h>
#include <libalx/base/errno/error.h>
#include <libalx/base/socket/tcp/client.h>
#include <libalx/base/stdio/printf/sbprintf.h>
#include <libalx/base/stdlib/getenv/getenv_i.h>
#include <libalx/base/stdlib/getenv/getenv_s.h>
#include <libalx/base/string/strcpy/strlcpys.h>
#include <libalx/base/sys/types.h>
#include <libalx/extra/cv/core/array.h>
#include <libalx/extra/cv/core/img.h>
#include <libalx/extra/cv/core/pixel.h>
#include <libalx/extra/cv/videoio/cam.h>


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
static	char	rob_addr[_POSIX_ARG_MAX];
static	char	rob_port[_POSIX_ARG_MAX];
static	int	camera_idx;
static	int	delay_us;
/* pid */
static	pid_t	pid;


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
static
int	init_env	(void);
static
int	session		(int i, img_s *restrict img, cam_s *restrict cam);
static
int	init_rob	(int *rob);
static
int	deinit_rob	(int rob);
static
int	init_cv		(img_s **restrict img, cam_s **restrict cam);
static
void	deinit_cv	(img_s *restrict img, cam_s *restrict cam);
static
int	proc_cv		(uint8_t *restrict blue11,
			 img_s *restrict img, cam_s *restrict cam);


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
int	main	(void)
{
	img_s	*img;
	cam_s	*cam;
	int	status;

	status	= 1;
	pid	= getpid();
	if (init_env())
		goto out0;
	status++;
	if (init_cv(&img, &cam))
		goto out0;
	status++;

	for (int i = 0; true; i++) {
		status	= session(i, img, cam);
		if (status < 0)
			goto out;
		if (status > 0)
			fprintf(stderr, "cam#%"PRIpid":session#%i:  timeout\n",
								pid, i);
		usleep(delay_us);
	}

	status	= 0;
out:
	deinit_cv(img, cam);
out0:
	fprintf(stderr, "cam#%"PRIpid": ERROR\n", pid);
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
	if (getenv_s(rob_addr, ARRAY_SIZE(rob_addr), ENV_ROB_ADDR))
		return	status;
	status--;
	if (getenv_s(rob_port, ARRAY_SIZE(rob_port), ENV_ROB_PORT))
		return	status;
	status--;
	if (getenv_i32(&camera_idx, ENV_CAMERA_IDX))
		return	status;
	status--;
	if (getenv_i32(&delay_us, ENV_DELAY_US))
		return	status;

	return	0;
}

static
int	session		(int i, img_s *restrict img, cam_s *restrict cam)
{
	int		rob;
	uint8_t		blue11;
	char		buf[BUFSIZ];
	ptrdiff_t	len;
	ssize_t		n;
	int		status;
	clock_t		time_0;
	clock_t		time_1;
	double		time_tot;

	time_0 = clock();

	if (init_rob(&rob))
		return	1;
	status	= -1;
	if (proc_cv(&blue11, img, cam))
		goto out;
	status--;
	if (sbprintf(buf, &len, "cam#%"PRIpid":session#%i:  img[B][1][1] = %"PRIu8"",
							pid, i, blue11))
		goto out;
	printf("%s\n", buf);
	status--;
	n	= send(rob, buf, len, 0);
	if (n < 0)
		goto out;

	time_1 = clock();
	time_tot = ((double) time_1 - time_0) / CLOCKS_PER_SEC;
	printf("session time:	%5.3lf s;\n", time_tot);
	status	= 0;
out:
	if (deinit_rob(rob))
		return	-1;
	return	status;
}

static
int	init_rob	(int *rob)
{

	*rob	= tcp_client_open(rob_addr, rob_port);
	return	*rob < 0;
}

static
int	deinit_rob	(int rob)
{
	return	close(rob);
}

static
int	init_cv		(img_s **restrict img, cam_s **restrict cam)
{

	if (alx_cv_alloc_img(img))
		return	-1;
	if (alx_cv_init_img(*img, 1, 1))
		goto out_free_1;
	if (alx_cv_alloc_cam(cam))
		goto out_deinit_1;
	alx_cv_init_cam(*cam, NULL, camera_idx, 0);
	return	0;

out_deinit_1:
	alx_cv_deinit_img(*img);
out_free_1:
	alx_cv_free_img(*img);
	return	-1;
}

static
void	deinit_cv	(img_s *restrict img, cam_s *restrict cam)
{

	alx_cv_deinit_cam(cam);
	alx_cv_free_cam(cam);
	alx_cv_deinit_img(img);
	alx_cv_free_img(img);
}

static
int	proc_cv		(uint8_t *restrict blue11,
			 img_s *restrict img, cam_s *restrict cam)
{
	clock_t	time_0;
	clock_t	time_1;
	double	time_tot;

	time_0 = clock();
	if (alx_cv_cam_read(img, cam))
		return	1;
	if (alx_cv_component(img, ALX_CV_CMP_BLUE))
		return	1;
	if (alx_cv_pixel_get_u8(img, blue11, 1, 1))
		return	1;
	time_1 = clock();

	time_tot = ((double) time_1 - time_0) / CLOCKS_PER_SEC;
	printf("cv time:	%5.3lf s;\n", time_tot);

	return	0;
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

