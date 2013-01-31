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


#include "Game.h"

Game::Game()
{
	testcase = false;
	quit = false;

	grabMouse = false;

	keyboard_state = new ALLEGRO_KEYBOARD_STATE();

	key[KEY_FORWARD] = ALLEGRO_KEY_W;
	key[KEY_LEFT] = ALLEGRO_KEY_A;
	key[KEY_BACKWARD] = ALLEGRO_KEY_S;
	key[KEY_RIGHT] = ALLEGRO_KEY_D;
	key[KEY_HEAL] = ALLEGRO_KEY_H;
	key[KEY_HEAL_ALT] = ALLEGRO_KEY_Q;
	key[KEY_JUMP] = ALLEGRO_KEY_SPACE;
	key[KEY_WEAPON1] = ALLEGRO_KEY_1;
	key[KEY_WEAPON2] = ALLEGRO_KEY_2;
	key[KEY_WEAPON3] = ALLEGRO_KEY_3;
	key[KEY_WEAPON4] = ALLEGRO_KEY_4;
	key[KEY_FIRE] = ALLEGRO_KEY_UP;
	key[KEY_FIRE_ALT] = ALLEGRO_KEY_X;
	key[KEY_LOOKLEFT] = ALLEGRO_KEY_LEFT;
	key[KEY_LOOKRIGHT] = ALLEGRO_KEY_RIGHT;
	key[KEY_SWAP_SCENERY] = ALLEGRO_KEY_R;

	digitalTurn = 0.0f;

    dragonHead = NULL;

	safe = true;
    paused = false;
	splash = true;
	end = false;
}

Game::~Game()
{
	//dtor
}





void Game::doStuff() {
	vec2 pos = player->getPos();

	if(safe && distanceBetween(lastPosition, vec2(0.0f, 0.0f)) > SAFE_RADIUS) {
		safe = false;
	}

	if(distanceBetween(lastPosition, pos) < 20 || safe) {
		lastPosition = pos;
		return;
	}

	lastPosition = pos;

    //We don't want too many enemies spawning at once. Sadly, that costs us the statistics of how many crates we've grabbed. Longer term, we should probably have a separate counter to control how many enemies we end up with.
    if (pickups > 3)
    {
        pickups = 3;
    }
	if(!bossSpawned && npc.size() <= (MAX_ENEMY_NUMBER * pickups) && rand()%100 < 7) {

		float angle = atan2(player->getVel().y, player->getVel().x);

		float addon = random(1.0f);
		addon *= addon*0.75f;

		angle += random(PI)*(addon+0.25f);

		float distance = ENEMY_SPAWN_DISTANCE + random(ENEMY_SPAWN_DISTANCE_RANDOM);

        Print("Spawning new skaters:", 3 * pickups);

		for(int i = 0; i < 3 * pickups; i++) {
			Character* c = new Character();
			c->setFrame(generateCharacterSuperContainer());
			c->setPosition(vec3(player->getPos().x+cos(angle)*distance +random(20.0f), player->getPos().y+sin(angle)*distance +random(20.0f), 0.0f));
			c->setProjectileStarburstBitmap(projectileBitmapStarburst);
			c->setProjectileHaloBitmap(projectileBitmapHalo);

			world3D->push(c);
			npc.push_back(c);

			audioMan->setGameState(GAME_STATE_COMBAT);
		}
	}

	if(!bossSpawned && pickups >= 3 && rand()%100 < 3) {

	    Print("Spawning dragon!");

		float angle = atan2(player->getVel().y, player->getVel().x);

		float distance = 450.0f;

		dragonHead = new DragonHead(world3D, &npc);
		dragonHead->setPosition(vec3(player->getPos().x+cos(angle)*distance +random(20.0f), player->getPos().y+sin(angle)*distance +random(20.0f), 0.0f));
		dragonHead->setProjectileStarburstBitmap(projectileBitmapStarburst2);
		dragonHead->setProjectileHaloBitmap(projectileBitmapHalo2);

		dragonHead->setPlayer(player);

		dragonHead->display = graphicsMan->getDisplay();

		bossSpawned = true;
		audioMan->setGameState(GAME_STATE_DRAGON);
	}

    //We use negative values to set an upper limit on pickup spawn time
//	if(pickupTimer >= 0) {
		pickupTimer -= 1.0f;
//	}

    Print("Pickup?", pickupTimer);
	if((pickup.size() < 1 && rand()%100 < 10 && pickupTimer <= 0) || pickupTimer < -20.0f){

        Print ("Spawning pickup");
		float angle = atan2(player->getVel().y, player->getVel().x);

		float addon = random(1.0f);
		addon *= addon*0.5f;

		angle += random(PI)*(addon+0.25f);

		float distance = ENEMY_SPAWN_DISTANCE + random(ENEMY_SPAWN_DISTANCE_RANDOM);

		pickup.push_back(new Pickup(world3D, vec3(player->getPos().x+cos(angle)*distance +random(10.0f), player->getPos().y+sin(angle)*distance +random(10.0f), 0.0f)));

        //It's actually kinda nice to have a distraction whilst chasing down the dragon
//        if (!bossSpawned)
//        {
            Print("Spawning pickup protectors:", 3 + (pickups * 2));
            for(int i = 0; i < 3 + (pickups * 2); i++) {
                Character* c = new Character();
                c->setFrame(generateCharacterSuperContainer());
                c->setPosition(vec3(player->getPos().x+cos(angle)*distance +random(10.0f), player->getPos().y+sin(angle)*distance +random(10.0f), 0.0f));
                c->setProjectileStarburstBitmap(projectileBitmapStarburst);
                c->setProjectileHaloBitmap(projectileBitmapHalo);

                world3D->push(c);
                npc.push_back(c);

                audioMan->setGameState(GAME_STATE_COMBAT);
            }
//        }

        pickupTimer = PICKUP_TIME;
	}
}





