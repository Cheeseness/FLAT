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


#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <cstdio>
#include <vector>
#include "GraphicsManager.h"
#include "AudioManager.h"
#include "utilities.h"
#include "DrawableLine.h"
#include "DrawableBitmap.h"
#include "DrawStack3D.h"
#include "DrawableText.h"
#include "Character.h"
#include "Player.h"
#include "Floor.h"
#include "DetailObject.h"
#include "Projectile.h"
#include "StaticObject.h"
#include "DragonHead.h"
#include "DragonSpine.h"
#include "Pickup.h"
#include "Splat.h"

#define KEY_FORWARD 0
#define KEY_BACKWARD 2
#define KEY_LEFT 1
#define KEY_RIGHT 3
#define KEY_JUMP 4
#define KEY_WEAPON1 5
#define KEY_WEAPON2 6
#define KEY_WEAPON3 7
#define KEY_WEAPON4 8
#define KEY_HEAL 9
#define KEY_FIRE 10
#define KEY_LOOKLEFT 11
#define KEY_LOOKRIGHT 12
#define KEY_HEAL_ALT 13
#define KEY_FIRE_ALT 14
#define KEY_SWAP_SCENERY 15


#define PROJECTILE_DAMAGE 0.2f


#define DIGITAL_DRAG_RELEASE 0.60
#define DIGITAL_DRAG_HOLD 0.985
#define DIGITAL_ACCELERATION 0.2f

#define DETAIL_OBJECT_CREATION_DISTANCE 390.0f
#define DETAIL_OBJECT_CREATION_DISTANCE_RANDOM 100.0f
#define DETAIL_OBJECT_NUMBER 800

#define STATIC_OBJECT_PERCENTAGE 25

#define SAFE_RADIUS 200.0f

#define MAX_ENEMY_NUMBER 1

#define ENEMY_SPAWN_DISTANCE 300.0f
#define ENEMY_SPAWN_DISTANCE_RANDOM 90.0f

#define PICKUP_TIME 15

#ifndef PI
#define PI 3.14159265
#endif

class Game
{
	public:
		Game();
		virtual ~Game();

		int initialize();
		int startUpdateLoop(int _fps);

		GraphicsManager* getGraphicsMan();
		AudioManager* getAudioMan();
	private:
		int fps;

		int frame;

		bool safe;

		bool paused;

		ALLEGRO_EVENT_QUEUE *event_queue;
		ALLEGRO_TIMER *updateTimer;
		ALLEGRO_TIMER *fpsCounter;
		ALLEGRO_KEYBOARD_STATE *keyboard_state;
		ALLEGRO_FONT* font;

		ALLEGRO_BITMAP* splashStart;
		ALLEGRO_BITMAP* splashPause;
		ALLEGRO_BITMAP* splashDeath;
		ALLEGRO_BITMAP* splashEnd;

		DrawableBitmap projectileBitmapStarburst;
		DrawableBitmap projectileBitmapHalo;
		DrawableBitmap projectileBitmapStarburst2;
		DrawableBitmap projectileBitmapHalo2;
		DrawableBitmap detailBitmap[18];

		DrawableBitmap crystalA[16];
		DrawableBitmap crystalB[16];
		DrawableBitmap crystalC[16];
		DrawableBitmap splatBitmap[4];
		DrawableBitmap grunt[16][7];

		DragonHead* dragonHead;

		vector <Character*> npc;
		vector <Projectile*> projectile;
		vector <DetailObject*> detail;
		vector <DetailObject*> stat;
		vector <Pickup*> pickup;
		vector <Splat*> splat;

		GraphicsManager* graphicsMan;
		AudioManager* audioMan;

		DrawStack3D* world3D;
		Floor* floor;

		DrawableText* fpsText;
		DrawableText* hintText;

		Player* player;

		float digitalTurn;
		float digitalDrag;

		int key[16];
		bool keyrel(int k);

		void update();
		void draw();

		void updateMouse();

		bool testcase;
		bool quit;
		bool grabMouse;

		float pickupTimer;

		void loadBitmaps();

		void doStuff();

		vec2 lastPosition;

		bool bossSpawned;

		float pickups;

		float splashTimer;

		bool end;
		bool splash;

		FrameSuperContainer generateCharacterSuperContainer();
};

#endif // GAME_H
