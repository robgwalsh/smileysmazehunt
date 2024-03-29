#include "SmileyEngine.h"
#include "Fountain.h"
#include "player.h"
#include "hgeresource.h"

extern SMH *smh;

#define RES_FOUNTAIN 69
#define FOUNTAIN_HEAL_RADIUS 300.0

Fountain::Fountain(int gridX, int gridY) {
	x = float(gridX) * 64.0 + 32.0;
	y = float(gridY) * 64.0 + 32.0;
	smh->resources->GetParticleSystem("fountain")->Fire();
}

Fountain::~Fountain() { 
	smh->resources->Purge(RES_FOUNTAIN);
}

bool Fountain::isAboveSmiley() {
	return (y + 32.0 > smh->player->y);
}

void Fountain::draw(float dt) {

	//No need to draw the fountain if Smiley isn't by it!
	if (Util::distance(x, y, smh->player->x, smh->player->y) < 1000) {

		smh->resources->GetAnimation("fountainRipple")->Update(dt);

		//Bottom fountain part and pool
		smh->resources->GetSprite("fountainBottom")->Render(smh->getScreenX(x), smh->getScreenY(y));
		smh->resources->GetAnimation("fountainRipple")->Render(smh->getScreenX(x), smh->getScreenY(y - 72.0));
		
		//Top fountain part and pool
		smh->resources->GetSprite("fountainTop")->Render(smh->getScreenX(x), smh->getScreenY(y - 115.0));
		smh->resources->GetAnimation("fountainRipple")->RenderEx(smh->getScreenX(x), smh->getScreenY(y - 215.0), 0.0, .35, .4);	

		//Fountain particle
		smh->resources->GetParticleSystem("fountain")->MoveTo(smh->getScreenX(x), smh->getScreenY(y - 220.0), true);
		smh->resources->GetParticleSystem("fountain")->Render();
	}

}

void Fountain::update(float dt) {
	smh->resources->GetParticleSystem("fountain")->Update(dt);

	//Heal the player when they are close
	if (Util::distance(x, y, smh->player->x, smh->player->y) < FOUNTAIN_HEAL_RADIUS) {
		smh->player->heal(0.5);
	}

}