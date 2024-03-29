#include "SmileyEngine.h"
#include "SpecialTileManager.h"
#include "player.h"
#include "CollisionCircle.h"
#include "WeaponParticle.h"
#include "ProjectileManager.h"
#include "environment.h"
#include "hgeparticle.h"
#include "hgeanim.h"
#include "ExplosionManager.h"

#define MUSHROOM_EXPLODE_TIME 5.0
#define MUSHROOM_GROW_TIME 2.0

#define MUSHROOM_STATE_IDLING 0
#define MUSHROOM_STATE_EXPLODING 1
#define MUSHROOM_STATE_GROWING 2

#define MUSHROOM_EXPLOSION_DAMAGE 0.75
#define MUSHROOM_EXPLOSION_KNOCKBACK 156.0

#define SILLY_PAD_TIME 40		//Number of seconds silly pads stay active

extern SMH *smh;

SpecialTileManager::SpecialTileManager() {
	collisionBox = new hgeRect();
}

SpecialTileManager::~SpecialTileManager() {
	reset();
	delete collisionBox;
}

/**
 * Deletes all special tiles that have been created.
 */ 
void SpecialTileManager::reset() {
	resetMushrooms();
	resetFlames();
	resetSillyPads();
	resetIceBlocks();
	resetTimedTiles();
	resetWarps();
}

/**
 * Draws all special tiles
 */
void SpecialTileManager::draw(float dt) {
	drawTimedTiles(dt);
	drawMushrooms(dt);
	drawSillyPads(dt);
	drawIceBlocks(dt);
	drawFlames(dt);
	drawWarps(dt);
}

/**
 * Updates all special tiles
 */
void SpecialTileManager::update(float dt) {
	updateTimedTiles(dt);
	updateMushrooms(dt);
	updateSillyPads(dt);
	updateFlames(dt);
	updateIceBlocks(dt);
	updateWarps(dt);
}

//////////// Ice Block Functions /////////////////

void SpecialTileManager::addIceBlock(int gridX, int gridY) {

	//Create new ice block
	IceBlock newIceBlock;
	newIceBlock.gridX = gridX;
	newIceBlock.gridY = gridY;
	newIceBlock.hasBeenMelted = false;

	//Add it to the list
	iceBlockList.push_back(newIceBlock);

}

/**
 * Updates all the ice blocks that have been created.
 */
void SpecialTileManager::updateIceBlocks(float dt) {
	std::list<IceBlock>::iterator i;
	for(i = iceBlockList.begin(); i != iceBlockList.end(); i++) {
		collisionBox->SetRadius(i->gridX*64.0+32.0, i->gridY*64.0+32.0,30.0);
		if (!i->hasBeenMelted && smh->player->fireBreathParticle->testCollision(collisionBox)) {
			i->hasBeenMelted = true;
			i->timeMelted = smh->getGameTime();
		}
		if (i->hasBeenMelted && smh->timePassedSince(i->timeMelted) > 0.5) {
			smh->environment->collision[i->gridX][i->gridY] = WALKABLE;
			i = iceBlockList.erase(i);
		}
	}
}

/**
 * Draws all the ice blocks that have been created.
 */
void SpecialTileManager::drawIceBlocks(float dt) {
	std::list<IceBlock>::iterator i;
	for(i = iceBlockList.begin(); i != iceBlockList.end(); i++) {
		float scale = !i->hasBeenMelted ? 1.0 : 1.0 - min(1.0, smh->timePassedSince(i->timeMelted) * 2.0);
		//Scale the size of the ice block based on its "health"
		smh->resources->GetAnimation("walkLayer")->SetFrame(FIRE_DESTROY);
		smh->resources->GetAnimation("walkLayer")->SetHotSpot(32.0,32.0);
		smh->resources->GetAnimation("walkLayer")->RenderEx(smh->getScreenX(i->gridX*64.0+32.0), 
			smh->getScreenY(i->gridY*64.0+32.0), 0.0, scale, scale);
		smh->resources->GetAnimation("walkLayer")->SetHotSpot(0.0,0.0);
	}
}

/**
 * Deletes all the ice blocks that have been created.
 */
void SpecialTileManager::resetIceBlocks() {
	std::list<IceBlock>::iterator i;
	for(i = iceBlockList.begin(); i != iceBlockList.end(); i++) {
		i = iceBlockList.erase(i);
	}
	iceBlockList.clear();
}

