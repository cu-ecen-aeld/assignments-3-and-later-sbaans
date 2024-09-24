/** aesdsocket  application for assignment 5 part 1: it will create a server with sockets
* Author: sbaans
*/

/**
* main application to be able to create a server with sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define CONNECTION_PORT "9000"

#define LISTEN_QUEUE_SIZE 20

#define MAX_RECEIVED_LENGTH (20 * 1024)

#define SERVER_FILE_NAME "/var/tmp/aesdsocketdata"

bool bcaught_signal;

// get sockaddr, IPv4 or IPv6:
void *get_client_IP_addr(struct sockaddr *client_addr)
{
	if (client_addr->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)client_addr)->sin_addr);
	}

	return &(((struct sockaddr_in6*)client_addr)->sin6_addr);
}

static void sigaction_handler ( int signalnumber)
{
	if ( (signalnumber == SIGINT) || (signalnumber == SIGTERM) ) {
		bcaught_signal = true;
	}
}

int main(int argc, char *argv[])
{
    int return_status;
    int socket_descriptor, accept_socket_descriptor;
    struct addrinfo socket_requirements;
    struct addrinfo *server_info;
    struct addrinfo *socket_test;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size;
    char client_ip_addr[INET6_ADDRSTRLEN];
    int max_received_length = MAX_RECEIVED_LENGTH;
    char received_buff[MAX_RECEIVED_LENGTH];
    char *send_buff = NULL;
    int received_length, send_length;
    FILE *filetobewritten;
    int bEnd_of_line = 0;
    struct stat file_stat;
    int bDaemonmode = 0;
    int sock_option=1;
    struct sigaction signal_management;
    
    bcaught_signal = false;


    memset(&signal_management, 0, sizeof(struct sigaction));

    signal_management.sa_handler = sigaction_handler;


	/* Activate syslog */
	openlog(NULL,0,LOG_USER);
	
	if (argc > 2)
	{
		printf("Error, too many arguments");
		syslog(LOG_ERR,"Invalid Number of arguments: %d , it should be 1 or 2", (argc - 1));
		return -1;
	}
	syslog(LOG_DEBUG,"Good Number of parameters");
	
	if (argc == 2) {
		if ( strcmp(argv[1],"-d") != 0 ) {
			syslog(LOG_ERR,"Invalid Argument: only -d is accepted");
			return -1;
		} else {
			bDaemonmode = 1;
		}
	} else {
		bDaemonmode = 0;
	}

	if (bDaemonmode == 1) {
		syslog(LOG_DEBUG,"Daemon mode started");
		daemon(0,0);
	}

	/* Call to the getaddrinfo function */
	memset(&socket_requirements, 0, sizeof socket_requirements);
	
	socket_requirements.ai_family = AF_INET;
	socket_requirements.ai_socktype = SOCK_STREAM;
	socket_requirements.ai_flags = AI_PASSIVE;
	
	if ( ( return_status = getaddrinfo(NULL, CONNECTION_PORT, &socket_requirements, &server_info) ) != 0) {
	    printf("Error during getaddrinfo : %s\n", gai_strerror(return_status) );
		syslog(LOG_ERR,"Error during getaddrinfo : %s\n", gai_strerror(return_status));
		return -1;
	}
	
	
	/* Find a good socket */
	for ( socket_test = server_info; socket_test != NULL; socket_test = socket_test->ai_next) {
	    /* test socket */
	    socket_descriptor = socket(socket_test->ai_family, socket_test->ai_socktype, socket_test->ai_protocol);
	    
	    if ( socket_descriptor == -1) {
	        /* this is not a valid one */
	        continue;
	    } else {

	    	if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &sock_option,sizeof(int)) == -1) {
	    		printf("Unable to setsockopt\n");
	    		syslog(LOG_ERR,"Unable to setsockopt\n");
	    		exit(1);
	    	}
	    	/* test bind */
	        if ( bind(socket_descriptor, socket_test->ai_addr, socket_test->ai_addrlen) == -1) {
	            /* this is not a valid one */
	            close(socket_descriptor);
	            continue;
	        } else {
	            /* we found a good one */
	            break;
	          }
	    }
    }
	
	/* we can free the structure */
	freeaddrinfo(server_info);
	
	/* Test if we found a good one */
	if (socket_test == NULL) {
	    printf("Unable to find a good socket and bind\n");
		syslog(LOG_ERR,"Unable to find a good socket and bind\n");
		return -1;
	}
	
	sigaction(SIGTERM, &signal_management, NULL);
	sigaction(SIGINT, &signal_management, NULL);

	/* listening */
	if ( listen(socket_descriptor, LISTEN_QUEUE_SIZE) == -1) {
	    printf("Error: could not listen\n");
		syslog(LOG_ERR,"Error: could not listen\n");
		return -1;
	}

	filetobewritten = fopen(SERVER_FILE_NAME, "wa");
	if (filetobewritten == NULL)
	{
		printf("Could not create the file\n");
		syslog(LOG_ERR,"Could not create the file");
		return -1;
	}

	while(1) {
		bEnd_of_line = 0;
		client_addr_size = sizeof client_addr;
		accept_socket_descriptor = accept(socket_descriptor, (struct sockaddr *)&client_addr, &client_addr_size);

		if (accept_socket_descriptor == -1) {
			printf("Error: could not accept\n");
			syslog(LOG_ERR,"Error: could not accept\n");
			if (filetobewritten != NULL) {
					fclose(filetobewritten);
				}
			return -1;
		}

		inet_ntop(client_addr.ss_family,get_client_IP_addr((struct sockaddr *)&client_addr),client_ip_addr, sizeof client_ip_addr);
		printf("server: got connection from %s\n", client_ip_addr);
		syslog(LOG_DEBUG,"Accepted connection from %s\n", client_ip_addr);

		if (!fork()) {
			close(socket_descriptor);

			while (bEnd_of_line == 0) {
				received_length = recv(accept_socket_descriptor, &received_buff[0], max_received_length, 0);
				if ( received_length == -1) {
					syslog(LOG_ERR,"Error: could not receive\n");
				}

				/* Append the file */
				fputs(received_buff, filetobewritten);

				printf("append the file with %d characters\n", received_length);

				/* Test if it was the end of the line*/
				if (received_buff[received_length-1] == '\n')
				{
					bEnd_of_line = 1;
					printf("end of the line\n");
				}
			}
			fclose(filetobewritten);
			/* send back the file */
			filetobewritten = fopen(SERVER_FILE_NAME, "r");
			stat(SERVER_FILE_NAME, &file_stat);
			send_length = file_stat.st_size;
			send_buff = malloc(send_length);

			fread(send_buff, sizeof(char), send_length, filetobewritten);

			if (send(accept_socket_descriptor, &send_buff[0], send_length, 0) == -1) {
				syslog(LOG_ERR,"Error: could not send\n");
			}

			close(accept_socket_descriptor);
			if (filetobewritten != NULL) {
					fclose(filetobewritten);
				}
			if (send_buff != NULL){
				free(send_buff);
			}
			exit(0);
		}

		close(accept_socket_descriptor);

		if (bcaught_signal == true) {
			/* a signal was sent */
			if (send_buff != NULL){
				free(send_buff);
			}

			if (filetobewritten != NULL) {
				fclose(filetobewritten);
			}
			syslog(LOG_DEBUG,"Caught signal, exiting");
			remove(SERVER_FILE_NAME);
			return 0;
		}

	}
	if (send_buff != NULL){
		free(send_buff);
	}
	if (filetobewritten != NULL) {
		fclose(filetobewritten);
	}
	return 0;
}
