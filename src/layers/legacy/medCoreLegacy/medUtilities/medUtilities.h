#pragma once

#include <medUtilitiesExport.h>

#include <QStringList>

class medAbstractData;
class medAbstractView;

namespace MEDUTILITIES_EXPORT medUtilities
{
    enum Mode3DType
    {
        VR,
        MIP_MAXIMUM,
        MIP_MINIMUM,
        MSR
    };

    void setDerivedMetaData(medAbstractData* derived, medAbstractData* original,
                                   QString derivationDescription, bool queryForDescription = false);

    void copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QList<QString> metaDataKeys);
    void copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QString metaDataKey);

    QStringList metaDataKeysToCopyForDerivedData(medAbstractData* derived);

    void generateSeriesAndSOPInstanceId(medAbstractData* data);
    
    void querySeriesDescription(medAbstractData* data);

    /*void applyOrientationMatrix(medAbstractView* view, double* inPoint, double* outPoint, bool withTranslation = true);

    void applyInverseOrientationMatrix(medAbstractView* view, double* inPoint, double* outPoint, bool withTranslation = true);*/

    void switchTo3D(medAbstractView *view, Mode3DType mode3D = VR); // Display mesh in 3D orientation

    void computeMeanAndVariance(QList<double> samples, double* mean, double* variance);
};
