#ifndef DCMTKFINDDATASET_H
#define DCMTKFINDDATASET_H

#include "dcmtkBaseDataset.h"
#include <iostream>
#include <vector>

/**
 * Experimental at the moment
 */
class dcmtkFindDataset : public dcmtkBaseDataset
{
public:
        const char* patientName;
        const char* patientID;
        const char* patientSex;
        const char* patientBd;
        const char* accNo;
        const char* studyID;
        const char* studyDescr;
        const char* studyDate;
        const char* studyTime;
        const char* studyInstanceUID;
        const char* refPhysician;
        const char* modalitiesInStudy;

        std::string m_studyInstUID;
        std::string m_patientName;
        std::string m_seriesDescr;
        std::string m_seriesInstUID;
        std::string m_imageType;

        void copy();
        
        const char* seriesDate;
        const char* seriesTime;
        const char* seriesDescription;
        const char* bodyPart;
        const char* seriesNumber;
        const char* seriesModality;
        const char* protocolName;
        const char* patientPosition;
        const char* frameOfRefUID;
        const char* seriesInstUID;

        const char* imageType;

        void setPatientName(const char* str){m_patientName=str;};
        std::string getPatientName(){return m_patientName;};



};


#endif DCMTKFINDDATASET_H