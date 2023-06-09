#include "shell.h"

/**
 * main - entry point, makes the infinite loop
 * and rediredt to the execve functioni
 * @av: passed argument list
 * @ac: av count
 * @env: the enviroment list
 *
 * Return: 0
 */
int main(int ac, char **av, char **env)
{
	data_t data_struct, *data = &data_struct;

	(void)ac;
	(void)av;

	signal(SIGINT, &handle_sig);

	init_data(data, ac, av, env);

	shell_loop(data, ac, av);

	writechar(BUF_FLUSH);
	return (0);
}

/**
 * init_data - initialization the data
 * @data: data_t struct holds commands
 * @av: passed argument
 * @ac: av counter
 * @env: env list
 *
 */
void init_data(data_t *data, int ac, char **av, char **env)
{
	int envc, i;

	(void)ac;

	data->cmd_count = 1;
	data->run_cmd = NULL;
	data->get_cmd = NULL;
	data->cmd = NULL;
	data->env = NULL;
	data->prog_name = av[0];

	if (env)
	{
		envc = 0;
		while (env[envc])
			envc++;
		data->env = malloc(sizeof(char *) * (envc + 1));

		for (i = 0; env[i]; i++)
			data->env[i] = full_strdup(env[i]);
		data->env[i] = NULL;
	}
}

/**
 * clear_getline - if the last char is '\n', remove it
 * @str: the string from getline
 *
 * Return: str after clearing it
 */
char *clear_getline(char *str)
{
	size_t last = _strlen(str) - 1;

	str[last] = str[last] == 10 ? '\0' : str[last];

	return (str);
}

/**
 * shell_loop - the shell's infinte loop
 * @data: data_t struct holds commands
 * @av: passed argument
 * @ac: av counter
 *
 */
void shell_loop(data_t *data, int ac, char **av)
{
	int error_code = 0, cmd_len, acc = ac;
	size_t BUF = 1024;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			writestr(SHELL_MSG);
		if (acc == 1)
		{
			error_code = getline(&data->get_cmd, &BUF, stdin);
			if (error_code == -1 ||  error_code == EOF)
			{
				free_data(data);
				exit(0);
			}
			clear_getline(data->get_cmd);
			if ((_strstr(data->get_cmd, "exit")) != NULL)
			{
				free_data(data);
				writechar(BUF_FLUSH);
				exit(0);
			}
			if (check_space(data->get_cmd))
			{
				free_used_data(data);
				continue;
			}
			filter_cmd(data);
		}
		else
			args(data, ac, av);
		acc = 1;

		get_location(data);
		cmd_len = _strlen(data->cmd[0]);
		exe(data);
		if (cmd_len >= 1)
			free_used_data(data);
		data->cmd_count++;
	}
}

/**
 * handle_sig - check is there is CTRL C entered
 * @sig: signal code
 *
 */
void handle_sig(int sig)
{
	(void)sig;

	writechar(10);
	writestr(SHELL_MSG);
}