//////////// Timed Tile Functions ///////////////////

/**
 * Permanantly transforms the square at (gridX, gridY) to have a new collision,
 * but the same terrain and item layer.
 */
void SpecialTileManager::addTimedTile(int gridX, int gridY, int newCollision) 
{	
	addTimedTile(gridX, gridY, smh->environment->terrain[gridX][gridY], newCollision, 
		smh->environment->item[gridX][gridY], 1.0);
}

/**
 * Permanantly transforms the square at (gridX, gridY) to have a new collision,
 * terrain, and item layer.
 */
void SpecialTileManager::addTimedTile(int gridX, int gridY, int newTerrain, int newCollision, int newItemLayer, float fadeTime) 
{
	if (isTimedTileAt(gridX, gridY)) return;

	TimedTile tile;

	tile.gridX = gridX;
	tile.gridY = gridY;
	tile.duration = 99999.0;
	tile.fadeTime = fadeTime;
	tile.timeCreated = smh->getGameTime();
	tile.alpha = 0.0;

	tile.newTerrain = newTerrain;
	tile.oldTerrain = smh->environment->terrain[gridX][gridY];
	tile.newCollision = newCollision;
	tile.oldCollision = smh->environment->collision[gridX][gridY];
	tile.newItemLayer = newItemLayer;
	tile.oldItemLayer = smh->environment->item[gridX][gridY];

	timedTileList.push_back(tile);

	//Set the new tile stuff now, we will fade out the old stuff in the draw method
	smh->environment->terrain[gridX][gridY] = newTerrain;
	smh->environment->item[gridX][gridY] = newItemLayer;
	if (newCollision == PIT) 
		smh->environment->collision[gridX][gridY] = FAKE_PIT;
	else
		smh->environment->collision[gridX][gridY] = newCollision;

}

void SpecialTileManager::removeTimedTile(int gridX, int gridY) 
{
	for(std::list<TimedTile>::iterator i = timedTileList.begin(); i != timedTileList.end(); i++) 
	{
		
	}
}

/**
 * Update all timed tiles.
 */
void SpecialTileManager::updateTimedTiles(float dt) 
{
	std::list<TimedTile>::iterator i;
	for(i = timedTileList.begin(); i != timedTileList.end(); i++) 
	{
		if (smh->timePassedSince(i->timeCreated) <= i->duration) 
		{
			//Fading in
			if (i->alpha < 255.0) 
			{
				i->alpha += (255.0 / i->fadeTime) * dt;
				
				if (i->newCollision == PIT && i->alpha > 165.0) 
					smh->environment->collision[i->gridX][i->gridY] = PIT;
				
				if (i->alpha > 255.0) 
				{
					i->alpha = 255.0;
				}
			}
		} 
		else 
		{
			//Fading out
			if (i->alpha == 255.0) 
			{
				//smh->environment->collision[i->gridX][i->gridY] = i->oldCollision;
				//smh->environment->item[i->gridX][i->gridY] = i->oldItemLayer;
				//smh->environment->terrain[i->gridX][i->gridY] = i->oldTerrain;
			}
			i->alpha -= (255.0 / i->fadeTime) * dt;
			if (i->alpha < 0.0) 
			{
				i->alpha = 0.0;
				i = timedTileList.erase(i);
			}
		}
	}
}

/**
 * Draw all timed tiles.
 */
void SpecialTileManager::drawTimedTiles(float dt) 
{
	for(std::list<TimedTile>::iterator i = timedTileList.begin(); i != timedTileList.end(); i++) 
	{
		if (Util::distance(smh->player->gridX, smh->player->gridY, i->gridX, i->gridY) < 12 && i->alpha < 255.0) 
		{
			float x = smh->getScreenX(i->gridX*64);
			float y = smh->getScreenY(i->gridY*64);

			//Fade out old terrain
			smh->resources->GetAnimation("mainLayer")->SetFrame(i->oldTerrain);
			smh->resources->GetAnimation("mainLayer")->SetColor(ARGB(255.0 - i->alpha, 255, 255, 255));
			smh->resources->GetAnimation("mainLayer")->Render(x, y);
			smh->resources->GetAnimation("mainLayer")->SetColor(ARGB(255, 255, 255, 255));
			
			//Fade out old collision layer
			if (smh->environment->shouldEnvironmentDrawCollision(i->oldCollision)) 
			{
				smh->resources->GetAnimation("walkLayer")->SetFrame(i->oldCollision);
				smh->resources->GetAnimation("walkLayer")->SetColor(ARGB(255.0 - i->alpha, 255, 255, 255));
				smh->resources->GetAnimation("walkLayer")->Render(x, y);
				smh->resources->GetAnimation("walkLayer")->SetColor(ARGB(255, 255, 255, 255));
			}

			//Fade out old item layer
			smh->environment->itemLayer[i->oldItemLayer]->SetColor(ARGB(255.0 - i->alpha, 255, 255, 255));
			smh->environment->itemLayer[i->oldItemLayer]->Render(x, y);
			smh->environment->itemLayer[i->oldItemLayer]->SetColor(ARGB(255, 255, 255, 255));
		}
	}
}

