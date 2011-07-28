#ifndef medDicomMetaData_h__
#define medDicomMetaData_h__

#define GETSET(var) \
private: \
    QString _##var; \
public: \
    QString get##var() \
         {\
         return _##var; \
         }\
         void set##var(QString val) \
         {\
         _##var = val; \
         }

#include "medCoreExport.h"
#include <iostream>

/**
 * Holds dicom specific data used for exporting
 */
class MEDCORE_EXPORT medDicomMetaData
{
public:

    // meta
    
    GETSET(sopClassUid);
    GETSET(sopInstUid);

    // identifying group (0008)
    GETSET(specificCharSet);

    // patient group (0010)
    GETSET(patientName);
    GETSET(patientId);
    GETSET(patientBd);
    GETSET(patientSex);
    GETSET(patientAge);
    

    // study group (0020)
    GETSET(studyInstUid);
    GETSET(studyDate);
    GETSET(studyTime);
    GETSET(refPhysName);
    GETSET(studyId);
    GETSET(accessionNumber);
    GETSET(studyDescription);

    //series
    GETSET(seriesInstUid);
    GETSET(manufacturer);
    GETSET(modality);
    GETSET(institution);
    GETSET(laterality);
    GETSET(patientPosition);
    GETSET(seriesDescription);
    GETSET(seriesNumber);


    // Frame of Reference
    GETSET(frameOfReference);
    GETSET(positionReferenceIndicator);
    
    //image
    GETSET(conversionType);
    GETSET(instanceNumber);
    GETSET(patientOrientation);
    GETSET(contentDate);
    GETSET(contentTime);
    GETSET(windowCenter);
    GETSET(windowWidth);
    GETSET(imageOrientationPatient);
    GETSET(imagePositionPatient);
    GETSET(sliceLocation);
    GETSET(photoMetricInterpetation);
    GETSET(pixelSpacing);
    GETSET(pixelAspectRatio);
    GETSET(imageType);
    GETSET(rescaleIntercept);
    GETSET(rescaleSlope);
    GETSET(kvp);
    GETSET(aquisitionNumber);
    GETSET(pixelRepresentation);
    GETSET(samplesPerPixel);
    GETSET(planarConfiguration);
    GETSET(rows);
    GETSET(cols);
    GETSET(bitsAllocated);
    GETSET(bitsStored);
    GETSET(highBit);
};

#endif 