int Game::initialize() {

	pickups = 0;

	bossSpawned = false;

	pickupTimer = 0.0f;

	graphicsMan = new GraphicsManager(1280, 720);
	audioMan = new AudioManager();
	audioMan->setGameState(GAME_STATE_NORMAL);

	al_set_window_title(graphicsMan->getDisplay(), "#7DFPS - FLAT");

	event_queue = NULL;
	updateTimer = NULL;


	font = al_load_font("font.ttf", 14, 0);


	player = new Player();

	fpsText = new DrawableText(font, al_map_rgb(255, 255, 255));
	fpsText->setText("FPS:");

	hintText = new DrawableText(font, al_map_rgb(255, 255, 255));
	hintText->setText(" Press H or Q to heal. ");
	hintText->setText("Hold W to skate faster.");
	hintText->setText("Follow the Pulse Dragon.");
	hintText->setText("");
	hintText->setPosition(vec2(550.0f, 600.0f));


	world3D = new DrawStack3D();

	loadBitmaps();

	splashStart = al_load_bitmap("gfx/splash_start.png");
	splashPause = al_load_bitmap("gfx/splash_pause.png");
	splashDeath = al_load_bitmap("gfx/splash_death.png");
	splashEnd = al_load_bitmap("gfx/splash_end.png");

	pickup.push_back(new Pickup(world3D, vec3(30.0f, 5.0f, 0.0f)));


	for(int i = 0; i < DETAIL_OBJECT_NUMBER; i++) {
		DetailObject* o = new DetailObject();

		float angle = random(PI);

		if(rand()%100 > STATIC_OBJECT_PERCENTAGE) {

			o->setBitmap(detailBitmap[0]);
			o->setPosition(vec3(sin(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), cos(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), 3.5f));

			for(int j = 1; j < 6; j++) {
				if(rand()%6-j == 0) {
					o->setBitmap(detailBitmap[j]);
					break;
				}
			}
		}
		else {
			o = new StaticObject();

			if(rand()%3 == 0) {
				for(int i = 0; i < 16; i++) {
					o->pushImage(crystalA[i]);
					o->setPrescale(30.0f);
					o->setPosition(vec3(sin(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), cos(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), -4.0f));
				}
			}
			else if(rand()%2 == 0) {
				for(int i = 0; i < 16; i++) {
					o->pushImage(crystalB[i]);
					o->setPrescale(30.0f);
					o->setPosition(vec3(sin(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), cos(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), -4.0f));
				}
			}
			else {
				for(int i = 0; i < 16; i++) {
					o->pushImage(crystalC[i]);
					o->setPrescale(30.0f);
					o->setPosition(vec3(sin(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), cos(angle)*random(DETAIL_OBJECT_CREATION_DISTANCE), -2.0f));
					o->setJumpable();
				}
			}
			stat.push_back(o);
		}

		world3D->push(o);
		detail.push_back(o);
	}

	floor = new Floor();

	graphicsMan->pushDrawableObject(floor);
	graphicsMan->pushDrawableObject(world3D);
	graphicsMan->pushDrawableObject(player);
	graphicsMan->pushDrawableObject(fpsText);
	graphicsMan->pushDrawableObject(hintText);

	startUpdateLoop(60);
	return 1;
}

