#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dragon.h"
#include <errno.h>
#include "dshlib.h"

/****
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 *
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 *
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 *
 *   Also, use the constants in the dshlib.h in this code.
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 *
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 *
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 *
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int execute_pipeline(command_list_t *clist)
{
    int num_commands = clist->num;
    pid_t pids[num_commands];
    int pipes[num_commands - 1][2];

    // create all pipes
    for (int i = 0; i < num_commands - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // create processes for each command
    for (int i = 0; i < num_commands; i++)
    {
        int in_fd = -1, out_fd = -1;

        if (clist->commands[i].input_file)
        {
            in_fd = open(clist->commands[i].input_file, O_RDONLY);
            if (in_fd < 0)
            {
                perror("Error opening input file");
                exit(EXIT_FAILURE);
            }
        }

        // Check for output redirection
        if (clist->commands[i].output_file)
        {
            int flags = O_WRONLY | O_CREAT;
            if (clist->commands[i].append_mode)
                flags |= O_APPEND;
            else
                flags |= O_TRUNC;

            out_fd = open(clist->commands[i].output_file, flags, 0644);
            if (out_fd < 0)
            {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
        }

        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) // child process
        {
            // input pipe for all except first process
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            // if redirection exists overwrite input
            if (in_fd != -1)
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }

            // output pipe for all except last process
            if (i < num_commands - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            // same thing for output
            if (out_fd != -1)
            {
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }

            // close all pipe ends in child
            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // execute commands
            if (strcmp(clist->commands[i].argv[0], "dragon") == 0)
            {
                print_dragon();
                exit(OK);
            }
            else
            {
                execvp(clist->commands[i].argv[0], clist->commands[i].argv);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }

        // close file descriptors
        if (in_fd != -1)
            close(in_fd);
        if (out_fd != -1)
            close(out_fd);
    }

    // parent process: close all pipe ends
    for (int i = 0; i < num_commands - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // wait for all children
    for (int i = 0; i < num_commands; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char *token;
    int rc;
    int cmd_count = 0;

    // initialize clist
    memset(clist, 0, sizeof(command_list_t));

    if (strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS;
    }

    // tokenize on pipes
    token = strtok(cmd_line, "|");

    while (token != NULL)
    {
        if (cmd_count >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }

        // for each pipe command call build_cmd_buff and populate clist
        rc = build_cmd_buff(token, &clist->commands[cmd_count]);
        if (rc != OK)
        {
            return rc;
        }
        cmd_count++;
        token = strtok(NULL, "|");
    }

    // finalize clist construction and return OK
    clist->num = cmd_count;
    return OK;
}

// same thing as prior assignment except addition of redirection
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd)
{
    if (cmd_line == NULL)
    {
        return WARN_NO_CMDS;
    }

    if (strlen(cmd_line) >= SH_CMD_MAX)
    {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    // remove leading spaces
    while (isspace((unsigned char)*cmd_line))
    {
        cmd_line++;
    }

    // remove trailing spaces
    char *end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && isspace((unsigned char)*end))
    {
        *end-- = '\0';
    }

    if (*cmd_line == '\0')
    {
        return WARN_NO_CMDS;
    }

    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_mode = false;

    char *token = cmd_line;
    int in_quotes = 0;
    char quote_char = '\0';

    while (*token)
    {
        // skip space outside of quotes
        while (*token && isspace((unsigned char)*token) && !in_quotes)
        {
            token++;
        }

        if (*token == '\0')
            break;

        // check for redirections
        if (*token == '<')
        {
            token++;
            while (*token && isspace((unsigned char)*token))
                token++; // skip spaces between redirection and filename
            cmd->input_file = strdup(token);
            while (*token && !isspace((unsigned char)*token))
                token++; // go past filename
            continue;
        }
        else if (*token == '>')
        {
            token++;
            if (*token == '>')
            {
                cmd->append_mode = true;
                token++;
            }
            while (*token && isspace((unsigned char)*token))
                token++;
            cmd->output_file = strdup(token);
            while (*token && !isspace((unsigned char)*token))
                token++;
            continue;
        }

        // check if arg starts with quote
        if (*token == '"' || *token == '\'')
        {
            in_quotes = 1;
            quote_char = *token;
            token++;
        }

        char *arg_start = token;
        char *arg_end = token;

        while (*arg_end)
        {
            if (in_quotes)
            {
                // check for closing quote
                if (*arg_end == quote_char)
                {
                    in_quotes = 0;
                    *arg_end = '\0';
                    arg_end++;
                    break;
                }
            }
            else if (isspace((unsigned char)*arg_end))
            {
                *arg_end = '\0';
                arg_end++;
                break;
            }
            arg_end++;
        }

        // store info in struct
        if (cmd->argc >= ARG_MAX - 1)
        {
            return ERR_TOO_MANY_COMMANDS;
        }
        cmd->argv[cmd->argc] = strdup(arg_start);
        if (cmd->argv[cmd->argc] == NULL)
        {
            return ERR_MEMORY;
        }
        cmd->argc++;
        token = arg_end;
    }
    // last argv will be NULL
    cmd->argv[cmd->argc] = NULL;
    return OK;
}

int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    command_list_t clist;

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        // remove trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // handle exit directly
        if (strcmp(cmd_buff, EXIT_CMD) == 0)
        {
            break;
        }

        // build command list
        rc = build_cmd_list(cmd_buff, &clist);
        if (rc == WARN_NO_CMDS)
        {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }
        else if (rc == ERR_TOO_MANY_COMMANDS)
        {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }
        else if (rc == ERR_MEMORY)
        {
            fprintf(stderr, "Memory allocation error.\n");
            return ERR_MEMORY;
        }
        else if (rc == ERR_CMD_OR_ARGS_TOO_BIG)
        {
            fprintf(stderr, "Too many characters.\n");
            continue;
        }

        // handle cd and dragon in parent process
        if (clist.num == 1 && strcmp(clist.commands[0].argv[0], "cd") == 0)
        {
            if (clist.commands[0].argv[1] == NULL)
            {
                continue;
            }
            else if (chdir(clist.commands[0].argv[1]) != 0)
            {
                perror("cd");
            }
            continue;
        }
        if (clist.num == 1 && strcmp(clist.commands[0].argv[0], "dragon") == 0)
        {
            print_dragon();
            continue;
        }

        rc = execute_pipeline(&clist);
        if (rc == EXIT_SC)
        {
            break;
        }
    }
    return OK;
}
