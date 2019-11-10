
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define clear() printf("\033[H\033[J") 

char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	"find",
	"wc"
};

int built_exit(char **args){
	return 1;
}

int built_help(char **args){
	int i;
	printf("UNIX PROJECT IT202 NITK\n");
	printf("Team Members: Ryan, Nadeer, Sulaiman, Rohan\n");
	printf("The following are built in:\n");
	int n_cmd=sizeof(builtin_str) / sizeof(char*);
	for (i = 0; i < n_cmd; i++){
		printf("  %s\n", builtin_str[i]);
	}
	return 0;
}


int built_cd(char **args){
	if (args[1] == NULL) {
		fprintf(stderr, "Arguments expected to cd");
	} 
	else {
		if (chdir(args[1]) != 0) {
			perror("Not a valid directory");
		}
	}
	return 0;
}

#define LINE_LENGTH 300;
int built_find(char **args){
	if (args[1] == NULL || args[2] == NULL) {
		fprintf(stderr, "Arguments expected to find");
		return 0;
	}
	FILE *fp;
	int l=LINE_LENGTH;
	char *line= malloc(l * sizeof(char));
	fp=fopen(args[1],"r");
	if(!fp){
		perror("The file is not present");
		return 0;
	}
	int ln=0;
	while(fgets(line,l,fp)!=NULL){
		ln++;
		if(strstr(line,args[2])){
			printf("%d:%s\n",ln,line);
		}
	}
	fclose(fp);
	free(line);
	return 0;
}

int built_wc(char **args){
	if (args[1] == NULL) {
		fprintf(stderr, "Arguments expected to wc");
		return 0;
	}
	FILE *fp;
	int l=LINE_LENGTH;
	char *line= malloc(l * sizeof(char));
	fp=fopen(args[1],"r");
	if(!fp)
	{
		int i=1;
		int n=0;
		while(args[i]!=NULL){
			n++;
			i++;

		}
		printf("%d",n);
		return 0;

	}
	else
	{
		int c, n=1, flag=0;
		//code = malloc(1000);
		while ((c = fgetc(fp)) != EOF)
    	{
      		flag=1;
		if (c==' ' || c=='\n')
      			n++;
	}
		if (flag==0)
			printf("0");
		else
			printf("%d",n);
	fclose(fp);
	free(line);
	return 0;

	
}
}

int run_external(char **args){
	pid_t pid = fork();
	if (pid == 0){
		if (execvp(args[0],args) < 0 ){
			perror("The command could not be found");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0) {
		perror("Failed to fork the child");
		exit(EXIT_FAILURE);
	}
	else{
		//waiting for child to terminate
		//better implementation would be to check all chid processes
		wait(NULL); 
		return 0;
	}
}


int (*builtin_func[]) (char **) = {
	&built_cd,
	&built_help,
	&built_exit,
	&built_find,
	&built_wc
};

int cmd_execute(char **args){
	int i;
	if (args[0] == NULL) {		
	//empty command
		return 1;
	}
	int n_cmd=sizeof(builtin_str) / sizeof(char *);

	for (i = 0; i<n_cmd; i++){
		if (strcmp(args[0], builtin_str[i]) == 0) {
			
			return (*builtin_func[i])(args);
		}
	}
	return run_external(args);
}

#define T_CMD_SIZE 1024

char* cmd_read(void){
	int cmd_size = T_CMD_SIZE;
	char *cmd_str = malloc(cmd_size * sizeof(char));
	int pos = 0;
	int c; 

	if(cmd_str){
		while(1){
			c = getchar();

			if (c == EOF || c == '\n'){
					cmd_str[pos] = '\0';
					return cmd_str;
			} 
			else {
					cmd_str[pos] = c;
			}

			pos++;
			if (pos>cmd_size){
				cmd_size += T_CMD_SIZE;
				cmd_str = realloc(cmd_str,cmd_size * sizeof(char*));

				if(!cmd_str){
					printf("Space not allocated\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	else{
		printf("Space not allocated\n");
		exit(EXIT_FAILURE);
	}
}




#define CMD_SIZE 64
#define DELIM " "

char** cmd_split(char* line){
	int cmd_size = CMD_SIZE;
	char **blocks = malloc(cmd_size * sizeof(char*));
	char *cmd_arg;
	int pos = 0;

	if(blocks){
		cmd_arg = strtok(line, DELIM);
		while (cmd_arg != NULL) {
				blocks[pos] = cmd_arg;
				pos++;

				if (pos>cmd_size){
					cmd_size += CMD_SIZE;
					blocks = realloc(blocks,cmd_size * sizeof(char*));

					if(!blocks){
						printf("Space not allocated\n");
						exit(EXIT_FAILURE);
					}

				}


				cmd_arg = strtok(NULL, DELIM); 
		}
	}
	else{
		printf("Space not allocated\n");
		exit(EXIT_FAILURE);
	}
	free(cmd_arg); 
	blocks[pos] = NULL;
	return blocks;

}

void printDir() 
{ 
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("Dir:%s>>", cwd); 
} 





void repl_loop(void){
	char *line;
	char **args;
	int status;
	char* user=getenv("USER");
	//read commands from user
	//parse the command
	//execute
	do{
		printf("\n%s@it202@",user);
		printDir();
		line=cmd_read();

		
		args=cmd_split(line);
		status=cmd_execute(args);

		//free memory
		free(line);
		free(args);
	}while(!status);


}


void shell_init(void){
	clear();
	printf("This is a shell created for a project of IT202\n");
	printf("UNIX PROJECT IT202 NITK\n");
	printf("Team Members: Ryan, Nadeer, Sulaiman, Rohan\n");
	sleep(6);
	clear();
}



int main(int argc,char** agrv){
	//load the configuration
	shell_init();

	//run an REPL loop
	repl_loop();
	//Perform any shutdown cleanup
	return 0;
}

