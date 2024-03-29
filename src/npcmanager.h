#ifndef _NPCMANAGER_H_
#define _NPCMANAGER_H_

#include <list>

class Tongue;
class CollisionCircle;
class NPC;

struct NPCStruct {
	NPC *npc;
};

class NPCManager {

public:
	NPCManager();
	~NPCManager();

	//methods
	void draw(float dt);
	void update(float dt);
	void addNPC(int id, int textID, int x, int y);
	void reset();
	bool npcCollision(hgeRect *box, int id);
	bool talkToNPCs(Tongue *tongue);

	//Variables
	std::list<NPCStruct> theNPCs;
	CollisionCircle *collisionCircle;

};

#endif