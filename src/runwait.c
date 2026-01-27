#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <cmd> [args]\n",a); exit(1);}
static double d(struct timespec a, struct timespec b){
  return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}
int main(int c,char**v){
  if (c < 2) usage(v[0]);

  struct timespec t0, t1;
  if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0) {
    perror("clock_gettime(start)");
    return 1;
  }

  int child_pid = fork();
  if (child_pid < 0) {
    perror("fork"); 
    return 1;
  } else if (child_pid == 0) {
    execvp(v[1], &v[1]);
    perror("execvp");
    _exit(127);
  }
  
  int status = 0;
  waitpid(child_pid, &status, 0);

  clock_gettime(CLOCK_MONOTONIC, &t1);

  if (WIFEXITED(status)) {
    status = WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    status = WTERMSIG(status);
  }

  printf("procinfo runwait\n");
  printf("pid=%d, elapsed=%.6f exit=%d\n", child_pid, d(t0, t1), status);
  return 0;
}
