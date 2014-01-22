/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dViewFiberInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medMessageController.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkFiberDataSetManager.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkLimitFibersToVOI.h>
#include <vtkPointData.h>
#include <vtkLookupTableManager.h>
#include <vtkFiberDataSet.h>
#include <vtkMatrix4x4.h>
#include <vtkLimitFibersToROI.h>
#include <vtkIsosurfaceManager.h>
#include <vtkCellArray.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkFiberBundleStatisticsCalculator.h>

#include "v3dView.h"
#include "medVtkView.h"

#include <QInputDialog>
#include <QColorDialog>

class v3dViewFiberInteractorPrivate
{
public:
    dtkAbstractData        *data;
    v3dView                *view;
    dtkAbstractData        *projectionData;
    vtkFiberDataSetManager *manager;
    vtkIsosurfaceManager   *roiManager;
    vtkSmartPointer<vtkFiberDataSet> dataset;
    
    QMap<QString, double> meanLengthList;
    QMap<QString, double> minLengthList;
    QMap<QString, double> maxLengthList;
    QMap<QString, double> varLengthList;
};

v3dViewFiberInteractor::v3dViewFiberInteractor(): medAbstractVtkViewInteractor(), d(new v3dViewFiberInteractorPrivate)
{
    d->data    = 0;
    d->dataset = 0;
    d->view    = 0;
    d->manager = vtkFiberDataSetManager::New();
    d->manager->SetHelpMessageVisibility(0);
    d->roiManager = vtkIsosurfaceManager::New();
    // d->manager->SetBoxWidget (0);

    vtkLookupTable* lut = vtkLookupTableManager::GetSpectrumLookupTable();
    d->manager->SetLookupTable(lut);
    lut->Delete();
}

v3dViewFiberInteractor::~v3dViewFiberInteractor()
{
    this->disable();
    d->manager->Delete();
    d->roiManager->Delete();
    delete d;
    d = 0;
}

QString v3dViewFiberInteractor::description() const
{
    return tr("Interactor to help visualising Fibers");
}

QString v3dViewFiberInteractor::identifier() const
{
    return "v3dViewFiberInteractor";
}

QStringList v3dViewFiberInteractor::handled() const
{
    return QStringList () << v3dView::s_identifier() << medVtkView::s_identifier();
}

bool v3dViewFiberInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType == "v3dDataFibers")
        return true;
    
    return false;
}

bool v3dViewFiberInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewFiberInteractor",
                                                                          QStringList () << v3dView::s_identifier() << medVtkView::s_identifier(),
                                                                          createV3dViewFiberInteractor);
}

void v3dViewFiberInteractor::setData(dtkAbstractData *data)
{
    if (!data)
        return;

    if (data->identifier()=="v3dDataFibers") {
        if (vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet *>(data->data())) {
            d->dataset = dataset;
            d->manager->SetInput (d->dataset);
            if (!data->hasMetaData("BundleList"))
                data->addMetaData("BundleList", QStringList());
            if (!data->hasMetaData("BundleColorList"))
                data->addMetaData("BundleColorList", QStringList());

            // add bundles to 2d
            vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
            vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
            while (it!=bundles.end())
            {
                if (d->view)
                    d->view->renderer2d()->AddActor(
                        d->manager->GetBundleActor ((*it).first ));
                    ++it;
            }

            this->clearStatistics();

            d->data = data;
        }
    }
}

dtkAbstractData *v3dViewFiberInteractor::data()
{
    return d->data;
}

void v3dViewFiberInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = qobject_cast<v3dView*>(view) ) {
        d->view = v3dview;
        d->manager->SetRenderer( d->view->renderer3d() );
        d->manager->SetRenderWindowInteractor( d->view->interactor() );
        d->view->renderer2d()->AddActor( d->manager->GetOutput() );
        d->roiManager->SetRenderWindowInteractor( d->view->interactor() );
    }
}

dtkAbstractView *v3dViewFiberInteractor::view()
{
    return d->view;
}

void v3dViewFiberInteractor::enable()
{
    if (this->enabled())
        return;

    if (d->view) {
        d->manager->Enable();
        d->roiManager->Enable();
        if (d->dataset) {
            vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
            vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
            while (it!=bundles.end())
            {
                if (d->view)
                    d->view->renderer2d()->AddActor(
                        d->manager->GetBundleActor ((*it).first ));
                    ++it;
            }
        }
    }

    dtkAbstractViewInteractor::enable();
}

void v3dViewFiberInteractor::disable()
{
    if (!this->enabled())
        return;

    if (d->view) {
        d->manager->Disable();
        d->roiManager->Disable();
        if (d->dataset) {
            vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
            vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
            while (it!=bundles.end())
            {
                if (d->view)
                    d->view->renderer2d()->RemoveActor(
                        d->manager->GetBundleActor ((*it).first ));
                    ++it;
            }
        }
    }

    dtkAbstractViewInteractor::disable();
}

