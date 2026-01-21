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

  FILE *f = fopen(path, "r");
  if (!f) return 1;

  char buf[4096];
  if (fgets(buf, sizeof(buf), f) == NULL) {
    perror("fgets");
    fclose(f);
    return 1;
  }

  fclose(f);

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

  return 0;
}
