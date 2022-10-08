
#include "get_path.h"

extern int pid;
int sh( int argc, char **argv, char **envp);

/* which
 * Arg1: string that contains the command that you want to look for.
 * Arg2: head of a linked list containing all paths
 * Return: returns a string that contains the first directory found that contains the command.
 */
char *which(char *command, struct pathelement *pathlist);

/* where
 * Arg1: string that contains the command that you want to look for.
 * Arg2: head of a linked list containing all the paths
 * No return, instead prints out each path that has the command on a line per path.
 */
void where(char *command, struct pathelement *pathlist);

/* list
 * Arg1: string containing a directory the be printed
 * No return, instead prints out each item in the directory on a line per item.
 */
void list ( char *dir );

/* getEnvValue
 * Arg1: string that contains the name of an environment variable
 * Return: returns a string that contains the value of a given environment variable
 */
char *getEnvValue(char *envvar);

/* oursetenv
 * Arg1: string that contains the name of a new environment variable
 * Arg2: string that contains the value of a new environment varialb
 * No return, and sets a new enviroment variable, and if Arg2 is NULL makes an empty enviroment variable
 */
void oursetenv(char *arg1, char *arg2);

/* ourcd
 * Arg1: string containing a path to a directory to change to
 * No return, sets the current directory to be input directory, if Arg1 is NULL sets path to be HOME, if Arg1 is '-' sets path to be previous directory.
 */
void ourcd(char *pathdir);

#define PROMPTMAX 32
#define MAXARGS 10
