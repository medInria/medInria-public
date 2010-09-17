#ifndef DCMTKFINDDATASET_H
#define DCMTKFINDDATASET_H

// inc
#include <iostream>
#include <vector>
#include "dcmtkContainer.h"

class dcmtkKey
{
public:
    int group;
    int elem;
    std::string value;
};

class dcmtkKeyContainer : public dcmtkContainer<dcmtkKey*>
{
public:
       const char* findKeyValue(int group, int elem);
};

/**
 * @class dcmtkFindDataset
 * @author Michael Knopke
 * @brief class to store results of a C-FIND
 * The key container contains all requested and found values
 * The basic search keys can be get from container or from function (e.g.g getStudyInstanceUID)
 */
class dcmtkFindDataset
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


#endif DCMTKFINDDATASET_H