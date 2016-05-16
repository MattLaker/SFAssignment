#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, 22);
  player->SetPosition(player_pos);

  const int number_of_aliens = 10;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(((canvas_w/number_of_aliens) * i) + 25, 300.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

	auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
	auto pos  = Point2((canvas_w/2), (canvas_h/2));
	coin->SetPosition(pos);
	coin->SetType(SFASSET_DEAD);
	coins.push_back(coin);

	const int numberOfWalls = 3;
	for(int i =0; i < numberOfWalls; i++){
		auto brick = make_shared<SFAsset>(SFASSET_BRICKS, sf_window);
		pos = Point2((canvas_w/5)*i + 190, 65);
		brick->SetPosition(pos);
		bricks.push_back(brick);
	}
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // wrap an SDL_Event with our SFEvent
    SFEvent sfevent((const SDL_Event) event);
    // handle our SFEvent
    OnEvent(sfevent);
  }
}
int direction = -1;
void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }

  // Update enemy positions

  for(auto a : aliens) {
	if (direction == -1) {
		a->GoWest();
}
	if (direction == 1) {
		a->GoEast();	
	}
  }
	for( auto a : aliens) {
		if(a->GetPosX() == 15 || a->GetPosX() == 625) {
			direction = direction * -1;
			break;
		}	
	}

  // Detect collisions for projectiles
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision(a);  
		score += 10;
      }
    }
	for (auto c : coins) {
		if(p->CollidesWith(c)) {
			p->HandleCollision(c);
		}
	}	
	for (auto b : bricks) {
		if(p->CollidesWith(b)){
			p->HandleCollision(b);
	}
		}
}
	//Detect collisions for the player
	for(auto b : bricks) {
		if(player->CollidesWith(b)) {
			player->HandleCollision(b);			
		}
	}
	for(auto a : aliens) {
		if(player->CollidesWith(a)) {
			player->HandleCollision(a);
		}
	}

	for(auto c : coins) {
		if(player->CollidesWith(c)) {
			player->HandleCollision(c);
		}
	}


  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
if(player->IsAlive()){
	player->OnRender();
} else {
	player->OnRender();
	std::cout << "Game Over" << std::endl << "You're score is: " << score << std::endl;
	is_running = false;
}

	for(auto b: bricks) {
		b->OnRender();
	}

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

int count = 0;
  for(auto a: aliens) {	
    if(a->IsAlive()) {
		a->OnRender();
		count++;
	}
  }

if(count == 0) {
	for(auto c: coins) {
		c->SetType(SFASSET_COIN);
		c->OnRender();		
	}
}

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
