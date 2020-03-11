/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medUtilities.h"

#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medMetaDataKeys.h>
#include <medStringListParameterL.h>
#include <medVtkViewBackend.h>

#include <QLineEdit>
#include <QInputDialog>

#include <vtkImageView3D.h>
#include <vtkMatrix4x4.h>

void medUtilities::setDerivedMetaData(medAbstractData* derived, medAbstractData* original, QString derivationDescription, bool queryForDescription)
{
    copyMetaDataIfEmpty(derived, original, metaDataKeysToCopyForDerivedData(derived));

    if (derivationDescription != "")
    {
        QString newSeriesDescription;
        newSeriesDescription = original->metadata(medMetaDataKeys::SeriesDescription.key());
        newSeriesDescription += " (" + derivationDescription + ")";
        derived->setMetaData(medMetaDataKeys::SeriesDescription.key(), newSeriesDescription);
    }
    else
    {
        derived->setMetaData(medMetaDataKeys::SeriesDescription.key(), original->metadata(medMetaDataKeys::SeriesDescription.key()));
    }

    if (queryForDescription)
    {
        querySeriesDescription(derived);
    }

    generateSeriesAndSOPInstanceId(derived);
}

void medUtilities::copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QList<QString> metaDataKeys)
{
    foreach (QString metaDataKey, metaDataKeys)
    {
        copyMetaDataIfEmpty(derived, original, metaDataKey);
    }
}

void medUtilities::copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QString metaDataKey)
{
    if (!derived->hasMetaData(metaDataKey) && original->hasMetaData(metaDataKey))
    {
        derived->setMetaData(metaDataKey, original->metadata(metaDataKey));
    }
}

QStringList medUtilities::metaDataKeysToCopyForDerivedData(medAbstractData* derived)
{
    QStringList keys;

    keys << medMetaDataKeys::PatientID.key()
         << medMetaDataKeys::PatientName.key()
         << medMetaDataKeys::Age.key()
         << medMetaDataKeys::BirthDate.key()
         << medMetaDataKeys::Gender.key()
         << medMetaDataKeys::Description.key()
         << medMetaDataKeys::StudyID.key()
         << medMetaDataKeys::StudyDicomID.key()
         << medMetaDataKeys::StudyDescription.key()
         << medMetaDataKeys::Institution.key()
         << medMetaDataKeys::Referee.key()
         << medMetaDataKeys::StudyDate.key()
         << medMetaDataKeys::StudyTime.key()
         << medMetaDataKeys::Modality.key()
         << medMetaDataKeys::Performer.key()
         << medMetaDataKeys::Report.key()
         << medMetaDataKeys::Protocol.key()
         << medMetaDataKeys::Orientation.key()
         << medMetaDataKeys::Origin.key()
         << medMetaDataKeys::AcquisitionDate.key()
         << medMetaDataKeys::AcquisitionTime.key();

    if (qobject_cast<medAbstractImageData*>(derived))
    {
        keys << medMetaDataKeys::Columns.key()
             << medMetaDataKeys::Rows.key()
             << medMetaDataKeys::Dimensions.key()
             << medMetaDataKeys::NumberOfDimensions.key()
             << medMetaDataKeys::SliceThickness.key()
             << medMetaDataKeys::Spacing.key()
             << medMetaDataKeys::XSpacing.key()
             << medMetaDataKeys::YSpacing.key()
             << medMetaDataKeys::ZSpacing.key()
             << medMetaDataKeys::NumberOfComponents.key()
             << medMetaDataKeys::ComponentType.key()
             << medMetaDataKeys::PixelType.key();
    }

    return keys;
}

void medUtilities::generateSeriesAndSOPInstanceId(medAbstractData* data)
{
    QString generatedSeriesID = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    data->setMetaData(medMetaDataKeys::SeriesID.key(), generatedSeriesID);

    QString generatedSOPInstanceID = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    data->setMetaData(medMetaDataKeys::SOPInstanceUID.key(), generatedSOPInstanceID);
    
    QString generatedSeriesDicomID = QUuid::createUuid().toString().replace("{", "").replace("}", "");
    data->setMetaData(medMetaDataKeys::SeriesDicomID.key(), generatedSeriesDicomID);
}

