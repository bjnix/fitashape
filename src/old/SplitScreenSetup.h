#ifndef SPLITSCREENSETUP_H
#define SPLITSCREENSETUP_H

/**
 * \brief Represents the dimensions of the whole virtual screen we display on.
 */
class Screen {
public:
	Screen(int width, int height);
	int getWidth();
	int getHeight();
private:
	int width;  /** The width of the screen. */
	int height; /** The height of the screen. */
};

/**
 * \brief Represents a viewport within a screen.
 */
class Viewport : public Screen {
public:
	Viewport(int xoff, int yoff, int width, int height);
	int getXOffset();
	int getYOffset();
private:
	int xoff; /** The x offset of the viewport. */
	int yoff; /** The y offset of the viewport. */
};

/**
 * \brief Represents one screen's geometry in relation to the whole virtual screen.
 */
class LocalGeometry : public Viewport {
public:
	LocalGeometry(int xoff, int yoff, int width, int height);
private:
};

/**
 * \brief Determines the number of rows and columns and the viewport for each player intelligently.
 */
class SplitScreenSetup {
public:
	SplitScreenSetup(int players, LocalGeometry* local, Screen* global);
	int getNumPlayers();
	Screen* getTotalScreen();
	Screen* getLocalScreen();
	Viewport* getLocalViewportForPlayer(int player);
	
private:
	int players;          /** The number of players. */
	LocalGeometry* local; /** The LocalGeometry of this screen. */
	Screen* global;       /** The global virtual screen. */
};

#endif
