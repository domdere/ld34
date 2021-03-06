#include <camera.h>
#include <whitgl/logging.h>

whitgl_fvec space_camera_point(whitgl_fvec p, space_camera cam)
{
	whitgl_fvec half_screen_size = whitgl_fvec_scale_val(cam.screen_size, 0.5);
	p = whitgl_fvec_sub(p, cam.pos);
	p = whitgl_fvec_scale_val(p, cam.scale);
	p = whitgl_fvec_add(p, half_screen_size);
	return p;
}

space_camera space_camera_update(space_camera c, space_camera_focus focus, whitgl_ivec screen_size, whitgl_fvec camera_offset)
{
	if(focus.num_foci == 0)
	{
		c.pos = whitgl_fvec_add(c.pos, c.speed);
		c.speed = whitgl_fvec_scale_val(c.speed, 0.99);
	}

	whitgl_fvec total = whitgl_fvec_zero;
	whitgl_faabb bounds = focus.foci[0];
	whitgl_int i;
	for(i=0; i<focus.num_foci; i++)
	{
		if(focus.foci[i].a.x < bounds.a.x) bounds.a.x = focus.foci[i].a.x;
		if(focus.foci[i].a.y < bounds.a.y) bounds.a.y = focus.foci[i].a.y;
		if(focus.foci[i].b.x > bounds.b.x) bounds.b.x = focus.foci[i].b.x;
		if(focus.foci[i].b.y > bounds.b.y) bounds.b.y = focus.foci[i].b.y;
		total.x += (focus.foci[i].a.x+focus.foci[i].b.x)/2;
		total.y += (focus.foci[i].a.y+focus.foci[i].b.y)/2;
	}
	if(bounds.a.x == bounds.b.x) bounds.b.x += 0.1;
	if(bounds.a.y == bounds.b.y) bounds.b.y += 0.1;
	whitgl_fvec center = whitgl_fvec_divide_val(total, focus.num_foci);
	whitgl_fvec screen_size_units = whitgl_fvec_divide_val(c.screen_size, c.scale);
	whitgl_fvec unit_offset = whitgl_fvec_scale(screen_size_units, camera_offset);
	center = whitgl_fvec_add(center, unit_offset);
	whitgl_fvec old_pos = c.pos;
	c.pos = whitgl_fvec_interpolate(c.pos, center, 0.075);
	c.speed = whitgl_fvec_sub(c.pos, old_pos);

	whitgl_fvec size = whitgl_fvec_sub(bounds.b, bounds.a);
	whitgl_fvec best_scale = whitgl_fvec_divide(whitgl_ivec_to_fvec(screen_size), size);
	whitgl_float target_scale = whitgl_fmin(best_scale.x, best_scale.y)*0.8;
	target_scale = whitgl_fclamp(target_scale, 1, 128);
	c.scale = c.scale*0.97 + target_scale*0.03;
	return c;
}
