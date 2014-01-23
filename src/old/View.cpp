#include <osg/Texture2D>

#include <osgDB/ReadFile>

#include <osgGA/NodeTrackerManipulator>

#include <osgViewer/Viewer>
#include <osg/TexEnv>
#include <osg/ShapeDrawable>

#include <osgUtil/SceneView>

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "View.h"

using namespace std;

extern char **environ;

/**
 * \brief Constructs the viewable area of the game, this includes the arena, players, and in game created objects.
 *
 * \param players the player information to associate with each player view.
 * \param setup the view configuration for this run of the game.
 * \param mapx the x width of the map.
 * \param mapy the y width of the map.
 *
 */
View::View(vector<Player*> players, SplitScreenSetup *setup, int mapx, int mapy):setup(setup){
	root = new osg::Group;
	viewer = new osgViewer::CompositeViewer;
	
	//int i = 0;
	//while(environ[i]) {
	//  printf("Environ[%d]\t%s\n", i, environ[i]);
	//  i++;
	//}
	
	//camera->setClearColor(osg::Vec4(0, 0, 255, 1)); // black background
	//camera->setProjectionMatrixAsPerspective(30, 4.0 / 3.0, 0.1, 100);
	//camera->setViewMatrixAsLookAt(
	//	osg::Vec3(-7.5, 7.5, 0), // eye above xy-plane
	//	osg::Vec3(0, 3, 0),    // gaze at cycle
	//	osg::Vec3(0, 1, 0));   // usual up vector
	//	
	//view->setCamera(camera);
	//view->setSceneData(root);	
	//view->setUpViewInWindow(200, 200, 500, 500);
	
	char *display = getenv("DISPLAY");
	printf("Display variable is %s\n", display);
	char hostname[256] = {0};
	char displayStr[256] = {0};
	char screenStr[256] = {0};
	
	char *tmpHostName = display;
	char *tmpDisplayNumStr;
	char *tmpScreenNumStr;	
	
	char *cursor = display;
	if(display){
		while(cursor && *cursor){
			if(*cursor == ':' || *cursor == '.'){
				*cursor = '\0';
				cursor++;
				break;
			}
			cursor++;
		}
		tmpDisplayNumStr = cursor;
		while(cursor && *cursor){
			if(*cursor == ':' || *cursor == '.'){
				*cursor = '\0';
				cursor++;
				break;
			}
			cursor++;
		}
		tmpScreenNumStr = cursor;
		
		if(strlen(hostname) == 0){
			strcpy(hostname, "localhost");
		} else{
			strcpy(hostname, tmpHostName);
		}
	
		if(strlen(tmpDisplayNumStr) == 0){
			printf("Could not parse display variable\n");
			exit(1);
		} else{
			strcpy(displayStr, tmpDisplayNumStr);
		}
	
		if(strlen(tmpScreenNumStr) == 0){
			strcpy(screenStr, "0");
		} else{
			strcpy(screenStr, tmpScreenNumStr);
		}
	} else{
		printf("Absent display variable assuming 'localhost:0.0'");
		strcpy(hostname, "localhost");
		strcpy(displayStr, "0");
		strcpy(screenStr, "0");
	}
	
	int displayNum = atoi(displayStr);
	int screenNum = atoi(screenStr);
	printf("Creating graphics context on %s:%d.%d\n", hostname, displayNum, screenNum);
	
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits; 
	traits->x = 0; 
	traits->y = 0; 
	traits->width = setup->getLocalScreen()->getWidth(); 
	traits->height = setup->getLocalScreen()->getHeight(); ; 
	traits->windowDecoration = false; 
	traits->supportsResize = false; 
	traits->windowName = "FitAShape"; 
	traits->doubleBuffer = true;
	traits->hostName = hostname;
	traits->displayNum = displayNum;
	traits->screenNum = screenNum;

	gc = osg::GraphicsContext::createGraphicsContext(traits); 
	if (gc.valid()) 
	{ 
		gc->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.6f, 1.0f)); 
		gc->setClearMask(GL_COLOR_BUFFER_BIT | 
		GL_DEPTH_BUFFER_BIT); 
	} else{
		printf("Failed to create graphics context\n");
		exit(1);
	}
	
	createArena(mapx, mapy);
	createPlayers(players);
	
	hud = new HUD(players);
	root->addChild(hud->getHudRoot());
}

/**
 * \brief Deconstructor for the View object.
 */
