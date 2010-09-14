#include "dcmtkFindDataset.h"

void dcmtkFindDataset::copy()
{
    if (studyInstanceUID != NULL)
        m_studyInstUID = studyInstanceUID;
    if (patientName != NULL)
        m_patientName = patientName;

    if (seriesDescription != NULL)
        m_seriesDescr = seriesDescription;

    if (seriesInstUID != NULL)
        m_seriesInstUID = seriesInstUID;

    if (imageType != NULL)
        m_imageType = imageType;
 
}

//---------------------------------------------------------------------------------------------

