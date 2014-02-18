#ifndef VICONINPUTCLIENT_H
#define VICONINPUTCLIENT_H

#include "ViconSegment.h"
#include "ViconClient.h"

//#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

class ViconInputClient
{
	private:
	std::string * hostname;
    void viconInit();
    std::vector<ViconSegment> solidBodies;
	void viconAddSolidBody(std::vector<std::string> * SubjectNames, std::vector<std::string> * SegmentNames);
    
	public:
	//void exitCallback(void);
	ViconDataStreamSDK::CPP::Client MyClient; //may cause collisions later if making multiple clients
	
	std::vector<ViconSegment> * GetSolidBodies();
	void viconExit();
	void viconUpdate();	
	void printViconData(ViconSegment * segment);

	ViconInputClient(std::string * hostname, std::vector<std::string> * SubjectNames, std::vector<std::string> * SegmentNames);
};
#endif