void v3dViewFiberInteractor::setVisibility(bool visible)
{
    d->manager->SetVisibility(visible);
}

void v3dViewFiberInteractor::setBoxVisibility(bool visible)
{
    if (d->view && d->view->property("Orientation") != "3D") {
        medMessageController::instance()->showError("View must be in 3D mode to activate the bundling box",
                                                    3000);
        d->manager->SetBoxWidget(false);
        return;
    }

    d->manager->SetBoxWidget(visible);
    d->view->update();
}

void v3dViewFiberInteractor::setRenderingMode(RenderingMode mode)
{
    switch(mode)
    {
        case v3dViewFiberInteractor::Lines:
            d->manager->SetRenderingModeToPolyLines();
            break;

        case v3dViewFiberInteractor::Ribbons:
            d->manager->SetRenderingModeToRibbons();
            break;

        case v3dViewFiberInteractor::Tubes:
            d->manager->SetRenderingModeToTubes();
            break;
        default:
            qDebug() << "v3dViewFiberInteractor: unknown rendering mode";
    }

    d->view->update();
}

void v3dViewFiberInteractor::activateGPU(bool activate)
{
    if (activate) {
        vtkFibersManager::UseHardwareShadersOn();
        d->manager->ChangeMapperToUseHardwareShaders();
    } else {
        vtkFibersManager::UseHardwareShadersOff();
        d->manager->ChangeMapperToDefault();
    }

    d->view->update();
}

void v3dViewFiberInteractor::setColorMode(ColorMode mode)
{
    switch(mode)
    {
        case v3dViewFiberInteractor::Local:
            d->manager->SetColorModeToLocalFiberOrientation();
            break;

        case v3dViewFiberInteractor::Global:
            d->manager->SetColorModelToGlobalFiberOrientation();
            break;

        case v3dViewFiberInteractor::FA:
            d->manager->SetColorModeToLocalFiberOrientation();
            for (int i=0; i<d->manager->GetNumberOfPointArrays(); i++) {
                if (d->manager->GetPointArrayName (i)) {
                    if (strcmp ( d->manager->GetPointArrayName (i), "FA")==0)
                    {
                        d->manager->SetColorModeToPointArray (i);
                        break;
                    }
                }
            }
            break;

        default:
            qDebug() << "v3dViewFiberInteractor: unknown color mode";
    }

    d->view->update();
}

void v3dViewFiberInteractor::setBoxBooleanOperation(BooleanOperation op)
{
    switch(op)
    {
        case v3dViewFiberInteractor::Plus:
            d->manager->GetVOILimiter()->SetBooleanOperationToAND();
            break;

        case v3dViewFiberInteractor::Minus:
            d->manager->GetVOILimiter()->SetBooleanOperationToNOT();
            break;

        default:
            qDebug() << "v3dViewFiberInteractor: Unknown boolean operations";
    }

    d->manager->GetVOILimiter()->Modified();
    d->view->update();
}

void v3dViewFiberInteractor::tagSelection()
{
    d->manager->SwapInputOutput();
    d->view->update();
}

void v3dViewFiberInteractor::resetSelection()
{
    d->manager->Reset();
    d->view->update();
}

void v3dViewFiberInteractor::validateSelection(const QString &name, const QColor &color)
{
    if (!d->data)
        return;

    double color_d[3] = {(double)color.red()/255.0, (double)color.green()/255.0, (double)color.blue()/255.0};

    d->manager->Validate (name.toAscii().constData(), color_d);

    d->view->renderer2d()->AddActor (d->manager->GetBundleActor(name.toAscii().constData()));

    d->data->addMetaData("BundleList", name);
    d->data->addMetaData("BundleColorList", color.name());

    // reset to initial navigation state
    d->manager->Reset();
    d->view->update();
}

void v3dViewFiberInteractor::saveBundles()
{
    if (!d->dataset)
        return;
    
    vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
    vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
    
    dtkSmartPointer <dtkAbstractData> tmpBundle;
    
    while (it!=bundles.end())
    {
        tmpBundle = dtkAbstractDataFactory::instance()->createSmartPointer("v3dDataFibers");
        if (!tmpBundle)
            return;
        
        vtkSmartPointer <vtkFiberDataSet> bundle = vtkFiberDataSet::New();
        bundle->SetFibers((*it).second.Bundle);
        
        tmpBundle->setData(bundle);
        
        QString newSeriesDescription = d->data->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " ";
        newSeriesDescription += (*it).first.c_str();
        tmpBundle->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
        
        foreach ( QString metaData, d->data->metaDataList() )
        {
            if ((metaData == "BundleList")||(metaData == "BundleColorList"))
                continue;
            
            if (!tmpBundle->hasMetaData(metaData))
                tmpBundle->addMetaData (metaData, d->data->metaDataValues (metaData));
        }
        
        foreach ( QString property, d->data->propertyList() )
        tmpBundle->addProperty ( property,d->data->propertyValues ( property ) );
        
        QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
        tmpBundle->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );
        
        QString uuid = QUuid::createUuid().toString();
        medDataManager::instance()->importNonPersistent (tmpBundle, uuid);
        
        ++it;
    }
}

