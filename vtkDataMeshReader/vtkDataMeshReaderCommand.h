#include <vtkCommand.h>

class dtkAbstractDataReader;

class DataMeshReaderCommand : public vtkCommand
{
  
 public:
  
  void Execute(Object *caller, const EventObject &event);
  void Execute(const Object *caller, const EventObject &event);
  
  void SetDataMeshReader (dtkAbstractDataReader* reader)
  { m_Reader = reader; }
  
 protected:
  DataMeshReaderCommand(){ m_Reader = 0; };
  virtual ~DataMeshReaderCommand(){};
  
 private:
  dtkAbstractDataReader* m_Reader;
};
