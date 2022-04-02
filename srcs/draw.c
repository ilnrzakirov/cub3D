#include "../include/cub3d.h"
void	get_step(t_data *data, t_paint *paint)
{
	paint->cos = cos(paint->angle * PI / 180);
	paint->sin = sin(paint->angle * PI / 180);
	paint->dis_x_delta = 1 / fabs(paint->cos);
	paint->dis_y_delta = 1 / fabs(paint->sin);
	paint->map_y = (int)data->ply->y;
	paint->map_x = (int)data->ply->x;
	paint->step_y = -1;
	if (paint->sin > 0)
		paint->dis_y_side = (data->ply->y - paint->map_y) * paint->dis_y_delta;
	else
	{
		paint->step_y = 1;
		paint->dis_y_side = (paint->map_y + 1 - data->ply->y) *
							paint->dis_y_delta;
	}
	paint->step_x = 1;
	if (paint->cos > 0)
		paint->dis_x_side = (paint->map_x + 1 - data->ply->x) *
							paint->dis_x_delta;
	else
	{
		paint->step_x = -1;
		paint->dis_x_side = (data->ply->x - paint->map_x) * paint->dis_x_delta;
	}
}

void	get_side(t_data *data, t_map *map, t_paint *paint, t_ply *ply)
{
	paint->hit = 0;
	paint->side = 0;
	while (paint->hit == 0)
	{
		if (paint->dis_x_side < paint->dis_y_side)
		{
			paint->dis_x_side += paint->dis_x_delta;
			paint->map_x += paint->step_x;
			paint->side = 0;
		}
		else
		{
			paint->dis_y_side += paint->dis_y_delta;
			paint->map_y += paint->step_y;
			paint->side = 1;
		}
		if (map->map[paint->map_y][paint->map_x] == '1')
			paint->hit = 1;
	}
}

void	get_perp_wall(t_data *data, t_paint *paint) {
	if (paint->side == 0)
		paint->perpwalldist = (paint->dis_x_side - paint->dis_x_delta) \
 * cos((data->ply->angle - paint->angle) * PI / 180);
	else
		paint->perpwalldist = (paint->dis_y_side - paint->dis_y_delta) \
 * cos((data->ply->angle - paint->angle) * PI / 180);
	paint->lineheight = (int) (WIN_HEIGHT / paint->perpwalldist \
 / tan(60 / 2 * PI / 180));
	if (paint->side == 0)
		paint->wallx = \
            data->ply->y -
			(paint->dis_x_side - paint->dis_x_delta) * paint->sin;
	else
		paint->wallx = \
            data->ply->x +
			(paint->dis_y_side - paint->dis_y_delta) * paint->cos;
	paint->wallx -= floor((paint->wallx));
	paint->drawstart = -paint->lineheight / 2 + WIN_HEIGHT / 2;
	if (paint->drawstart < 0)
		paint->drawstart = 0;
	paint->drawend = paint->lineheight / 2 + WIN_HEIGHT / 2;
	if (paint->drawend >= WIN_HEIGHT)
		paint->drawend = WIN_HEIGHT - 1;
}

void	draw_ceiling(t_data *data, t_paint *paint)
{
	int	color;

	color = ((data->map->c[0]) & 0xff) + ((data->map->c[1]) & 0xff << 8) +
			((data->map->c[2]) & 0xff << 16);
	while (paint->y < paint->drawstart)
	{
		data->img.addr[paint->y * WIN_WIDTH + paint->x] = color;
		paint->y++;
	}
}

void	draw_flour(t_data *data, t_paint *paint)
{
	int color;

	color = ((data->map->f[0]) & 0xff) + ((data->map->f[1]) & 0xff << 8) +
			((data->map->f[2]) & 0xff << 16);
	while (paint->y < WIN_HEIGHT)
	{
		data->img.addr[paint->y * WIN_WIDTH + paint->x] =
				color;
		paint->y++;
	}
}

void	draw_wall(t_data *data,	t_map *map, t_paint *paint)
{
	while (paint->y < paint->drawend)
	{
		paint->texy = (int)paint->texpos & (map->tex->width - 1);
		paint->texpos += paint->step;
		if (paint->side == 1 && paint->map_y < data->ply->y)
			paint->tn = 0;
		else if (paint->side == 1)
			paint->tn = 1;
		else if (paint->side == 0 && paint->map_x < data->ply->x)
			paint->tn = 2;
		else
			paint->tn = 3;
		data->img.addr[paint->y * WIN_WIDTH + paint->x] = map->tex[paint->tn]
				.data[paint->texy * map->tex->width + paint->texx];
		paint->y++;
	}
}

void put_wall(t_data *data, t_paint *paint)
{
	paint->y = 0;
	draw_ceiling(data, paint);
	paint->texx = (int)(paint->wallx * data->map->tex->height);
	if ((paint->side == 0 && paint->cos > 0) || \
		(paint->side == 1 && paint->sin < 0))
		paint->texx = data->map->tex->width - paint->texx - 1;
	paint->step = (double)data->map->tex->height / paint->lineheight;
	paint->texpos = (paint->drawstart -	WIN_HEIGHT / 2 + paint->lineheight /
															2) * paint->step;
	draw_wall(data, data->map, paint);
	draw_flour(data, paint);
}

void	wall(t_data *data)
{
	t_paint 	paint;

	paint.start = data->ply->angle + 60 / 2;
	paint.end = data->ply->angle - 60 / 2;
	paint.step = 60 / WIN_WIDTH;
	paint.angle = paint.start;
	paint.x = 0;
	while (paint.angle > paint.end)
	{
		get_step(data, &paint);
		get_side(data, data->map, &paint, data->ply);
		get_perp_wall(data, &paint);
		put_wall(data, &paint);
		data->ply->buffer[paint.x] = paint.perpwalldist / cos(
				(data->ply->angle - paint.angle) * PI / 180);
		paint.angle -= paint.step;
		paint.x++;
	}
}

void	draw(t_data *data)
{
	wall(data);
	mlx_put_image_to_window(data->mlx, data->win, data->img.img, 0, 0);
}