void v3dViewFiberInteractor::bundleImageStatistics (const QString &bundleName,
                                                    QMap <QString, double> &mean,
                                                    QMap <QString, double> &min,
                                                    QMap <QString, double> &max,
                                                    QMap <QString, double> &var)
{
    vtkPolyData *bundleData = d->dataset->GetBundle (bundleName.toAscii().constData()).Bundle;
    
    if (!bundleData)
        return;
    
    if (bundleData->GetPointData()->HasArray("Tensors"))
    {
        // Specific TTK case
        itk::FiberBundleStatisticsCalculator::Pointer statCalculator = itk::FiberBundleStatisticsCalculator::New();
        statCalculator->SetInput (bundleData);
        
        try
        {
            statCalculator->Compute();
        }
        catch(itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return;
        }
        
        double meanVal, minVal, maxVal, varVal;
        statCalculator->GetADCStatistics(meanVal, minVal, maxVal, varVal);
        mean["ADC"] = meanVal;
        min["ADC"] = minVal;
        max["ADC"] = maxVal;
        var["ADC"] = varVal;
        
        statCalculator->GetFAStatistics(meanVal, minVal, maxVal, varVal);
        mean["FA"] = meanVal;
        min["FA"] = minVal;
        max["FA"] = maxVal;
        var["FA"] = varVal;
    }
    
    vtkPointData *bundlePointData = bundleData->GetPointData();
    unsigned int numberOfArrays = bundlePointData->GetNumberOfArrays();
    vtkPoints* points = bundleData->GetPoints();
    vtkCellArray* lines = bundleData->GetLines();
    
    for (unsigned int i = 0;i < numberOfArrays;++i)
    {
        // To do, explore fibers one by one, compute mean value on each fiber, then deduce all stats
        // Put them inside maps
        QString arrayName = bundlePointData->GetArrayName(i);
        vtkDataArray *imageCoefficients = bundlePointData->GetArray(i);
        unsigned int tupleSize = imageCoefficients->GetElementComponentSize();
        if (tupleSize != 1)
            continue;
        
        vtkIdType  npts  = 0;
        vtkIdType* ptids = 0;
        vtkIdType test = lines->GetNextCell (npts, ptids);
        
        double sumData = 0;
        double squareSumData = 0;
        double minValue = HUGE_VAL;
        double maxValue = - HUGE_VAL;
        unsigned int numberOfLines = 0;
        
        // go over lines, each cell is a line
        while (test)
        {
            double meanFiberData = 0;
            
            for (int k=0; k<npts; k++)
            {
                double pt[3];
                points->GetPoint (ptids[k], pt);
                
                double tupleValue = 0;
                imageCoefficients->GetTuple(ptids[k], &tupleValue);
                
                if (k == 0)
                {
                    minValue = tupleValue;
                    maxValue = tupleValue;
                }
                
                if (minValue > tupleValue)
                    minValue = tupleValue;
                
                if (maxValue < tupleValue)
                    maxValue = tupleValue;
                
                meanFiberData += tupleValue;
            }
            
            meanFiberData /= npts;
            
            if (minValue > meanFiberData)
                minValue = meanFiberData;
            
            if (maxValue < meanFiberData)
                maxValue = meanFiberData;
            
            sumData += meanFiberData;
            squareSumData += meanFiberData * meanFiberData;
            ++numberOfLines;
            
            test = lines->GetNextCell (npts, ptids);
        }
        
        if (std::isfinite(sumData / numberOfLines))
        {
            mean[arrayName] = sumData / numberOfLines;
            min[arrayName] = minValue;
            max[arrayName] = maxValue;
            var[arrayName] = (squareSumData - sumData * sumData / numberOfLines) / (numberOfLines - 1.0);
        }
    }
}

void v3dViewFiberInteractor::computeBundleLengthStatistics (const QString &name,
                                                            double &mean,
                                                            double &min,
                                                            double &max,
                                                            double &var)
{
    itk::FiberBundleStatisticsCalculator::Pointer statCalculator = itk::FiberBundleStatisticsCalculator::New();
    statCalculator->SetInput (d->dataset->GetBundle (name.toAscii().constData()).Bundle);
    try
    {
        statCalculator->Compute();
    }
    catch(itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        mean = 0.0;
        min  = 0.0;
        max  = 0.0;
        var  = 0.0;
        return;
    }

    statCalculator->GetLengthStatistics(mean, min, max, var);
}