bool Game::keyrel(int k)
{
    /*
    Checks if a key has been pressed AND released. This means key repeats doesn't affect the input
    */

    static bool initialized = false;
    static bool keyp[ALLEGRO_KEY_MAX];

    if (!initialized)
    {
        for (int i=0; i<ALLEGRO_KEY_MAX; i++) keyp[i] = false;
        initialized = 1;
    }

    if (al_key_down(keyboard_state, key[k]) && !keyp[k])
    {
        keyp[k] = 1;
        return 0;
    }
    else if (!al_key_down(keyboard_state, key[k]) && keyp[k])
    {
        keyp[k] = 0;
        return 1;
    }

    return 0;
}

void Game::update() {

	if (!splash && end)
	{
        Print("Quitting game");
        quit = true;
	}

	al_get_keyboard_state(keyboard_state);

	digitalDrag = DIGITAL_DRAG_RELEASE;

	if(al_key_down(keyboard_state, key[KEY_FIRE]) || al_key_down(keyboard_state, key[KEY_FIRE_ALT])) {
	    splash = false;

		if (paused) {
            paused = false;
        } else {
            player->fire();
        }
	}

	if(al_key_down(keyboard_state, ALLEGRO_KEY_ESCAPE)) {
		grabMouse = false;
		paused = true;
		splash = true;
		al_show_mouse_cursor(graphicsMan->getDisplay());
	}

    if (!paused && !quit)
    {
        if(al_key_down(keyboard_state, key[KEY_LOOKRIGHT])) {
            digitalTurn += DIGITAL_ACCELERATION;
            digitalDrag = DIGITAL_DRAG_HOLD;
        }
        if(al_key_down(keyboard_state, key[KEY_LOOKLEFT])) {
            digitalTurn -= DIGITAL_ACCELERATION;
            digitalDrag = DIGITAL_DRAG_HOLD;
        }

        if(al_key_down(keyboard_state, key[KEY_FORWARD])) {
            //world3D->incrementCamera(vec3(-0.05f, 0.0f, 0.0f));
            player->holdForward();
        }
        if(al_key_down(keyboard_state, key[KEY_BACKWARD])) {
            //world3D->incrementCamera(vec3(-0.05f, 0.0f, 0.0f));
            player->holdBackward();
        }
        if(al_key_down(keyboard_state, key[KEY_LEFT])) {
            //world3D->incrementCamera(vec3(0.0f, 0.05f, 0.0f));
            player->holdLeft();
        }
        if(al_key_down(keyboard_state, key[KEY_RIGHT])) {
            //world3D->incrementCamera(vec3(0.0f, -0.05f, 0.0f));
            player->holdRight();
        }
        if(al_key_down(keyboard_state, key[KEY_JUMP])) {
            player->jump();
        } else {
            player->jumpRelease();
        }

        if(al_key_down(keyboard_state, key[KEY_HEAL]) || al_key_down(keyboard_state, key[KEY_HEAL_ALT])) {
            player->heal();
        }

        if(al_key_down(keyboard_state, key[KEY_WEAPON1])) {
            player->switchWeapon(0);
        }

        if(al_key_down(keyboard_state, key[KEY_WEAPON2])) {
            player->switchWeapon(1);
        }

        if(al_key_down(keyboard_state, key[KEY_WEAPON3])) {
            player->switchWeapon(2);
        }
        if (keyrel(KEY_SWAP_SCENERY)){
            floor->swapScenery();
        }


        if(abs(digitalTurn) > 0.1f) {
            player->turn(digitalTurn);
            digitalTurn *= digitalDrag;
        }

        player->update();

        vec2 targetPos = player->getPos();
        vec2 targetVel = player->getVel();

        int len = npc.size();

        if(player->firing) {
            for(int j = 0; j < player->getWeaponRounds(); j++) {
                float closestDistance = 500;
                int closestIndex = -1;
                float damage = 0.0f;

                float randomSpread = player->getWeaponSpread()*random(1.0f);

                for(int i = 0; i < len; i++) {
                    Character *c = npc[i];

                    float distance = c->getPlayerDistance();
                    float angle = abs(c->getPlayerAngle())*distance+randomSpread;

                    if(angle < 0.31f && distance < closestDistance && !npc[i]->isInvurnable()) {
                        closestIndex = i;
                        closestDistance = distance;
                        damage = player->getWeaponDamage()*((0.35f-angle)/0.10f);
                    }
                }

                if(closestIndex != -1) {

                    FrameContainer c = FrameContainer();

                    for(int i = 0; i < 4; i++) {
                        for(int k = 0; k < 3; k++) {
                            c.push(splatBitmap[i]);
                        }
                    }

                    Splat* s = new Splat(c);

                    vec3 dPosition = npc[closestIndex]->getPosition() - player->getCameraPosition();

                    float creationAngle = atan2(dPosition.y, dPosition.x);

                    s->setPosition(player->getCameraPosition()+vec3(cos(creationAngle)*(closestDistance-0.3)+random(0.5), sin(creationAngle)*(closestDistance-0.3)+random(0.5), 0.5 + random(0.5)));
                    //splat->setPrescale(5.0f);

                    world3D->push(s);
                    splat.push_back(s);

                    npc[closestIndex]->dealDamage(damage);
                }
            }
        }

        for(int i = 0; i < npc.size(); i++) {
            Character* c = npc[i];

            c->updateTarget(targetPos, targetVel);
            c->update();

            if(c->isFiring()) {
                Projectile* o = c->getProjectile();

                float speed = c->getProjectileSpeed();
                float distance = c->getPlayerDistance();

                float time = (distance/speed)/2;


                vec2 targetPosition = vec2(player->getCameraPosition().x, player->getCameraPosition().y) + player->getVel() * time;
                vec2 originPosition = vec2(c->getPosition().x, c->getPosition().y);
                vec2 vel = normalize(targetPosition - originPosition)*speed;
                o->setVelocity(vel);
                world3D->push(o);
                projectile.push_back(o);
            }

            //TODO: Why doesn't this find the dragon? ;_;
            if(c->destroy) {
                if(c->dragon) {
                    Print("We killed a dragon!");
                    al_rest(1.0f);
                    end = true;
                    splash = true;
                    paused = true;
                }

                npc.erase(npc.begin()+i);
                i--;
            }


            if (npc.size() <= 0)
            {
                //Audio man, audio man. Doing the things an audio can.
                audioMan->setGameState(GAME_STATE_NORMAL);

                //TODO: Work out when we've been skating aimlessly for too long and display a hint message
  //              hintText->setText(" Hold W to skate faster. ");
            }
        }


        //TODO: This is questionable. Why doesn't the dragon show up properly in the queue?
        if (dragonHead != NULL)
        {
            if (dragonHead->destroy)
            {
                Print("We killed a dragon!");
                al_rest(1.0f);
                end = true;
                splash = true;
                paused = true;
            }
            //TODO: add hint text for when dragon goes out of range
/*            else if (dragonHead->)
            {
                hintText->setText("Follow the Pulse Dragon.");
            }
*/        }


        //TODO: add hint text for when player is dying
/*        if (player->getHealth() <= 0.0f)
        {
                hintText->setText(" Press H or Q to heal.");
        }
*/
        for(int i = 0; i < projectile.size(); i++) {
            Projectile* p = projectile[i];

            p->update();

            float distance = distanceBetween(p->getPos(), player->getPos());

            if(distance < 1.0f) {
                player->takeDamage(PROJECTILE_DAMAGE);
                p->destroy = true;
            }

            if(p->destroy) {
                projectile.erase(projectile.begin()+i);
                i--;
            }
        }

        for(int i = 0; i < splat.size(); i++) {
            Splat* s = splat[i];

            if(s->destroy) {
                splat.erase(splat.begin()+i);
                i--;
                continue;
            }

            s->update();
        }

        for(int i = 0; i < detail.size(); i++) {
            DetailObject* o = detail[i];
            if(o->destroy) {
                detail.erase(detail.begin()+i);
                i++;

                DetailObject* o = new DetailObject();

                float angle = atan2(player->getVel().y, player->getVel().x);

                float addon = random(1.0f);
                addon *= addon*0.75f;

                angle += random(PI)*(addon+0.25f);


                vec3 pos = player->getCameraPosition();

                float distance = DETAIL_OBJECT_CREATION_DISTANCE - abs(random(DETAIL_OBJECT_CREATION_DISTANCE_RANDOM));

                if(rand()%100 > STATIC_OBJECT_PERCENTAGE) {

                    o->setBitmap(detailBitmap[0]);
                    o->setPosition(vec3(pos.x + cos(angle)*distance, pos.y + sin(angle)*distance, 2.0f));

                    for(int j = 1; j < 6; j++) {
                        if(rand()%6-j == 0) {
                            o->setBitmap(detailBitmap[j]);
                            break;
                        }
                    }
                }
                else {
                    o = new StaticObject();

                    if(rand()%3 == 0) {
                        for(int i = 0; i < 16; i++) {
                            o->pushImage(crystalA[i]);
                            o->setPrescale(30.0f);
                            o->setPosition(vec3(pos.x + cos(angle)*distance, pos.y + sin(angle)*distance, -2.4f));
                        }
                    }
                    else if(rand()%2 == 0) {
                        for(int i = 0; i < 16; i++) {
                            o->pushImage(crystalB[i]);
                            o->setPrescale(30.0f);
                            o->setPosition(vec3(pos.x + cos(angle)*distance, pos.y + sin(angle)*distance, -2.4f));
                        }
                    }
                    else {
                        for(int i = 0; i < 16; i++) {
                            o->pushImage(crystalC[i]);
                            o->setPrescale(30.0f);
                            o->setPosition(vec3(pos.x + cos(angle)*distance, pos.y + sin(angle)*distance, -1.0f));
                            o->setJumpable();
                        }
                    }

                    stat.push_back(o);
                }

                world3D->push(o);
                detail.push_back(o);
            }
        }

        for(int i = 0; i < stat.size(); i++) {

            stat[i]->update();

            vec2 oPos = stat[i]->getPos();
            vec2 pPos = player->getPos();

            vec2 dPos = pPos-oPos;

            float l = length(dPos);
            float r = stat[i]->getRadius();


            if(l < r) {
                if(stat[i]->isJumpable()) {
                    if(!player->isJumping()) {
                        player->setVelocity(player->getVel()*0.90);
                    }
                }
                else {
                    player->setVelocity(dPos*length(player->getVel())*0.3);
                    player->setPosition(oPos+normalize(dPos)*r);
                }
            }
        }

        for(int i = 0; i < pickup.size(); i++) {
            if(distanceBetween(pickup[i]->getPos(), player->getPos()) < 3.0f) {
                pickup[i]->remove();

                player->pickup();

                pickupTimer = PICKUP_TIME;
                pickups++;

                pickup.erase(pickup.begin()+i);
                break;
            }

            if (pickup[i]->destroy) {
                pickup.erase(pickup.begin()+i);
                i++;

            }
        }

        world3D->setCamera(player->getCameraPosition(), player->getCameraAngle(), player->getCameraTilt());
        floor->setCamera(player->getCameraPosition(), player->getCameraAngle(), player->getCameraTilt());

        if (player->gameOver)
        {
            paused = true;
            splash = true;
            end = true;
        }
    }
}