View::~View(){
}


/**
 * \brief creates the floor of the arena with the dimensions passed in.
 *
 * \param width the x width of the floor.
 * \param height the y width of the floor.
 *
 */
void View::createFloor(int width, int height){
	
	floor = new osg::Geode;

	osg::Vec2Array* texcoords = new osg::Vec2Array(4);
	(*texcoords)[0].set(0.0f,1.0f);
	(*texcoords)[1].set(0.0f,0.0f);
	(*texcoords)[2].set(1.0f,0.0f);
	(*texcoords)[3].set(1.0f,1.0f);
	
	osg::Vec3Array* normals = new osg::Vec3Array(1);
	(*normals)[0].set(0.0f,1.0f,0.0f);
	
	osg::Vec4Array* colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f,1.0f,1.0f,1.0f);
	
	
	for(int x = -(width / 2); x <= width/2; x = x + 5) { 
		for(int y = -(height / 2); y <= height/2; y = y + 5) {
			osg::ref_ptr<osg::Geometry> myPlane(new osg::Geometry());
	
			//plane verticies
			osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
			pyramidVertices->push_back( osg::Vec3( x, 0, y) ); // front left
			pyramidVertices->push_back( osg::Vec3( x, 0, y + 5) ); // front right
			pyramidVertices->push_back( osg::Vec3( x + 5, 0, y + 5) ); // back right 
			pyramidVertices->push_back( osg::Vec3( x + 5, 0, y) ); // front left
			pyramidVertices->push_back( osg::Vec3( x, 0, y) ); // front left
			
			myPlane->setVertexArray(pyramidVertices);
			
	 	 	myPlane->setTexCoordArray(0,texcoords);
			
		    myPlane->setNormalArray(normals);
		    myPlane->setNormalBinding(osg::Geometry::BIND_OVERALL);
			
		    myPlane->setColorArray(colors);
			myPlane->setColorBinding(osg::Geometry::BIND_OVERALL);
	
    		myPlane->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));
			floor->addDrawable(myPlane);
		}
	}

	//texture
	osg::Texture2D* KLN89FaceTexture = new osg::Texture2D;

	// protect from being optimized away as static state:
	KLN89FaceTexture->setDataVariance(osg::Object::DYNAMIC); 

	// load an image by reading a file: 
	osg::Image* klnFace = osgDB::readImageFile("./assets/TronBackground.jpg");
	if (!klnFace) {
		cout << " couldn't find texture, quiting." << endl;
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}

   	// Assign the texture to the image we read from file: 
  	 KLN89FaceTexture->setImage(klnFace);
  	 
	// Create a new StateSet with default settings: 
	osg::StateSet* stateOne = new osg::StateSet();

	// Assign texture unit 0 of our new StateSet to the texture 
	// we just created and enable the texture.
	stateOne->setTextureAttributeAndModes(0,KLN89FaceTexture,osg::StateAttribute::ON);
	
	// Associate this state set with the Geode that contains the pyramid: 
	floor->setStateSet(stateOne);
}


/**
 * \brief Creates a box with the specified widths and texture at the center, with reference to the parent that the box is placed in.
 *
 * \param textureFile the file location of the texture file to be loaded and applied to the box.
 * \param centerz the z offset from the center of the parent.
 * \param centery the y offset of this object from its parents center.
 * \param centerx the x offset of this plane from its parents center.
 * \param widthz the objects width in the z dimension.
 * \param widthy the objects width in the y dimension.
 * \param widthx the objects width in the x dimension.
 *
 * \return the plane created according to the passed in parameters.
 *
 **/
osg::Geode* createPlane(std::string textureFile, int centerz, int centery, int centerx, float widthz, float widthy, float widthx) {


	//Read in texture
	osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);

	texture->setDataVariance(osg::Object::DYNAMIC);
	osg::Image* face = osgDB::readImageFile(textureFile);

	if(!face) {
		std::cout << " couldn't find texture: " << textureFile << ". Quitting " << std::endl;
		printf("Exiting %s:%d\n", __FILE__, __LINE__);
		exit(1);
	}
	texture->setImage(face);


	osg::ref_ptr<osg::StateSet> decalStateSet(new osg::StateSet());
	osg::ref_ptr<osg::TexEnv> decalTexEnv(new osg::TexEnv());

	decalTexEnv->setMode(osg::TexEnv::DECAL);
	decalStateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	decalStateSet->setTextureAttribute(0, decalTexEnv);

	//Create box with dimensions and drawable
	osg::ref_ptr<osg::Box> plane(new osg::Box(osg::Vec3(centerz, centery, centerx), widthz, widthy, widthx));
	plane->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr<osg::ShapeDrawable> planeDrawable(new osg::ShapeDrawable(plane));

	//Place drawable box in geode
	//Seg Faults when you use an osg::ref_ptr, stick with new.
	osg::Geode* planeGeode = new osg::Geode();
	planeGeode->addDrawable(planeDrawable);
	planeGeode->setStateSet(decalStateSet);

	return planeGeode;
}

