#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include "dragon.h"
#include <errno.h>

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

int parse_command(char *cmd_buff, cmd_buff_t *cmd)
{
	if (cmd_buff == NULL)
	{
		return WARN_NO_CMDS;
	}

	if (strlen(cmd_buff) >= SH_CMD_MAX)
	{
		return ERR_CMD_OR_ARGS_TOO_BIG;
	}

	// remove leading spaces
	while (isspace((unsigned char)*cmd_buff))
	{
		cmd_buff++;
	}

	// remove trailing spaces
	char *end = cmd_buff + strlen(cmd_buff) - 1;
	while (end > cmd_buff && isspace((unsigned char)*end))
	{
		*end-- = '\0';
	}

	if (*cmd_buff == '\0')
	{
		return WARN_NO_CMDS;
	}

	cmd->argc = 0;
	char *token = cmd_buff;
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

		// check if arg starts withquote
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
	cmd_buff_t cmd;

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

		rc = parse_command(cmd_buff, &cmd);
		if (rc == WARN_NO_CMDS)
		{
			printf("%s\n", CMD_WARN_NO_CMD);
			continue;
		}
		else if (rc == ERR_TOO_MANY_COMMANDS)
		{
			printf("%s\n", CMD_ERR_PIPE_LIMIT);
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

		// command handling
		// exit
		if (strcmp(cmd.argv[0], EXIT_CMD) == 0)
		{
			break;
		}
		// dragon
		else if (strcmp(cmd.argv[0], "dragon") == 0)
		{
			print_dragon();
			continue;
		}
		// cd
		else if (strcmp(cmd.argv[0], "cd") == 0)
		{
			if (cmd.argc > 1)
			{
				if (chdir(cmd.argv[1]) != 0)
				{
					perror("cd");
				}
			}
			// unlike regular cd just do nothing
			continue;
		}
		// non built-in commands
		// fork exec pattern
		else
		{
			pid_t pid = fork();
			if (pid == 0)
			{
				execvp(cmd.argv[0], cmd.argv);
				int err = errno;
				// went through relevant ones from https://man7.org/linux/man-pages/man3/errno.3.html
				// attempted to recreate some with tests, couldn't figure out all
				switch (err)
				{
				case ENOENT:
					fprintf(stderr, "Command not found in PATH\n");
					break;
				case EACCES:
					fprintf(stderr, "Permission denied\n");
					break;
				case E2BIG:
					fprintf(stderr, "Argument list too long\n");
					break;
				case EINVAL:
					fprintf(stderr, "Invalid argument\n");
					break;
				case EPERM:
					fprintf(stderr, "Operation not permitted");
					break;
				default:
					perror(CMD_ERR_EXECUTE);
				}
				exit(err);
			}
			// parent
			else if (pid > 0)
			{
				// ec cont.
				// int prev_exit_status;
				int status;
				wait(&status);
				// check if process terminated normally (not signal) not using prev_exit_status for now
				if (WIFEXITED(status))
				{
					continue;
					// prev_exit_status = WEXITSTATUS(status); commenting out to remove error
				}
			}
			else
			{
				perror("fork");
			}
		}
	}
	return OK;
}
