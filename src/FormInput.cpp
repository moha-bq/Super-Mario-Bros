#include "FormInput.h"
#include "Game.h"

#include <cstring>



Forminput::Forminput(int _id, float _xPos, float _yPos, 
	float _length,std::string _label, char *prevalue){

		font = Game::getInstance()->mSmallFont;
		id = _id;
		length = _length;
		xPos = _xPos;
		yPos = _yPos;
		bStatic = false;
		bVisible = true;
		bEnabled = true;
		label=_label;
		caretVisible = true;
		focused = false;
		caretTimer = 0.0f;
		caret = "_";
		content = new char[maxchar+1]; //maxchar is defined in the header
		if(prevalue)
		{
			strncpy(content, prevalue, maxchar);
			int prevalue_length = (int)strlen(prevalue);
			if (prevalue_length < maxchar){
				caretposition = prevalue_length;
			}
			else{
				caretposition = maxchar-1;
			}
		}
		else
		{
			caretposition=0;
		}

		content[caretposition] = '\0';
		content[maxchar+1] = '\0'; //Last char will always be a string terminator
		rect.Set(xPos+font->GetStringWidth(label.c_str())+3, yPos, xPos+length+font->GetStringWidth(label.c_str())+3, yPos+(font->GetHeight()));
};

Forminput::~Forminput(){
}

void Forminput::Render(){
	font->Render(xPos, yPos, HGETEXT_LEFT, label.c_str());
	font->Render(xPos+font->GetStringWidth(label.c_str())+3, yPos, HGETEXT_LEFT, content);
	//Renders the caret
	if (focused && caretVisible){
		float width = font->GetStringWidth(content);
		font->Render(xPos+width+font->GetStringWidth(label.c_str())+3, yPos, HGETEXT_LEFT, caret);
	}
}

//Gets Focus with LeftMouseKlick
bool Forminput::MouseLButton(bool bDown){
	if (bDown){
		return false;
	}
	gui->SetFocus(id);
	return false;
};

//Saving focus state in focused
void Forminput::Focus(bool bFocused){
	if (!bFocused){
		focused = false;
	}
	else{
		focused = true;
	}

};

void Forminput::Update(float fDeltaTime){
	caretTimer += fDeltaTime;
	/*Makes the caret blinking effect
	You might want to change the value below for slower/faster blinking*/
	if (caretTimer >= 0.1f){
		caretVisible = !caretVisible;
		caretTimer = 0.0f;
	}
}

bool Forminput::KeyClick(int key, int chr){
	//Loses Focus with Enter
	if (key == HGEK_ENTER){
		gui->SetFocus(-1);
		return false;
	}

	if (key == HGEK_BACKSPACE){ //Delete one character per backspace
		if (caretposition != 0){
			caretposition -=1;
			content[caretposition] = '\0'; //Put string terminator one character back
		}

		return false;
	}
	/*When chr==0 no ascii-key (e.g. shift) is pressed and chr would
	be saved as string terminator. The other thing to look after, is
	whether we have max. characters*/
	if (chr != 0 && caretposition < maxchar-1){ 
		content[caretposition] = chr;
		content[caretposition+1] = '\0'; //add overwritten string terminator
		//Assures that the String will not take to much place
		//if (font->GetStringWidth(tmp) <= length){
		caretposition+=1;
		//}
	}
	return false;
};

//Setting the Text
void Forminput::setText(char *newtext){
	strncpy(content ,newtext ,maxchar);
	content = newtext;
};

char *Forminput::getText(){
	return content;
}