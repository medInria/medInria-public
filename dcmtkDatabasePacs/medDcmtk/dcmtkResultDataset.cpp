#include "dcmtkResultDataset.h"

//---------------------------------------------------------------------------------------------

dcmtkKeyContainer* dcmtkResultDataset::getKeyContainer()
{
    return &m_keyContainer;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkResultDataset::getStudyInstanceUID()
{
    return m_studyInstUID.c_str();
}

//---------------------------------------------------------------------------------------------

void dcmtkResultDataset::setStudyInstanceUID( const char* uid )
{
    if (uid !=  NULL)
        m_studyInstUID = uid;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkResultDataset::getSeriesInstanceUID()
{
    return m_seriesInstUID.c_str();
}

//---------------------------------------------------------------------------------------------

void dcmtkResultDataset::setSeriesInstanceUID( const char* uid )
{
    if (uid !=  NULL)
        m_seriesInstUID = uid;
}

//---------------------------------------------------------------------------------------------

const char* dcmtkResultDataset::getSOPInstanceUID()
{
    return m_sopInstUID.c_str();
}

//---------------------------------------------------------------------------------------------

void dcmtkResultDataset::setSOPInstanceUID( const char* uid )
{
    if (uid !=  NULL)
        m_sopInstUID = uid;
}

//---------------------------------------------------------------------------------------------
