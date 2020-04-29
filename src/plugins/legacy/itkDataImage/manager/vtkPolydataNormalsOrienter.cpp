/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkPolydataNormalsOrienter.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkFloatArray.h"
#include "vtkTransform.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkPolyDataNormalsOrienter)


vtkPolyDataNormalsOrienter::vtkPolyDataNormalsOrienter()
{
    Matrix = nullptr;
}


int vtkPolyDataNormalsOrienter::RequestData(
        vtkInformation *vtkNotUsed(request),
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and output
    vtkDataSet *input = vtkDataSet::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPointData *pd=input->GetPointData();
    vtkPointData *outPD=output->GetPointData();

    vtkDataArray *sourceNormals;
    vtkFloatArray *newNormals = nullptr;
    vtkTransform *trans;

    trans = vtkTransform::New();
    trans->PreMultiply();

    // First, copy the input to the output as a starting point
    output->CopyStructure( input );

    outPD->CopyAllOn();
    outPD->PassData(pd);

    if ( (sourceNormals = pd->GetNormals()) )
    {
        newNormals = vtkFloatArray::New();
        newNormals->SetNumberOfComponents(3);
        newNormals->SetName("Normals");
        newNormals->Allocate(sourceNormals->GetSize());
    }

    trans->Identity();

    if (Matrix && Matrix->Determinant() < 0)
    {
        trans->Scale(-1.0,-1.0,-1.0);
    }

    if( sourceNormals )
    {
        trans->TransformNormals(sourceNormals,newNormals);
    }

    if( newNormals )
    {
        outPD->SetNormals(newNormals);
        newNormals->Delete();
    }

    trans->Delete();

    return 1;

}
