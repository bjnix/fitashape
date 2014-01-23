/**fitashape.h
*/

#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>
#include <stdlib.h>
#include "driverChoice.h"
#include "vector3d.h"

void drawTargets(scene::ISceneManager* smgr);

scene::ISceneNode * currentNode();
void setCurrent(CircleNode noe);
void initializePosition();
void drawTargets(scene::ISceneManager* smgr);
void drawLimbs(scene::ISceneManager* smgr);
bool collide (CircleNode node);


