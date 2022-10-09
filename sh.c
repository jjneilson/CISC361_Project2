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
  		char **args = calloc(MAXARGS, sizeof(char*));		
		char ans[BUFFERSIZE];
    		int len;
    		/* print your prompt */
    		printf("%s [%s]> ", prefix, pwd);
    		/* get command line and process */
   		if (fgets(ans, BUFFERSIZE, stdin) != NULL) {
	    	len = (int) strlen(ans);
	    	if(len==1){
				continue;
			} else {
				ans[len - 1] = '\0';
			}
    	}
		char s[2] = " "; //delimiter for strtok
		arg = strtok(ans,s);
		argsct=0;
		while (arg!=NULL && i<10){
			args[argsct]=arg;
			arg=strtok(NULL,s);
			argsct+=1;
		}
    		/* check for each built in command and implement */
		/* creating ints for case switch */
		if(strcmp(args[0],"exit")==0){
			go = 0;
		}
		else if (strcmp(args[0],"which")==0){
			char* which_return = which(args[1],pathlist);
			printf("%s\n",which_return);
		}
		else if (strcmp(args[0],"where")==0){
			where(args[1],pathlist);
		}
		else if (strcmp(args[0],"ls")==0 || strcmp(args[0],"list")==0){
			if (args[1] == NULL) { //no args
				list(pwd);
			} else { //one or more args
				for (int i = 1; args[i] != NULL; i++) {
					list(args[i]);
				}
			}
		} else if (strcmp(args[0],"printenv")==0) {
			if (args[2] != NULL) {
				fprintf(stderr, "too many arguements for given command\n");
			} else if (args[1] == NULL) { //when your not given an environment variable 
				for (char **envvar = envp; *envvar != 0; envvar++) {
					char *env = *envvar;
					printf("%s=", env);
					char *envvalue = getEnvValue(env);
					printf("%s\n", envvalue);
					free(envvalue);                                    
				}
			} else { //when your given an environment variable 
				char *argenv = getEnvValue(args[1]);
				if (argenv != NULL) {
				printf("%s=", args[1]);
				printf("%s\n", argenv);
				}
			free(argenv);
			}                                			
		} else if (strcmp(args[0], "setenv")==0) {
			if (args[3] != NULL) {
				fprintf(stderr, "too many arguements for given command\n");
			} else if (args[1] == NULL) { //no args
				for (char **envvar = envp; *envvar != 0; envvar++) {
                                	char *env = *envvar;
                                	printf("%s=", env);
                                	char *envvalue = getEnvValue(env);
                                	printf("%s\n", envvalue);
                                	free(envvalue);                              
				}
			} else { //with one or more args
				if (args[1] == "HOME") {
					oursetenv(args[1], args[2]);
					strcpy(homedir, args[2]); //changing home directory
				} else if (args[1] == "PATH") {
					oursetenv(args[1], args[2]);
					//free pathlist before redeclaring
					free (pathlist->element);
					struct pathelement *p = pathlist;
					while (p) {
						struct pathelement *tmp = p;
						p = p->next;
						free(tmp);
					}
					pathlist = get_path();	
				} else {
					oursetenv(args[1], args[2]);
				}	
			}
		} else if (strcmp(args[0],"prompt")==0) {
			char new_prefix[BUFFERSIZE];
			if(args[1]==NULL||args[1]==""){
				printf("Enter a new prompt: ");
				if (fgets(new_prefix, BUFFERSIZE, stdin) != NULL) {
             		len = (int) strlen(new_prefix);
             		new_prefix[len - 1] = '\0';
					prefix = new_prefix;
				}
			} else {
				prefix=args[1];
			}
		} else if (strcmp(args[0], "pwd")==0) {
			printf("%s\n", pwd);
		} else if (strcmp(args[0], "pid")==0) {
			printf("%d\n", getpid());
		} else if (strcmp(args[0], "cd")==0) {
			if (args[1] == NULL) {
				chdir(homedir);
				pwd=homedir;
			} else {
				if (findlist(pwd, args[1])==1) { //checks if directory exists
					ourcd(args[1]);
					pwd= strcat(pwd, "/");
					pwd= strcat(pwd, args[1]);
				} else {
					printf("error, no such directory\n");
				}
			}
		} else if (strcmp(args[0], "kill")==0) {
			if (args[1] != NULL) {
				if (args[2] == NULL) {
					pid_t pid1 = atoi(args[1]); //do we need to add a '-' to the pid or int?
					kill(pid1, SIGTERM);
				} else {
					pid_t pid2 = atoi(args[1]);
					kill(pid2, atoi(args[1]));
				}
			}
		} else {
			if(which(args[0] ,pathlist)==NULL){
				fprintf(stderr, "%s: Command not found.\n", ans);
			} else {
        			/* find it */
            			char * exec_path = which(args[0], pathlist);
            			/* do fork(), exec() and waitpid() */
            			pid_t pid;
            			if((pid=fork())<0){
            				printf("ERROR\n");
            			} else if(pid == 0) {
					execve(exec_path, args, NULL);
				} else {
            				waitpid(pid,NULL,0);
            			}
      			}
		}
	free(args);
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
int findlist (char *curdir, char *targetdir) {
	DIR *dir;
	struct dirent *dirstruct;
	dir = opendir(curdir);
	while (dirstruct = readdir(dir)) {
		if (strcmp(dirstruct->d_name, targetdir)==0) {
			return 1;
		}
	}
	return 0;
}

char *getEnvValue(char *envvar) {
	char value[BUFFERSIZE];
	if (!getenv(envvar)) {
		printf("environment not found\n");
		return NULL;
	}
	if (snprintf(value, BUFFERSIZE, "%s", getenv(envvar)) >= BUFFERSIZE) {
		printf("BUFFERSIZE to small\n");
		return NULL;
	}
	char *valuep;
	valuep = (char*) malloc(strlen(value) * sizeof(char));
	strcpy(valuep, value);
	return valuep;
}
void oursetenv(char *arg1, char *arg2) { //have to deal with no args in main loop
	if (arg2 == NULL) {
		char *empty = "";
		setenv(arg1, empty, 1);
	} else { //given two args
		setenv(arg1, arg2, 1);
	}
}
void ourcd(char *pathdir) {
	if (strcmp(pathdir, "-")==0) {
		chdir("..");
	} else {
		if (chdir(pathdir) != 0) { 
			printf("error, no such directory\n");
		}
	}
}
