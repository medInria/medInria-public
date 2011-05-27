#include "itkSegmentationCommand.h"

#include <itkEventObject.h>
#include <itkProcessObject.h>

#include "msegController.h"

namespace itk
{
    void SegmentationCommand::Execute (Object *caller, const EventObject &event)
    {
        Execute(caller,event);
    }
        
    void SegmentationCommand::Execute (const Object *caller, const EventObject &event)
    {
        if(typeid(event) == typeid(itk::ProgressEvent))
        {
            const itk::ProcessObject *po = dynamic_cast<const ProcessObject *>(caller);

            if (!po)
                return;

            if (m_Algorithm)
                m_Algorithm->reportItkProgress( caller, (po->GetProgress()*100.));
        }
    }

}