/**
 * Delete all timed tiles.
 */ 
void SpecialTileManager::resetTimedTiles() {
	std::list<TimedTile>::iterator i;
	for(i = timedTileList.begin(); i != timedTileList.end(); i++) {
		i = timedTileList.erase(i);
	}
	timedTileList.clear();
}

/**
 * Returns whether or not there is a timed tile at the specified location.
 */
bool SpecialTileManager::isTimedTileAt(int gridX, int gridY) {
	std::list<TimedTile>::iterator i;
	for(i = timedTileList.begin(); i != timedTileList.end(); i++) {
		if (i->gridX == gridX && i->gridY == gridY) return true;
	}
	return false;
}

//////////// Silly Pad Functions ///////////////

/**
 * Creates a new silly pad at the specified grid location.
 */ 
void SpecialTileManager::addSillyPad(int gridX, int gridY) {

	//Create a new silly pad
	SillyPad newSillyPad;
	newSillyPad.gridX = gridX;
	newSillyPad.gridY = gridY;
	newSillyPad.timePlaced = smh->getGameTime();

	//Add it to the list
	sillyPadList.push_back(newSillyPad);

}


/**
 * Draws all silly pads that have been created.
 */
void SpecialTileManager::drawSillyPads(float dt) {
	std::list<SillyPad>::iterator i;
	for(i = sillyPadList.begin(); i != sillyPadList.end(); i++) {
		//Fade out during the last 2 seconds
		float timeLeft = (float)SILLY_PAD_TIME - smh->timePassedSince(i->timePlaced);
		if (timeLeft < 1.0f) {
			smh->resources->GetSprite("sillyPad")->SetColor(ARGB((timeLeft/1.0f)*255.0f,255,255,255));
		}
		smh->resources->GetSprite("sillyPad")->Render(smh->getScreenX(i->gridX*64.0), smh->getScreenY(i->gridY*64.0));
		smh->resources->GetSprite("sillyPad")->SetColor(ARGB(255,255,255,255));
	}
}

/**
 * Updates all silly pads that have been created.
 */
void SpecialTileManager::updateSillyPads(float dt) {
	std::list<SillyPad>::iterator i;
	for(i = sillyPadList.begin(); i != sillyPadList.end(); i++) {
	
		collisionBox->SetRadius(i->gridX*64.0+32.0, i->gridY*64.0+32.0, 32.0);

		//Test collision and silly pad aging
		if (smh->timePassedSince(i->timePlaced) > SILLY_PAD_TIME ||
				smh->projectileManager->killProjectilesInBox(collisionBox, PROJECTILE_FRISBEE) > 0 ||
				smh->projectileManager->killProjectilesInBox(collisionBox, PROJECTILE_LIGHTNING_ORB) > 0 ||
				smh->player->iceBreathParticle->testCollision(collisionBox) ||
				smh->player->fireBreathParticle->testCollision(collisionBox) ||
				smh->player->getTongue()->testCollision(collisionBox)) {
			i = sillyPadList.erase(i);
		}
	}
}

/**
 * Returns whether or not there is a silly pad at the specified grid location.
 */
bool SpecialTileManager::isSillyPadAt(int gridX, int gridY) {
	std::list<SillyPad>::iterator i;
	for(i = sillyPadList.begin(); i != sillyPadList.end(); i++) {
		if (i->gridX == gridX && i->gridY == gridY) {
			return true;
		}
	}
	return false;
}

/**
 * If there is a silly pad at the specified square, destroy it. Returns whether
 * or not a silly pad was destroyed
 */
