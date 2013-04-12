#include "vtkXMLFiberDataSetReader.h"
#include "vtkFiberDataSet.h"

int main (int narg, const char *args[])
{

  if (narg<2)
    return -1;
  
  vtkXMLFiberDataSetReader *reader = vtkXMLFiberDataSetReader::New();
  reader->SetFileName (args[1]);
  reader->Update();


  vtkFiberDataSet *fibers = reader->GetOutput();

  std::cout << *fibers;
   

  reader->Delete();
  
  return 0;
}
