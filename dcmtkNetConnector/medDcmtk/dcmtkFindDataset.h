#ifndef DCMTKFINDDATASET_H
#define DCMTKFINDDATASET_H

#include <iostream>
#include <vector>

class dcmtkFindDataset
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

        void copy();

};

#endif DCMTKFINDDATASET_H