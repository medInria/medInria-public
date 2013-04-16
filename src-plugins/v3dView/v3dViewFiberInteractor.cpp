/*=========================================================================

 MedInria

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
};

v3dViewFiberInteractor::v3dViewFiberInteractor(): medAbstractViewFiberInteractor(), d(new v3dViewFiberInteractorPrivate)
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

    // addProperty here
    this->addProperty("Visibility",    QStringList() << "true" << "false");
    this->addProperty("BoxVisibility", QStringList() << "true" << "false");
    this->addProperty("RenderingMode", QStringList() << "lines" << "ribbons" << "tubes");
    this->addProperty("GPUMode",       QStringList() << "true" << "false");
    this->addProperty("ColorMode",     QStringList() << "local" << "global" << "fa");
    this->addProperty("BoxBooleanOperation", QStringList() << "plus" << "minus");
    this->addProperty("Projection",    QStringList() << "true" << "false");
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
    return QStringList () << "v3dView";
}

bool v3dViewFiberInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewFiberInteractor", QStringList() << "v3dView", createV3dViewFiberInteractor);
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

void v3dViewFiberInteractor::onPropertySet(const QString& key, const QString& value)
{
    if (key=="Visibility")
        this->onVisibilityPropertySet (value);

    if (key=="BoxVisibility")
        this->onBoxVisibilityPropertySet (value);

    if (key=="RenderingMode")
        this->onRenderingModePropertySet (value);

    if (key=="GPUMode")
        this->onGPUModePropertySet (value);

    if (key=="ColorMode")
        this->onColorModePropertySet (value);

    if (key=="BoxBooleanOperation")
        this->onBoxBooleanOperationPropertySet (value);

    if (key=="Projection")
        this->onProjectionPropertySet (value);
}

void v3dViewFiberInteractor::onVisibilityPropertySet (const QString& value)
{
    if (value=="true") {
        d->manager->SetVisibility(1);
    }
    else {
        d->manager->SetVisibility(0);
    }
}

void v3dViewFiberInteractor::onBoxVisibilityPropertySet (const QString& value)
{
    if (d->view && d->view->property("Orientation")!="3D") {
        medMessageController::instance()->showError("View must be in 3D mode to activate the bundling box",
                                                    3000);
        d->manager->SetBoxWidget(0);
        return;
    }

    if (value=="true")
        d->manager->SetBoxWidget(1);
    else
        d->manager->SetBoxWidget(0);
}

void v3dViewFiberInteractor::onRenderingModePropertySet (const QString& value)
{
    if (value=="lines") {
        d->manager->SetRenderingModeToPolyLines();
    }

    if (value=="ribbons") {
        d->manager->SetRenderingModeToRibbons();
    }

    if (value=="tubes") {
        d->manager->SetRenderingModeToTubes();
    }
}

void v3dViewFiberInteractor::onGPUModePropertySet (const QString& value)
{
    if (value=="true") {
        vtkFibersManager::UseHardwareShadersOn();
        d->manager->ChangeMapperToUseHardwareShaders();
    }
    else {
        vtkFibersManager::UseHardwareShadersOff();
        d->manager->ChangeMapperToDefault();
    }
}

void v3dViewFiberInteractor::onColorModePropertySet (const QString& value)
{
    if (value=="local") {
        d->manager->SetColorModeToLocalFiberOrientation();
    }

    if (value=="global") {
        d->manager->SetColorModelToGlobalFiberOrientation();
    }

    if (value=="fa") {
        for (int i=0; i<d->manager->GetNumberOfPointArrays(); i++)
	    if (d->manager->GetPointArrayName (i))
	        if (strcmp ( d->manager->GetPointArrayName (i), "FA")==0)
		{
            d->manager->SetColorModeToPointArray (i);
            break;
        }
    }
}

void v3dViewFiberInteractor::onBoxBooleanOperationPropertySet (const QString& value)
{
    if (value=="plus")
        d->manager->GetVOILimiter()->SetBooleanOperationToAND();

    if (value=="minus")
        d->manager->GetVOILimiter()->SetBooleanOperationToNOT();

    d->manager->GetVOILimiter()->Modified();
}

void v3dViewFiberInteractor::onSelectionTagged()
{
    d->manager->SwapInputOutput();
}

void v3dViewFiberInteractor::onSelectionReset()
{
    d->manager->Reset();
}

void v3dViewFiberInteractor::onSelectionValidated(const QString &name, const QColor &color)
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

void v3dViewFiberInteractor::onProjectionPropertySet(const QString& value)
{
    if (!d->view)
        return;

    if (value=="true") {
        d->view->view2d()->AddDataSet( d->manager->GetCallbackOutput() );
    }
}

void v3dViewFiberInteractor::onRadiusSet (int value)
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

void v3dViewFiberInteractor::setBundleVisibility(bool visibility)
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

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewFiberInteractor()
{
    return new v3dViewFiberInteractor;
}

