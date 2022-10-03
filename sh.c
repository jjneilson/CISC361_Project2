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
	char s[2] = " ";
	char *split = strtok(ans,s);
	char *split2 = strtok(NULL,s);
    /* check for each built in command and implement */
	if (strcmp(split, "exit") == 0) { //exits
		go = 0;
	}
	else if (strcmp(split, "which") == 0){
		char* which_return = which(split2, pathlist);
		printf("%s\n",which_return);
	}
    /*  else  program to exec */
	else{
		if(which(split,pathlist) == NULL){
        	fprintf(stderr, "%s: Command not found.\n", split);
		}
		else{
			/* find it */
			char* exec_path = which(ans,pathlist);
        	/* do fork(), execve() and waitpid() */
			pid_t pid;
			if((pid = fork()) < 0){
				printf("ERROR\n");
			}
			else if(pid == 0){
				
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
	struct pathelement *temp = pathlist;
	char *cmd_path;
	while (temp) {
		snprintf(cmd_path, BUFFERSIZE, "%s/%s", temp->element, command);
		if (access(cmd_path, X_OK) == 0) {
			return cmd_path;
		}
		temp = temp->next;
	}
return NULL;

/* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
} /* which */
char *where(char *command, struct pathelement *pathlist )
{ //returned string starts with a : and seperates each path with a :
	struct pathelement *temp = pathlist;
	char *cmd_path;
	char *all_cmd_paths;
	while (temp) {
		snprintf(cmd_path, BUFFERSIZE, "%s/%s", temp->element, command);
		if (access(cmd_path, X_OK) == 0)
			snprintf(all_cmd_paths, BUFFERSIZE, "%s:%s", all_cmd_paths, cmd_path);
		temp = temp->next;
	}
return all_cmd_paths;
  /* siilarly loop through finding all locations of command */
} /* where() */

void list ( char *dir )
{
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
} /* list() */

