/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 01:31:11 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/29 06:02:13 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	precision_sleep(int time_to_sleep, t_philo *self)
{
	unsigned long	checkpoint;

	checkpoint = get_timestamp_ms();
	while ((int)(get_timestamp_ms() - checkpoint) <= time_to_sleep)
	{
		if ((int)(get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
			return (1);
		usleep(1000);
	}
	return (0);
}

unsigned long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}
