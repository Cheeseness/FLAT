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


#include "Player.h"

Player::Player()
{
    gameOver = false;
    passingOut = false;

	time = 0.0f;

	hurtOverlay = DrawableBitmap("gfx/hurt.png");
	hurtOverlay.setPosition(vec2(640, 360));

	healthBitmap = al_load_bitmap("gfx/health.png");

	skateSound = al_load_sample("sfx/skate.ogg");
	painSounds[0] = al_load_sample("sfx/pain1.ogg");
	painSounds[1] = al_load_sample("sfx/pain2.ogg");
	painSounds[2] = al_load_sample("sfx/pain3.ogg");
	deathSound = al_load_sample("sfx/death.ogg");
	pickupSound = al_load_sample("sfx/pickup.ogg");

	lastDir = 0;

	healthpacks = 0;

	healthCD = 0.0f;
	health = 1.0f;
	loseTimer = LOSE_TIME;

	weaponPickup = -1;

	angle = 0.0f;
	tilt = 0.0f;
	position = vec3(0.0f, 0.0f, 0.0f);
	height = 0.0f;

	acceleration = vec2(0.0f, 0.0f);
	velocity = vec2(0.0f, 0.0f);

	frictionmodifier = 0.0f;

	lean = 0.0f;
	leanmodifier = 0.0f;

	holdingBack = false;
	jumping = false;

	//Setup Weapons:

	//PISTOL
	pistol = Weapon("gfx/gun.png", "sfx/gun.ogg");

	pistol.automatic = false;

	pistol.kickback = 0.002f;

	pistol.fireDamage = 0.2f;

	pistol.rounds = 1;
	pistol.spread = 0.4;

	pistol.standardPosition = vec2(960, 640);

	pistol.fireTimerMax = 0.15f;
	pistol.fireAnimationTimerMax = 0.3f;
	pistol.aimTimerMax = 0.7f;

	pistol.angleDuration = 0.8f;
	pistol.angleMultiplier = 0.15f;

	pistol.xKick = 70.0f;
	pistol.yKick = -80.0f;
	pistol.zKick = 0.25f;
	pistol.lingerKick = -30.0f;

	pistol.muzzlePosition = vec2(790, 420);

	pistol.muzzleScale = 1.0f;


	//SHOTGUN
	shotgun = Weapon("gfx/shotgun.png", "sfx/shotgun.ogg");

	shotgun.automatic = true;

	shotgun.kickback = 0.05f;

	shotgun.fireDamage = 0.04f;

	shotgun.rounds = 8;
	shotgun.spread = 1.5f;

	shotgun.standardPosition = vec2(800, 650);

	shotgun.fireTimerMax = 0.6f;
	shotgun.fireAnimationTimerMax = 0.5f;
	shotgun.aimTimerMax = 1.0f;

	shotgun.angleDuration = 1.5f;
	shotgun.angleMultiplier = 0.2f;

	shotgun.xKick = 200.0f;
	shotgun.yKick = 100.0f;
	shotgun.zKick = 0.4f;
	shotgun.lingerKick = 10.0f;

	shotgun.muzzlePosition = vec2(780, 490);

	shotgun.muzzleScale = 1.7f;


	//SMG
	smg = Weapon("gfx/smg.png", "sfx/smg.ogg");

	smg.automatic = true;

	smg.kickback = 0.003f;

	smg.fireDamage = 0.10f;

	smg.rounds = 1;
	smg.spread = 1.0f;

	smg.standardPosition = vec2(810, 610);

	smg.fireTimerMax = 0.1f;
	smg.fireAnimationTimerMax = 0.3f;
	smg.aimTimerMax = 0.4f;

	smg.angleDuration = 0.8f;
	smg.angleMultiplier = 0.1f;

	smg.xKick = 70.0f;
	smg.yKick = -10.0f;
	smg.zKick = 0.3f;
	smg.lingerKick = 10.0f;

	smg.muzzlePosition = vec2(720, 450);

	smg.muzzleScale = 0.7f;


	weaponList[0] = &pistol;
	weaponList[1] = &shotgun;
	weaponList[2] = &smg;

	nextWeaponIndex = 0;
	currentWeaponIndex = 0;
	currentWeapon = &pistol;

	weaponSwitchTimeout = WEAPON_SWITCH_TIME/2.0f;

	holdingJump = false;

	displayHealthTimer = 0.0f;
}

Player::~Player()
{
	//dtor
}

