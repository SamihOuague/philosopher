/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 06:44:10 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 06:45:37 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi(char *str)
{
	int	n;
	int	i;
	int	sign;

	n = 0;
	i = 0;
	sign = 1;
	if (*(str) == '-')
		sign *= -(++i);
	while (*(str + i) >= '0' && *(str + i) <= '9')
		n = (n * 10) + (*(str + (i++)) - '0');
	return (n * sign);
}

int	is_numeric(char *str)
{
	int	i;

	i = -1;
	while (*(str + (++i)) != '\0')
	{
		if (*(str + i) > '9' || *(str + i) < '0')
			return (0);
	}
	return (1);
}

void	ft_putstr_fd(char *str, int fd)
{
	int	i;

	i = -1;
	while (*(str + (++i)) != '\0')
		write(fd, (str + i), 1);
}

int	ft_strcpy(char *dest, char *src)
{
	int	i;

	i = -1;
	while (src[(++i)] != '\0')
		dest[i] = src[i];
	dest[i] = '\0';
	return (i);
}

int	*check_args(int argc, char **argv)
{
	int	i;
	int	*args;

	i = 0;
	args = malloc(sizeof(int) * 5);
	args[4] = -1;
	while ((++i) < argc)
	{
		args[i - 1] = ft_atoi(argv[i]);
		if (!is_numeric(argv[i])
			|| (args[i - 1] >= 1000000 || args[i - 1] <= 0))
		{
			free(args);
			args = NULL;
			return (NULL);
		}
	}
	return (args);
}
