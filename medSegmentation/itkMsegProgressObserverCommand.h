#ifndef ITKMSEGPROGRESSOBSERVERCOMMAND_H
#define ITKMSEGPROGRESSOBSERVERCOMMAND_H

#include "msegAlgorithmGeneric.h"

#include <itkCommand.h>

class Controller;

namespace itk
{
  //! Interface between an ITK process and Qt.
  class MsegProgressObserverCommand : public Command
  {
    
  public:

    typedef MsegProgressObserverCommand         Self;
    typedef Command                        Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    itkTypeMacro( MsegProgressObserverCommand, Command );
    itkNewMacro (Self);
    
    void Execute(Object *caller, const EventObject &event);
    void Execute(const Object *caller, const EventObject &event);
    
    void SetAlgorithm (mseg::AlgorithmGeneric* alg)
    { m_Algorithm = alg; }

  protected:
    MsegProgressObserverCommand(){ m_Algorithm = 0; };
    virtual ~MsegProgressObserverCommand(){};
    
  private:
    mseg::AlgorithmGeneric* m_Algorithm;
  };
}

#endif // ITKMSEGPROGRESSOBSERVERCOMMAND_H
