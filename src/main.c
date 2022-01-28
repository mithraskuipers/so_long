#include "so_long.h"

/*
clear && make re && clear && ./so_long map1_5x5.ber
*/

static void	ft_exit_failure(char *s)
{
	ft_putstr_fd("Error\n", 2); // Don't touch this. Subject wants this.
	ft_putstr_fd(s, 2);
	exit (EXIT_FAIL);
}

static void	ft_map_failure(t_game *game, char *s)
{
	int	i;

	i = 0;
	while ((i < game->map.ntiles_y) && (game->map.map[i]))
	{
		free (game->map.map[i]);
		i++;
	}
	free (game->map.map);
	ft_exit_failure(s);
}

int		check_input_validity(int argc, char **argv)
{
	int	fd;
	if (argc < 2)
		ft_exit_failure("Please provide a map.");
	if (!(ft_strnstr(argv[1], ".ber", ft_strlen(argv[1]))))
		ft_exit_failure("Please provide a map with .ber extension.");
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		close(fd);
		ft_exit_failure("Error while reading the map. fd < 0");
	}
	close(fd);
	return (0);
}

static void	get_map_width(t_game *game)
{
	char	buff[1];
	int		len;
	int		nbytes;

	buff[0] = '\0';
	nbytes = 1;
	len = 0;
	while (nbytes)
	{
		nbytes = read(game->map.fd, buff, 1);
		if (buff[0] != '\n')
			len++;
		else
			break ;
	}
	game->map.ntiles_x = len;
}

static void	get_map_height(t_game *game)
{
	int fd;
	int i;

	fd = open(game->map.path, O_RDONLY);
	if (fd < 0)
	{
		close(fd);
		ft_exit_failure("File could not be opened.");
	}
	i = 1;
	while (get_next_line(game->map.fd))
		i++;
	game->map.ntiles_y = i;
	close(fd);
}

static void	check_map_rectangular(t_game *game)
{
	if (game->map.ntiles_x != game->map.ntiles_y)
		ft_map_failure(game, "Map is not rectangular.");
}

static void	check_map_contents1(t_game *game)
{
	int	i;
	int	j;

	i = 0;
	while (i < game->map.ntiles_y)
	{
		j = 0;
		while (j < game->map.ntiles_x)
		{
			j++;
			if (game->map.map[i][j] == 'P')
				(game->map.content.players)++;
			else if (game->map.map[i][j] == 'C')
				(game->map.content.collectables)++;
			else if (game->map.map[i][j] == 'E')
				(game->map.content.exits)++;
			else if ((!(game->map.map[i][j] != '0')) && \
			(!(game->map.map[i][j] != '1')))
				(game->map.content.invalids)++;
		}
		i++;
	}
}

static void	check_map_contents2(t_game *game)
{
	if (game->map.content.players > 1)
		ft_map_failure(game, "You map has more than 1 player spawnpoint.");
	if (game->map.content.players < 1)
		ft_map_failure(game, "You map does not have 1 player spawnpoint.");
}

static void	check_map_borders(t_game *game)
{
	int	i;
	int	j;

	i = 0;
	while (i < game->map.ntiles_y)
	{
		j = 0;
		if ((i == 0) || (i == (game->map.ntiles_y) - 1))
		{
			while (j < game->map.ntiles_x)
			{
				if (game->map.map[i][j] != '1')
					ft_map_failure(game, "Your map is not enclosed in borders");
				j++;
			}
		}
		else
		{
			if ((game->map.map[i][0] != '1') || \
			(game->map.map[i][game->map.ntiles_x - 1] != '1'))
					ft_map_failure(game, "Your map is not enclosed in borders");
			j++;
		}
		i++;
	}
}

static void	read_map_into_memory(t_game *game)
{
	game->map.fd = open(game->map.path, O_RDONLY);
	if (game->map.fd < 0)
	{
		close(game->map.fd);
		ft_exit_failure("Failing to read the map.");
	}
	int i;

	i = 0;
	game->map.map = (char **)malloc(sizeof(char *) * (game->map.ntiles_y + 1));
	if (!(game->map.map))
		ft_exit_failure("Malloc error.");
	while (i < game->map.ntiles_y)
	{
		game->map.map[i] = get_next_line(game->map.fd);
		i++;
	}
	close(game->map.fd);
}

