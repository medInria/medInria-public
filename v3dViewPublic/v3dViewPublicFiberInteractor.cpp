#include "v3dViewPublicFiberInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkFiberDataSetManager.h>
#include <vtkViewImage.h>
#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkLimitFibersToVOI.h>
#include <vtkPointData.h>
#include "vtkLookupTableManager.h"
#include "vtkFiberDataSet.h"

#include "v3dViewPublic.h"

#include <QInputDialog>
#include <QColorDialog>

class v3dViewPublicFiberInteractorPrivate
{
public:
    dtkAbstractData        *data;
    v3dViewPublic          *view;
    dtkAbstractData        *projectionData;
    vtkFiberDataSetManager *manager;
    vtkSmartPointer<vtkFiberDataSet> dataset;
};

v3dViewPublicFiberInteractor::v3dViewPublicFiberInteractor(): dtkAbstractViewInteractor(), d(new v3dViewPublicFiberInteractorPrivate)
{
    d->data    = 0;
    d->dataset = 0;
    d->view    = 0;
    d->manager = vtkFiberDataSetManager::New();
    d->manager->SetHelpMessageVisibility(0);
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

v3dViewPublicFiberInteractor::~v3dViewPublicFiberInteractor()
{
    this->disable();
    d->manager->Delete();

    delete d;
    d = 0;
}

QString v3dViewPublicFiberInteractor::description(void) const
{
    return "v3dViewFiberInteractor";
}

QStringList v3dViewPublicFiberInteractor::handled(void) const
{
    return QStringList () << "v3dViewPublic";
}

bool v3dViewPublicFiberInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewFiberInteractor", QStringList() << "v3dViewPublic", createV3dViewPublicFiberInteractor);
}

void v3dViewPublicFiberInteractor::setData(dtkAbstractData *data)
{
    if (!data)
        return;
  
    if (data->description()=="v3dDataFibers") {
        if (vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet *>(data->data())) {
            d->dataset = dataset;
            d->manager->SetInput (d->dataset);
            if (!data->hasMetaData("BundleList"))
                data->addMetaData("BundleList", QStringList());
            d->data = data;
        }
    }
}

dtkAbstractData *v3dViewPublicFiberInteractor::data (void)
{
    return d->data;
}

void v3dViewPublicFiberInteractor::setView(dtkAbstractView *view)
{
    if (v3dViewPublic *v3dview = dynamic_cast<v3dViewPublic*>(view) ) {
        d->view = v3dview;
        d->manager->SetRenderer( d->view->renderer3D() );
        d->manager->SetRenderWindowInteractor( d->view->interactor3D() );
    }
}

dtkAbstractView *v3dViewPublicFiberInteractor::view (void)
{
    return d->view;
}

void v3dViewPublicFiberInteractor::enable(void)
{
    if (this->enabled())
        return;
	
    if (d->view)
        d->manager->Enable();
    
    dtkAbstractViewInteractor::enable();
}

void v3dViewPublicFiberInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (d->view)
        d->manager->Disable();
    
    dtkAbstractViewInteractor::disable();
}

void v3dViewPublicFiberInteractor::onPropertySet(const QString& key, const QString& value)
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

void v3dViewPublicFiberInteractor::onVisibilityPropertySet (const QString& value)
{
    if (value=="true")
        d->manager->SetVisibility(1);
    else
        d->manager->SetVisibility(0);
}

void v3dViewPublicFiberInteractor::onBoxVisibilityPropertySet (const QString& value)
{
    if (value=="true")
        d->manager->SetBoxWidget(1);
    else
        d->manager->SetBoxWidget(0);
}

void v3dViewPublicFiberInteractor::onRenderingModePropertySet (const QString& value)
{
    if (value=="lines")
        d->manager->SetRenderingModeToPolyLines();
	
    if (value=="ribbons")
	d->manager->SetRenderingModeToRibbons();
	
    if (value=="tubes")
        d->manager->SetRenderingModeToTubes();
}

void v3dViewPublicFiberInteractor::onGPUModePropertySet (const QString& value)
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

void v3dViewPublicFiberInteractor::onColorModePropertySet (const QString& value)
{
    if (value=="local")
        d->manager->SetColorModeToLocalFiberOrientation();
	
    if (value=="global")
        d->manager->SetColorModelToGlobalFiberOrientation();

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

void v3dViewPublicFiberInteractor::onBoxBooleanOperationPropertySet (const QString& value)
{
    if (value=="plus")
        d->manager->GetVOILimiter()->SetBooleanOperationToAND();
    
    if (value=="minus")
        d->manager->GetVOILimiter()->SetBooleanOperationToNOT();
	
    d->manager->GetVOILimiter()->Modified();
}

void v3dViewPublicFiberInteractor::onSelectionTagged(void)
{
    d->manager->SwapInputOutput();
}

void v3dViewPublicFiberInteractor::onSelectionReset(void)
{
    d->manager->Reset();
}

void v3dViewPublicFiberInteractor::onSelectionValidated(QString name)
{
    if (!d->data)
        return;
  
    if (d->manager->GetCallbackOutput()->GetNumberOfLines()==0)
        return;
    
    QColor color = QColor::fromHsv(qrand()%360, 255, 210);
    double color_d[3] = {(double)color.red()/255.0, (double)color.green()/255.0, (double)color.blue()/255.0};
	
    d->manager->Validate (name.toAscii().constData(), color_d);
    
    d->data->addMetaData("BundleList", name);
}

void v3dViewPublicFiberInteractor::onProjectionPropertySet(const QString& value)
{
    if (!d->view)
        return;
	
    if (value=="true") {
        d->view->view2D()->AddDataSet( d->manager->GetCallbackOutput() );
    }
}

void v3dViewPublicFiberInteractor::onRadiusSet (int value)
{
    d->manager->SetRadius (value);
 
    if (d->view)
        d->view->update();
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewPublicFiberInteractor(void)
{
    return new v3dViewPublicFiberInteractor;
}
