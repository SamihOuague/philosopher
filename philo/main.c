/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:54:41 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/08 10:31:17 by souaguen         ###   ########.fr       */
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
	{
		sign *= -1;
		i++;
	}
	while (*(str + i) >= '0' && *(str + i) <= '9')
		n = (n * 10) + (*(str + (i++)) - '0');
	return (n * sign);
}

void	ft_putstr_fd(char *str, int fd)
{
	int	i;

	i = -1;
	while (*(str + (++i)) != '\0')
		write(fd, (str + i), 1);
}

void	*forks_init(unsigned int n_philo)
{
	int	i;
	t_fork	*forks;

	i = -1;
	forks = malloc(sizeof(t_fork) * n_philo);
	if (forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
		forks[i].free_fork = 0;
	return (forks);
}

void	*philo_info_init(int argc, char **argv)
{
	int		i;
	int		n_philo;
	t_thread_info	*philo;
	t_fork		*forks;

	i = -1;
	n_philo = ft_atoi(argv[1]);
	philo = malloc(sizeof(t_thread_info) * n_philo);
	forks = forks_init(n_philo);
	if (philo == NULL || forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
	{
		philo[i].id = i + 1;
		philo[i].time_to_die = ft_atoi(argv[2]);
		philo[i].time_to_eat = ft_atoi(argv[3]);
		philo[i].time_to_sleep = ft_atoi(argv[4]);
		philo[i].n_time_eat = -1;
		philo[i].n_fork = n_philo;
		philo[i].forks = forks;
		if (argc == 6)
			philo[i].n_time_eat = ft_atoi(argv[5]);
	}
	return (philo);
}

int	philo_mutex_init(t_thread_info *philo, unsigned int n_philo)
{
	int	i;

	i = -1;
	while ((++i) < n_philo)
	{
		if (pthread_mutex_init(&philo[i].eat_lock, NULL) == 0)
			return (0);
		if (pthread_mutex_init(&philo[i].die_lock, NULL) == 0)
			return (0);
		if (pthread_mutex_init(&philo[i].sleep_lock, NULL) == 0)
			return (0);
		if (pthread_mutex_init(&philo[i].forks[i].mut, NULL) == 0)
			return (0);
	}
	return (1);
}

void	philo_mutex_destroy(t_thread_info *philo, unsigned int n_philo)
{
	int	i;

	i = -1;
	while ((++i) < n_philo)
	{
		pthread_mutex_destroy(&philo[i].eat_lock);
		pthread_mutex_destroy(&philo[i].die_lock);
		pthread_mutex_destroy(&philo[i].sleep_lock);
		pthread_mutex_destroy(&philo[i].forks[i].mut);
	}
}



int	main(int argc, char **argv)
{
	t_thread_info	*philo;
	unsigned int	n_philo;
	unsigned int	i;

	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Usage: ./philo [N philos] [time to die] [time to eat] ", 2);
		ft_putstr_fd("[time to sleep] (optional)[N time to eat]\n", 2);
		return (1);
	}	
	n_philo = ft_atoi(argv[1]);
	philo = philo_info_init(argc, argv);
	philo_mutex_init(philo, n_philo);
	philo_mutex_destroy(philo, n_philo);
	free(philo[0].forks);
	free(philo);
	return (0);
}
