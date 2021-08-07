/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtanigaw <dtanigaw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/29 03:31:37 by dtanigaw          #+#    #+#             */
/*   Updated: 2021/08/07 04:27:18 by dtanigaw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../so_long.h"

int	sl_get_color_from_img(t_img *img, int x, int y)
{
	return (*(int *)(img->addr + (y * img->line_len + x * (img->bpp / 8))));
}

void	sl_img_pixel_put(t_img *img, int x , int y, int color)
{
	char	*pixel;
	int		i;

	i = img->bpp - 8;
	pixel = img->addr + (y * img->line_len + x * (img->bpp / 8));
	while (i >= 0)
	{
		if (img->endian)
			*pixel++ = (color >> i) & 0xFF;
		else
			*pixel++ = (color >> (img->bpp - 8 - i)) & 0xFF;
		i -= 8;
	}
}

int		sl_render_colored_bloc(t_img *img, int color, int x, int y)
{
	int	i;
	int	j;

	i = 0;
	while (i < BLOC_LEN)
	{
		j = 0;
		while (j < BLOC_LEN)
		{
			sl_img_pixel_put(img, j + x, i + y, color);
			++j;
		}
		++i;
	}
	return (0);
}

void	sl_render_bloc_with_xpm(t_img *img, t_img *xpm_img, int x, int y)
{
	unsigned int	color;
	int	i;
	int	j;

	i = 0;
	while (i < BLOC_LEN)
	{
		j = 0;
		while (j < BLOC_LEN)
		{
			color = sl_get_color_from_img(xpm_img, j, i);
			sl_img_pixel_put(img, j + x, i + y, color);
			++j;
		}
		++i;
	}
}

void	sl_render_bkgd(t_env *env)
{
	char	**map;
	int		i;
	int		j;

	map = env->map;
	i = 0;
	while (i < env->height)
	{
		j = 0;
		while (j < env->width)
		{
			if (map[i][j] != '1')
				sl_render_colored_bloc(&env->bkgd, GREEN_PXL, BLOC_LEN * j, BLOC_LEN * i);
			else
				sl_render_bloc_with_xpm(&env->bkgd, &env->tex.wall,  BLOC_LEN * j,  BLOC_LEN * i);
			if (map[i][j] == '2' && map[env->p1.pos.y][env->p1.pos.x] != ITEM_BOMB)
				sl_render_bloc_with_xpm(&env->bkgd, &env->tex.bomb.item_bomb,  BLOC_LEN * j,  BLOC_LEN * i);
			++j;
		}
		++i;
	}
}

void	sl_handle_textures_while_moving(t_env *env, int delta_x, int delta_y)
{
	int	x;
	int	y;

	x = env->p1.pos.x + delta_x;
	y = env->p1.pos.y + delta_y;
	if (env->map[y][x] == ITEM_BOMB)
	{
		sl_render_colored_bloc(&env->bkgd, GREEN_PXL, BLOC_LEN * x, BLOC_LEN * y);
		env->map[y][x] = '0';
		++env->tex.bomb.collected;
		if (env->tex.bomb.collected == env->tex.bomb.to_collect)
			env->tex.exit.appear = true;
	}
	// not bombs
	if (env->map[y][x] == MAP_EXIT && env->tex.exit.appear == true)
	{
		printf("GAME CLEAR\n");
		exit(EXIT_SUCCESS);
	}
}

void	sl_animate_sprite(t_env *env, t_sprite *sprite, t_dlr *img, bool *state, int x, int y)
{
	static int	i;
	int			pos_x;
	int			pos_y;

	pos_x = sprite->pos.x + x;
	pos_y = sprite->pos.y + y;
	if (env->map[pos_y][pos_x] == WALL)
	{
		x = 0;
		y = 0;
	}
	if (i <= 800)
	{
		sprite->curr_state = &img->l;
		sprite->sub_pos.x = sprite->pos.x * BLOC_LEN + x * (BLOC_LEN / 3);
		sprite->sub_pos.y = sprite->pos.y * BLOC_LEN + y * (BLOC_LEN / 3);
		sl_handle_textures_while_moving(env, x, y);
	}
	if (i > 800)
	{
		sprite->curr_state = &img->r;
		sprite->sub_pos.x = sprite->pos.x * BLOC_LEN + x * (2 * (BLOC_LEN / 3));
		sprite->sub_pos.y = sprite->pos.y * BLOC_LEN + y * (2 * (BLOC_LEN / 3));
	}
	if (i == 1600)
	{
		if (x != 0 || y != 0)
			++env->p1.moves;
		sprite->curr_state = &img->def;
		sprite->pos.x += x;
		sprite->pos.y += y;
		sprite->sub_pos.x = sprite->pos.x * BLOC_LEN;
		sprite->sub_pos.y = sprite->pos.y * BLOC_LEN;
		*state = false;
		i = 0;
	}
	else
		++i;
}

void	sl_reveal_exit(t_env *env)
{
	t_exit		exit;
	t_img		*curr_state;
	static int	i;

	exit = env->tex.exit;
	curr_state = NULL;
	if (i <= 800)
		curr_state = &exit.state0;
	if (i > 800 && i <= 1600)
		curr_state = &exit.state1;
	if (i > 1600 && i <= 2400)
		curr_state = &exit.state2;
	if (i > 2400 && i <= 3200)
		curr_state = &exit.state3;
	if (i > 3200)
		curr_state = &exit.state4;
	if (i == 4000)
		curr_state = &exit.state5;
	else
		++i;
	mlx_put_image_to_window(env->mlx_ptr, env->win_ptr, curr_state->mlx_img, exit.pos.x * BLOC_LEN, exit.pos.y * BLOC_LEN);
}

void	sl_put_move_count_to_window(t_env *env)
{
	char	*count;
	char	*str;

	count = ft_itoa(env->p1.moves);
	mlx_string_put(env->mlx_ptr, env->win_ptr, 15, 15, 0xFFFFFF, count);
}

//put img to window (not render
int	sl_render(t_env *env)
{
	t_img	*img;
	
	mlx_put_image_to_window(env->mlx_ptr, env->win_ptr, env->bkgd.mlx_img, 0, 0);
	if (env->p1.curr_dir.up)
		sl_animate_sprite(env, &env->p1, &env->p1.img.up, &env->p1.curr_dir.up, 0, UP);
	if (env->p1.curr_dir.down)
		sl_animate_sprite(env, &env->p1, &env->p1.img.down, &env->p1.curr_dir.down, 0, DOWN);
	if (env->p1.curr_dir.left)
		sl_animate_sprite(env, &env->p1, &env->p1.img.left, &env->p1.curr_dir.left, LEFT, 0);
	if (env->p1.curr_dir.right)
		sl_animate_sprite(env, &env->p1, &env->p1.img.right, &env->p1.curr_dir.right, RIGHT, 0);
	if (env->tex.exit.appear == true)
		sl_reveal_exit(env);
	img = env->p1.curr_state;
	mlx_put_image_to_window(env->mlx_ptr, env->win_ptr, img->mlx_img, env->p1.sub_pos.x, env->p1.sub_pos.y);
	sl_put_move_count_to_window(env);
	return (0);
}
