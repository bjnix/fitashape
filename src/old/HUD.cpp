#include "HUD.h"
#include <stdio.h>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osgText/Text>

struct HUDForPlayer{
public:
	HUDForPlayer(osg::ref_ptr<osgText::Text> identifier, osg::ref_ptr<osgText::Text> score):scoreText(score),identifier(identifier){}
	void update(int score){
		char scoreStrBuf[32] = {'\0'};
		sprintf(scoreStrBuf, "Score %d", score);
		std::string str(scoreStrBuf);
		scoreText->setText(str);
	}
private:
	osg::ref_ptr<osgText::Text> scoreText;
	osg::ref_ptr<osgText::Text> identifier;
};

HUD::HUD(std::vector<Player*> players):players(players){
	hudRoot = new osg::Group();
	setupPlayers();
	
}

osg::Node* HUD::getHudRoot(){
	return hudRoot;
}

void HUD::update(){
	for(unsigned int i = 0; i < huds.size(); i++){
		huds[i]->update(players[i]->getScore());
	}
}

float fabs(float a){
	if(a < 0) return -a;
	return a;
}

void HUD::setupPlayers(){
	for(unsigned i = 0; i < players.size(); i++){
		setupPlayer(i);
	}
	update();
}

void HUD::setupPlayer(int player){
	osg::ref_ptr<osgText::Text> identifier = new osgText::Text;
	osg::ref_ptr<osgText::Text> score = new osgText::Text;
	char buf[256];
	
	HUDForPlayer *h = new HUDForPlayer(identifier, score);
	huds.push_back(h);

	identifier->setCharacterSize(5);
	identifier->setFont("../arial.ttf");
	sprintf(buf, "Player %d", player+1);
	identifier->setText(buf);
	identifier->setPosition( osg::Vec3(-67,47,-200) );
	identifier->setColor( osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f) );

	score->setCharacterSize(5);
	score->setFont("../arial.ttf");
	score->setPosition( osg::Vec3(-67,40,-200) );
	score->setColor( osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f) );

	osg::Geode* tankLabelGeode = new osg::Geode();
	tankLabelGeode->addDrawable(identifier);
	tankLabelGeode->addDrawable(score);
	osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
	HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	HUDModelViewMatrix->setMatrix(osg::Matrix::identity());
	HUDModelViewMatrix->addChild(tankLabelGeode);

	osg::StateSet* HUDStateSet = new osg::StateSet(); 
	tankLabelGeode->setStateSet(HUDStateSet);
	HUDStateSet->setRenderBinDetails( 11, "DepthSortedBin");
	HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
	HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	hudRoot->addChild(HUDModelViewMatrix);
	HUDModelViewMatrix->setNodeMask(0x1 << player);
}
