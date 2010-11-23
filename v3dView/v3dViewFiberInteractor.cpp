#include "v3dViewFiberInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkPolyData.h>
#include <vtkFibersManager.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include "v3dView.h"
#include "v3dFiberBundle.h"

#include <QInputDialog>
#include <QColorDialog>

class v3dViewFiberInteractorPrivate
{
public:
    dtkAbstractData  *data;
    v3dView          *view;
    dtkAbstractData  *projectionData;
    vtkFibersManager *manager;
  
    QHash<QString, dtkAbstractData*> bundleList;
    QHash<QString, v3dFiberBundle*>  v3dBundleList;
};

v3dViewFiberInteractor::v3dViewFiberInteractor(): dtkAbstractViewInteractor(), d(new v3dViewFiberInteractorPrivate)
{
	d->data = 0;
	d->view = 0;
	d->manager = vtkFibersManager::New();
	d->manager->SetHelpMessageVisibility(0);
	
	// addProperty here
	this->addProperty("Visibility", QStringList() << "true" << "false");
	this->addProperty("BoxVisibility", QStringList() << "true" << "false");
	this->addProperty("RenderingMode", QStringList() << "lines" << "ribbons" << "tubes");
	this->addProperty("GPUMode", QStringList() << "true" << "false");
	this->addProperty("ColorMode", QStringList() << "local" << "global" << "fa");
	this->addProperty("BoxBooleanOperation", QStringList() << "plus" << "minus");
	this->addProperty("Projection", QStringList() << "true" << "false");
}

v3dViewFiberInteractor::~v3dViewFiberInteractor()
{
    this->disable();
    QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
    while (it!=d->v3dBundleList.end()) {
      (*it)->Delete();
      ++it;
    }
    d->v3dBundleList.clear();
    d->manager->Delete();

    delete d;
    d = 0;
}

QString v3dViewFiberInteractor::description(void) const
{
	return "v3dViewFiberInteractor";
}

QStringList v3dViewFiberInteractor::handled(void) const
{
	return QStringList () << "v3dView";
}

bool v3dViewFiberInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewFiberInteractor", QStringList() << "v3dView", createV3dViewFiberInteractor);
}

void v3dViewFiberInteractor::setData(dtkAbstractData *data)
{
  if (data->description()=="vtkDataFibers") {
    if (vtkPolyData *fibers = dynamic_cast<vtkPolyData *>((vtkDataObject *)(data->data()))) {
        d->manager->SetInput(fibers);
	d->data = data;
    }
  }
}

void v3dViewFiberInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}

void v3dViewFiberInteractor::enable(void)
{
    if (this->enabled())
        return;
	
    if (d->view) {
        d->manager->SetRenderer( d->view->renderer3D() );
	d->manager->SetRenderWindowInteractor( d->view->interactor() );
	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	    d->view->renderer3D()->AddViewProp ( (*it)->GetActor() );
	}
    }
    dtkAbstractViewInteractor::enable();
}

void v3dViewFiberInteractor::disable(void)
{
    if (!this->enabled())
        return;

    if (d->view) {
        d->manager->SetRenderer( 0 );
	d->manager->SetRenderWindowInteractor( 0 );
	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	  d->view->renderer3D()->RemoveViewProp ( (*it)->GetActor() );
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
	
    if (d->view)
        d->view->update();
}

void v3dViewFiberInteractor::onVisibilityPropertySet (const QString& value)
{
    if (value=="true")
        d->manager->SetVisibility(1);
    else
        d->manager->SetVisibility(0);
}

void v3dViewFiberInteractor::onBoxVisibilityPropertySet (const QString& value)
{
    if (value=="true")
        d->manager->SetBoxWidget(1);
    else
        d->manager->SetBoxWidget(0);
}

void v3dViewFiberInteractor::onRenderingModePropertySet (const QString& value)
{
    if (value=="lines") {
        d->manager->SetRenderingModeToPolyLines();
	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	    (*it)->SetRenderingModeToPolyLines ();
	    ++it;
	}
    }
	
    if (value=="ribbons") {
	d->manager->SetRenderingModeToRibbons();
	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	    (*it)->SetRenderingModeToRibbons ();
	    ++it;
	}
    }
	
    if (value=="tubes") {
        d->manager->SetRenderingModeToTubes();
	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	    (*it)->SetRenderingModeToTubes ();
	    ++it;
	}
    }
}

