/*
Copyright 2012 Johan "SteelRaven7" Hassel
Copyright 2012 Josh "Cheeseness" Bush

This file is part of FLAT.

FLAT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

FLAT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FLAT.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Pickup.h"

Pickup::Pickup()
{
	time = 0.0f;
}

Pickup::Pickup(DrawStack3D* world3D, vec3 _position) {

	DrawableBitmap b[8];

	b[0] = DrawableBitmap("gfx/capsule00.png");
	b[1] = DrawableBitmap("gfx/capsule01.png");
	b[2] = DrawableBitmap("gfx/capsule02.png");
	b[3] = DrawableBitmap("gfx/capsule03.png");
	b[4] = DrawableBitmap("gfx/capsule04.png");
	b[5] = DrawableBitmap("gfx/capsule05.png");
	b[6] = DrawableBitmap("gfx/capsule06.png");
	b[7] = DrawableBitmap("gfx/capsule07.png");

    skyBeam = al_load_bitmap("gfx/skyBeam.png");

	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 8; j++) {
			b[j].setReflection(true);
			pushImage(b[j]);
		}
	}

	_position.z = -1.0f;
	setPosition(_position);

	world3D->push(this);

	entityType = 1;

	setPrescale(5.0f);

    //Start playing the hum silently. We'll increase its volume as we get closer.
    humSample = al_load_sample("sfx/pickup_spawn.ogg");
    al_play_sample(humSample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
}

Pickup::~Pickup()
{
	//dtor
}

void Pickup::draw() {
	time += TIME_STEP;

	if(!visible) {
		return;
	}
	frames.draw();

	//We don't need this rectangle anymore :3
//	al_draw_filled_rectangle(cposition.x-5, cposition.y+10, cposition.x + 5, -20000, al_map_rgba(200, 200, 255, 200));
    //Or this circle
//	al_draw_filled_circle(cposition.x, cposition.y - 800*cscale, cscale*mod(time, 1.0f)*2500.0f, al_map_rgba(255, 100, 100, 150));

    float dAngle = rotation;

    while(dAngle > PI) {
        dAngle -= 2*PI;
    }

    while(dAngle < -PI) {
        dAngle += 2*PI;
    }


    float dx = (dAngle*900.0f*(0.9f+0.1f*abs(sin(dAngle))));

    //TODO: Someone cleverer than me is going to need to clear up the perceived vertical bounce - Cheese
    al_draw_tinted_scaled_rotated_bitmap(skyBeam, al_map_rgba(255, 255, 255, 255), al_get_bitmap_width(skyBeam)/2, 600+(cscale*5000), cposition.x, 360.0f + sin(rotation)*dx, 50.0f*cscale, 1.0f, rotation*0.95f, 0);

}

void Pickup::setImageParameters(vec2 _position, float _scale, float _rotation) {
	update();

	int len = frames.getNumber();
	float angle = playerAngle;
	rotation = _rotation;

	//Clamp angle between 0 and 2*PI
	angle += PI;

	frames.setIndex((int)(angle/(2*PI)*((float)len)));

	frames.setImageParameters(_position, _scale*scale, _rotation);
	cposition = _position;
	cscale = _scale;
}

void Pickup::remove() {

	destroy = true;
}
