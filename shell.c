
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>



/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
char *line_compatible(char *line);
char **lsh_split_line(char *line);
int lsh_execute(char **args,char*com_his,int index);
char * get_files_folders(void);
int lsh_launch(char **args);






/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

int is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  printf("  cd\n  ls\n  rm\n  history\n  issue\n  rmexcept\n  program_name\n  program_name m\n");

  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_history(char **args,char*com_his,int index)
{
  if(args[1]==NULL)
  {
    for(int i=0;i<index;i++)printf("%c",com_his[i]);
    return 1;
  }
  else
  {
    int  number=0;
    for(int i=0;i<8;i++)
    {
      if((int)args[1][i]==0) break;
      if(isdigit(args[1][i]))
        {
          number*=10;
          number+=(int)args[1][i]-48;
        }
        else 
        {
          printf("error:argument given is not a number\n");
          return 1;
        }
    }
    int enters=0,start_command;
    for(int i=0;i<index;i++)   if(com_his[i]=='\n')enters++;
    if(number>enters)
    {
      printf("error:number of command written is less than argument passed\n");
      return 1;
    } 
    start_command=enters-number;
    int find_start_command_index=0;
    for(int i=0;i<index;i++)
    {
      if(com_his[i]=='\n')
        {
          start_command--;
          if(start_command==0)
            {
              find_start_command_index=i+1;
              break;
            }
        }
      if(start_command==0)
      {
        find_start_command_index=i;
        break;
      }
    }
    for(int i=find_start_command_index;i<index;i++)
    {
      printf("%c",com_his[i]);
    }
  }
  return 1;
}
int lsh_issue(char **args,char*com_his,int index)
{
  
  if (args[1] == NULL) 
  {
    printf("lsh: expected argument to \"issue\"\n");
    return 1;
  }
  else
  {
    int  number=0;
    for(int i=0;i<8;i++)
    {
      if((int)args[1][i]==0) break;
      if(isdigit(args[1][i]))
      {
        number*=10;
        number+=(int)args[1][i]-48;
      }
      else 
      {
        printf("error:argument given is not a number\n");
        return 1;
      }
    }
    int enters=0,start_command;
    for(int i=0;i<index;i++)   if(com_his[i]=='\n')enters++;
    if(number>enters)
    {
      printf("error:number of command written is less than argument passed\n");
      return 1;
    } 
    start_command=enters-number;
    int find_start_command_index=0;
    for(int i=0;i<index;i++)
    {
      if(com_his[i]=='\n')
      {
        start_command--;
        if(start_command==0)
        {
          find_start_command_index=i+1;
          break;
        }
      }
      if(start_command==0)
      {
        find_start_command_index=i;
        break;
      }
    }
    char*line=malloc(sizeof(char)*1024);
    char** args;
    for(int i=find_start_command_index;i<index;i++)
      {
        line[i-find_start_command_index]=com_his[i];
        if(com_his[i]=='\n')break;
      }
     line= line_compatible(line);
      printf("executing: %s\n",line);
       args = lsh_split_line(line);
       {
        printf("error:issuing the issue command");
        return 1;
       }
      return  lsh_execute(args,com_his,index);
  }
  return 1;
}
#define LSH_RL_BUFSIZE 1024

