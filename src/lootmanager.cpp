#include "SmileyEngine.h"
#include "lootmanager.h"
#include "player.h"
#include "collisioncircle.h"
#include "WindowFramework.h"
#include "environment.h"
#include "hgesprite.h"
#include "EnemyFramework.h"

extern SMH *smh;

/**
 * Constructor
 */ 
LootManager::LootManager() {
	sprites[LOOT_HEALTH] = smh->resources->GetSprite("healthLoot");
	sprites[LOOT_MANA] = smh->resources->GetSprite("manaLoot");
	sprites[LOOT_NEW_ABILITY] = smh->resources->GetSprite("newAbilityLoot");
	radius[LOOT_HEALTH] = 15;
	radius[LOOT_MANA] = 15;
	radius[LOOT_NEW_ABILITY] = 33;
	collisionBox = new hgeRect();
}


/**
 * Destructor
 */ 
LootManager::~LootManager() { 
	delete collisionBox;
}


/**
 * Add loot
 */
void LootManager::addLoot(int type, int x, int y, int ability) {
	Loot newLoot;
    newLoot.type = type;
	newLoot.x = x;
	newLoot.y = y;
	newLoot.ability = ability;
	newLoot.groupID = -1;
	theLoot.push_back(newLoot);
}

/**
 * Add loot with a groupID attached
 * Used by bosses, so that when the loot is picked up, the enemy blocks disappear
 */
void LootManager::addLoot(int type, int x, int y, int ability, int groupID) {
	Loot newLoot;
    newLoot.type = type;
	newLoot.x = x;
	newLoot.y = y;
	newLoot.ability = ability;
	newLoot.groupID = groupID;
	theLoot.push_back(newLoot);
}

/**
 * Draw all the active loot
 */
void LootManager::draw(float dt) {
	std::list<Loot>::iterator i;
	for (i = theLoot.begin(); i != theLoot.end(); i++) {
		sprites[i->type]->Render(smh->getScreenX(i->x), smh->getScreenY(i->y));
	}
}


/**
 * Update the loot
 */
void LootManager::update(float dt) {
	
	//Loop through loot
	std::list<Loot>::iterator i;
	for (i = theLoot.begin(); i != theLoot.end(); i++) {

		//Update the loot collision box
		collisionBox->SetRadius(i->x, i->y, radius[i->type]);

		//Test for collision
		if (smh->player->collisionCircle->testBox(collisionBox)) {

			bool collected = false;

			//Give the player the loot
			if (i->type == LOOT_HEALTH) {
				if (smh->player->getHealth() != smh->player->getMaxHealth()) {
					smh->player->setHealth(smh->player->getHealth() + 1.0);
					//Play sound effect
					smh->soundManager->playSound("snd_Health");
					collected = true;
				} else {
					smh->popupMessageManager->showFullHealth();
				}
			} else if (i->type == LOOT_MANA) {
				if (smh->player->getMana() != smh->player->getMaxMana()) {
					smh->player->setMana(smh->player->getMana() + 20.0);
					if (smh->player->getMana() > smh->player->getMaxMana()) smh->player->setMana(smh->player->getMaxMana());
					collected = true;
					//Play sound effect
					smh->soundManager->playSound("snd_Mana");
				} else {
					smh->popupMessageManager->showFullMana();
				}
			} else if (i->type == LOOT_NEW_ABILITY) {
				smh->hge->System_Log("new ability: %d", i->ability);
				smh->saveManager->hasAbility[i->ability] = true;
				smh->windowManager->openNewAbilityTextBox(i->ability);
				collected = true;
				smh->soundManager->playSound("snd_NewAbility");

				//if there is a groupID, notify of death for it
				//this is used for bosses so their enemy blocks don't disappear until the loot is collected
				if (i->groupID != -1) smh->enemyGroupManager->notifyOfDeath(i->groupID);

			}

			if (collected) {
				i = theLoot.erase(i);
			}
		}

	}
}


/**
 * Deletes all managed loot
 */
void LootManager::reset() {
	std::list<Loot>::iterator i;
	for (i = theLoot.begin(); i != theLoot.end(); i++) {
		i = theLoot.erase(i);
	}
	theLoot.clear();
}

