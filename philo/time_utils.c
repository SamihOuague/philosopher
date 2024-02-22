/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 01:31:11 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/22 02:59:03 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	precision_sleep(int time_to_sleep)
{
	unsigned long	checkpoint;

	checkpoint = get_timestamp_ms();
	while ((get_timestamp_ms() - checkpoint) < time_to_sleep)
		usleep(500);
}

unsigned long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}
