
#include "get_path.h"

extern int pid;
int sh( int argc, char **argv, char **envp);
char *which(char *command, struct pathelement *pathlist);
void where(char *command, struct pathelement *pathlist);
void list ( char *dir );
void printenv(char **envp);
char *getEnvValue(char *envvar);
void oursetenv(char *arg1, char *arg2);

#define PROMPTMAX 32
#define MAXARGS 10
