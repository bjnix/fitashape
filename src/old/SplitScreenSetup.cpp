#include "SplitScreenSetup.h"

Screen::Screen(int width, int height):width(width),height(height){
}

int Screen::getWidth(){
	return width;
}

int Screen::getHeight(){
	return height;
}


Viewport::Viewport(int xoff, int yoff, int width, int height):Screen(width, height),xoff(xoff),yoff(yoff){
}

int Viewport::getXOffset(){
	return xoff;
}

int Viewport::getYOffset(){
	return yoff;
}


LocalGeometry::LocalGeometry(int xoff, int yoff, int width, int height):Viewport(xoff, yoff, width, height){
}


SplitScreenSetup::SplitScreenSetup(int players, LocalGeometry* local, Screen* global):players(players),local(local),global(global){
}

int SplitScreenSetup::getNumPlayers(){
	return players;
}

Screen* SplitScreenSetup::getTotalScreen(){
	return global;
}

Screen* SplitScreenSetup::getLocalScreen(){
	return local;
}

Viewport* SplitScreenSetup::getLocalViewportForPlayer(int player){
	int tileWidth = 1;
	int tileHeight = 1;
	//force more displays horiuzontally
	while(tileWidth * tileWidth < players){
		tileWidth++;
	}
	//find the tallest configuration which is square
	while(tileWidth < players && players % tileWidth != 0){
		tileWidth++;
	}
	//if its bad, just accept that some display slots will be empty
	if(tileWidth - tileHeight > 1){
		tileWidth--;
		tileHeight++;
	}
	int tileX = player % tileWidth;
	int tileY = player / tileWidth;
	int playerViewWidth = global->getWidth() / tileWidth;
	int playerViewHeight = global->getHeight() / tileHeight;
	int globalXoffPlayer = tileX * playerViewWidth;
	int globalYoffPlayer = tileY * playerViewHeight;
	int adjustmentX = globalXoffPlayer - local->getXOffset();
	int adjustmentY = globalYoffPlayer - local->getYOffset();
	return new Viewport(adjustmentX, adjustmentY, playerViewWidth, playerViewHeight);
}