/**
 * \brief Creates the walls for the arena.
 *
 * \param width, the x width of the arena.
 * \parma heigth, the y width of the arena.
 *
 */
void View::createWalls(int width, int height){
	walls.push_back(createPlane("./assets/TronBackground.jpg", -width/2, 3, 0, 1.0f, 6.0f, width + 1));
	walls.push_back(createPlane("./assets/TronBackground.jpg", width/2, 3, 0, 1.0f, 6.0f, width + 1));
	walls.push_back(createPlane("./assets/TronBackground.jpg", 0, 3, -height/2, height + 1, 6.0f, 1.0f));
	walls.push_back(createPlane("./assets/TronBackground.jpg", 0, 3, height/2, height + 1, 6.0f, 1.0f));
}

/**
 * \brief Creates the arena, this includes the floor and walls.
 *
 * \param width the x width of the arena.
 * \param height the y width of the arena.
 */
void View::createArena(int width, int height){
	arena = new osg::Group;
	//arena->addChild(wallOne);
	//arena->addChild(wallTwo);
	//arena->addChild(wallThree);
	//arena->addChild(wallFour);

	createFloor(width, height);
	arena->addChild(floor);
	createWalls(width, height);
	for(std::vector<osg::ref_ptr<osg::Geode> >::iterator it = walls.begin(); it != walls.end(); ++it) {
		arena->addChild(*it);
	}

	root->addChild(arena);
	root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

/**
 * \brief Creates each players view and graphics context and sets its initial position.
 *
 * \param players the backend information for the players.
 *
 */
void View::createPlayers(std::vector<Player*> players){
	
	int x = 0;
	int width = 500;
	int maskID = 0x1;
	for(std::vector<Player*>::iterator it = players.begin(); it != players.end(); ++it) {
	
		int inheritanceMask = 
          (osgUtil::SceneView::ALL_VARIABLES &
          ~osgUtil::SceneView::CULL_MASK);
		
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
	
		//camera->setClearColor(osg::Vec4(0, 0, 255, 1)); // black background
		//camera->setProjectionMatrixAsPerspective(30, 4.0 / 3.0, 0.1, 100);
		//camera->setViewMatrixAsLookAt(
		//	osg::Vec3(-7.5, 7.5, 0), // eye above xy-plane
		//	osg::Vec3(0, 3, 0),    // gaze at cycle
		//	osg::Vec3(0, 1, 0));   // usual up vector
		
		camera->setGraphicsContext(gc);
		Viewport *v = setup->getLocalViewportForPlayer((*it)->getID());
		camera->setViewport(new osg::Viewport(v->getXOffset(), v->getYOffset(), v->getWidth(), v->getHeight()));
		x += width;

		(*it)->setView(new PlayerView((*it)->getID(), root, (*it)->getPosition(), (*it)->getViewDirection(), (*it)->getNormal(), camera));
			
		view->setCamera(camera);
		view->setSceneData(root);
		
		 view->getCamera()->setInheritanceMask(inheritanceMask);
		 view->getCamera()->setCullMask(maskID);
	
		viewer->addView(view);
		maskID <<= 1;
	}
}

/**
 * Realize the viewer, call this to initialize opengl and openscenegraph.
 */
void View::realize(){
	viewer->realize();
}

/**
 * Is the viewer realized?
 */
bool View::isRealized(){
	return viewer->isRealized();
}

/**
 * Has the client requested to exit?
 */
bool View::done(){
	return viewer->done();
}

/**
 * Render a frame to the screen.
 */
void View::frame(){
	hud->update();
	viewer->frame();
}

/**
 * Get the CompositeViewer that does the rendering and compositing.
 */
osg::ref_ptr<osgViewer::CompositeViewer> View::getOSGViewer() {
	return viewer;
}
