#include "medItk/medITKProcessObjectObserver.h"

#include <QDebug>
#include <QtCore>

#include "itkProcessObject.h"

namespace itk
{

  QtSignalAdaptor::QtSignalAdaptor()
  {
    this->command = CommandType::New();
    this->command->SetQtSignalAdaptor (this);
  }


  QtSignalAdaptor::~QtSignalAdaptor()
  {}
  
  
  medProcessObjectObserver::medProcessObjectObserver()
  {
    this->adaptor = 0;
  }
  
  medProcessObjectObserver::~medProcessObjectObserver()
  {
  }

  void medProcessObjectObserver::Execute (itk::Object *caller, const itk::EventObject &event)
  {
    itk::ProcessObject *po = dynamic_cast<itk::ProcessObject *>(caller);
    if (! po) {
      return;
    }
    if (this->adaptor) {
      this->adaptor->EmitSignal ((int)(po->GetProgress()*100.0) );
      qApp->processEvents();
    }
  }
  
  void medProcessObjectObserver::Execute (const itk::Object *caller, const itk::EventObject &event)
  {
    itk::ProcessObject *po = dynamic_cast<itk::ProcessObject *>(const_cast<itk::Object *>(caller));
    if (! po)
      return;
    
    if (this->adaptor) {
      this->adaptor->EmitSignal ((int)(po->GetProgress()*100.0) );
      qApp->processEvents();
    }
  }
  
}
