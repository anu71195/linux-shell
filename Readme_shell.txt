Anurag Ramteke 150101010
-----------------------------------------CODE OVERVIEW--------------------------------------------

This program simulates working of command line interface in Unix environment

Functions implemented :	1. ls, clear ,vi etc - All external commands 
			2. cd ,pwd - Internal commands 
			3. rm with support options -r,-f,-v 
			4. history n ( even with n omitted)
			5. issue n
			6. Exit
			7. rmexcept
			8. Echo
			9. Export
			10. '!' operator (!!, !-1, !10,!-10 etc)
			11. Initialize and use environment variables
			12. Pipes “|” (multiple) (Ex: ls | grep 'a' | wc)




Implementation Details

The shell.c contains the main function which takes the input from user and checks it for pipeline. If pipeline exist it processes the data separately else it passes the data to the functions. 

int with_pipe_execute():
This function is the initial function which is called for checking the all the command after initial preprocessing . It passes the processed output to function split

int split(char *cmd_exec, int input, int first, int last):

This function is responsible for splitting of command and passing it to command function


static int command(int input, int first, int last, char *cmd_exec):
this does the major part of the program. It  checks for various possibilities of commands. The types of commands that are checked are as under:
1) Internal commands: pwd and cd
2) echo commands, setting and getting environment variables
3) PIPE
4) External commands
it make use of various funtions like tokenise_redirect_input_output, tokenise_redirect_input, tokenise_redirect_output which internally calls tokenise_commands() for tokenization


Helper functions:
getcwd():

gets the current woring Directory

signal():
Handle Interrupt Signal

void prompt():
initiates new Promt 

lsh_rmexcept()
rmexcept function deletes all the files and folders except the ones which are given as its arguments. Arguments can be from 0 to any number of files. whenever rmexcept is called with some arguments(there can be even no arguments) then the function will first get all the files and folders by recursively gather the file names and folders from the current direction where the shell is present. the function to gather files and folders name is get_files_folders() with no argument. The function will return the names of folders and files present in the current directory. After gathering all the information about files and directories, it will see which are the files are present in the arguments given by the user(it will also remove all the white spaces gathered while collection individual file name and directory name and remove it). If that files name is not present in the argument given by the user then the file will be deleted. This will be done for each and every file present in the directory. This will make sure all the files except the ones given by the user as argument are deleted inclusive of directories. strcmp()function is used to compare the argument given by the user and name of files and directories. A function is_dir is created to check whether the given argument is a directory if not a directory then rm command to delete the file is used else rmdir is used to delete the directory. Finally lsh_launch function is used where the file name is passed on and the argument of rm or rmdir is given  accordingly. lsh_launch executes the rm or rmdir command.

get_files_folders()
this function uses the dirent.h library of the c and the inbuilt functions already inside that library to gather files and folders. It will open the current directory and if successfully opened then it will use readdir() function to get the file and if the pointer is null it will break the loop else it will gather the information and store and finally return it at the end of the function.

lsh_launch()
it it will first fork and create a child process and will send the parent to the waiting status while the child is not exited or terminated. while the parent is waiting child will use the execvp function and use rm or rmdir function to delete the files or folders as present in the argument given to this function. if any error it gives an error and return to the shell.


 
