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


#include "Character.h"

Character::Character()
{

    fireSample = al_load_sample("sfx/zap_2.ogg");
    skateSample = al_load_sample("sfx/enemy_skate.ogg");
    painSample = al_load_sample("sfx/enemy_pain.ogg");
    deathSample = al_load_sample("sfx/enemy_death.ogg");

	dragon = false;

	chaseTimer = GRUNT_CHASE_TIME;

	usingContainer = false;

	visible = true;

	invurnable = false;

	targetPosition = vec2(0.0f, 0.0f);
	targetVelocity = vec2(0.0f, 0.0f);

	enemyType = TYPE_GRUNT;

	entityType = 1;

	friction = 0.98f;

	playerAngle = 0.0f;
	playerDistance = 0.0f;

	surroundDir = random(GRUNT_CIRCLE_SPEED);

	health = 1.5f;

	fireTimer = 0.5f;

	firing = false;

	facingAngle = 0.0f;

	animationFrame = 0.0f;

	headingAngle = 0.0f;
}

Character::~Character()
{
	//dtor
}

void Character::update() {
	if(enemyType == TYPE_GRUNT) {
		updateGruntAI();
	}

	position.x += velocity.x;
	position.y += velocity.y;

	velocity *= friction;
}

void Character::updateTarget(vec2 _targetPosition, vec2 _targetVelocity) {
	targetPosition = _targetPosition;
	targetVelocity = _targetVelocity;
}

void Character::draw() {

	if(visible) {
		if(!usingContainer) {
			bitmap.draw();
		}
		else {
			container.draw();
		}
	}
}

void Character::setVisibility(bool _visible) {
	visible = _visible;
}

void Character::setBitmap(DrawableBitmap _bitmap) {
	bitmap = _bitmap;
	bitmap.setReflection(true);
}

void Character::setProjectileStarburstBitmap(DrawableBitmap _bitmap) {
	projectileStarburstBitmap = _bitmap;
	projectileStarburstBitmap.setReflection(true);
}

void Character::setProjectileHaloBitmap(DrawableBitmap _bitmap) {
	projectileHaloBitmap = _bitmap;
	projectileHaloBitmap.setReflection(true);
}

DrawableBitmap Character::getBitmap() {
	return bitmap;
}

vec3 Character::getPosition() {
	return position;
}

float Character::getFacingAngle() {
	return angle;
}

void Character::setPosition(vec3 _position) {
	position = _position;
}

void Character::setFacingAngle(float _angle) {
	angle = _angle;
}

void Character::setImageParameters(vec2 _position, float _scale, float _rotation) {

	if(usingContainer) {
		int len = container.getNumber();
		float angle = rotationAngle + headingAngle;

		//Clamp angle between 0 and 2*PI
		angle += PI;

		while(angle < 0) {
			angle += 2*PI;
		}

		while(angle >= 2*PI) {
			angle -= 2*PI;
		}

		container.setIndex((int)(angle/(2*PI)*((float)len)));

		container.setSubIndex((int)animationFrame);

		container.setImageParameters(_position, _scale, _rotation);
	}
	else {
		bitmap.setPosition(_position);
		bitmap.setScale(_scale*7.0f);if(animationFrame >= FRAME_NUMBER) {
		animationFrame -= FRAME_NUMBER;
	}
		bitmap.setRotation(_rotation);
	}
}

float Character::getPlayerAngle() {
	return playerAngle;
}

void Character::setPlayerAngle(float angle) {
	playerAngle = angle;
}

float Character::getPlayerDistance() {
	return playerDistance;
}

void Character::setPlayerDistance(float distance) {
	playerDistance = distance;
}

void Character::dealDamage(float howMuch) {
	health -= howMuch;

	destroy = health <= 0;

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

bool Character::isFiring() {
	return firing;
}

float Character::getProjectileSpeed() {
	if(enemyType == TYPE_GRUNT) {
		return GRUNT_PROJECTILE_SPEED;
	}
	else if(enemyType == TYPE_DRAGON) {
		return DRAGON_PROJECTILE_SPEED;
	}
	return 0.1f;
}

Projectile* Character::getProjectile() {
	Projectile* p = new Projectile();
	p->setStarburstBitmap(projectileStarburstBitmap);
	p->setHaloBitmap(projectileHaloBitmap);

	p->setPosition(position);

	return p;
}


bool Character::isInvurnable() {
	return invurnable;
}

void Character::setFrame(FrameSuperContainer c) {
	container = c;
	usingContainer = true;
}

void Character::useContainer() {
	usingContainer = true;
}

void Character::setPlayerAngle2(float angle) {
	rotationAngle = angle;
}


//AI Routines:

void Character::updateGruntAI() {

	float boost = 0.0f;

	if(chaseTimer > 0) {
		boost = GRUNT_CHASE_BOOST;
		chaseTimer -= TIME_STEP;
	}

	vec2 pos = vec2(position.x, position.y);

	vec2 dp = targetPosition - pos;
	float distance = length(dp);
	vec2 dpNormalized = dp/distance;
	vec2 dpNormalizerPerpendicular = vec2(-dpNormalized.y, dpNormalized.x);

	if(distance > 30.0f) {
		friction = GRUNT_CRUISE_FRICTION;
		velocity += dpNormalized*(GRUNT_CRUISE_SPEED+boost+random(GRUNT_CRUISE_SPEED_RANDOM)) + dpNormalizerPerpendicular*surroundDir*0.5f;
	}
	else if(distance > 12.0f) {
		friction = GRUNT_BRAKE_FRICTION;
		velocity += dpNormalized*GRUNT_BRAKE_SPEED + dpNormalizerPerpendicular*surroundDir*0.3f;
	}
	else {
		friction = GRUNT_CIRCLE_FRICTION;
		velocity += dpNormalizerPerpendicular*(surroundDir+random(0.005));
	}

	animationFrame += ANIMATION_SPEED*length(velocity);

	firing = false;
	if(distance < GRUNT_FIRE_RANGE) {
		if(fireTimer > 0.0f) {
			fireTimer-=TIME_STEP;
		} else if (playerDistance > 12.01f) {
			fireTimer = GRUNT_FIRE_TIMER + random(GRUNT_FIRE_TIMER_RANDOM);

            float amp = (10.0f/playerDistance);

            if(amp > 1.0) {
                amp = 1.0f;
            }

            al_play_sample(fireSample, amp, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);

			firing = true;
		}
	}

	if(animationFrame >= FRAME_NUMBER) {
		animationFrame -= FRAME_NUMBER;
	}

	if (animationFrame > 7.9f && animationFrame < 8.0f)
	{
            float amp = (20.0f/playerDistance);

            if(amp > 1.0) {
                amp = 1.0f;
            }
            al_play_sample(skateSample, amp, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);

	}

	headingAngle = atan2(-velocity.y, velocity.x);
}
