#pragma once

#include <hgegui.h>
#include <hgefont.h>
#include <string>

class Forminput : public hgeGUIObject{
 
public:
	Forminput(int _id, float xPos, float yPos,
		float length,std::string _label, char* prevalue=0);
	~Forminput();
	virtual void Render();
	//void Render(float x, float y);
	virtual bool MouseLButton(bool bDown);
	virtual void Focus(bool bFocused);
	virtual bool KeyClick(int key, int chr);
	virtual void Update(float fDeltaTime);
	void setText(char *newText);
	char *getText();
 
protected:
	int caretposition;
	char* caret;
	bool caretVisible; //for blinking effect
	bool focused;
	float caretTimer; //for blinking effect
	const static int maxchar = 30;
	float xPos;
	float yPos;
	hgeFont *font;
	float length;
	char *content;
	std::string label;
};