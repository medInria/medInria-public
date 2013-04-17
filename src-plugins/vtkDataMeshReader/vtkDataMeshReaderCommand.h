/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
