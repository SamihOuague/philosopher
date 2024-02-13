/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:54:41 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/13 00:19:38 by souaguen         ###   ########.fr       */
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

unsigned long   get_timestamp_ms()
{
	struct timeval  tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}

void	ft_putstr_fd(char *str, int fd)
{
	int	i;

	i = -1;
	while (*(str + (++i)) != '\0')
		write(fd, (str + i), 1);
}
