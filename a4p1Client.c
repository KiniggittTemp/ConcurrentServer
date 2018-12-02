#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 10010 /*port*/

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE], recvline[MAXLINE];

	 // alarm(300);  // to terminate after 300 seconds
	 //basic check of the arguments
    	 //additional checks can be inserted
	if (argc !=4) {
   		perror("Usage: TCPClient <Server IP> <Server Port>"); 
    		exit(1);
	}
    // 	        	
    // 	        	 //Create a socket for the client
    // 	        	  //If sockfd<0 there was an error in the creation of the socket
    	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    		perror("Problem in creating the socket");
    		exit(2);
    	 }
    // 	        	        	
    // 	        	        	 //Creation of the socket
    	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;
    	servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    	servaddr.sin_port =  htons(atoi(argv[2])); 
    	//servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order      	        	      	
    // 	        	        	      	 //Connection of the client to the socket 
    	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
    		perror("Problem in connecting to the server");
		exit(3);
    	}
    // 	        	        	      	       	
    	while (fgets(sendline, MAXLINE, stdin) != "exit\n") {
		//system("hostname;date");
		char buf[256];
		snprintf(buf, sizeof(buf), "hostname >> %s ",argv[3]); 
		system(buf);
		snprintf(buf, sizeof(buf), "date >> %s ", argv[3]);
		system(buf);
		FILE * f;
		f = fopen(argv[3], "a");
		fprintf(f, sendline);
		//snprintf(buf, sizeof(buf), "echo %s >> %s", sendline, argv[3]);
		//system(buf); 
		FILE *fp;
		char path[1025];
		fp = popen("hostname;date", "r");
		while(fgets(path, sizeof(path)-1, fp) != NULL) {
			printf("%s", path);
		}
		pclose(fp);

  		send(sockfd, sendline, strlen(sendline), 0);
		if(strcmp(sendline, "exit") == 0) {
			printf("Exit called...");
			break;
		}
    		if (recv(sockfd, recvline, MAXLINE,0) == 0){
    			//error: server terminated prematurely
    			perror("The server terminated prematurely");
			exit(4);
    		}
    		printf("%s", "String received from the server: ");
    		fputs(recvline, stdout);
		fprintf(f, recvline);
		fprintf(f, "\n\n");
		fclose(f);
		//snprintf(buf, sizeof(buf), "echo %s\n\n >> %s", recvline, argv[3]);
		//system(buf);
	}
	send(sockfd, "", strlen(""), 0);
	exit(0);
}
