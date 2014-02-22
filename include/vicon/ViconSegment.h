#ifndef VICONSEGMENT_H
#define VICONSEGMENT_H
#include "ViconClient.h"


class ViconSegment
{
  private:
    double * X_Y_Z;
    double * phi_theta_psi;
    std::string SegmentName;
    std::string SubjectName;
    ViconDataStreamSDK::CPP::Client * segClient;
  public:
    double * setTranslation(ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation);
    double * setOrientationEuler(ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationEulerXYZ);
    double * getTranslation();
    double * getOrientationEuler();
    double getX();
    double getY();
    double getZ();
    double getPhi();
    double getTheta();
    double getPsi(); 

    
    std::string getSubjectName();
    std::string getSegmentName();

    ViconSegment(std::string SubjectName, std::string SegmentName, ViconDataStreamSDK::CPP::Client *SegClient);
    //~ViconSegment();



};
#endif
