#ifndef ITKSEGMENTATIONCOMMAND_H
#define ITKSEGMENTATIONCOMMAND_H

#include "msegAlgorithmGeneric.h"

#include <itkCommand.h>

class Controller;

namespace itk
{
  class SegmentationCommand : public Command
  {
    
  public:

    typedef SegmentationCommand         Self;
    typedef Command                        Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    itkTypeMacro( SegmentationCommand, Command );
    itkNewMacro (Self);
    
    void Execute(Object *caller, const EventObject &event);
    void Execute(const Object *caller, const EventObject &event);
    
    void SetAlgorithm (mseg::AlgorithmGeneric* alg)
    { m_Algorithm = alg; }

  protected:
    SegmentationCommand(){ m_Algorithm = 0; };
    virtual ~SegmentationCommand(){};
    
  private:
    mseg::AlgorithmGeneric* m_Algorithm;
  };
}

#endif // ITKSEGMENTATIONCOMMAND_H
