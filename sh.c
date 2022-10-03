#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"

#define BUFFERSIZE 128

int sh( int argc, char **argv, char **envp )
{
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start out with*/
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';

  /* Put PATH into a linked list */
  pathlist = get_path();
  
  while ( go )
  {
    char ans[BUFFERSIZE];
    int len;
    /* print your prompt */
    printf("[%s]> ", pwd);
    /* get command line and process */
    if (fgets(ans, BUFFERSIZE, stdin) != NULL) {
	    len = (int) strlen(ans);
	    ans[len - 1] = '\0';
    }
    /* check for each built in command and implement */
	if (strcmp(ans, "exit") == 0) { //exits
		go = 0;
	}
    /*  else  program to exec */
	else{
		if(which(ans,pathlist) == NULL){
        	fprintf(stderr, "%s: Command not found.\n", ans);
		}
		else{
			/* find it */
			char* exec_path = which(ans,pathlist);
        	/* do fork(), execve() and waitpid() */
			pid_t pid;
			if((pid = fork()) < 0){
				printf("ERROR\n");
			else if(pid == 0){
				execve(ans);
			}
			else{
				waitpid(pid,NULL,0);
			}
		}
	}
  }
  return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathlist )
{
	/*
	struct pathelement *temp = pathlist;
	char *cmd;
	while (temp != NULL) {
		sprintf(cmd, "%s/gcc", temp->element);
		if (access(cmd, F_OK) == 0)
			return cmd;
		temp = temp->next;
	}
	/* while (temp != NULL) {
	 *	if (strcmp(command, temp->element ) == 0) {
	 *		return temp->element;
	 *	}
	 *	temp = temp->next;
	 * }
	*/
return NULL;
/* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
} /* which */
char *where(char *command, struct pathelement *pathlist )
{

  /* siilarly loop through finding all locations of command */
} /* where() */

void list ( char *dir )
{
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
} /* list() */

