#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}
int main(int c,char**v){
  if(c!=2||!isnum(v[1])) usage(v[0]);
  // open /proc/<pid>/stat and print stuff
  int pid = atoi(v[1]);

  char path[64];
  snprintf(path, sizeof(path), "/proc/%d/stat", pid);
  FILE *f1 = fopen(path, "r");

  snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
  FILE *f2 = fopen(path, "r");

  snprintf(path, sizeof(path), "/proc/%d/status", pid);
  FILE *f3 = fopen(path, "r");

  char buf[4096];
  if (fgets(buf, sizeof(buf), f1) == NULL) {
    perror("fgets");
    fclose(f1);
    return 1;
  }
  fclose(f1);

  char *tokens[100];
  int count = 0;

  char *token = strtok(buf, " \t\n");
  while (token != NULL && count < 100) {
    tokens[count] = strdup(token);
    if (tokens[count] == NULL) {
      perror("strdup");
      return 1;
    }
    count++;
    token = strtok(NULL, " \t\n");
  }

  char *utime_str = tokens[15];
  char *stime_str = tokens[16];

  unsigned long long utime = strtoull(utime_str, NULL, 10);
  unsigned long long stime = strtoull(stime_str, NULL, 10);

  unsigned long long total_ticks = utime + stime;
  long clk = sysconf(_SC_CLK_TCK);
  double cpu_sec = (double)total_ticks / (double)clk;

  char line[256];
  long rss_kb = -1;
  while (fgets(line, sizeof(line), f3)) {
	  if (strncmp(line, "VmRSS:", 6) == 0) {
		  sscanf(line + 6, "%ld", &rss_kb);
		  break;
	  }
  }
  fclose(f3);

  char cmdline[256];

  printf("PID:%s\n", tokens[0]);
  printf("State:%s\n", tokens[2]);
  printf("PPID:%s\n", tokens[3]);
  printf("Cmd:-%s\n", fgets(cmdline, sizeof(cmdline), f2));
  printf("CPU: %llu %.3f\n", total_ticks, cpu_sec);
  printf("VmRSS: %ld\n", rss_kb);
  return 0;
}
