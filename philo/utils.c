/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:54:41 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/29 01:06:57 by souaguen         ###   ########.fr       */
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

void	init_track(unsigned long **last_meal, int **n_time_eat, int size)
{
	int	i;

	i = -1;
	*n_time_eat = NULL;
	*last_meal = NULL;
	*n_time_eat = malloc(sizeof(int) * size);
	if (*n_time_eat == NULL)
		return ;
	*last_meal = malloc(sizeof(unsigned long) * size);
	if (*last_meal == NULL)
	{
		free(*n_time_eat);
		*n_time_eat = NULL;
		return ;
	}
	(*last_meal)[0] = get_timestamp_ms();
	while ((++i) < size)
	{
		(*n_time_eat)[i] = 0;
		(*last_meal)[i] = *last_meal[0];
	}
}
