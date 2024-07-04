#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int retvalue;
    
    /* Call system function */
    retvalue = system(cmd);
    
    /* Test if the system call completed with success or not */
    if (retvalue == -1)
    {
        /* The system call could not be completed */   
        return false;
    }
    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    pid_t forkpid;
    int returnedvalue;
    int returnedexec;
    int returnedwait;
    
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
        
    }
    command[count] = NULL;
    
    
 /*   command[count] = command[count];*/

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    /* Fork */
    forkpid = fork();
    
    if (forkpid == -1)
    {
        /* Fork failed */
        va_end(args);
        return false;
    }

    if (forkpid == 0)
    {
        /* Call execv */
    /*    printf("\n exec with arguments : %s %s ", command[0], command[1]);*/
      returnedexec = execv(command[0],command);
 
        
        if ( returnedexec == -1 )
        {
            /* execv failed */
            va_end(args);
            exit (EXIT_FAILURE);
        }
        va_end(args);

        return true;
    }
    else {

        /* Wait the end of completion */
        returnedwait = wait(&returnedvalue);
        if ( returnedwait == -1)
        {
            /* Waitpid failed */
            va_end(args);
            return false;
        }

        if ( WIFEXITED(returnedvalue))
        {
            /* the child exited correctly */
            
            /* Test the returned value of the child */
            if ( WEXITSTATUS(returnedvalue))
            {
                va_end(args);
                return false;
            }
            else
            {    
                va_end(args);
                return true;
            }
        }
        else
        {
            /* the child exited with an error */
            va_end(args);
            return false;
        }

        va_end(args);

        return true;
   }
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    pid_t forkpid;
    int returnedvalue;
    int returnedexec;
    int returnedwait;
    
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

/* Open the file that will be used as redirection */
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);

    if (fd < 0) 
    {
    /* Could not open the file */
        return false;
    }

    /* Fork */
    forkpid = fork();
    
    if (forkpid == -1)
    {
        /* Fork failed */
        va_end(args);
        return false;
    }

    if (forkpid == 0)
    {
        if (dup2(fd, 1) < 0) 
        { 
          va_end(args);
          exit (EXIT_FAILURE);  
        }
        /* Call execv */
    /*    printf("\n exec with arguments : %s %s ", command[0], command[1]);*/
      returnedexec = execv(command[0],command);
 
        
        if ( returnedexec == -1 )
        {
            /* execv failed */
            va_end(args);
            exit (EXIT_FAILURE);
        }
        va_end(args);

        return true;
    }
    else {

        /* Wait the end of completion */
        returnedwait = wait(&returnedvalue);
        if ( returnedwait == -1)
        {
            /* Waitpid failed */
            va_end(args);
            return false;
        }

        if ( WIFEXITED(returnedvalue))
        {
            /* the child exited correctly */
            
            /* Test the returned value of the child */
            if ( WEXITSTATUS(returnedvalue))
            {
                va_end(args);
                return false;
            }
            else
            {    
                va_end(args);
                return true;
            }
        }
        else
        {
            /* the child exited with an error */
            va_end(args);
            return false;
        }

        va_end(args);

        return true;
   }
}
