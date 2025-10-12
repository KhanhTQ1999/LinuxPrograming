#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// struct in_addr { /* IPv4 4-byte address */
//     in_addr_t s_addr; /* Unsigned 32-bit integer */
// };
// struct sockaddr_in { /* IPv4 socket address */
//     sa_family_t sin_family; /* Address family (AF_INET) */
//     in_port_t sin_port; /* Port number */
//     struct in_addr sin_addr; /* IPv4 address */
//     unsigned char __pad[X]; /* Pad to size of 'sockaddr'
//     structure (16 bytes) */
// };

int main(int argc, char* argv[]){
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	char send_buff[1025];
	time_t ticks;

	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(send_buff, '0', sizeof(send_buff));

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(1)
	{
		/* In the call to accept(), the server is put to sleep and when for an incoming
		 * client request, the three way TCP handshake* is complete, the function accept()
		 * wakes up and returns the socket descriptor representing the client socket.
		 */
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		/* As soon as server gets a request from client, it prepares the date and time and
		 * writes on the client socket through the descriptor returned by accept()
		 */
		// ticks = time(NULL);
		snprintf(send_buff, sizeof(send_buff), "Hello client from server\n");
		write(connfd, send_buff, strlen(send_buff));

		close(connfd);
		sleep(1);
	}

    return 0;
}