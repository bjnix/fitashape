#ifndef VICONSEGMENT_H
#define VICONSEGMENT_H
#include "ViconClient.h"
#include <string>
#include <vector>

class ViconSegment
{
  private:
    double * X_Y_Z;
    double * phi_theta_psi;
    std::string SegmentName;
    std::string SubjectName;
    ViconDataStreamSDK::CPP::Client *SegClient;
  public:
    double * getTranslation();
    double * getOrientationEuler();
    double getX();
    double getY();
    double getZ();
    double getPhi();
    double getTheta();
    double getPsi(); 

    //TODO add printing or preferably return a string
    //void printSubjectName();
    //void printSegmentName();

    ViconSegment(std::string &SubjectName, std::string &SegmentName, ViconDataStreamSDK::CPP::Client *SegClient);
    //~ViconSegment();



};
#endif