void Game::updateMouse() {
	ALLEGRO_MOUSE_STATE state;

	al_get_mouse_state(&state);

	if(!grabMouse && al_mouse_button_down(&state, 1)) {
		al_set_mouse_xy(graphicsMan->getDisplay(), 200, 200);
		al_get_mouse_state(&state);

		al_hide_mouse_cursor(graphicsMan->getDisplay());

		grabMouse = true;
	}

	if(grabMouse && al_mouse_button_down(&state, 1)) {
		splash = false;

		if (paused) {
            paused = false;
        } else {
            player->fire();
        }
	}

	if(grabMouse) {

		al_set_mouse_xy(graphicsMan->getDisplay(), 200, 200);

		int dx = state.x - 200;
		int dy = state.y - 200;

		player->turn(dx);
	}
}


int Game::startUpdateLoop(int _fps) {
	fps = _fps;

	updateTimer = al_create_timer(1.0 / fps);
	if(!updateTimer) {
		Print("Error setting up updateTimer.");
		return 0;
	}

	fpsCounter = al_create_timer(1.0);
	if(!fpsCounter) {
		Print("Error setting up updateTimer.");
		return 0;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) {
		Print("Error creating event_queue.");
		return 0;
	}

	ALLEGRO_TIMER* doStuffTimer = al_create_timer(1.0);

	al_register_event_source(event_queue, al_get_display_event_source(graphicsMan->getDisplay()));
	al_register_event_source(event_queue, al_get_timer_event_source(updateTimer));
	al_register_event_source(event_queue, al_get_timer_event_source(fpsCounter));
	al_register_event_source(event_queue, al_get_timer_event_source(doStuffTimer));

	al_start_timer(updateTimer);
	al_start_timer(fpsCounter);
	al_start_timer(doStuffTimer);

	frame = 0;

	//Main game loop
	while(1) {

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		bool redraw = false;

		if(quit) {
			break;
		}


		if(ev.timer.source == fpsCounter) {
			char integer_string[4];

			sprintf(integer_string, "%d", frame);

			char text[64] = "FPS: ";

			strcat(text, integer_string);

			fpsText->setText(text);
			frame = 0;
		}

		if(ev.timer.source == doStuffTimer) {
			doStuff();
		}


		if(ev.timer.source == updateTimer) {
			update();
            if(quit) {
                break;
            }

			redraw = true;

			updateMouse();
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			if(splash) {

				hintText->setText("");

				al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

				if(end == true) {
				    if (player->gameOver) {
				        al_draw_bitmap(splashDeath, 0, 0, 0);
				    } else {
                        al_draw_bitmap(splashEnd, 0, 0, 0);
				    }
				} else if (paused == true) {

                    al_draw_bitmap(splashPause, 0, 0, 0);
				} else {
                    al_draw_bitmap(splashStart, 0, 0, 0);
                }

				al_flip_display();


				al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
			}
			else {
				graphicsMan->draw();
			}

			frame++;
		}
	}

	//Program exited noramlly.
	return 1;
}

