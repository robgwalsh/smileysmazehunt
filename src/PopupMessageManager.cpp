#include "SmileyEngine.h"
#include "WindowFramework.h"

extern SMH *smh;

PopupMessageManager::PopupMessageManager() 
{
	adviceManMessageActive=false;
}

PopupMessageManager::~PopupMessageManager() 
{
	//should never be deleted
}

void PopupMessageManager::draw(float dt) 
{
	if (smh->timePassedSince(timeMessageStarted) < messageDuration) 
	{
		//Determine text alpha - fade out near the end
		if (smh->timePassedSince(timeMessageStarted) > messageDuration * 0.75) 
		{
			messageAlpha -= 255.0 * (1.0/(messageDuration * 0.25)) * dt;
			if (messageAlpha < 0.0) {messageAlpha = 0.0; adviceManMessageActive=false;}
		}

		if (adviceManMessageActive) smh->resources->GetFont("inventoryFnt")->SetScale(0.8);
		smh->resources->GetFont("inventoryFnt")->SetColor(ARGB(messageAlpha,255.0,255.0,255.0));
		smh->resources->GetFont("inventoryFnt")->printf(540,710,HGETEXT_CENTER, message.c_str());
		smh->resources->GetFont("inventoryFnt")->SetColor(ARGB(255.0,255.0,255.0,255.0));
		if (adviceManMessageActive) smh->resources->GetFont("inventoryFnt")->SetScale(1.0);

		if (adviceManMessageActive) 
		{
			smh->resources->GetSprite("adviceManDown")->SetColor(ARGB(messageAlpha, 255, 255, 255));
			smh->resources->GetSprite("adviceManDown")->RenderEx(40.0, 725.0, 0.0, 0.8, 0.8);
			smh->resources->GetSprite("adviceManDown")->SetColor(ARGB(255, 255, 255, 255));
		}
	}
}

void PopupMessageManager::update(float dt) 
{
	//Got rid of "Press N to view it Now" cause it's messy
	/*
	if (adviceManMessageActive && smh->hge->Input_KeyDown(HGEK_N)) 
	{
		smh->windowManager->openAdviceTextBox(advice);
		timeMessageStarted = 0.0;
		adviceManMessageActive=false;
	}
	*/
}

void PopupMessageManager::showFullHealth() 
{
	if (adviceManMessageActive) return;
	message = "Your health is already full!";
	startMessage(1.5);
}

void PopupMessageManager::showFullMana() 
{
	if (adviceManMessageActive) return;
	message = "Your mana is already full!";
	startMessage(1.5);
}

void PopupMessageManager::showNewAdvice(int _advice) {
	message = "New advice is available from Monocle Man. Go see him near your house to view it.";
	startMessage(5.5);
	adviceManMessageActive = true;
	advice = _advice;
	smh->soundManager->playSound("snd_HintMan");
}

void PopupMessageManager::showSaveConfirmation() {
	if (adviceManMessageActive) return;
	message = "Game File ";
	message += Util::intToString(smh->saveManager->currentSave + 1);
	message += " Saved!";

	startMessage(2.5);
}

void PopupMessageManager::startMessage(float duration) {
	messageAlpha = 255.0;
	messageDuration = duration;
	timeMessageStarted = smh->getGameTime();
	adviceManMessageActive = false;
}