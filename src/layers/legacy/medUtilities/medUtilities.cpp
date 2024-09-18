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

#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QInputDialog>
#include <QGuiApplication>
#include <QScreen>

#include <vtkImageView3D.h>
#include <vtkMatrix4x4.h>

#include <gdcmUIDGenerator.h>

void medUtilities::setDerivedMetaData(medAbstractData* derived, medAbstractData* original, QString derivationDescription, bool queryForDescription, bool outputSchema)
{
    copyMetaDataIfEmpty(derived, original, metaDataKeysToCopyForDerivedData(derived));

    if (derivationDescription != "")
    {
        QString newSeriesDescription;
        if (outputSchema)
        {
            newSeriesDescription = original->metadata(medMetaDataKeys::key("SeriesDescription"));
            newSeriesDescription += " (" + derivationDescription + ")";
        }
        else
        {
            newSeriesDescription = derivationDescription;
        }
        derived->setMetaData(medMetaDataKeys::key("SeriesDescription"), newSeriesDescription);
    }
    else
    {
        derived->setMetaData(medMetaDataKeys::key("SeriesDescription"), original->metadata(medMetaDataKeys::key("SeriesDescription")));
    }

    if (queryForDescription)
    {
        querySeriesDescription(derived);
    }

    generateSeriesAndSOPInstanceId(derived);

    derived->addParentData(original);
}

void medUtilities::copyMetaDataIfEmpty(medAbstractData* derived, medAbstractData* original, QList<QString> metaDataKeys)
{
    for(QString metaDataKey : metaDataKeys)
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

    keys << medMetaDataKeys::key("PatientID")
         << medMetaDataKeys::key("PatientName")
         << medMetaDataKeys::key("Age")
         << medMetaDataKeys::key("BirthDate")
         << medMetaDataKeys::key("Gender")
         << medMetaDataKeys::key("Description")
         << medMetaDataKeys::key("StudyID")
         << medMetaDataKeys::key("StudyInstanceUID")
         << medMetaDataKeys::key("StudyDescription")
         << medMetaDataKeys::key("Institution")
         << medMetaDataKeys::key("Referee")
         << medMetaDataKeys::key("StudyDate")
         << medMetaDataKeys::key("StudyTime")
         << medMetaDataKeys::key("Modality")
         << medMetaDataKeys::key("Performer")
         << medMetaDataKeys::key("Report")
         << medMetaDataKeys::key("Protocol")
         << medMetaDataKeys::key("Orientation")
         << medMetaDataKeys::key("Origin")
         << medMetaDataKeys::key("AcquisitionDate")
         << medMetaDataKeys::key("AcquisitionTime");

    if (qobject_cast<medAbstractImageData*>(derived))
    {
        keys << medMetaDataKeys::key("Columns")
             << medMetaDataKeys::key("Rows")
             << medMetaDataKeys::key("Dimensions")
             << medMetaDataKeys::key("NumberOfDimensions")
             << medMetaDataKeys::key("SliceThickness")
             << medMetaDataKeys::key("Spacing")
             << medMetaDataKeys::key("XSpacing")
             << medMetaDataKeys::key("YSpacing")
             << medMetaDataKeys::key("ZSpacing")
             << medMetaDataKeys::key("NumberOfComponents")
             << medMetaDataKeys::key("ComponentType")
             << medMetaDataKeys::key("PixelType")
             << medMetaDataKeys::key("PatientPosition")
             << medMetaDataKeys::key("PatientOrientation")
             << medMetaDataKeys::key("ImageType")
             << medMetaDataKeys::key("AcquisitionNumber")
             << medMetaDataKeys::key("FrameOfReferenceUID")
             << medMetaDataKeys::key("PositionReferenceIndicator")
             << medMetaDataKeys::key("FrameOfReferenceUID")
             << medMetaDataKeys::key("Manufacturer")
             << medMetaDataKeys::key("KVP")
             << medMetaDataKeys::key("FlipAngle")
             << medMetaDataKeys::key("EchoTime")
             << medMetaDataKeys::key("RepetitionTime");
    }

    return keys;
}

QString medUtilities::generateUID()
{
    gdcm::UIDGenerator uidGenerator;
    return QString::fromStdString(uidGenerator.Generate());
}

void medUtilities::generateStudyIdAndInstanceUid(medAbstractData* data)
{
    gdcm::UIDGenerator uidGenerator;
    QString generatedStudyId = QString::fromStdString(uidGenerator.Generate());
    data->setMetaData(medMetaDataKeys::key("StudyID"), generatedStudyId);

    QString generatedStudyInstanceUid = QString::fromStdString(uidGenerator.Generate());
    data->setMetaData(medMetaDataKeys::key("StudyInstanceUID"), generatedStudyInstanceUid);
}

void medUtilities::generateSeriesAndSOPInstanceId(medAbstractData* data)
{
    gdcm::UIDGenerator uidGenerator;
    QString generatedSeriesID = QString::fromStdString(uidGenerator.Generate());
    data->setMetaData(medMetaDataKeys::key("SeriesID"), generatedSeriesID);

    QString generatedSOPInstanceID = QString::fromStdString(uidGenerator.Generate());
    data->setMetaData(medMetaDataKeys::key("SOPInstanceUID"), generatedSOPInstanceID);

    QString generatedSeriesInstanceUID = QString::fromStdString(uidGenerator.Generate());
    data->setMetaData(medMetaDataKeys::key("SeriesInstanceUID"), generatedSeriesInstanceUID);
}

void medUtilities::querySeriesDescription(medAbstractData* data)
{
    QString currentDescription = data->metadata(medMetaDataKeys::key("SeriesDescription"));
    QString queriedDescription = QInputDialog::getText(nullptr, "Series description", "Enter the name of the series:",
                                                       QLineEdit::Normal, currentDescription);
    if (!queriedDescription.isEmpty())
    {
        currentDescription = queriedDescription;
    }

    data->setMetaData(medMetaDataKeys::key("SeriesDescription"), currentDescription);
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

        for(medAbstractNavigator* navigator : view->navigators())
        {
            for(medAbstractParameterL* extraParameter : navigator->linkableParameters())
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

void medUtilities::computeMedian(QList<double> samples,
                                 double* median)
{
    int size = samples.size();

    if (size == 0)
    {
        *median = 0.0;  // Undefined, really.
    }
    else if (size == 1)
    {
        *median = samples[0];
    }
    else
    {
        qSort(samples.begin(), samples.end());
        if (size % 2 == 0)
        {
            *median = (samples[size / 2 - 1] + samples[size / 2]) / 2.0;
        }
        else
        {
            *median = samples[size / 2];
        }
    }
}

int medUtilities::getDevicePixelRatio(QMouseEvent* mouseEvent)
{
    int devicePixelRatio = 1;
#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    devicePixelRatio = QGuiApplication::screenAt(mouseEvent->globalPos())->devicePixelRatio();
#else
    int screenNumber = QApplication::desktop()->screenNumber(mouseEvent->globalPos());
    devicePixelRatio = QGuiApplication::screens().at(screenNumber)->devicePixelRatio();
#endif
    return devicePixelRatio;
}

/**
 * @brief Get the screen pixel ratio according to the current view
 * 
 * @param view a current view to get the screen ratio from
 * @return int the screen pixel ratio
 */
int medUtilities::getDevicePixelRatio(medAbstractView *view)
{
    auto * widgetView = view->viewWidget();
    auto positionView = widgetView->mapToGlobal({widgetView->width()/2,0});
    int devicePixelRatio = QGuiApplication::screenAt(positionView)->devicePixelRatio();

    return devicePixelRatio;
}
