#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 10000

void sigintHandler() {
    puts("Press enter for shell\n");
}


int main() {

    signal(SIGINT, sigintHandler);

    int i = 0;
    bool is_background, is_read, is_write;
    char read_file[BUFFER_SIZE], write_file[BUFFER_SIZE];
    int fd;
    int total_command;

    pid_t pid, pid_table[100];
    int pid_count = 0;
    int status;

    char *command_line_string = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    char *command;
    bool is_wrong_input;
    size_t line_size;

    char exit_command[] = {"exit"};

    while (1) {
        printf("minish>");
        getline(&command_line_string, &line_size, stdin);

        is_background = false;
        is_read = false;
        is_write = false;
        is_wrong_input = false;

        char **command_array = (char **) malloc(sizeof(char *) * BUFFER_SIZE);
        i = 0;
        command = strtok(command_line_string, " \t\r\n\a");

        if (command == NULL) {
            continue;
        }

        while (command != NULL) {
            command_array[i++] = command;
            command = strtok(NULL, " \t\r\n\a");
        }

        total_command = i;

        /*!
         * handle background arg : &
         */
        if (!strcmp(command_array[total_command - 1], "&")) {
            is_background = true;
            command_array[total_command - 1] = NULL;
        }

        /* printf("Command in List: \n");
         for (i = 0; i < total_command; i++) {
             printf("%s ", command_array[i]);
         }
         printf("\n");*/



        /*!
         * Output redirect to File
         */
        for (i = 0; i < total_command; i++) {
            if (command_array[i] != NULL) {
                if (!strcmp(command_array[i], ">")) {
                    if (command_array[i + 1] != NULL) {
                        is_write = true;
                        command_array[i] = NULL;
                        strcpy(write_file, command_array[i + 1]);
                        command_array[i + 1] = NULL;
                        break;
                    } else {
                        fprintf(stderr, "syntax error near unexpected token `newline'\n");
                        is_wrong_input = true;
                        break;
                    }
                }
            }
        }

        /*!
         * Input from File
         */
        for (i = 0; i < total_command; i++) {
            if (command_array[i] != NULL) {
                if (!strcmp(command_array[i], "<")) {
                    if (command_array[i + 1] != NULL) {
                        is_read = true;
                        command_array[i] = NULL;
                        strcpy(read_file, command_array[i + 1]);
                        command_array[i + 1] = NULL;
                        break;
                    } else {
                        fprintf(stderr, "syntax error near unexpected token `newline'\n");
                        is_wrong_input = true;
                        break;
                    }
                }
            }
        }

        if (is_wrong_input) {
            continue;
        }

        if (!strcmp(command_array[0], exit_command)) {
            printf("minish finished.\n");
            printf("Have a nice day! \n");
            exit(0);
        }

//        printf("command in List: \n");
//        for (i = 0; i < total_command; i++) {
//            printf("%s ", command_array[i]);
//        }
//        printf("\n");


        for (i = 0; i < pid_count; i++) {
            waitpid(pid_table[i], &status, WNOHANG | WUNTRACED);
        }


        command_array[total_command++] = NULL;

        pid = fork();

        if (pid == -1) {
            fprintf(stderr, "fork() failed\n");
            exit(1);

        } else if (pid == 0) {
//            printf("Inside child fork()\n");
            signal(SIGINT, SIG_DFL);

            if (is_write) {
                fd = open(write_file, O_CREAT | O_WRONLY, 0777);
                if (fd == -1) {
                    fprintf(stderr, "%s : Error while opening file.\n", write_file);
                    continue;
                }
                dup2(fd, 1);
                close(fd);
            }

            if (is_read) {
                fd = open(read_file, O_RDONLY);
                if (fd == -1) {
                    fprintf(stderr, "%s : No such file or directory.\n", read_file);
                    continue;
                }
                dup2(fd, 0);

                char *command_from_file = (char *) malloc(sizeof(char) * BUFFER_SIZE);

                getline(&command_from_file, &line_size, stdin);
                for (i = 0; i < total_command; i++) {
                    if (command_array[i] == NULL) {
                        break;
                    }
                }

                command = strtok(command_from_file, " \t\r\n\a");

                while (command != NULL) {
                    command_array[i] = command;
                    i++;
                    command = strtok(NULL, " \t\r\n\a");
                }

                close(fd);
            }

            if (execvp(command_array[0], command_array) == -1) {
                fprintf(stderr, "%s : command not found\n", command_array[0]);
            }


        } else if (pid > 0) {
//            printf("Inside Parent fork()\n");
            if (is_background) {
                printf("PID: %d running in background \n", pid);
                /*!
                 *  pid_table for processes which are running in background
                 */
                pid_table[pid_count] = pid;
                pid_count = pid_count + 1;
                continue;
            } else {
                do {
                    waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }
        }
    }
}