static void	parse_map(t_game *game)
{
	game->map.fd = open(game->map.path, O_RDONLY);
	get_map_width(game);
	get_map_height(game);
	check_map_rectangular(game);
	game->px_x = game->map.ntiles_x * TILE_WIDTH;
	game->px_y = game->map.ntiles_y * TILE_WIDTH;
	read_map_into_memory(game);
	check_map_contents1(game);
	check_map_contents2(game);
	check_map_borders(game);
}

static void map_contents_init(t_game *game)
{
	game->map.content.players = 0;
	game->map.content.collectables = 0;
	game->map.content.exits = 0;
	game->map.content.invalids = 0;
}

static void xpm_init(t_game *game)
{
	game->img[BG].path = "./assets/bg.xpm";
	game->img[WALL].path = "./assets/wall.xpm";
	game->img[PLAYER].path = "./assets/player.xpm";
	game->img[WALL_UL].path = "./assets/wall_ul.xpm";
	game->img[WALL_UR].path = "./assets/wall_ur.xpm";
	game->img[WALL_LL].path = "./assets/wall_ll.xpm";
	game->img[WALL_LR].path = "./assets/wall_lr.xpm";
}

static void	xpm_loader(t_game *game)
{
	int k; // hoe werkt dit?
	int l; // hoe werkt dit??

	game->img[BG].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[BG].path, &k, &l);
	game->img[WALL].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[WALL].path, &k, &l);
	game->img[PLAYER].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[PLAYER].path, &k, &l);
	game->img[WALL_UL].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[WALL_UL].path, &k, &l);
	game->img[WALL_UR].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[WALL_UR].path, &k, &l);
	game->img[WALL_LL].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[WALL_LL].path, &k, &l);
	game->img[WALL_LR].mlx_img =  mlx_xpm_file_to_image(game->mlx.init, \
	game->img[WALL_LR].path, &k, &l);
}

static void	draw_bg(t_game *game)
{
	int x;
	int y;

	x = 0;
	y = 0;
	while (y < game->px_y)
	{
		x = 0;
		while (x < game->px_x)
		{
			mlx_put_image_to_window(game->mlx.init, game->mlx.win, \
			game->img[BG].mlx_img, x, y);
			x = x + TILE_WIDTH;
		}
		y = y + TILE_WIDTH;
	}
}

static void	superimpose_tile(t_game *game, int x, int y)
{
	int k = x * TILE_WIDTH;
	int l = y * TILE_WIDTH;
	if (game->map.map[x][y] == '1')
	{
		if ((x == 0) && (y == 0))
			mlx_put_image_to_window(game->mlx.init, game->mlx.win, game->img[WALL_UL].mlx_img, l, k);
		else if ((x == 0) && (y == (game->map.ntiles_y - 1)))
			mlx_put_image_to_window(game->mlx.init, game->mlx.win, game->img[WALL_UR].mlx_img, l, k);
		else if ((x == (game->map.ntiles_x - 1)) && (y == 0))
			mlx_put_image_to_window(game->mlx.init, game->mlx.win, game->img[WALL_LL].mlx_img, l, k);
		else if ((x == (game->map.ntiles_x - 1)) && (y == (game->map.ntiles_y - 1)))
			mlx_put_image_to_window(game->mlx.init, game->mlx.win, game->img[WALL_LR].mlx_img, l, k);
	}	
	else if (game->map.map[x][y] == 'P')
		mlx_put_image_to_window(game->mlx.init, game->mlx.win, game->img[PLAYER].mlx_img, l, k);
}


static void	draw_map(t_game *game)
{
	int	x;
	int	y;

	y = 0;
	draw_bg(game);
	while (y < (game->map.ntiles_y))
	{
		x = 0;
		while (x < (game->map.ntiles_x))
		{
			superimpose_tile(game, x, y);
			x = x + 1;
		}
		y = y + 1;
	}
}

int	main(int argc, char **argv)
{
	t_game	*game;

	game = malloc(sizeof(t_game));
	if (!(game))
		ft_exit_failure("Memory allocation issue.");
	game->map.path = argv[1];
	map_contents_init(game);
	check_input_validity(argc, argv);
	parse_map(game);
	game->mlx.init = mlx_init();
	game->mlx.win = mlx_new_window(game->mlx.init, game->px_x, game->px_y, "MITHRAS");
	xpm_init(game);
	xpm_loader(game);
	draw_map(game);
	mlx_loop(game->mlx.init);
	return (0);
}