void v3dViewFiberInteractor::onGPUModePropertySet (const QString& value)
{
    if (value=="true") {
        vtkFibersManager::UseHardwareShadersOn();
	d->manager->ChangeMapperToUseHardwareShaders();

	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	    (*it)->UseHardwareShadersOn ();
	    ++it;
	}
    }
    else {
        vtkFibersManager::UseHardwareShadersOff();
	d->manager->ChangeMapperToDefault();

	QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
	while (it!=d->v3dBundleList.end()) {
	    (*it)->UseHardwareShadersOff ();
	    ++it;
	}
    }
}

void v3dViewFiberInteractor::onColorModePropertySet (const QString& value)
{
    if (value=="local")
        d->manager->SetColorModeToLocalFiberOrientation();
	
    if (value=="global")
        d->manager->SetColorModelToGlobalFiberOrientation();

    if (value=="fa")
        d->manager->SetColorModeToPointArray(1);
}

void v3dViewFiberInteractor::onBoxBooleanOperationPropertySet (const QString& value)
{
    if (value=="plus")
        d->manager->GetVOILimiter()->SetBooleanOperationToAND();
    
    if (value=="minus")
        d->manager->GetVOILimiter()->SetBooleanOperationToNOT();
	
    d->manager->GetVOILimiter()->Modified();
}

void v3dViewFiberInteractor::onSelectionTagged(void)
{
    d->manager->SwapInputOutput();
}

void v3dViewFiberInteractor::onSelectionReset(void)
{
    d->manager->Reset();
}

void v3dViewFiberInteractor::onSelectionValidated(void)
{
    if (d->manager->GetCallbackOutput()->GetNumberOfLines()==0)
        return;
    
    bool ok;
    QString text;
    if (d->view)
      text = QInputDialog::getText(d->view->widget(), tr("Enter bundle name"),
				   tr(""), QLineEdit::Normal, tr(""), &ok);
    else
      text = QInputDialog::getText(0, tr("Enter bundle name"),
				   tr(""), QLineEdit::Normal, tr(""), &ok);
      
    if (ok && !text.isEmpty())
      if (dtkAbstractData *data = dtkAbstractDataFactory::instance()->create ("vtkDataFibers")) {
	  data->setData ( d->manager->GetCallbackOutput() );
	  d->bundleList.insert (text, data);
/*
	  QColorDialog *cdialog = new QColorDialog(Qt::white, d->view->widget());
	  cdialog->exec();
	  QColor color = cdialog->selectedColor();
 */
	  //QColor color = QColorDialog::getColor(Qt::white, d->view->widget()); // buggy on Mac
		  
		  QColor color = QColor::fromHsv(qrand()%360, 255, 210);
	  float color_d[3] = {(float)color.red()/255.0, (float)color.green()/255.0, (float)color.blue()/255.0};
	  //float color_d[3] = {(float)qrand()/(float)RAND_MAX, (float)qrand()/(float)RAND_MAX, (float)qrand()/(float)RAND_MAX};
	  
	  v3dFiberBundle* fiberBundle = v3dFiberBundle::New();
	  fiberBundle->SetName ( text.toAscii().constData() );
	  fiberBundle->SetColor (color_d);
	  fiberBundle->SetFiberColors ( d->manager->GetInput()->GetPointData()->GetScalars() );
	  fiberBundle->SetPoints ( d->manager->GetInput()->GetPoints() );
	  fiberBundle->SetCells ( d->manager->GetSelectedCells() );
	  fiberBundle->SetUseHardwareShaders ( vtkFibersManager::GetUseHardwareShaders() );
	  fiberBundle->SetRenderingMode( vtkFibersManager::GetRenderingMode() );
	  fiberBundle->Create();

	  d->v3dBundleList.insert (text, fiberBundle);
	    
	  if (d->view && d->view->renderer3D())
	      d->view->renderer3D()->AddViewProp ( fiberBundle->GetActor() );
	  
	  emit selectionValidated (text);
      }
}

void v3dViewFiberInteractor::onProjectionPropertySet(const QString& value)
{
    if (!d->view)
        return;
	
      if (value=="true") {
	  d->view->viewAxial()->AddDataSet( d->manager->GetCallbackOutput() );
	  d->view->viewSagittal()->AddDataSet( d->manager->GetCallbackOutput() );
	  d->view->viewCoronal()->AddDataSet( d->manager->GetCallbackOutput() );
      }
}

void v3dViewFiberInteractor::onRadiusSet (int value)
{
    d->manager->SetRadius (value);
    QHash<QString, v3dFiberBundle*>::iterator it = d->v3dBundleList.begin();
    while (it!=d->v3dBundleList.end()) {
        (*it)->SetRadius (value);
	++it;
    }
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewFiberInteractor(void)
{
    return new v3dViewFiberInteractor;
}
