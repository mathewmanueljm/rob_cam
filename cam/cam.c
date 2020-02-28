
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define ALX_NO_PREFIX
#include <libalx/base/compiler/size.h>
#include <libalx/base/errno/error.h>
#include <libalx/base/socket/tcp/client.h>
#include <libalx/base/stdio/printf/sbprintf.h>
#include <libalx/base/sys/types.h>


#define ROB_ADDR	"rob"
#define ROB_PORT	"13100"


int	main	(void)
{
	int		tcp;
	pid_t		pid;
	char		buf[BUFSIZ];
	ptrdiff_t	len;
	ssize_t		n;
	int		status;

	status	= EXIT_FAILURE;

	tcp	= tcp_client_open(ROB_ADDR, ROB_PORT);
	if (tcp < 0)
		goto out0;

	pid	= getpid();
	if (sbprintf(buf, &len, "Hi, this is %"PRIpid"!", pid))
		goto out;

	n	= send(tcp, buf, len, 0);
	if (n < 0)
		goto out;

	status	= EXIT_SUCCESS;
out:
	if (close(tcp))
		status	= EXIT_FAILURE;
out0:
	perrorx(NULL);

	return	status;
}

