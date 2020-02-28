

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


#define ROBOT_ADDR	"tst3"
#define ROBOT_PORT	"23"
#define ROBOT_USER	"user"
#define ROBOT_PASSWD	""

#define ROB_PORT	"13100"
#define CLIENTS_MAX	(50)

#define LOG_TELNET_OUT	"log.telnet.out"
#define LOG_TELNET_ERR	"log.telnet.err"

#define DELAY_US	(10 * 1000)


void	cam_session	(int cam, FILE *telnet);


int	main		(void)
{
	FILE			*telnet;
	int			tcp;
	int			cam;
	struct sockaddr_storage	cam_addr = {0};
	socklen_t		cam_addr_len;
	int			status;

	status	= EXIT_FAILURE;

	if (telnet_open_client(&telnet, ROBOT_ADDR, ROBOT_PORT, "w",
						ALX_TELNET_TCP_LOG_OVR,
						LOG_TELNET_OUT, LOG_TELNET_ERR))
		goto out0;
	if (telnet_login(telnet, ROBOT_USER, ROBOT_PASSWD))
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
		usleep(DELAY_US);
		cam = accept(tcp, (struct sockaddr *)&cam_addr, &cam_addr_len);
		if (cam < 0)
			continue;

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

void	cam_session	(int cam, FILE *telnet)
{
	static	int i = 0;
	char	buf[BUFSIZ];
	ssize_t	n;

	i++;
	while (true) {
		usleep(DELAY_US);
		n = read(cam, buf, ARRAY_SIZE(buf) - 1);
		if (n < 0)
			return;
		buf[n]	= 0;
		if (telnet_send(telnet, buf))
			return;
		if (!n)
			return;
	}
}


