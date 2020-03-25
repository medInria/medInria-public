#include "itkQtSignalAdaptor.h"

#include <QDebug>

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

      // qDebug() << po->GetNameOfClass() << " " << po->GetProgress();

      if (this->adaptor)
	  this->adaptor->EmitSignal ((int)(po->GetProgress()*100.0) );
  }
	
  void medProcessObjectObserver::Execute (const itk::Object *caller, const itk::EventObject &event)
  {
      itk::ProcessObject *po = dynamic_cast<itk::ProcessObject *>(const_cast<itk::Object *>(caller));
      if (! po)
	  return;

      // qDebug() << po->GetNameOfClass() << " " << po->GetProgress();

      if (this->adaptor)
	  this->adaptor->EmitSignal ((int)(po->GetProgress()*100.0) );
  } 
}
