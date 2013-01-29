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


#include "DragonHead.h"

DragonHead::DragonHead() {}

DragonHead::DragonHead(DrawStack3D* world3D, vector <Character*> *npc)
{
	roar1 = al_load_sample("sfx/dragon_roar_2.ogg");
	roar2 = al_load_sample("sfx/dragon_roar_1.ogg");
	deathSample = al_load_sample("sfx/dragon_death.ogg");
	painSample = al_load_sample("sfx/dragon_pain.ogg");
	ambience = al_load_sample("sfx/dragon_ambience.ogg");

	splashEnd = al_load_bitmap("gfx/splash_end.png");

	roar1Timeout = 30.0f;
	roar2Timeout = 10.0f;

	health = 40.0f;

	time = 0.0f;

	stage = 0;

	entityType = 2;

	enemyType = TYPE_DRAGON;

	headFrames = FrameContainer();

	DrawableBitmap b[16];

	invurnable = true;

	dragon = true;

	b[0] = DrawableBitmap("gfx/dragon_head_red00.png");
	b[1] = DrawableBitmap("gfx/dragon_head_red01.png");
	b[2] = DrawableBitmap("gfx/dragon_head_red02.png");
	b[3] = DrawableBitmap("gfx/dragon_head_red03.png");
	b[4] = DrawableBitmap("gfx/dragon_head_red04.png");
	b[5] = DrawableBitmap("gfx/dragon_head_red05.png");
	b[6] = DrawableBitmap("gfx/dragon_head_red06.png");
	b[7] = DrawableBitmap("gfx/dragon_head_red07.png");
	b[8] = DrawableBitmap("gfx/dragon_head_red08.png");
	b[9] = DrawableBitmap("gfx/dragon_head_red09.png");
	b[10] = DrawableBitmap("gfx/dragon_head_red10.png");
	b[11] = DrawableBitmap("gfx/dragon_head_red11.png");
	b[12] = DrawableBitmap("gfx/dragon_head_red12.png");
	b[13] = DrawableBitmap("gfx/dragon_head_red13.png");
	b[14] = DrawableBitmap("gfx/dragon_head_red14.png");
	b[15] = DrawableBitmap("gfx/dragon_head_red15.png");

	for(int i = 0; i < 16; i++) {
		b[i].setReflection(true);
		headFrames.push(b[i]);
	}

	world3D->push(this);
	npc->push_back(this);

	b[0] = DrawableBitmap("gfx/dragon_body_red00.png");
	b[1] = DrawableBitmap("gfx/dragon_body_red01.png");
	b[2] = DrawableBitmap("gfx/dragon_body_red02.png");
	b[3] = DrawableBitmap("gfx/dragon_body_red03.png");
	b[4] = DrawableBitmap("gfx/dragon_body_red04.png");
	b[5] = DrawableBitmap("gfx/dragon_body_red05.png");
	b[6] = DrawableBitmap("gfx/dragon_body_red06.png");
	b[7] = DrawableBitmap("gfx/dragon_body_red07.png");
	b[8] = DrawableBitmap("gfx/dragon_body_red08.png");
	b[9] = DrawableBitmap("gfx/dragon_body_red09.png");
	b[10] = DrawableBitmap("gfx/dragon_body_red10.png");
	b[11] = DrawableBitmap("gfx/dragon_body_red11.png");
	b[12] = DrawableBitmap("gfx/dragon_body_red12.png");
	b[13] = DrawableBitmap("gfx/dragon_body_red13.png");
	b[14] = DrawableBitmap("gfx/dragon_body_red14.png");
	b[15] = DrawableBitmap("gfx/dragon_body_red15.png");

	FrameContainer spineFrames = FrameContainer();

	for(int i = 0; i < 16; i++) {
		b[i].setReflection(true);
		spineFrames.push(b[i]);
	}

	for(int i = 0; i < DRAGON_SPINE_LENGTH; i++) {

		DragonSpine* dragonSpine = new DragonSpine();

		for(int j = 0; j < 16; j++) {
			dragonSpine->setFrameContainer(spineFrames);
		}

		dragonSpine->setPosition(getPosition()+ vec3(1.0f*i, 0.0f, 0.0f));

		world3D->push(dragonSpine);
		spine.push_back(dragonSpine);
		npc->push_back(dragonSpine);
	}
}

DragonHead::~DragonHead()
{
	//dtor
}

void DragonHead::setPlayer(Player* _player) {
	player = _player;
}

