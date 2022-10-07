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
  	char *prefix="";

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
    	printf("%s [%s]> ", prefix, pwd);
    	/* get command line and process */
   		if (fgets(ans, BUFFERSIZE, stdin) != NULL) {
	    	len = (int) strlen(ans);
	    	ans[len - 1] = '\0';
    	}
		char s[2] = " ";
		char *split = strtok(ans,s);
		char *split2 = strtok(NULL,s);
    	/* check for each built in command and implement */
		/* creating ints for case switch */
		if(strcmp(split,"exit")==0){
			go = 0;
		}
		else if (strcmp(split,"which")==0){
			char* which_return = which(split2,pathlist);
			printf("%s\n",which_return);
		}
		else if (strcmp(split,"where")==0){
			where(split2,pathlist);
		}
		else if (strcmp(split,"ls")==0){
				list(pwd);
		}
		else if (strcmp(split,"prompt")==0){
			char new_prefix[BUFFERSIZE];
			if(split2==NULL||split2==""){
				printf("Enter a new prompt");
				if (fgets(new_prefix, BUFFERSIZE, stdin) != NULL) {
             		len = (int) strlen(new_prefix);
             		new_prefix[len - 1] = '\0';
					prefix = new_prefix;
				}
			} else {
				prefix=split2;
			}
		}
		else{
			if(which(split,pathlist)==NULL){
			fprintf(stderr, "%s: Command not found.\n", ans);
        	} else {
        		/* find it */
            	char* exec_path = which(split, pathlist);
            	/* do fork(), exec() and waitpid() */
            	pid_t pid;
            	if((pid=fork())<0){
            		printf("ERROR\n");
            	}
           		else if(pid == 0){}
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
		snprintf(cmd_path,BUFFERSIZE,"%s/%s",temp->element,command);
		if (access(cmd_path, X_OK) == 0) {
			return cmd_path;
		}
		temp = temp->next;
	}
return NULL;

/* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
} /* which */
void where(char *command, struct pathelement *pathlist )
{ //returned string starts with a : and seperates each path with a :
	struct pathelement *temp = pathlist;
	char *cmd_path;
	while (temp) {
		snprintf(cmd_path, BUFFERSIZE, "%s/%s", temp->element, command);
		if (access(cmd_path, X_OK) == 0)
			printf("%s\n", cmd_path);
		temp = temp->next;
	}
  /* similarly loop through finding all locations of command */
} /* where() */

void list ( char *dir )
{
  DIR *dir2;
  struct dirent *dirstruct;

  dir2 = opendir(dir);
  if (dir2 == NULL) {
	printf("Unable to read directory\n");
  } else {
	printf("%s\n", dir);
	while (dirstruct = readdir(dir2)) {
		printf("%s\n", dirstruct->d_name);
	}
  }
  closedir(dir2);
	/* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
} /* list() */

char *getEnvValue(char *envvar) {
	char value[BUFFERSIZE];
	if (!getenv(envvar)) {
		printf("environment not found");
		return NULL;
	}
	if (snprintf(value, BUFFERSIZE, "%s", getenv(envvar)) >= BUFFERSIZE) {
		printf("BUFFERSIZE to small");
		return NULL;
	}
	char *valuep;
	valuep = (char*) malloc(strlen(value) * sizeof(char));
	strcpy(valuep, value);
	return valuep;
}
/*
 * Code for printenv command case
if (split2 == "" || split2 == NULL) { //when your not given an environment variable 
	for (char **envvar = envp; *envvar != 0; envvar++) {
		char *env = *envvar;
		printf("%s=", env);
		char *envvalue = getEnvValue(env);
		printf("%s", envvalue);
		free(envvalue);
	}
} else { //when your given an environment variable 
	char *argenv = getEnvValue(split2);
	if (argenv != NULL) {
		printf("%s", split2);
		printf("%s", argenv);
	}
	free(argenv);
}
*/
