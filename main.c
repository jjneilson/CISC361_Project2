#include "sh.h"
#include <signal.h>
#include <stdio.h>

/*	Partner names & emails
 *	John Neilson (jneilson@udel.edu)
 *	William Hart (wilhart@udel.edu)
 */
void sig_handler(int signal); 

int main( int argc, char **argv, char **envp )
{
  /* put signal set up stuff here */
  signal(SIGINT,sig_handler);
  return sh(argc, argv, envp);
}

void sig_handler(int signal)
{
  /* define your signal handler */
}

