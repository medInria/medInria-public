#include "medUtilities.h"

// dtk
#include <dtkCore/dtkAbstractProcessFactory.h>

// medinria
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medAbstractMeshData.h>
#include <medAbstractProcess.h>
#include <medAbstractView.h>
#include <medMetaDataKeys.h>
#include <medStringListParameter.h>
#include <medVtkViewBackend.h>
#include <statsROI.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkMetaDataSet.h>

// vtk
#include <vtkDataSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkDataArray.h>
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

        dtkSmartPointer<medAbstractProcess> process = dtkAbstractProcessFactory::instance()->createSmartPointer("medDecimateMeshProcess");
        process->setInput(mesh,0);
        process->setParameter(decimateValue,0);
        process->update();

        return process->output();
    }
    return mesh;
}

double medUtilities::minimumValue(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::MINMAX);
    statsProcess.update();
    return statsProcess.output().at(0);
}

double medUtilities::maximumValue(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::MINMAX);
    statsProcess.update();
    return statsProcess.output().at(1);
}

double medUtilities::volume(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::VOLUMEML);
    statsProcess.update();
    return statsProcess.output().at(0);
}

vtkDataArray* medUtilities::getArray(dtkSmartPointer<medAbstractData> data,
                                     QString arrayName)
{
    vtkDataArray* result = nullptr;
    if (data->identifier().contains("vtkDataMesh") ||
        data->identifier().contains("EPMap"))
    {
        vtkMetaDataSet* metaData = static_cast<vtkMetaDataSet*>(data->data());
        vtkDataSet* mesh = metaData->GetDataSet();
        // try point data first
        result = mesh->GetPointData()->GetArray(qPrintable(arrayName));
        if (!result)
        {
            // try cell data
            result = mesh->GetCellData()->GetArray(qPrintable(arrayName));
            if (!result)
            {
                // try field data
                result = mesh->GetFieldData()->GetArray(qPrintable(arrayName));
            }
        }
    }
    return result;
}

QList<double> medUtilities::peekArray(dtkSmartPointer<medAbstractData> data,
                                      QString arrayName,
                                      int index)
{
    QList<double> result;
    vtkDataArray* array = getArray(data, arrayName);
    if (array && (index < array->GetNumberOfTuples()))
    {
        int nbComponents = array->GetNumberOfComponents();
        double* tuple = new double[nbComponents];
        array->GetTuple(index, tuple);
        for (int i = 0; i < nbComponents; ++i)
        {
            result.push_back(tuple[i]);
        }
        delete[] tuple;
    }
    return result;
}

QList<double> medUtilities::arrayRange(dtkSmartPointer<medAbstractData> data,
                                       QString arrayName,
                                       int component)
{
    QList<double> result;
    vtkDataArray* array = getArray(data, arrayName);
    if (array && (component < array->GetNumberOfComponents()))
    {
        double* range = new double[2]();
        array->GetRange(range, component);
        result.push_back(range[0]);
        result.push_back(range[1]);
        delete[] range;
    }
    return result;
}

QList<double> medUtilities::arrayStats(dtkSmartPointer<medAbstractData> data,
                                       QString arrayName,
                                       int component)
{
    QList<double> result;
    vtkDataArray* array = getArray(data, arrayName);
    if (array && (component < array->GetNumberOfComponents()))
    {
        vtkIdType nbTuples = array->GetNumberOfTuples();
        // compute mean and variance
        double value, tmpMean, delta, mean, variance;
        value = tmpMean = delta =  mean = variance = 0.0;
        for (vtkIdType i = 0; i < nbTuples; ++i)
        {
            value = array->GetComponent(i, component);
            tmpMean = mean;
            delta = value - mean;
            mean += delta / (i + 1);
            variance += delta * (value - tmpMean);
        }
        if (nbTuples > 1)
        {
            variance /= nbTuples - 1;
        }
        else
        {
            variance = 0.0;
        }

        result.push_back(mean);
        result.push_back(std::sqrt(variance));
    }
    return result;
}
