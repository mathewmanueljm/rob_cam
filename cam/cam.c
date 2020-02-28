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
#include <libalx/base/sys/types.h>
#include <libalx/extra/cv/core/array.h>
#include <libalx/extra/cv/core/img.h>
#include <libalx/extra/cv/core/pixel.h>
#include <libalx/extra/cv/videoio/cam.h>


/******************************************************************************
 ******* define ***************************************************************
 ******************************************************************************/
#define ROB_ADDR		"rob"
#define ROB_PORT		"13100"

#define CAM_IDX			(0)

#define CAM_SESSIONS_MAX	(1000)

#define DELAY			(100 * 1000)



/******************************************************************************
 ******* enum *****************************************************************
 ******************************************************************************/


/******************************************************************************
 ******* struct / union *******************************************************
 ******************************************************************************/


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
static
int	session		(pid_t pid, int session,
			 img_s *restrict img, cam_s *restrict cam);
static
int	init_rob	(int *restrict rob,
			 char *restrict rob_addr, char *restrict rob_port);
static
int	deinit_rob	(int rob);
static
int	init_cv		(img_s **restrict img, cam_s **restrict cam, int idx);
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
	img_s		*img;
	cam_s		*cam;
	pid_t		pid;
	int		status;
	int		s;

	status	= EXIT_FAILURE;

	if (init_cv(&img, &cam, CAM_IDX))
		goto out0;
	pid	= getpid();

	for (int i = 0; true; i++) {
		s	= session(pid, i, img, cam);
		if (s < 0) {
			fprintf(stderr, "cam#%"PRIpid"; msg#%i;  ERROR: %i\n",
								pid, i, s);
			goto out;
		}
		if (s > 0)
			fprintf(stderr, "cam#%"PRIpid"; msg#%i;  WARNING: rob not found\n",
								pid, i);
		usleep(DELAY);
	}

	status	= EXIT_SUCCESS;
out:
	deinit_cv(img, cam);
out0:
	perrorx(NULL);

	return	status;
}


/******************************************************************************
 ******* static functions (definitions) ***************************************
 ******************************************************************************/
static
int	session		(pid_t pid, int session,
			 img_s *restrict img, cam_s *restrict cam)
{
	int		rob;
	uint8_t		blue11;
	char		buf[BUFSIZ];
	ptrdiff_t	len;
	ssize_t		n;
	int		status;

	if (init_rob(&rob, ROB_ADDR, ROB_PORT))
		return	1;
	status	= -1;
	if (proc_cv(&blue11, img, cam))
		goto out;
	status--;
	if (sbprintf(buf, &len, "cam#%"PRIpid"; msg#%i;  img[B][1][1] = %"PRIu8"",
							pid, session, blue11))
		goto out;
	printf("%s\n", buf);
	status--;
	n	= send(rob, buf, len, 0);
	if (n < 0)
		goto out;
	status	= 0;
out:
	if (deinit_rob(rob))
		return	-1;
	return	status;
}

static
int	init_rob	(int *restrict rob,
			 char *restrict rob_addr, char *restrict rob_port)
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
int	init_cv		(img_s **restrict img, cam_s **restrict cam, int idx)
{

	if (alx_cv_alloc_img(img))
		return	-1;
	if (alx_cv_init_img(*img, 1, 1))
		goto out_free_1;
	if (alx_cv_alloc_cam(cam))
		goto out_deinit_1;
	alx_cv_init_cam(*cam, NULL, idx, 0);
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
	printf("Total time:	%5.3lf s;\n", time_tot);

	return	0;
}


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