void Player::update() {



	if(displayHealthTimer >= 0) {
		displayHealthTimer -= TIME_STEP;
	}



	if(invurnabilityTimeout >= 0) {
		invurnabilityTimeout -= TIME_STEP;
	}


	currentWeapon->setPhase(lean*5.0f + random(0.03f*length(velocity)));


	//Weapon switching
	if(weaponSwitchTimeout > 0) {
		weaponSwitchTimeout -= TIME_STEP;

		if(currentWeaponIndex != nextWeaponIndex && weaponSwitchTimeout < WEAPON_SWITCH_TIME/2.0f) {
			currentWeaponIndex = nextWeaponIndex;
			currentWeapon = weaponList[currentWeaponIndex];

			//Make sure the next weapon cannot be fired.
			currentWeapon->setSwitching(true);
		}

		currentWeapon->setSwitchRatio(abs((weaponSwitchTimeout-WEAPON_SWITCH_TIME/2.0f)/(WEAPON_SWITCH_TIME/2.0f)));

		if(weaponSwitchTimeout <= 0) {
			currentWeapon->setSwitching(false);
		}
	}

	//Player movement
	float moveAngle = atan2(velocity.y, velocity.x);
	float forwardAngle = angle-moveAngle;

	float frontMultiplier = cos(forwardAngle);

	float frontMultiplierStop = frontMultiplier;

	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	if(frontMultiplierStop < 0) {
		frontMultiplierStop = 0;
	}



	//Jumping
	float jumpHeight = 0.0f;

	if(wantsToJump && holdingLeft) {
		vec2 direction = normalize(vec2(-sinAngle, cosAngle));
		float push = length(velocity) == 0 ? 0 : JUMP_PUSH - 2*JUMP_PUSH*(dot(normalize(velocity), direction));

		if(push < 0) {
			push = 0;
		}
		velocity += direction*push;
		jumpPhase = PI;
		jumpDTime = JUMP_DTIME;
		airHeight = JUMP_HEIGHT;
	}
	else if(wantsToJump && holdingRight) {
		vec2 direction = normalize(vec2(sinAngle, -cosAngle));
		float push = JUMP_PUSH - 2*JUMP_PUSH*(dot(normalize(velocity), direction));

		if(push < 0) {
			push = 0;
		}

		velocity += direction*push;
		jumpPhase = PI;
		jumpDTime = JUMP_DTIME;
		airHeight = JUMP_HEIGHT;
	}
	else if(wantsToJump) {
		jumpPhase = PI;
		jumpDTime = JUMP_DTIME;
		airHeight = JUMP_HEIGHT;
	}

	jumping = jumpPhase > 0;

	if(jumping) {
		jumpPhase -= jumpDTime;
		jumpHeight = -sin(jumpPhase)*airHeight;
	}



	//Ground movement
	float friction = FRICTION_BASE + frictionmodifier*frontMultiplier;

	if(jumping) {
		friction = 0.99f;
	}

	if(leanmodifier < 0 && lastDir == 1) {
		al_play_sample(skateSound, 0.8f, -1.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
	}

	if(leanmodifier > 0 && lastDir == -1) {
		al_play_sample(skateSound, 0.8f, 1.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
	}

	if(leanmodifier < 0) {
		lastDir = -1;
	}

	if(leanmodifier > 0) {
		lastDir = 1;
	}

	lean += leanmodifier;

	if((leanmodifier > 0.0f && lean < 0.3f) || (leanmodifier < 0.0f && lean > -0.3f)) {
		float absLean = (0.75f+0.25f*frontMultiplierStop*length(velocity))*lean*lean;

		//Allow acceleration backwards if holding S.
		if(holdingBack) {
			absLean *= -SPEED_BACKWARDS_MULTIPLIER;
		}

		velocity += vec2(cosAngle*SPEED*absLean, sinAngle*SPEED*absLean);
	}

	//Carving:
	//float carvingAngle = moveAngle+lean*0.01f;
	float carvingAngle = moveAngle + PI/2;
	float carvingSpeed = length(velocity)*lean*SPEED_CARVING*frontMultiplier;

	float deadzoneFrontMultiplier = frontMultiplier;

	if(deadzoneFrontMultiplier > 0.9f) {
		deadzoneFrontMultiplier = 1.0f;
	}

	float dLean = abs(leanmodifier*sin(lean*5.2f));

	//Print("l: ", dLean*abs(sin(lean*5.2f)));

	velocity += vec2(cos(carvingAngle)*carvingSpeed+deadzoneFrontMultiplier*cosAngle*abs(carvingSpeed)/4.0f, sin(carvingAngle)*carvingSpeed+deadzoneFrontMultiplier*sinAngle*abs(carvingSpeed)/4.0f);

	lean *= LEAN_DRAG;

	tilt = 0.5*lean;

	velocity *= friction;

	height += HEIGHT_ACCELERATE_AMMOUNT*frictionmodifier*frontMultiplier;

	height *= HEIGHT_DRAG;

	leanmodifier = 0.0f;
	frictionmodifier = 0.0f;

	holdingBack = false;

	velocity.x -= currentWeapon->kb*cosAngle;
	velocity.y -= currentWeapon->kb*sinAngle;

	position.x += velocity.x;
	position.y += velocity.y;
	position.z = height + jumpHeight + abs(tilt) - CHARACTER_HEIGHT;



	if(passingOut) {
		displayHealthTimer = 9999;

		loseTimer -= TIME_STEP;

		position.z += ((LOSE_TIME-loseTimer)/LOSE_TIME)*2.0f;

		if(loseTimer <= 0 && !gameOver) {
			gameOver = true;
			Print("Game over!");
		}
	}



	firing = currentWeapon->firing;

	currentWeapon->update();

	wantsToJump = false;
	holdingLeft = false;
	holdingRight = false;

	leanmodifier = 0.0f;

	time += TIME_STEP;

	healthCD -= TIME_STEP;
}

void Player::setVelocity(vec2 vel) {
	velocity = vel;
}

void Player::setPosition(vec2 pos) {
	position.x = pos.x;
	position.y = pos.y;
}

void Player::knockback(vec2 direction, float force, float air, float dTime) {
	velocity += direction*force;
	airHeight = air;
	jumpDTime = dTime;
	jumpPhase = PI;
	jumping = true;

	invurnabilityTimeout = INVURNABILITY_TIME;
}

bool Player::isJumping() {
	return jumping;
}

bool Player::isVurnable() {
	return invurnabilityTimeout < 0;
}

void Player::draw() {
	if(weaponPickup != -1  && currentWeaponIndex != -1) {
		currentWeapon->draw();
	}

	int blackoutAlpha = (int)(((LOSE_TIME - loseTimer)/LOSE_TIME)*255.0f);

/*	if(blackoutAlpha > 255) {
		blackoutAlpha = 255;
	}

	if(blackoutAlpha < 0) {
		blackoutAlpha = 0;
	}
*/
    //This allows the death screen to be shown
    if (blackoutAlpha > 255 || blackoutAlpha < 0)
    {
        blackoutAlpha = 0;
    }

	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	al_draw_filled_rectangle(0, 0, 1280, 720, al_map_rgba(100, 0, 0, blackoutAlpha));

	hurtOverlay.setTransparency(health + sin(time*4.0f)*(1.0f-health)/2);
	hurtOverlay.draw();
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	if(displayHealthTimer > 0) {
		for(int i = 0; i < healthpacks; i++) {
			al_draw_bitmap(healthBitmap, 50.0 + 90*i, 580, 0);
		}
	}
}

void Player::fire() {
	if(weaponPickup != -1 && currentWeaponIndex != -1 && loseTimer > (LOSE_TIME /2)) {
		currentWeapon->fire();
	}
}

void Player::jump() {
	if(jumping) {
		return;
	}

	wantsToJump = !holdingJump;
	holdingJump = true;
}

void Player::jumpRelease() {
	holdingJump = jumping;
}

void Player::holdForward() {
	frictionmodifier = FRICTION_MODIFIER_MULTIPLIER;
}

void Player::holdBackward() {
	frictionmodifier = -FRICTION_MODIFIER_MULTIPLIER;
	holdingBack = true;
}

void Player::holdLeft() {
	leanmodifier += LEAN_MULTIPLIER;

	holdingLeft = true;
}

void Player::holdRight() {
	leanmodifier += -LEAN_MULTIPLIER;

	holdingRight = true;
}

void Player::turn(float ratio) {
	angle -= ratio*TURN_SENSETIVITY;
}

vec3 Player::getCameraPosition() {
	return position;
}

float Player::getCameraAngle() {
	return angle;
}

float Player::getCameraTilt() {
	return tilt;
}

void Player::switchWeapon(int index) {

	if(weaponPickup < index) {
		return;
	}

	if(index == currentWeaponIndex || index == nextWeaponIndex) {
		return;
	}

	currentWeapon->setSwitching(true);

	nextWeaponIndex = index;

	weaponSwitchTimeout = WEAPON_SWITCH_TIME;
}

vec2 Player::getPos() {
	return vec2(position.x, position.y);
}

vec2 Player::getVel() {
	return velocity;
}

float Player::getWeaponDamage() {
	return currentWeapon->fireDamage;
}

int Player::getWeaponRounds() {
	return currentWeapon->rounds;
}

float Player::getWeaponSpread() {
	return currentWeapon->spread;
}

void Player::pickup() {
	if(weaponPickup == -1) {
		currentWeaponIndex = -1;
		nextWeaponIndex = -1;
	}

	displayHealthTimer = 3.0f;

	weaponPickup++;
	healthpacks += 3;

    al_play_sample(pickupSound, 0.8f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);


	if(weaponPickup <= 2) {
		switchWeapon(weaponPickup);
	}
}

void Player::takeDamage(float dmg) {
	health -= dmg;
	passingOut = health <= 0;
    int i = rand() % 3;
    if (health >= 0 && loseTimer >= 0) {
        Print("Ouch!");
        al_play_sample(painSounds[i], 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (!gameOver) {
        al_play_sample(deathSound, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

}

int Player::getHealth()
{
    return health;
}

void Player::heal() {
	if(gameOver || healthCD > 0.0f || healthpacks <= 0 || health == 1.0f) {
		return;
	}

	displayHealthTimer = 3.0f;

	healthpacks--;

	Print("Healed!");

	health = 1.0f;
	passingOut = false;
	loseTimer = LOSE_TIME;
	healthCD = 1.0f;
}
