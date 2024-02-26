/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/26 06:09:57 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

typedef struct s_philo
{
	unsigned long	last_meal;
	unsigned long	started_at;
	int	id;
	int	n_fork;
	pid_t	pid;
	sem_t	*forks;
	sem_t	*msg_lock;
	sem_t	*fork_lock;
	sem_t	*die_lock;
	t_list	**msg_queue;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	n_time_eat;
	int	*args;
}	t_philo;

typedef struct s_monitor
{
	pthread_t		thread;
	sem_t	*msg_lock;
	t_philo			*philos;
	t_list			**msg_queue;
	unsigned long	started_at;
	unsigned int	n_philo;	
	int				*deadbeef;
}	t_monitor;

typedef struct s_msg
{
	unsigned int	id;
	unsigned int	status;
	unsigned long	timestamp;
}	t_msg;

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*cursor;

	cursor = lst;
	while ((*cursor).next != NULL)
		cursor = (*cursor).next;
	return (cursor);
}

t_list	*ft_lstnew(void *content)
{
	t_list	*lst;

	lst = malloc(sizeof(t_list));
	if (lst == NULL)
		return (NULL);
	(*lst).content = content;
	(*lst).next = NULL;
	return (lst);
}

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = ft_lstlast(*lst);
	(*last).next = new;
}

t_list	*ft_pop(t_list **lst)
{
	t_list	*tmp;

	if (lst == NULL || *lst == NULL)
		return (NULL);
	tmp = *lst;
	*lst = (**lst).next;
	(*tmp).next = NULL;
	return (tmp);
}

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

unsigned long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}

void	send_msg(t_philo *self, int status)
{
	unsigned long	t;
	char		*str;

	str = "";
	if ((*(*self).die_lock).__align == 0)
		return ;
	sem_wait((*self).msg_lock);
	t = get_timestamp_ms();
	if (status == 1)
		str = "has taken a fork";
	else if (status == 0)
		str = "is thinking";
	else if (status == 2)
		str = "is eating";
	else if (status == 3)
		str = "is sleeping";
	printf("%ld %d %s\n", t - (*self).started_at, (*self).id, str);
	sem_post((*self).msg_lock);
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

int	precision_sleep(int time_to_sleep, t_philo *self)
{
	unsigned long	checkpoint;

	checkpoint = get_timestamp_ms();
	while ((get_timestamp_ms() - checkpoint) < time_to_sleep)
	{
		if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
			return (1);
		usleep(500);
	}
	return (0);
}

int	wait_forks(t_philo *self)
{
	sem_t	*forks;

	forks = (*self).forks;
	if ((*forks).__align > (*self).n_fork)
		return (1);
	while ((*forks).__align < 2)
	{
		if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
			break ;
		usleep(1000);
	}
	sem_wait((*self).forks);
	send_msg(self, 1);
	sem_wait((*self).forks);
	send_msg(self, 1);
	return (0);
}

int	philo_proc(t_philo *philo, int index)
{
	pid_t	pid;
	int		i;
	int		current_fork;
	int		next_fork;
	int		exit_code;
	char		n[6];
	t_philo		*self;

	self = &philo[index];
	i = 0;
	pid = fork();
	if ((*self).id % 2 == 0)
		usleep(1000);
	if (pid == 0)
	{
		i = -1;
		(*self).last_meal = get_timestamp_ms();
		while ((++i) != (*self).n_time_eat)
		{	
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;
			send_msg(self, 0);
			if ((*self).n_fork <= 1)
			{
				sem_wait((*self).forks);
				send_msg(self, 1);
				usleep((*self).time_to_die * 1000);	
				sem_post((*self).forks);
				break ;
			}	
			sem_wait((*self).fork_lock);
			if ((*(*self).die_lock).__align == 0)
				break ;
			if (wait_forks(self))
				break ;
			sem_post((*self).fork_lock);

			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;

			(*self).last_meal = get_timestamp_ms();

			send_msg(self, 2);	
			if (precision_sleep((*self).time_to_eat, self))
				break ;
			
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;	
			send_msg(self, 3);
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;	
			sem_post((*self).forks);
			sem_post((*self).forks);
			if (precision_sleep((*self).time_to_sleep, self))
				break ;
		}
		sem_post((*self).fork_lock);
		sem_post((*self).forks);
		sem_post((*self).forks);
		exit_code = 0;	
		if (i != (*self).n_time_eat)
			exit_code = (*self).id;
		sem_close((*self).forks);
		sem_unlink("forks1");
		free((*self).args);
		sem_close((*self).msg_lock);
		sem_unlink("msg_lock");
		sem_close((*self).fork_lock);
		sem_unlink("fork_lock");	
		sem_close((*self).die_lock);
		sem_unlink("die_lock");
		free(philo);
		exit(exit_code);
	}
	return (pid);
}


int	ft_strlen(char *str)
{
	int	i;
	
	i = 0;
	while (*(str + i) != '\0')
		i++;
	return (i);
}

t_philo	*init_philo(int *args)
{
	t_philo	*philo;
	int		i;

	if (args == NULL)
		return (NULL);
	philo = malloc(sizeof(t_philo) * args[0]);
	if (philo == NULL)
		return (NULL);
	i = -1;
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].time_to_die = args[1];
		philo[i].time_to_eat = args[2];
		philo[i].time_to_sleep = args[3];
		philo[i].n_time_eat = args[4];
		philo[i].n_fork = args[0];
		philo[i].args = args;
	}
	return (philo);
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


