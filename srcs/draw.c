#include "../include/cub3d.h"

void	get_step(t_data *data, t_paint *paint)
{
	paint->cos = cos(paint->angle * PI / 180);
	paint->sin =  sin(paint->angle * PI / 180);
	paint->delta_x = 1 / fabs(paint->cos);
	paint->delta_y = 1 / fabs(paint->sin);
	paint->map_x = (int)data->ply->x;
	paint->map_y = (int)data->ply->y;
	paint->step_y = -1;
	if (paint->sin > 0)
		paint->dis_y_side = (data->ply->y - paint->map_y) * paint->delta_y;
	else
	{
		paint->step_y = 1;
		paint->dis_y_side = (paint->map_y + 1 - data->ply->y) * paint->delta_y;
	}
	paint->step_x = 1;
	if (paint->cos > 0)
		paint->dis_x_side = (paint->map_x + 1 - data->ply->x) * paint->delta_x;
	else
	{
		paint->step_x = -1;
		paint->dis_x_side = (data->ply->x - paint->map_x) * paint->delta_x;
	}
}

void	get_side(t_data *data, t_paint  *paint)
{
	paint->side = 0;
	paint->hit = 0;
	while (paint->side == 0 && paint->hit == 0)
	{
		if (paint->dis_x_side > paint->dis_y_side)
		{
			paint->dis_x_side = paint->dis_x_side + paint->dis_x_side;
			paint->map_x = paint->map_x + paint->step_x;
			paint->side = 0;
		}
		else
		{
			paint->dis_y_side = paint->dis_y_side + paint->dis_y_side;
			paint->map_y = paint->map_y + paint->step_y;
			paint->side = 1;
		}
		if (data->map->map[paint->map_y][paint->map_x] == '1')
			paint->hit = 1;
		paint->wallx -= floor((paint->wallx));
		paint->drawstart = -paint->lineheight / 2 + WIN_HEIGHT / 2;
		if (paint->drawstart < 0)
			paint->drawstart = 0;
		paint->drawend = paint->lineheight / 2 + WIN_HEIGHT / 2;
		if (paint->drawend >= WIN_HEIGHT)
			paint->drawend = WIN_HEIGHT - 1;
	}
}

void	get_perp_wall(t_data *data, t_paint *paint)
{
	if (paint->side == 0)
		paint->perpwalldist = (paint->dis_x_side - paint->dis_x_delta) * cos
				((data->ply->angle - paint->angle) * PI / 180);
	else
		paint->perpwalldist = (paint->dis_y_side - paint->dis_y_delta) * cos
				((data->ply->angle - paint->angle) * PI / 180);
	paint->lineheight = (int)(WIN_HEIGHT / paint->perpwalldist / tan(60 / 2 *
			PI / 180));
	if (paint->side == 0)
		paint->wallx = data->ply->y - (paint->dis_x_side - paint->dis_x_delta) *
				paint->sin;
	else
		paint->wallx = data->ply->x + (paint->dis_y_side - paint->dis_y_delta)
				* paint->cos;
}

void put_wall(t_data *data, t_paint *paint)
{
	paint->y = 0;

}

void	draw_wall(t_data *data)
{
	t_paint paint;

	paint.start = data->ply->angle + 30;
	paint.end = data->ply->angle - 30;
	paint.step = 60 / WIN_WIDTH;
	paint.angle = paint.start;
	paint.x = 0;
	while (paint.angle > paint.end)
	{
		get_step(data, &paint);
		paint.angle = paint.angle - paint.step;
		get_side(data, &paint);
		get_perp_wall(data, &paint);
		put_wall(data, &paint);
	}
}

void	draw(t_data *data)
{
	draw_wall(data);
}