GraphicsManager* Game::getGraphicsMan() {
	return graphicsMan;
}

AudioManager* Game::getAudioMan() {
	return audioMan;
}

void Game::loadBitmaps() {

	projectileBitmapStarburst = DrawableBitmap("gfx/starburst.png");
	projectileBitmapHalo = DrawableBitmap("gfx/halo.png");
	projectileBitmapStarburst2 = DrawableBitmap("gfx/starburst2.png");
	projectileBitmapHalo2 = DrawableBitmap("gfx/halo2.png");
	projectileBitmapStarburst.setFloatingHeight(138.0f);
	projectileBitmapHalo.setFloatingHeight(138.0f);


	detailBitmap[0] = DrawableBitmap("gfx/scuff1.png");
	detailBitmap[1] = DrawableBitmap("gfx/scuff2.png");
	detailBitmap[2] = DrawableBitmap("gfx/scuff3.png");
	detailBitmap[3] = DrawableBitmap("gfx/scuff4.png");
	detailBitmap[4] = DrawableBitmap("gfx/scuff5.png");
	detailBitmap[5] = DrawableBitmap("gfx/scuff6.png");
	detailBitmap[6] = DrawableBitmap("gfx/scuff1.png");
	detailBitmap[7] = DrawableBitmap("gfx/scuff2.png");
	detailBitmap[8] = DrawableBitmap("gfx/scuff3.png");
	detailBitmap[9] = DrawableBitmap("gfx/scuff4.png");
	detailBitmap[10] = DrawableBitmap("gfx/scuff5.png");
	detailBitmap[11] = DrawableBitmap("gfx/scuff6.png");
	detailBitmap[12] = DrawableBitmap("gfx/scuff1.png");
	detailBitmap[13] = DrawableBitmap("gfx/scuff2.png");
	detailBitmap[14] = DrawableBitmap("gfx/scuff3.png");
	detailBitmap[15] = DrawableBitmap("gfx/scuff4.png");
	detailBitmap[16] = DrawableBitmap("gfx/scuff5.png");
	detailBitmap[17] = DrawableBitmap("gfx/scuff6.png");

	for(int i = 0; i < 18; i++) {
		detailBitmap[i].setReflection(false);
    }



	crystalA[0] = DrawableBitmap("gfx/crystal00.png");
	crystalA[1] = DrawableBitmap("gfx/crystal01.png");
	crystalA[2] = DrawableBitmap("gfx/crystal02.png");
	crystalA[3] = DrawableBitmap("gfx/crystal03.png");
	crystalA[4] = DrawableBitmap("gfx/crystal04.png");
	crystalA[5] = DrawableBitmap("gfx/crystal05.png");
	crystalA[6] = DrawableBitmap("gfx/crystal06.png");
	crystalA[7] = DrawableBitmap("gfx/crystal07.png");
	crystalA[8] = DrawableBitmap("gfx/crystal08.png");
	crystalA[9] = DrawableBitmap("gfx/crystal09.png");
	crystalA[10] = DrawableBitmap("gfx/crystal10.png");
	crystalA[11] = DrawableBitmap("gfx/crystal11.png");
	crystalA[12] = DrawableBitmap("gfx/crystal12.png");
	crystalA[13] = DrawableBitmap("gfx/crystal13.png");
	crystalA[14] = DrawableBitmap("gfx/crystal14.png");
	crystalA[15] = DrawableBitmap("gfx/crystal15.png");

	for(int i = 0; i < 16; i++) {
		crystalA[i].setReflection(true);
	}



	crystalB[0] = DrawableBitmap("gfx/crystalb00.png");
	crystalB[1] = DrawableBitmap("gfx/crystalb01.png");
	crystalB[2] = DrawableBitmap("gfx/crystalb02.png");
	crystalB[3] = DrawableBitmap("gfx/crystalb03.png");
	crystalB[4] = DrawableBitmap("gfx/crystalb04.png");
	crystalB[5] = DrawableBitmap("gfx/crystalb05.png");
	crystalB[6] = DrawableBitmap("gfx/crystalb06.png");
	crystalB[7] = DrawableBitmap("gfx/crystalb07.png");
	crystalB[8] = DrawableBitmap("gfx/crystalb08.png");
	crystalB[9] = DrawableBitmap("gfx/crystalb09.png");
	crystalB[10] = DrawableBitmap("gfx/crystalb10.png");
	crystalB[11] = DrawableBitmap("gfx/crystalb11.png");
	crystalB[12] = DrawableBitmap("gfx/crystalb12.png");
	crystalB[13] = DrawableBitmap("gfx/crystalb13.png");
	crystalB[14] = DrawableBitmap("gfx/crystalb14.png");
	crystalB[15] = DrawableBitmap("gfx/crystalb15.png");

	for(int i = 0; i < 16; i++) {
		crystalB[i].setReflection(true);
	}


	crystalC[0] = DrawableBitmap("gfx/crystalc00.png");
	crystalC[1] = DrawableBitmap("gfx/crystalc01.png");
	crystalC[2] = DrawableBitmap("gfx/crystalc02.png");
	crystalC[3] = DrawableBitmap("gfx/crystalc03.png");
	crystalC[4] = DrawableBitmap("gfx/crystalc04.png");
	crystalC[5] = DrawableBitmap("gfx/crystalc05.png");
	crystalC[6] = DrawableBitmap("gfx/crystalc06.png");
	crystalC[7] = DrawableBitmap("gfx/crystalc07.png");
	crystalC[8] = DrawableBitmap("gfx/crystalc08.png");
	crystalC[9] = DrawableBitmap("gfx/crystalc09.png");
	crystalC[10] = DrawableBitmap("gfx/crystalc10.png");
	crystalC[11] = DrawableBitmap("gfx/crystalc11.png");
	crystalC[12] = DrawableBitmap("gfx/crystalc12.png");
	crystalC[13] = DrawableBitmap("gfx/crystalc13.png");
	crystalC[14] = DrawableBitmap("gfx/crystalc14.png");
	crystalC[15] = DrawableBitmap("gfx/crystalc15.png");

	for(int i = 0; i < 16; i++) {
		crystalC[i].setReflection(true);
	}


	splatBitmap[0] = DrawableBitmap("gfx/splat0.png");
	splatBitmap[1] = DrawableBitmap("gfx/splat1.png");
	splatBitmap[2] = DrawableBitmap("gfx/splat2.png");
	splatBitmap[3] = DrawableBitmap("gfx/splat3.png");


	grunt[0][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_00.png");
	grunt[1][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_01.png");
	grunt[2][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_02.png");
	grunt[3][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_03.png");
	grunt[4][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_04.png");
	grunt[5][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_05.png");
	grunt[6][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_06.png");
	grunt[7][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_07.png");
	grunt[8][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_08.png");
	grunt[9][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_09.png");
	grunt[10][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_10.png");
	grunt[11][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_11.png");
	grunt[12][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_12.png");
	grunt[13][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_13.png");
	grunt[14][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_14.png");
	grunt[15][0] = DrawableBitmap("gfx/skate_anim_yellow/skate_f1_15.png");

	grunt[0][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_00.png");
	grunt[1][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_01.png");
	grunt[2][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_02.png");
	grunt[3][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_03.png");
	grunt[4][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_04.png");
	grunt[5][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_05.png");
	grunt[6][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_06.png");
	grunt[7][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_07.png");
	grunt[8][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_08.png");
	grunt[9][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_09.png");
	grunt[10][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_10.png");
	grunt[11][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_11.png");
	grunt[12][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_12.png");
	grunt[13][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_13.png");
	grunt[14][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_14.png");
	grunt[15][1] = DrawableBitmap("gfx/skate_anim_yellow/skate_f2_15.png");

	grunt[0][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_00.png");
	grunt[1][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_01.png");
	grunt[2][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_02.png");
	grunt[3][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_03.png");
	grunt[4][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_04.png");
	grunt[5][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_05.png");
	grunt[6][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_06.png");
	grunt[7][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_07.png");
	grunt[8][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_08.png");
	grunt[9][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_09.png");
	grunt[10][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_10.png");
	grunt[11][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_11.png");
	grunt[12][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_12.png");
	grunt[13][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_13.png");
	grunt[14][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_14.png");
	grunt[15][2] = DrawableBitmap("gfx/skate_anim_yellow/skate_f3_15.png");

	grunt[0][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_00.png");
	grunt[1][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_01.png");
	grunt[2][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_02.png");
	grunt[3][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_03.png");
	grunt[4][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_04.png");
	grunt[5][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_05.png");
	grunt[6][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_06.png");
	grunt[7][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_07.png");
	grunt[8][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_08.png");
	grunt[9][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_09.png");
	grunt[10][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_10.png");
	grunt[11][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_11.png");
	grunt[12][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_12.png");
	grunt[13][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_13.png");
	grunt[14][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_14.png");
	grunt[15][3] = DrawableBitmap("gfx/skate_anim_yellow/skate_f4_15.png");

	grunt[0][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_00.png");
	grunt[1][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_01.png");
	grunt[2][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_02.png");
	grunt[3][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_03.png");
	grunt[4][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_04.png");
	grunt[5][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_05.png");
	grunt[6][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_06.png");
	grunt[7][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_07.png");
	grunt[8][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_08.png");
	grunt[9][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_09.png");
	grunt[10][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_10.png");
	grunt[11][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_11.png");
	grunt[12][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_12.png");
	grunt[13][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_13.png");
	grunt[14][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_14.png");
	grunt[15][4] = DrawableBitmap("gfx/skate_anim_yellow/skate_f5_15.png");

	grunt[0][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_00.png");
	grunt[1][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_01.png");
	grunt[2][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_02.png");
	grunt[3][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_03.png");
	grunt[4][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_04.png");
	grunt[5][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_05.png");
	grunt[6][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_06.png");
	grunt[7][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_07.png");
	grunt[8][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_08.png");
	grunt[9][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_09.png");
	grunt[10][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_10.png");
	grunt[11][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_11.png");
	grunt[12][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_12.png");
	grunt[13][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_13.png");
	grunt[14][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_14.png");
	grunt[15][5] = DrawableBitmap("gfx/skate_anim_yellow/skate_f6_15.png");

	grunt[0][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_00.png");
	grunt[1][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_01.png");
	grunt[2][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_02.png");
	grunt[3][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_03.png");
	grunt[4][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_04.png");
	grunt[5][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_05.png");
	grunt[6][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_06.png");
	grunt[7][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_07.png");
	grunt[8][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_08.png");
	grunt[9][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_09.png");
	grunt[10][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_10.png");
	grunt[11][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_11.png");
	grunt[12][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_12.png");
	grunt[13][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_13.png");
	grunt[14][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_14.png");
	grunt[15][6] = DrawableBitmap("gfx/skate_anim_yellow/skate_f7_15.png");

	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 7; j++) {
			grunt[i][j].setReflection(true);
		}
	}
}

FrameSuperContainer Game::generateCharacterSuperContainer() {
	FrameSuperContainer superC = FrameSuperContainer();

	for(int i = 0; i < 16; i++) {
		FrameContainer c = FrameContainer();

		for(int j = 0; j < 7; j++) {
			c.push(grunt[i][j]);
		}

		for(int j = 5; j >= 1; j--) {
			c.push(grunt[i][j]);
		}

		superC.push(c);
	}

	return superC;
}