int	main(int argc, char **argv)
{	
	unsigned long	t;
	unsigned long	started_at;
	int	i;
	char	n[6];
	char	*name;
	sem_t	*forks;
	sem_t	*msg_sem;
	sem_t	*fork_sem;
	sem_t	*die_sem;
	t_philo	*philo;
	int	*args;
	int	deadbeef;
	int	exit_status;
	t_monitor	monitor;
	pid_t		pid;

	if (!(argc == 5 || argc == 6))
		return (1);
	
	deadbeef = 0;
	i = -1;
	memset(n, '\0', 6);
	ft_strcpy(n, "fork0");
	started_at = get_timestamp_ms();	
	args = check_args(argc, argv);
	philo = init_philo(args);
	if (philo == NULL)
		return (1);	
	sem_unlink("forks1");	
	sem_unlink("fork_lock");	
	sem_unlink("die_lock");
	forks = sem_open("forks1", O_CREAT, 0666, args[0]);
	fork_sem = sem_open("fork_lock", O_CREAT, 0666, 1);
	die_sem = sem_open("die_lock", O_CREAT, 0666, 1);
	i = -1;
	if (philo == NULL)
		return (1);
	sem_unlink("msg_lock");
	msg_sem = sem_open("msg_lock", O_CREAT, 0666, 1);	
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].forks = forks;
		philo[i].msg_lock = msg_sem;
		philo[i].started_at = started_at;
		philo[i].fork_lock = fork_sem;
		philo[i].die_lock = die_sem;
		if (philo_proc(philo, i) == 0)
			break ;
	}
	i = -1;
	while ((++i) < args[0])
	{	
		pid = waitpid(-1, &exit_status, 0);	
		if (WEXITSTATUS(exit_status) != 0 && !deadbeef)
		{
			sem_wait(msg_sem);
			printf("%ld %d died\n", get_timestamp_ms() - started_at, WEXITSTATUS(exit_status));
			deadbeef = 1;
			sem_post(msg_sem);
			sem_wait(die_sem);
		}
	}
	i = -1;
	sem_close(forks);
	sem_unlink("forks1");
	sem_close(msg_sem);
	sem_unlink("msg_lock");
	sem_close(fork_sem);
	sem_unlink("fork_lock");
	sem_close(die_sem);
	sem_unlink("die_lock");
	free(args);
	free(philo);
	return (0);
}
