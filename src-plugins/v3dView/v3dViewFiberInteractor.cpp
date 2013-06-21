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
    
    QMap<QString, double> meanFAList;
    QMap<QString, double> minFAList;
    QMap<QString, double> maxFAList;
    QMap<QString, double> varFAList;
    
    QMap<QString, double> meanADCList;
    QMap<QString, double> minADCList;
    QMap<QString, double> maxADCList;
    QMap<QString, double> varADCList;
    
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
}

void v3dViewFiberInteractor::tagSelection()
{
    d->manager->SwapInputOutput();
}

void v3dViewFiberInteractor::resetSelection()
{
    d->manager->Reset();
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
}

void v3dViewFiberInteractor::computeBundleFAStatistics (const QString &name,
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

    statCalculator->GetFAStatistics(mean, min, max, var);
}

void v3dViewFiberInteractor::bundleFAStatistics(const QString &name,
                                                double &mean,
                                                double &min,
                                                double &max,
                                                double &var)
{
    if (!d->meanFAList.contains(name)) {
        this->computeBundleFAStatistics(name, mean, min, max, var);
        d->meanFAList[name] = mean;
        d->minFAList[name] = min;
        d->maxFAList[name] = max;
        d->varFAList[name] = var;
    }
    else {
        mean = d->meanFAList[name];
        min  = d->minFAList[name];
        max  = d->maxFAList[name];
        var  = d->varFAList[name];
    }
}

void v3dViewFiberInteractor::computeBundleADCStatistics (const QString &name,
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

    statCalculator->GetADCStatistics(mean, min, max, var);
}

void v3dViewFiberInteractor::bundleADCStatistics(const QString &name,
                                                 double &mean,
                                                 double &min,
                                                 double &max,
                                                 double &var)
{
    if (!d->meanADCList.contains(name)) {
        this->computeBundleADCStatistics(name, mean, min, max, var);
        d->meanADCList[name] = mean;
        d->minADCList[name] = min;
        d->maxADCList[name] = max;
        d->varADCList[name] = var;
    }
    else {
        mean = d->meanADCList[name];
        min  = d->minADCList[name];
        max  = d->maxADCList[name];
        var  = d->varADCList[name];
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
    d->meanFAList.clear();
    d->minFAList.clear();
    d->maxFAList.clear();
    d->varFAList.clear();
    
    d->meanADCList.clear();
    d->minADCList.clear();
    d->maxADCList.clear();
    d->varADCList.clear();
    
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
}

void v3dViewFiberInteractor::setRoiBoolean(int roi, int meaning)
{
    d->manager->GetROILimiter()->SetBooleanOperation (roi, meaning);
}

int v3dViewFiberInteractor::roiBoolean(int roi)
{
    return d->manager->GetROILimiter()->GetBooleanOperationVector()[roi+1];
}

void v3dViewFiberInteractor::setBundleVisibility(const QString &name, bool visibility)
{
    d->manager->SetBundleVisibility(name.toAscii().constData(), (int)visibility);
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
}

void v3dViewFiberInteractor::onBundlingBoxBooleanOperatorChanged(int value)
{
    if (value==0)
        d->manager->GetVOILimiter()->SetBooleanOperationToNOT();
    else
        d->manager->GetVOILimiter()->SetBooleanOperationToAND();

    d->manager->Execute();
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

