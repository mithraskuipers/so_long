#include <stdio.h>
#include <fcntl.h>
#include "../libftplusplus/libft.h"
#include "../mlx/mlx.h"
#include "get_next_line.h"


#define EXIT_FAIL 1
#define TILE_WIDTH 32
#define N_IMAGES 3

typedef struct s_mlx
{
	void		*init;
	void		*win;
}				t_mlx;

typedef struct	s_content
{
	int			players;
	int			collectables;
	int			exits;
	int			invalids;
}				t_content;

typedef struct	s_map
{
	t_content	content;
	char		*filepath;
	char		**map;
	int			fd;
	int			ntiles_x;
	int			ntiles_y;
}				t_map;

typedef	struct	s_game
{
	t_mlx		mlx;
	t_map		map;
	int			px_x;
	int			px_y;
}				t_game;