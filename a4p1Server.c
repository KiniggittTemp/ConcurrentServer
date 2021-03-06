#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

/*
 *     CONCURRENT SERVER: THREAD EXAMPLE
 *         Must be linked with the "pthread" library also, e.g.:
 *                cc -o example example.c -lnsl -lsocket -lpthread 
 *         This program creates a connection socket, binds a name to it, then
 *         listens for connections to the sockect.  When a connection is made,
 *         it accepts messages from the socket until eof, and then waits for
 *         another connection...
 *
 *         This is an example of a CONCURRENT server -- by creating threads several
 *         clients can be served at the same time...
 *
 *         This program has to be killed to terminate, or alternately it will abort in
 *         120 seconds on an alarm...
 */

#define PORTNUMBER 10010

struct serverParm {
	int connectionDesc;
};

FILE *f;

void *serverThread(void *parmPtr) {

	#define PARMPTR ((struct serverParm *) parmPtr)
	int recievedMsgLen;
	char messageBuf[1025];
	//char runBuf[1025];
	char path[1025];
	/* Server thread code to deal with message processing */
	printf("DEBUG: connection made, connectionDesc=%d\n", PARMPTR->connectionDesc);
	if (PARMPTR->connectionDesc < 0) {
		printf("Accept failed\n");
		return(0);    /* Exit thread */
	}
	 /* Receive messages from sender... */
	while ((recievedMsgLen=read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0) {
		recievedMsgLen[messageBuf] = '\0';

		if(strcmp(messageBuf, "exit") == 0) {
			printf("Exit called...");
			break;
		}

		fprintf(f, "Thread ID: %d\n", pthread_self());
		fprintf(f, "Server PID: %d\n", getpid());
		fprintf(f, "%s\n", messageBuf);
		
		printf("Thread ID: %d\n", pthread_self());
		printf("Server PID: %d\n", getpid());
		printf("%s\n", messageBuf);
		
		char runBuf[1025];
		strcpy(runBuf, "");
		FILE *fp;
		fp = popen(messageBuf, "r");
		while(fgets(path, sizeof(path)-1, fp) != NULL) {
		//	printf("%s", path);
			strcat(runBuf, path);
		}

		fprintf(f, "%s\n\n", runBuf);
		printf("%s\n\n", runBuf);
		pclose(fp);
		//printf("%s", runBuf);
		if (write(PARMPTR->connectionDesc,runBuf,sizeof(runBuf)) < 0) {
			 perror("Server: write error");
			 return(0);
		}
		//memset(runBuf, 0, sizeof(runBuf);

	}
	close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
	free(PARMPTR);                   /* And memory leaks */
	return(0);                       /* Exit thread */
}
int main (int argc, char* argv[]) {
	//int PORTNUMBER = argv[1];
	//FILE *f;
	f = fopen(argv[2], "a");
	int listenDesc;
	struct sockaddr_in myAddr;
	struct serverParm *parmPtr;
	int connectionDesc;
	pthread_t threadID;
	/* For testing purposes, make sure process will terminate eventually */
	alarm(120);  /* Terminate in 120 seconds */
        /* Create socket from which to read */
	if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("open error on socket");
		exit(1);
	}
	/* Create "name" of socket */
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(atoi(argv[1]));
	if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
		perror("bind error");
		exit(1);
	}
	printf("Server is running...\n");
	/* Start accepting connections.... */
	/* Up to 5 requests for connections can be queued... */
	listen(listenDesc,5);
	while (1) /* Do forever */ {
		/* Wait for a client connection */
		connectionDesc = accept(listenDesc, NULL, NULL);
		/* Create a thread to actually handle this client */
		parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
		parmPtr->connectionDesc = connectionDesc;
		if (pthread_create(&threadID, NULL, serverThread, (void *)parmPtr) != 0) {
			perror("Thread create error");
			close(connectionDesc);
			close(listenDesc);
			exit(1);
		}
		printf("Parent ready for another connection\n");
	}
}

