#ifndef _CONSERVATORYBOSS_H_
#define _CONSERVATORYBOSS_H_

#include "boss.h"

class hgeParticleSystem;
class WeaponParticleSystem;

struct EyeFlash {
	float red,green,blue,alpha;
	bool eyeFlashing;
	float timeStartedFlash;
	float x,y,dx,dy; //these are variables for the BULLS-EYE that tracks Smiley
};

//these launch the grid of projectiles
struct ProjectileLauncher {
	float angle;
	float x,y;

	//timing is based on a MASTER PULSE GENERATOR. Each projectile launcher has a time of offset that
	//defines how long after the master pulse generator it fires.
	// 0.0 is in sync, 1.0 is in sync, 0.5 is perfectly out of sync
	float timingOffset;
	bool hasFiredDuringThisPulse;
};

struct floatingEye {
	float x,y;
	float yElevation;
	float angleFacing, angleMoving, desiredAngleFacing;
	float timeCreated;
	float swayPeriod; //the floating eyes sway back and forth in place, each with a slightly different time period
	int state;
	float timeArrivedAtBottom;
	hgeRect *collisionBox;

	float xCrosshair,yCrosshair;
	float aimX,aimY;
	float timeStartedCountdown;

	float timeOfLastShot;
};

class ConservatoryBoss : public Boss {

public:
	ConservatoryBoss(int _gridX, int _gridY, int _groupID);
	~ConservatoryBoss();

	//methods
	void draw(float dt);
	void drawBarvinoid();
	void drawAfterSmiley(float dt);
	bool update(float dt);
	void enterState(int _state);
	void finish();
	void placeCollisionBoxes();
	void initGridOfProjectiles();
	void doGridOfProjectiles();
	void addFloatingEye(float addX, float addY);
	void updateFloatingEyes(float dt);
	void drawFloatingEyes();
	void drawFloatingEyeShadows();
	void updateMouthAnim(float dt);
	void drawMouthAnim();
	void updateEyeGlow(int eye);
	void testCollisions(float dt);
	void purgeFloatingEyes();
	void makeEyesFloatAway();

	//State methods
	void doEyeAttackState(float dt);
	void doHoppingState(float dt);
	void doHoppingToEdgeState(float dt);
	void doFloatingEyeReleaseState(float dt);
	void doFloatingEyeState(float dt);
	void doHoppingToCenterState(float dt);
	void doHop(float dt, float destinationX, float destinationY);
		
	//Variables
	int gridX, gridY;
	float x, y;
	float xLoot,yLoot;
	float xUpperEdge,yUpperEdge;
	int state;
	bool startedIntroDialogue;
	float lastHitByTongue;
	float timeEnteredState;
	float timeEnteredGSS;
	hgeRect *collisionBoxes[3];
	bool barvinoidCanPass[256];
	bool droppedLoot;
	bool shouldDrawAfterSmiley;
	bool startedComplainDialogue;
	bool startedDeathDialogue;
	bool inAir; //this is used to trigger the HOVERING sound effect
	bool justLanded; //this is used to trigger the THUD sound effect

	//These variables are used to implement a delay from the time he's hit by a floating eye to the time
	//he complains about it. That way the explosion has time to be seen before the dialogue box comes up
	float timeHitByFloatingEye;
	bool hasBeenHitByFloatingEye;

	//hopping variables
	float timeStartedHop;
	float hopY;
	
	//Eye attack variables
	EyeFlash eyeFlashes[2];

	float lastEyeAttackTime;
	float eyeAttackInterval; //the interval between eye attacks decreases with each attack
	int lastEyeToAttack;

	//Grid of projectiles variables
	float timeOfLastMasterPulse;
	float masterPulseInterval;
	ProjectileLauncher projectileLauncher[16];

	//Floating eye variables
	std::list<floatingEye> theFloatingEyes;
	int numFloatingEyes;
	float lastFloatingEyeTime;
	int mouthState; //Not active; opening; staying open; closing
	float beginMouthStayOpenTime; //when did the mouth open all the way?
	float smileyLastSeenAtX, smileyLastSeenAtY;
};

#endif