void medUtilities::querySeriesDescription(medAbstractData* data)
{
    QString currentDescription = data->metadata(medMetaDataKeys::SeriesDescription.key());
    QString queriedDescription = QInputDialog::getText(nullptr, "Series description", "Enter the name of the series:",
                                                       QLineEdit::Normal, currentDescription);
    if (!queriedDescription.isEmpty())
    {
        currentDescription = queriedDescription;
    }

    data->setMetaData(medMetaDataKeys::SeriesDescription.key(), currentDescription);
}

void medUtilities::applyOrientationMatrix(medAbstractView* view, double* inPoint, double* outPoint, bool withTranslation)
{
    double homogeneousVector[4];
    vtkImageView3D* view3D = static_cast<medVtkViewBackend*>(view->backend())->view3D;

    std::copy(inPoint, inPoint + 3, homogeneousVector);
    homogeneousVector[3] = withTranslation ? 1 : 0;
    view3D->GetOrientationMatrix()->MultiplyPoint(homogeneousVector, homogeneousVector);
    std::copy(homogeneousVector, homogeneousVector + 3, outPoint);
}

void medUtilities::applyInverseOrientationMatrix(medAbstractView* view, double* inPoint, double* outPoint, bool withTranslation)
{
    double homogeneousVector[4];
    vtkImageView3D* view3D = static_cast<medVtkViewBackend*>(view->backend())->view3D;

    std::copy(inPoint, inPoint + 3, homogeneousVector);
    homogeneousVector[3] = withTranslation ? 1 : 0;
    view3D->GetInvertOrientationMatrix()->MultiplyPoint(homogeneousVector, homogeneousVector);
    std::copy(homogeneousVector, homogeneousVector + 3, outPoint);
}

void medUtilities::switchTo3D(medAbstractView *view, Mode3DType mode3D)
{
    if (view)
    {
        medAbstractImageView* imageView = qobject_cast<medAbstractImageView*>(view);
        imageView->setOrientation(medImageView::VIEW_ORIENTATION_3D);

        medStringListParameterL *mode3DParam = nullptr, *renderer3DParam = nullptr;

        foreach (medAbstractNavigator* navigator, view->navigators())
        {
            foreach (medAbstractParameterL* extraParameter, navigator->linkableParameters())
            {
                if (extraParameter->name() == "3D Mode")
                {
                    mode3DParam = qobject_cast<medStringListParameterL*>(extraParameter);
                }
                if (extraParameter->name() == "Renderer")
                {
                    renderer3DParam = qobject_cast<medStringListParameterL*>(extraParameter);
                }
                if(mode3DParam && renderer3DParam)
                {
                    break;
                }
            }
            if (mode3DParam && renderer3DParam)
            {
                break;
            }
        }

        if(mode3DParam)
        {
            switch(mode3D)
            {
                case VR:
                    mode3DParam->setValue("VR");
                    break;
                case MIP_MAXIMUM:
                    mode3DParam->setValue("MIP - Maximum");
                    break;
                case MIP_MINIMUM:
                    mode3DParam->setValue("MIP - Minimum");
                    break;
                case MSR:
                    mode3DParam->setValue("MSR");
                    break;
            }

            if(renderer3DParam)
            {
                renderer3DParam->setValue("Default");
            }
        }
    }
}

void medUtilities::computeMeanAndVariance(QList<double> samples,
                                          double* mean,
                                          double* variance)
{
    double value, tmpVar, delta, finalMean;
    *mean = *variance = value = tmpVar = finalMean = 0.0;
    unsigned int nbSamples = samples.size();
    // only one pass using Welford algorithm
    for (unsigned int i = 0; i < nbSamples; ++i)
    {
        value = samples[i];
        delta = value - finalMean;
        finalMean += delta / (i + 1);
        tmpVar  += delta * (value - finalMean);
    }

    if (nbSamples > 1)
    {
        tmpVar /= nbSamples - 1;
    }
    else
    {
        tmpVar = 0.0;
    }

    *mean = finalMean;
    *variance = tmpVar;
}
