#include "SmileyEngine.h"
#include "environment.h"
#include "EnemyFramework.h"
#include "Player.h"
#include "hgeanim.h"

extern SMH *smh;

DefaultEnemy::DefaultEnemy(int id, int x, int y, int groupID) {

	//Call parent function to initialize enemy info
	initEnemy(id, x, y, groupID);

	//Start in the wander state
	setState(new ES_Wander(this));

	chaseRadius = 4;

}

DefaultEnemy::~DefaultEnemy() {
	
}

void DefaultEnemy::draw(float dt) {
	graphic[facing]->Render(screenX, screenY);
}

/** 
 * Called every frame from the Enemy Manager. All the basic shit is handled by the
 * FRAMEWORK, in this function only state transitions need to be implemented.
 */
void DefaultEnemy::update(float dt) {
	
	float tempDX, tempDY;
	if (chases) doAStar();

	//Wander state
	if (currentState->instanceOf("ES_Wander\0")) {

		//Wander -> Chase
		if (chases && inChaseRange(chaseRadius)) {
			setState(new ES_Chase(this));
		}

		//Wander -> Ranged
		if (hasRangedAttack && canShootPlayer()) {
			setState(new ES_RangedAttack(this));
		}

	//Chase state
	} else if (currentState->instanceOf("ES_Chase\0")) {
		
		//Chase -> Wander
		if (!inChaseRange(chaseRadius)) {
			setState(new ES_Wander(this));
		}

		//Chase -> RangedAttack
		if (hasRangedAttack && canShootPlayer()) {
			setState(new ES_RangedAttack(this));
		}

	//Ranged attack state
	} else if (currentState->instanceOf("ES_RangedAttack\0")) {

		if (!canShootPlayer()) {

			//When leaving ranged mode increase the chase range
			//so that the enemy always chases the player to try
			//to get back into attack range.
			if (chases && inChaseRange(7)) {
				chaseRadius = 7;
				setState(new ES_Chase(this));
			} else {
				chaseRadius = 4;
				setState(new ES_Wander(this));
			}

		}
		
	}

	//No special conditions for moving
	move(dt);

}