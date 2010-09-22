#include "dcmtkFindDataset.h"

//---------------------------------------------------------------------------------------------

dcmtkKeyContainer* dcmtkFindDataset::getKeyContainer()
{
    return &m_keyContainer;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkFindDataset::getStudyInstanceUID()
{
    return m_studyInstUID.c_str();
}

//---------------------------------------------------------------------------------------------

void dcmtkFindDataset::setStudyInstanceUID( const char* uid )
{
    if (uid !=  NULL)
        m_studyInstUID = uid;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkFindDataset::getSeriesInstanceUID()
{
    return m_seriesInstUID.c_str();
}

//---------------------------------------------------------------------------------------------

void dcmtkFindDataset::setSeriesInstanceUID( const char* uid )
{
    if (uid !=  NULL)
        m_seriesInstUID = uid;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkFindDataset::getSOPInstanceUID()
{
    return m_sopInstUID.c_str();
}

//---------------------------------------------------------------------------------------------

void dcmtkFindDataset::setSOPInstanceUID( const char* uid )
{
    if (uid !=  NULL)
        m_sopInstUID = uid;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkKeyContainer::findKeyValue( int group, int elem )
{
    const char* result = "";
    for(iter = data.begin(); iter != data.end(); iter++)
    {
        if ( ((*iter)->group == group) && ((*iter)->elem == elem) )
        {
                result = (*iter)->value.c_str();
                break;
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------
