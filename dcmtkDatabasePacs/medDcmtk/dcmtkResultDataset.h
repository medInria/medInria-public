#ifndef DCMTKRESULTDATASET_H
#define DCMTKRESULTDATASET_H

// inc
#include <iostream>

#include "dcmtkKeyContainer.h"

//fwd
class dcmtkKeyContainer;


/**
 * @class dcmtkResultDataset
 * @author Michael Knopke
 * @brief class to store results of a C-FIND
 * The key container contains all requested and found values
 * The basic search keys can be get from container or from function (e.g.g getStudyInstanceUID)
 */
class dcmtkResultDataset
{
public:

        const char* getStudyInstanceUID();
        void setStudyInstanceUID(const char* uid);

        const char* getSeriesInstanceUID();
        void setSeriesInstanceUID(const char* uid);

        const char* getSOPInstanceUID();
        void setSOPInstanceUID(const char* uid);

        dcmtkKeyContainer* getKeyContainer();

protected:

        dcmtkKeyContainer        m_keyContainer;

        std::string              m_studyInstUID;
        std::string              m_seriesInstUID;
        std::string              m_sopInstUID;


};



#endif DCMTKRESULTDATASET_H