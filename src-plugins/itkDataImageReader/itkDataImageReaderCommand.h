#include <itkCommand.h>

class dtkAbstractDataReader;

namespace itk {

  class DataImageReaderCommand: public Command {
  public:

    typedef DataImageReaderCommand         Self;
    typedef Command                        Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    itkTypeMacro( DataImageReaderCommand, Command );
    itkNewMacro (Self);
    
    void Execute(Object *caller, const EventObject &event);
    void Execute(const Object *caller, const EventObject &event);
    
    void SetDataImageReader (dtkAbstractDataReader* reader)
    { m_Reader = reader; }

  protected:
    DataImageReaderCommand(){ m_Reader = 0; };
    virtual ~DataImageReaderCommand(){};
    
  private:
    dtkAbstractDataReader* m_Reader;
  };
}
