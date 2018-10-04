# minishell
This code create a mini shell which can execute and control any binary executable just as we would do in a normal shell. After executing minishell binary it prompt minish as bash.

Firstly, this program is taking the input command with/without argument as a string, then splitting those by space delimiter.

Secondly, handling the special input argument &, < , > 

checking if command_array[] contains "&" then setting the boolean is_background = true

checking if command_array[] contains ">" then setting the boolean is_write = true
for is_write true, opening the file descriptor for write_file and passing the return value to dup2()

checking if command_array[] contains < then setting the boolean is_read = true
for is_read true, opening the file descriptor for read_file and passing the return value to dup2()

Thirdly, if received input is "exit" then the program is terminating.

Fourthly, forming the command array and passing it to execv().


It uses fork(), execv(), waitpid system call.

Below are the steps to run the minishell :

Step 1: Use the makefile to compile the code
$make all
Step 2: Run the code without argument.
$./minishell


If any problem contact me at: 
Name : Antra Aruj
Email : aaruj1@binghamton.edu
