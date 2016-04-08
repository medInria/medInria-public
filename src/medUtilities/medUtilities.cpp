#include "medUtilities.h"

#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medAbstractMeshData.h>
#include <medAbstractProcess.h>
#include <medAbstractView.h>
#include <medMetaDataKeys.h>
#include <medStringListParameter.h>
#include <medVtkViewBackend.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkMatrix4x4.h>
#include <vtkMetaDataSet.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

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
    if (!derived->hasMetaData(metaDataKey))
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
    QString queriedDescription = QInputDialog::getText(NULL, "Series description", "Enter the name of the series :",
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

        medStringListParameter * mode3DParam = 0, *renderer3DParam = 0;

        foreach (medAbstractNavigator* navigator, view->navigators())
        {
            foreach (medAbstractParameter* extraParameter, navigator->linkableParameters())
            {
                if (extraParameter->name() == "3D Mode")
                {
                    mode3DParam = qobject_cast<medStringListParameter*>(extraParameter);
                }
                if (extraParameter->name() == "Renderer")
                {
                    renderer3DParam = qobject_cast<medStringListParameter*>(extraParameter);
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
        default:
            qDebug()<<"medUtilities: wrong 3d mode";
            break;
        }

        renderer3DParam->setValue("Ray Cast");
    }
}

medAbstractData* medUtilities::changeMaxNumberOfMeshTriangles(medAbstractData *mesh, int maxNumber)
{
    vtkMetaDataSet * dataset = reinterpret_cast<vtkMetaDataSet*>(mesh->data());
    vtkPolyData * polydata = dynamic_cast<vtkPolyData*>(dataset->GetDataSet());

    int initialNumber = polydata->GetNumberOfPolys();

    if (maxNumber < initialNumber) // Decimate
    {
        double decimateValue = 1.0 - (double)maxNumber/(double)initialNumber;

        dtkSmartPointer<medAbstractProcess> process = dtkAbstractProcessFactory::instance()->createSmartPointer("decimateMeshProcess");
        process->setInput(mesh,0);
        process->setParameter(decimateValue,0);
        process->update();

        return process->output();
    }
    return mesh;
}
