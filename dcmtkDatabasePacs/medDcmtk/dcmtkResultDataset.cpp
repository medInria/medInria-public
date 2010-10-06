#include "dcmtkResultDataset.h"

//---------------------------------------------------------------------------------------------

dcmtkContainer<dcmtkKey*>* dcmtkResultDataset::getKeyContainer()
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

const char* dcmtkResultDataset::findKeyValue(int group, int elem)
{
    dcmtkContainer<dcmtkKey*>* cont= this->getKeyContainer();

    for(int i = 0; i < cont->size(); i++)
    {
        if( (cont->getAtPos(i)->group == group) && (cont->getAtPos(i)->elem == elem) )
            return cont->getAtPos(i)->value.c_str();
    }

    return NULL;
}

//---------------------------------------------------------------------------------------------
