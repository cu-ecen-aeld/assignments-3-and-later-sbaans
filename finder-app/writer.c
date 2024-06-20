/** Writer application for assignment 2: it will write a string to a file
* Author: sbaans
*/

/**
* main application to be able to write a string to a file, there are 2 parameters
* parameter 1: The name of the file with path included
* parameter 2: The string we want to write
*/

#include <stdio.h>
#include <syslog.h>



int main(int argc, char *argv[])
{
	FILE *filetobewritten;
	int returnvalue;

	/* Activate syslog */
	openlog(NULL,0,LOG_USER);

	/* Check if there are 2 arguments ( argc should be 3 because the name of the program is one argument also)
	 *
	 */
	if (argc != 3)
	{
		printf("Error, there should be 2 arguments");
		syslog(LOG_ERR,"Invalid Number of arguments: %d , it should be 2", (argc - 1));
		return 1;
	}
	syslog(LOG_DEBUG,"Good Number of parameters");
	/* parameter 1 is the name of the file with path */
	/* parameter 2 is the string to write */

	filetobewritten = fopen(argv[1], "wa");
	if (filetobewritten != NULL)
	{
		returnvalue = fputs(argv[2], filetobewritten);
		if (returnvalue == EOF)
		{
			printf("Error, Unable to write File");
			syslog(LOG_ERR,"Unable to write File %s", argv[1]);
			return 1;
		}

		returnvalue = fclose(filetobewritten);
		if (returnvalue == EOF)
		{
			printf("Error, Unable to close File");
			syslog(LOG_ERR,"Unable to close File %s", argv[1]);
			return 1;
		}
		printf("Writing %s to %s",argv[2],argv[1]);
		syslog(LOG_DEBUG,"Writing %s to %s",argv[2],argv[1]);
	}
	else
	{
		printf("Error, Unable to open File");
		syslog(LOG_ERR,"Unable to open File %s", argv[1]);
		return 1;
	}
	return 0;
}
