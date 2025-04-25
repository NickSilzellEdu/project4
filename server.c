
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

struct message {
	char source[50];
	char target[50]; 
	char msg[200]; // message body
};

void terminate(int sig) {
	printf("Exiting....\n");
	fflush(stdout);
	exit(0);
}

int main() {
	int server;
	int dummyfd;
	struct message req;
	signal(SIGPIPE,SIG_IGN);
	signal(SIGINT,terminate);
	server = open("serverFIFO",O_RDONLY);
	dummyfd = open("serverFIFO",O_WRONLY);

	while (1) {
		// read requests from serverFIFO
		ssize_t bytes = read(server, &req, sizeof(struct message));
		if(bytes == sizeof(struct message)){
			printf("Received a request from %s to send the message %s to %s.\n",req.source,req.msg,req.target);

			// open target
			int targetFileDescriptor = open(req.target, O_WRONLY);
			if(targetFileDescriptor == -1){
				perror("Failed to open target pipe");
				continue;
			}

			// write message to target
			write(targetFileDescriptor, &req, sizeof(struct message)) != sizeof(struct message);
			
			// close target pipe
			close(targetFileDescriptor);
		}
		else if(bytes == 0){
			// reopen server on end of file
			close(server);
			server = open("./server", O_RDONLY);
		}
		else perror("read");
	}
	close(server);
	close(dummyfd);
	return 0;
}