void DragonHead::setImageParameters(vec2 _position, float _scale, float _rotation) {
	updateIndex();

	if(stage == 1) {

		int GBtint = (int)(((health)/40.0f)*255.0f);

		headFrames.setTint(255, GBtint, GBtint);
	}

	if(stage == 2) {
		headFrames.setTint(255, 128, 128);
	}

	if(stage == 3) {
		headFrames.setTint(255, 128, 128);
	}

	if(stage == 4) {
		headFrames.setTint(255, 128, 128);
	}

	headFrames.setImageParameters(_position, _scale*18.0f, _rotation);
	headFrames.setImageFloatingHeight(-80.0f - 50.0f*position.z);
}

void DragonHead::updateIndex() {
	int len = headFrames.getNumber();
	float angle = playerAngle - facingAngle + PI/16 + PI;

	while(angle < 0) {
		angle += 2*PI;
	}
	while(angle >= 2*PI) {
		angle -= 2*PI;
	}

	headFrames.setIndex((int)(angle/(2*PI)*((float)len)));
}

void DragonHead::draw() {
	if(!visible) {
		return;
	}
	headFrames.draw();
}

void DragonHead::setPlayerAngle2(float angle) {
	playerAngle = angle;
}

void DragonHead::dealDamage(float howMuch) {
	if(invurnable) {
		return;
	}

	health -= howMuch;

	if(health < 0 && stage == 1) {
		invurnable = true;
		stage = 2;
		health = 10.0f;
		fireTimer = DRAGON_FLEE_FIRE_STARTUP_TIME;

		for(int i = 0; i < DRAGON_SPINE_LENGTH; i++) {
			spine[i]->setVurnability(true);
		}
	}

	if(roar2Timeout <= 0) {
		al_play_sample(roar2, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
		roar2Timeout = 15.0f + random(8.0f);
	}

	if(health < 0 && stage != 1) {
		destroy = true;



/*		al_rest(1.0f);
		al_draw_bitmap(splashEnd, 0, 0, 0);
		al_flip_display();

		al_rest(10.0f);
*/
	} else {
	    float amp = (10.0f/playerDistance);

        if(amp > 1.0) {
            amp = 1.0f;
        }

        if (!destroy)
        {
            al_play_sample(painSample, amp, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
        } else {
            al_play_sample(deathSample, amp, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
	}
}


bool DragonHead::isInvurnable() {
	return invurnable;
}

void DragonHead::update() {

	time += TIME_STEP;

	float waveAmplitude = DRAGON_WAVE_AMPLITUDE;


	if(roar1Timeout > 0) {
		roar1Timeout -= TIME_STEP;
	}

	if(roar2Timeout > 0) {
		roar2Timeout -= TIME_STEP;
	}

	if(roar1Timeout <= 0) {
		float amp = (20.0f/playerDistance);

		if(amp > 1.0) {
			amp = 1.0f;
		}

		al_play_sample(roar1, amp, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
		roar1Timeout = 30.0f + random(20.0f);
	}

	//Roaming stage
	if(stage == 0) {
		waveAmplitude = DRAGON_ROAM_WAVE_AMPLITUDE;
		position.z = -50.0f + DRAGON_ROAM_WAVE_AMPLITUDE*sin(time*5.0f) + 10.0f*sin(time/2.0f);
		velocity.x = DRAGON_ROAM_CENTRAL_FORCE*sin(time);
		velocity.y = DRAGON_ROAM_CENTRAL_FORCE*cos(time);

		if(playerDistance < 100.0f && playerDistance != 0) {
			stage = 1;
			invurnable = false;
			al_play_sample(roar1, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
			al_play_sample(ambience, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, NULL);
			roar1Timeout = 30.0f + random(20.0f);
		}
	}

	//Attacking stage
	if(stage == 1) {
		if(position.z < -1.0f + DRAGON_WAVE_AMPLITUDE*sin(time*5.0f)) {
			position.z += 0.2f;
		} else {
			position.z = -1.0f + DRAGON_WAVE_AMPLITUDE*sin(time*5.0f);
		}

		float acc = DRAGON_BASE_ACCELERATION + (0.5+sin(time*2.0f)*0.5)*sin(time/2.0f)*DRAGON_EXTRA_ACCELERATION;

		float ratio = playerDistance/DRAGON_MIN_CHARGE_DISTANCE;

		float friction = DRAGON_FRICTION;

		if(ratio > 1.0f) {
			ratio = 1.0f;
		}

		friction += (1.0f-ratio)*0.1;

		if(friction > 1.0f) {
			friction = 1.0f;
		}

		acc*=ratio;

		velocity.x -= acc*cos(playerAngle);
		velocity.y -= acc*sin(playerAngle);

		velocity.x *= friction;
		velocity.y *= friction;

		if(length(velocity) > DRAGON_TERMINAL_VELOCITY) {
			velocity = normalize(velocity) * DRAGON_TERMINAL_VELOCITY;
		}

		if(playerDistance < DRAGON_KNOCKBACK_RANGE && player->isVurnable()) {
			player->knockback(vec2(-cos(playerAngle), -sin(playerAngle)), DRAGON_KNOCKBACK_FORCE, DRAGON_KNOCKBACK_AIR, DRAGON_KNOCKBACK_DAIRTIME);
			player->takeDamage(DRAGON_ATTACK_DAMAGE);
		}
	}

	//Fleeing stage
	if(stage == 2) {
            float terminalVelocity = DRAGON_FLEE_TERMINAL_VELOCITY * ((spine.size() + DRAGON_SPINE_LENGTH))/DRAGON_SPINE_LENGTH;
            if (playerDistance <= DRAGON_FLEE_MAX_DISTANCE)
            {
                velocity.x += DRAGON_FLEE_ACCELERATION*cos(playerAngle) + random(DRAGON_FLEE_JERKING_ACCELERATION);
                velocity.y += DRAGON_FLEE_ACCELERATION*sin(playerAngle) + random(DRAGON_FLEE_JERKING_ACCELERATION);

                if(length(velocity) > terminalVelocity) {
                    velocity = normalize(velocity) * terminalVelocity;
                }
            } else {
                velocity.x = 0;
                velocity.y = 0;
                Print("Dragon too far away! D:");
            }

            position.z = -1.0f + DRAGON_WAVE_AMPLITUDE*sin(time*5.0f);

            if(spine.size() == 0) {
                stage = 3;
                invurnable = false;
            }

            if(fireTimer < 0) {
                firing = true;
                fireTimer = DRAGON_FLEE_FIRE_TIMER;
            } else {
                firing = false;
                fireTimer -= TIME_STEP;
            }
	}

	//Crashlanding stage
	if(stage == 3) {

		firing = false;

		if(position.z >= 0.2) {
			position.z = 0.2;
			velocity.x *= 0.97;
			velocity.y *= 0.97;
		}
		else {
			position.z += 0.03f;
		}

		if(length(velocity) < 0.01f) {
			writheOffset = time*5.0f;
			stage = 4;
		}
	}

	//Finish it! stage
	if(stage == 4) {
		firing = false;

		float jump = sin(time*5.0f-writheOffset);

		if(jump < 0) {
			jump = 0;
			velocity.x *= 0.95;
			velocity.y *= 0.95;

			vec2 dpos = vec2(cos(facingAngle)+0.015*cos(playerAngle), sin(facingAngle)+0.015*sin(playerAngle));
			facingAngle = atan2(dpos.y, dpos.x);
		}
		else {
			velocity.x = DRAGON_WRITHE_SPEED*cos(facingAngle);
			velocity.y = DRAGON_WRITHE_SPEED*sin(facingAngle);
		}

		position.z = 0.2 - DRAGON_WRITHE_AIR*jump;
	}
	else {
		facingAngle = atan2(velocity.y, velocity.x);
	}

	position.x += velocity.x;
	position.y += velocity.y;





	//Tail solving. IT'S IK!

	vec3 lastPosition = position;
	float lastPhase = time*5.0f;
	float dPhase = -PI/4;

	for(int i = 0; i < spine.size(); i++) {

		if(spine[i]->unlinked || spine[i]->destroy) {
			spine.erase(spine.begin()+i);

			//Increase length of the next spine element, more than 2.0f to make it "jerk around" a bit.
			if(i != spine.size()) {
				spine[i]->distance = 3.0f;
			}

			i--;
			continue;
		}

		vec3 deltaPosition = normalize(spine[i]->getPosition()-lastPosition)*spine[i]->distance;

		spine[i]->distance = ((spine[i]->distance-1.0f)*0.97)+1.0f;

		lastPosition += deltaPosition;
		lastPhase += dPhase;

		spine[i]->setFacingAngle(atan2(deltaPosition.y, deltaPosition.x));

		spine[i]->setPosition(vec3(lastPosition.x, lastPosition.y, lastPosition.z + 0.1f*waveAmplitude*sin(lastPhase)));
		spine[i]->setVelocity(deltaPosition*-0.3f);
	}
}
