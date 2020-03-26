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
#include <stdnoreturn.h>
#include <string.h>
//#include <time.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define ALX_NO_PREFIX
#include <libalx/base/compiler/size.h>
#include <libalx/base/errno/perror.h>
#include <libalx/base/socket/tcp/server.h>
#include <libalx/base/stdio/printf/sbprintf.h>


/******************************************************************************
 ******* define ***************************************************************
 ******************************************************************************/
#define SERVER_PORT	"30002"
#define LISTEN_BACKLOG	(50)
#define CYCLE_DELAY	(1000)

#define LINEBUF	(80)


/******************************************************************************
 ******* enum *****************************************************************
 ******************************************************************************/


/******************************************************************************
 ******* struct / union *******************************************************
 ******************************************************************************/


/******************************************************************************
 ******* static variables *****************************************************
 ******************************************************************************/


/******************************************************************************
 ******* static functions (prototypes) ****************************************
 ******************************************************************************/
noreturn void	tcp_session	(int rob, int i)
{
	char	buf[LINEBUF];
	ssize_t n;
	int	j;

	for (j = 0; true; j++) {
		n	= recv(rob, buf, ARRAY_SIZE(buf) - 1, 0);
		if (n < 0)
			goto err;
		buf[n]	= 0;
		printf("%s", buf);
		fflush(stdout);
		if (!n)
			break;
		usleep(CYCLE_DELAY);
	}
	fprintf(stderr, "session#%i: session finished succesfully!\n", i);
	close(rob);
	exit(EXIT_SUCCESS);
err:
	fprintf(stderr, "session#%i: msg#%i: read() failed; session ended\n", i, j);
	close(rob);
	exit(EXIT_FAILURE);
}


/******************************************************************************
 ******* main *****************************************************************
 ******************************************************************************/
int	main	(void)
{
	int				tcp;
	int				rob;
	struct sockaddr_storage	cli_addr = {0};
	socklen_t			cli_addr_len;
	int				status;
	pid_t				pid;

	status	= EXIT_FAILURE;
	tcp	= tcp_server_open(SERVER_PORT, LISTEN_BACKLOG);
	if (tcp < 0) {
		perrorx("tcp_server_open();");
		goto out;
	}

	cli_addr_len	= sizeof(cli_addr);

	for (int i = 0; true; i++) {
		rob = accept(tcp, (struct sockaddr *)&cli_addr, &cli_addr_len);
		if (rob < 0) {
			fprintf(stderr, "session#%i: accept() failed\n", i);
			continue;
		}

		pid	= fork();
		if (pid == -1) {
			fprintf(stderr, "session#%i: fork() failed\n", i);
			continue;
		} else if (!pid) {
			tcp_session(rob, i);
			__builtin_unreachable();
		} else {
			close(rob);
		}
	}
	status	= EXIT_SUCCESS;
out:
	return	close(tcp) || status;
}


/******************************************************************************
 ******* static functions (definitions) ***************************************
 ******************************************************************************/


/******************************************************************************
 ******* end of file **********************************************************
 ******************************************************************************/

