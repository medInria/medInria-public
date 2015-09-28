/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDiffXMLWriter.h"

#include <string>
#include <vector>
#include <vtksys/SystemTools.hxx>
#include <vtksys/ios/sstream>
#include "vtkSmartPointer.h"
#include "vtkDataManager.h"
#include "vtkMetaDataSetSequence.h"
#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkMetaSurfaceMesh.h"
#include <vtkPolyData.h>
#include <vtkDataArrayCollection.h>
#include <vtkCellArray.h>
#include <vtkActor.h>
#include <vtkProperty.h>
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkDiffXMLWriter);


//----------------------------------------------------------------------------
vtkDiffXMLWriter::vtkDiffXMLWriter()
{

  this->Input = 0;
  this->FileName = "";
}

//----------------------------------------------------------------------------
vtkDiffXMLWriter::~vtkDiffXMLWriter()
{
}


//----------------------------------------------------------------------------
void vtkDiffXMLWriter::PrintSelf(ostream& os, vtkIndent indent)
{
}

//----------------------------------------------------------------------------
void vtkDiffXMLWriter::SetInput (vtkDataManager* manager)
{
  this->Input = manager;
}

//----------------------------------------------------------------------------
vtkDataManager* vtkDiffXMLWriter::GetInput()
{
  return this->Input;
}


//----------------------------------------------------------------------------
void vtkDiffXMLWriter::Update()
{
  std::cout<<"writing DIFF XML type"<<std::endl;
  
  std::ostringstream os;
  
  std::vector<vtkMetaDataSet*> datasetlist = this->Input->GetMetaDataSetList();
  
  os<<"<?xml version=\"1.0\"?>"<<std::endl;
  os<<"<DIF>"<<std::endl;
  os<<"\t<DIFBody>"<<std::endl;
  os<<"\t\t<Volumes number=\""<<datasetlist.size()<<"\">"<<std::endl;
  
  vtkMetaSurfaceMesh* metasurface = 0;
  vtkDataArrayCollection* arrays = vtkDataArrayCollection::New();
  vtkIdList* pointids = vtkIdList::New();
  double point[3];
  
  for (unsigned int j=0; j<datasetlist.size(); j++)
  {
    
    vtkMetaDataSet* metadata = datasetlist[j];
    std::string filepath = metadata->GetFilePath();
    int val = metadata->GetType();

    if (val != vtkMetaDataSet::VTK_META_SURFACE_MESH)
      continue;
    
    metasurface = vtkMetaSurfaceMesh::SafeDownCast(metadata);
    metadata->GetColorArrayCollection (arrays);
    
    os<<"\t\t\t<Volume name=\""<<metadata->GetName()<<"\" color=\"";
    
    // // TODO: Color required to be in Hexadecimal values, but when asked from vtk object it is returned as RGB values, Thus conversion required here.    
    // //double *color; color = metadata->GetActor(0)->GetProperty()->GetColor();
    					
    // //printf("color %x",color[0]);
    // double dv;
    // char *sv;
    // int i;
    
    // dv=0.1;
    // sv=(char *) &dv;
    // printf("sv -> ");
    // for (i=0;i<sizeof (double);++i)
    //   printf("%x ",sv[i]);
    // printf("\n");
    
    os<<"\">"<<std::endl;

    os <<"\t\t\t\t<Vertices number=\""<<metasurface->GetPolyData()->GetNumberOfPoints()<<"\">"<<std::endl;
    for(vtkIdType i=0;i<metasurface->GetPolyData()->GetNumberOfPoints();i++)
    {
      metasurface->GetPolyData()->GetPoint(i,point);
      os<<"\t\t\t\t\t"<<point[0]<<" "<<point[1]<<" "<<point[2]<<std::endl;
    }
    os<<"\t\t\t\t</Vertices>"<<std::endl;
    
    os<<"\t\t\t\t<Polygons number=\""<<metasurface->GetPolyData()->GetPolys()->GetNumberOfCells()<<"\">"<<std::endl;
    for(int i=0;i<metasurface->GetPolyData()->GetPolys()->GetNumberOfCells();++i)
    {
      metasurface->GetPolyData()->GetCellPoints(i, pointids);
      if (pointids->GetNumberOfIds() != 3)
      {
	vtkWarningMacro(<<"warning : wrong type of cells !"<<endl);
	pointids->Delete();
	return;
      }
      os<<"\t\t\t\t\t"<<(pointids->GetId(0)+1)<<" "<<(pointids->GetId(1)+1)<<" "<<(pointids->GetId(2)+1)<<std::endl;
    }
    os<<"\t\t\t\t</Polygons>"<<std::endl;
    os<<"\t\t\t</Volume>"<<std::endl;
  }

  pointids->Delete();
  arrays->Delete();
  
  os<<"\t\t</Volumes>"<<std::endl;
  os<<"\t<DIFBody>"<<std::endl;
  os <<"<DIF>"<<std::endl;
  
  // open the file:
  std::ofstream buffer (this->FileName.c_str());
  if( buffer.fail() )
  {
    vtkErrorMacro(<<"Cannot write in file !"<<endl);
    buffer.close();
  }
  
  buffer << os.str().c_str() <<std::endl;
  buffer.close();
}

