#ifndef __LOCAL_INPUT_CLIENT_H
#define __LOCAL_INPUT_CLIENT_H

#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <osgViewer/Viewer>
#include "Keys.h"
#include "Game.h"
#include "InputClient.h"

/**
 * \brief Represents an input client that gets its input from an X server connection.
 */
class LocalInputClient : public InputClient, public osgGA::GUIEventHandler {
public:
	LocalInputClient(int players, Game *g);
	virtual ~LocalInputClient();
	virtual void run();
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
    virtual void accept(osgGA::GUIEventHandlerVisitor& v);
private:
	void handleKeyDown(int key);
	void handleKeyUp(int key);
};

#endif