void v3dViewFiberInteractor::bundleLengthStatistics(const QString &name,
                                                    double &mean,
                                                    double &min,
                                                    double &max,
                                                    double &var)
{
    if (!d->meanLengthList.contains(name)) {
        this->computeBundleLengthStatistics(name, mean, min, max, var);
        d->meanLengthList[name] = mean;
        d->minLengthList[name] = min;
        d->maxLengthList[name] = max;
        d->varLengthList[name] = var;
    }
    else {
        mean = d->meanLengthList[name];
        min  = d->minLengthList[name];
        max  = d->maxLengthList[name];
        var  = d->varLengthList[name];
    }
}

void v3dViewFiberInteractor::clearStatistics(void)
{    
    d->meanLengthList.clear();
    d->minLengthList.clear();
    d->maxLengthList.clear();
    d->varLengthList.clear();
}

void v3dViewFiberInteractor::setProjection(const QString& value)
{
    if (!d->view)
        return;

    if (value=="true") {
        d->view->view2d()->AddDataSet( d->manager->GetCallbackOutput() );
    }
}

void v3dViewFiberInteractor::setRadius (int value)
{
    d->manager->SetRadius (value);

    if (d->view)
        d->view->update();
}

void v3dViewFiberInteractor::setROI(dtkAbstractData *data)
{
    if (!data)
        return;

    if (data->identifier()!="itkDataImageUChar3")
        return;

    typedef itk::Image<unsigned char, 3> ROIType;

    ROIType::Pointer roiImage = static_cast<ROIType*>(data->data());

    if (!roiImage.IsNull()) {
        itk::ImageToVTKImageFilter<ROIType>::Pointer converter = itk::ImageToVTKImageFilter<ROIType>::New();
        converter->SetReferenceCount(2);
        converter->SetInput(roiImage);
        converter->Update();

        ROIType::DirectionType directions = roiImage->GetDirection();
        vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
        matrix->Identity();
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
                matrix->SetElement (i, j, directions (i,j));

        d->manager->Reset();
        d->manager->GetROILimiter()->SetMaskImage (converter->GetOutput());
        d->manager->GetROILimiter()->SetDirectionMatrix (matrix);

        ROIType::PointType origin = roiImage->GetOrigin();
        vtkMatrix4x4 *matrix2 = vtkMatrix4x4::New();
        matrix2->Identity();
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
                matrix2->SetElement (i, j, directions (i,j));
        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
            v_origin[i] = origin[i];
        v_origin[3] = 1.0;
        matrix->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
            matrix2->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        d->roiManager->SetInput (converter->GetOutput());
        d->roiManager->SetDirectionMatrix (matrix2);

        // d->manager->GetROILimiter()->Update();
        d->roiManager->GenerateData();

        matrix->Delete();
        matrix2->Delete();
    }
    else {
        d->manager->GetROILimiter()->SetMaskImage (0);
        d->manager->GetROILimiter()->SetDirectionMatrix (0);

        d->roiManager->ResetData();
    }

    d->view->update();
}

void v3dViewFiberInteractor::setRoiBoolean(int roi, int meaning)
{
    d->manager->GetROILimiter()->SetBooleanOperation (roi, meaning);
    d->view->update();
}

int v3dViewFiberInteractor::roiBoolean(int roi)
{
    return d->manager->GetROILimiter()->GetBooleanOperationVector()[roi+1];
}

void v3dViewFiberInteractor::setBundleVisibility(const QString &name, bool visibility)
{
    d->manager->SetBundleVisibility(name.toAscii().constData(), (int)visibility);
    d->view->update();
}

bool v3dViewFiberInteractor::bundleVisibility(const QString &name) const
{
    return d->manager->GetBundleVisibility(name.toAscii().constData());
}

void v3dViewFiberInteractor::setAllBundlesVisibility(bool visibility)
{
    if (visibility)
        d->manager->ShowAllBundles();
    else
        d->manager->HideAllBundles();

    d->view->update();
}

void v3dViewFiberInteractor::setOpacity(dtkAbstractData * /*data*/, double /*opacity*/)
{
}

double v3dViewFiberInteractor::opacity(dtkAbstractData * /*data*/) const
{
    //TODO
    return 100;
}

void v3dViewFiberInteractor::setVisible(dtkAbstractData * /*data*/, bool /*visible*/)
{
    //TODO
}

bool v3dViewFiberInteractor::isVisible(dtkAbstractData * /*data*/) const
{
    //TODO
    return true;
}


// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewFiberInteractor()
{
    return new v3dViewFiberInteractor;
}