bool SpecialTileManager::destroySillyPad(int gridX, int gridY) {
	std::list<SillyPad>::iterator i;
	for(i = sillyPadList.begin(); i != sillyPadList.end(); i++) {
		if (i->gridX == gridX && i->gridY == gridY) {
			i = sillyPadList.erase(i);
			return true;
		}
	}
	return false;
}

/**
 * Deletes all silly pads that have been created.
 */
void SpecialTileManager::resetSillyPads() {
	std::list<SillyPad>::iterator i;
	for(i = sillyPadList.begin(); i != sillyPadList.end(); i++) {
		i = sillyPadList.erase(i);
	}
	sillyPadList.clear();
}

//////////// Flame Functions ////////////////

void SpecialTileManager::addFlame(int gridX, int gridY) {

	//Create the new flame
	Flame newFlame;
	newFlame.x = gridX * 64.0 + 32.0;
	newFlame.y = gridY * 64.0 + 32.0;
	newFlame.timeFlamePutOut = -10.0;
	newFlame.particle = new hgeParticleSystem(&smh->resources->GetParticleSystem("flame")->info);
	newFlame.particle->FireAt(smh->getScreenX(newFlame.x), smh->getScreenY(newFlame.y));
	newFlame.collisionBox = new hgeRect(1,1,1,1);

	//Add it to the list
	flameList.push_back(newFlame);

}

/**
 * Draws all flames that have been created.
 */
void SpecialTileManager::drawFlames(float dt) {
	std::list<Flame>::iterator i;
	for(i = flameList.begin(); i != flameList.end(); i++) {
		i->particle->MoveTo(smh->getScreenX(i->x), smh->getScreenY(i->y), true);
		i->particle->Render();
	}
}

/**
 * Updates all flames that have been created.
 */
void SpecialTileManager::updateFlames(float dt) 
{
	std::list<Flame>::iterator i;
	for(i = flameList.begin(); i != flameList.end(); i++) 
	{
		//Damage and knock the player back if they run into the fire
		i->collisionBox->SetRadius(i->x, i->y, 20.0);
		if (smh->player->collisionCircle->testBox(i->collisionBox)) 
		{
			smh->player->dealDamageAndKnockback(1.0, true, true, 100.0, i->x, i->y); 
			smh->setDebugText("Smiley hit by Flame tile");
		}

		//Flames are put out by ice breath. The flame isn't deleted yet so that
		//the flame particle can animate to completion
		if (i->timeFlamePutOut < 0.0 && smh->player->iceBreathParticle->testCollision(i->collisionBox)) 
		{
			i->timeFlamePutOut = smh->getGameTime();
			i->particle->Stop();
		}

		//If the flame has been put out and is done animating, delete it
		if (i->timeFlamePutOut > 0.0 && smh->timePassedSince(i->timeFlamePutOut) > 0.4) 
		{
			smh->environment->collision[Util::getGridX(i->x)][Util::getGridY(i->y)] = WALKABLE;
			delete i->collisionBox;
			delete i->particle;
			i = flameList.erase(i);
		}
		else 
		{
			i->particle->Update(dt);
		}
	}
}

/**
 * Deletes all flames that have been created.
 */
void SpecialTileManager::resetFlames() {
	std::list<Flame>::iterator i;
	for(i = flameList.begin(); i != flameList.end(); i++) {
		delete i->particle;
		delete i->collisionBox;
		i = flameList.erase(i);
	}
	flameList.clear();
}


//////////// Mushroom Functions //////////////

void SpecialTileManager::addMushroom(int _gridX, int _gridY, int _graphicsIndex) {
	Mushroom newMushroom;

	newMushroom.state = MUSHROOM_STATE_GROWING;
	newMushroom.beginGrowTime = smh->getGameTime();

	newMushroom.gridX = _gridX;
	newMushroom.gridY = _gridY;
	newMushroom.x = newMushroom.gridX * 64;
	newMushroom.y = newMushroom.gridY * 64;
	newMushroom.mushroomCollisionCircle = new CollisionCircle();
	newMushroom.mushroomCollisionCircle->x = newMushroom.x + 32.0;
	newMushroom.mushroomCollisionCircle->y = newMushroom.y + 32.0;
	newMushroom.mushroomCollisionCircle->radius = 28.0;

    newMushroom.graphicsIndex = _graphicsIndex;

	theMushrooms.push_back(newMushroom);
}

