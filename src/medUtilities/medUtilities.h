#pragma once

#include <QHash>

class medAbstractData;
class medAbstractView;

class medUtilities
{
public:
    static void setDerivedMetaData(medAbstractData* derived, medAbstractData* original,
                                   QString derivationDescription, bool queryForDescription = false);

    static void copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QList<QString> metaDataKeys);
    static void copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QString metaDataKey);

    static QStringList metaDataKeysToCopyForDerivedData(medAbstractData* derived);

    static void generateSeriesAndSOPInstanceId(medAbstractData* data);

    static void querySeriesDescription(medAbstractData* data);

    static void applyOrientationMatrix(medAbstractView* view, double* inPoint, double* outPoint, bool withTranslation = true);

    static void applyInverseOrientationMatrix(medAbstractView* view, double* inPoint, double* outPoint, bool withTranslation = true);

    static void switchToVolumeRendering(medAbstractView *view); // Display mesh in 3D orientation

    static medAbstractData *changeMaxNumberOfMeshTriangles(medAbstractData *mesh, int value);
};
