#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "post_proc.h"

extern int profile(int argc, char* argv[]);
extern int rotate(int argc, char* argv[]);
extern int custom(int argc, char* argv[]);
static int help(int argc, char* argv[]);

static const struct command options[]={
        {"profile",profile},
        {"rotate",rotate},
        {"custom",custom},
        {"help", help},
        {"-h", help},
        {"--help", help},
};

static int help(int argc, char *argv[]) {
        int i;
        char *module_opt[]={"--help"};
        if(!argc) puts("usage postproc <module> [<args>]\n\nfor command info use \n\t postproc help module");
        else 
                for(i=0; i<sizeof(options)/sizeof(struct command); i++) {
                        if(!strcmp(options[i].command,argv[0])) {
                                options[i].function(1, module_opt);
                                break;
                        }
                }
      exit(0);
      return(0);
}


int main (int argc, char **argv) {

        struct command options[]={
                {"profile",profile},
                {"rotate",rotate},
                {"custom",custom},
                {"help", help},
                {"-h", help},
        };
        int i;
        argc--;
        argv++;
        if(!argc) { 
                help(0,argv); 
                exit(0);
        }
        for(i=0; i<sizeof(options)/sizeof(struct command); i++) {
                if(!strcmp(options[i].command,argv[0])) {
                        options[i].function(--argc, ++argv);
                        break;
                }
        }
}