char *line_compatible(char *line)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
int index=0;
  while (1) {
    c = line[index++];

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
   
    }
    position++;
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}
int lsh_rmexcept(char **args)
{
  char *files,*to_delete_files;
  if(args[1]==NULL)
  {

    printf("%s",args[0]);
  }
  files=get_files_folders();
  int i=0;
  while(files[i]!='\n')
  {
    printf("%c",files[i++]);;
  }
  i=0;
  while(args[i]!=NULL)
  {
    printf("%s ",args[i++]);
  }
  printf("\n\n");
  i=0;
  char * file_name=malloc(sizeof(char)*100);
  char * cfile_name=malloc(sizeof(char)*100);
  char  ** launch_passing_args=malloc(sizeof(char)*3);

  int j=0;
  while(1)
  {
    if(files[i]=='\n')break;

    if(files[i]==' ')
    {
      int k=0;
      file_name[j]='\0';
      for(int t=0;t<=j;t++)
      {
        if(file_name[t]!=' ')cfile_name[k++]=file_name[t];
        file_name=cfile_name;
      }
      j=0;
      k=1;
      if(file_name[0]!='.')
      {
        int flag=0;
        while(args[k]!=NULL)
        {
          if(strcmp(file_name,args[k++])==0)flag=1;
        }
        if(!flag)
        {
          if(is_dir(file_name))launch_passing_args[0]="rmdir";
          else launch_passing_args[0]="rm";
          launch_passing_args[1]=file_name;
          lsh_launch(launch_passing_args);
        }
      }
      char * file_name=malloc(sizeof(char)*100);
    }
    file_name[j++]=files[i++];
  }  

return 1;


}

char * get_files_folders(void)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    char * files=malloc(sizeof(char*)*1024);
    int index=0;
    if (d)
    {
        printf("\n");
        while ((dir = readdir(d)) != NULL)
        {
            for(int i=0;i<20;i++)
            {
              if((int)dir->d_name[i]==0)break;
              files[index++]=dir->d_name[i];
            }
            printf("\n");
            files[index++]=' ';
            
        }
        closedir(d);
    }
    files[index]='\n';
    return files;
    
}

int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args,char*com_his,int index)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  if(strcmp(args[0],"history")==0)return lsh_history(args,com_his,index);
  else if(strcmp(args[0],"issue")==0)return lsh_issue(args,com_his,index);
  else if(strcmp(args[0],"rmexcept")==0)return lsh_rmexcept(args);

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(char **com_his,int * index)
{
  //position variable tells  the position inside the buffer pointer where the next character should be put as given by the user input
  //index is the number of characters stored in com_history i.e. command history 
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  int e_index=*index;
  char * e_line=*com_his;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;
//if buffer is not allocated space then the shell will respond by telling allocation error
  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    // Read a character
    //character is taken into int because EOF is integer value so to compare character is taken into int
    c = getchar();
    //if reached end of file then exit 
    //else keep taking input and store it in buffer till \n is not found
    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
    //storing the commands in buffer as well as in history
      buffer[position] = c;
      e_line[e_index++]=c;
      *index=e_index;
     *com_his=e_line;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  /*
  bufsize is the allocated memory to the tokens which stores the input form the user
  in the form of arguments and is increased by 64 everytime it is filled completely

  tokens stored the arguments from the user and token is used to get the argument from the user input
  
  tokens backup is used to store the token when the bufsize is increment that is tokens is increased
  */
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;
//tokens store the string arguments which are then stored into tokens 
 //checks if the tokens is allocated the memory if not exit it
  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
//strtok get the pointer to the first argument of the input line as separated by delimiters stored in LSH_TOK_DELIM
  token = strtok(line, LSH_TOK_DELIM);
//while token doesn't reach the end of line while loop will go on
//token in each loop is stored tokens and position is used as a index for tokens where the
//token will be stored and then incremented for the further use when next token will be gathered
  while (token != NULL) {
    tokens[position] = token;
    position++;
//if statement checks if the tokens is full if so then increase its size using realloc
    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  //line is the input command taken from the user
  //after line is divided by space delimiter into arguments and stored in args (string arrays)
  //status tells whether the shell should be on or it should be closed when status is 1 shell will be on
  char *line,*com_his=malloc(sizeof(char)*4096);
  char **args;
  int status,index=0,n;

  do {
    //prompting the user for command
    printf("> ");
    //lsh_read_line takes the input from the user store it in line pointer
    //lsh_split_line divides the input command from user into argument by space delmiter
    //lsh_execute executes the command given by the user
    //com_his pointer stores the entire history of the commands given by the user
    line = lsh_read_line(&com_his,&index);
    com_his[index++]='\n';
    args = lsh_split_line(line);
    status = lsh_execute(args,com_his,index);

//free the memory;
    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