void SpecialTileManager::drawMushrooms (float dt) {
	std::list<Mushroom>::iterator i;
	for(i = theMushrooms.begin(); i != theMushrooms.end(); i++) {
		switch (i->state) {
			case MUSHROOM_STATE_IDLING:
				smh->resources->GetAnimation("walkLayer")->SetFrame(i->graphicsIndex);
				smh->resources->GetAnimation("walkLayer")->Render(smh->getScreenX(i->x),smh->getScreenY(i->y));
				if (smh->isDebugOn()) i->mushroomCollisionCircle->draw();
				break;
			case MUSHROOM_STATE_EXPLODING:
				break;
			case MUSHROOM_STATE_GROWING:
				//temporarily change the hot spot of the graphic so it grows from the center
				smh->resources->GetAnimation("walkLayer")->SetFrame(i->graphicsIndex);
				smh->resources->GetAnimation("walkLayer")->SetHotSpot(32.0,32.0);
				
				//Calculate size to draw it, then draw it
				float percentage = smh->timePassedSince(i->beginGrowTime) / MUSHROOM_GROW_TIME;
				percentage = min(percentage, 1.0); //Cap the size at 1 to prevent it from "overgrowing"
				smh->resources->GetAnimation("walkLayer")->RenderEx((int)smh->getScreenX(i->x+32),(int)smh->getScreenY(i->y+32),0.0,percentage,percentage);

				//change hot spot back
                smh->resources->GetAnimation("walkLayer")->SetHotSpot(0.0,0.0);
				break;
		};
	}
}

void SpecialTileManager::updateMushrooms(float dt) {

	std::list<Mushroom>::iterator i;
	for(i = theMushrooms.begin(); i != theMushrooms.end(); i++) {
		switch (i->state) {
			case MUSHROOM_STATE_IDLING:
				if (i->mushroomCollisionCircle->testCircle(smh->player->collisionCircle) && !smh->player->isFlashing()) {
					i->state = MUSHROOM_STATE_EXPLODING;
					i->beginExplodeTime = smh->getGameTime();
					smh->explosionManager->addExplosion(i->x+32.0, i->y+32.0, 0.75, MUSHROOM_EXPLOSION_DAMAGE, MUSHROOM_EXPLOSION_KNOCKBACK);
                }
				break;
			case MUSHROOM_STATE_EXPLODING:
				if (smh->timePassedSince(i->beginExplodeTime) > MUSHROOM_EXPLODE_TIME) {
					i->beginGrowTime = smh->getGameTime();
					i->state = MUSHROOM_STATE_GROWING;					
				}
				break;
			case MUSHROOM_STATE_GROWING:
				if (smh->timePassedSince(i->beginGrowTime) > MUSHROOM_GROW_TIME) {
					i->state = MUSHROOM_STATE_IDLING;
				}
		}
	}
}

/**
 * Deletes all mushrooms that have been created.
 */
void SpecialTileManager::resetMushrooms() {
	std::list<Mushroom>::iterator i;
	for(i = theMushrooms.begin(); i != theMushrooms.end(); i++) {
		delete i->mushroomCollisionCircle;
		i = theMushrooms.erase(i);
	}
	theMushrooms.clear();
}

//////////// Warp Functions //////////////

void SpecialTileManager::addWarp(int gridX, int gridY, int warpTile) 
{
	Warp newWarp;

	newWarp.gridX = gridX;
	newWarp.gridY = gridY;
	newWarp.angle = 0.0;
	newWarp.warpTile = warpTile;

	warpList.push_back(newWarp);
}
	
void SpecialTileManager::updateWarps(float dt) 
{
	for (std::list<Warp>::iterator i = warpList.begin(); i != warpList.end(); i++) 
	{
		i->angle -= 2.0 * PI * dt;
	}
}

void SpecialTileManager::drawWarps(float dt) 
{
	for (std::list<Warp>::iterator i = warpList.begin(); i != warpList.end(); i++) 
	{
		smh->resources->GetAnimation("walkLayer")->SetFrame(i->warpTile);
		smh->resources->GetAnimation("walkLayer")->SetHotSpot(32.0,32.0);
		smh->resources->GetAnimation("walkLayer")->RenderEx
			(smh->getScreenX(i->gridX*64.0+32.0), smh->getScreenY(i->gridY*64.0+32.0), i->angle);
		smh->resources->GetAnimation("walkLayer")->SetHotSpot(0.0,0.0);
	}
}

void SpecialTileManager::resetWarps() 
{
	//Warps have no objects to delete
	warpList.clear();
}




