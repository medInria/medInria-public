/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkVectorOrienter.h"

#include "vtkCellData.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkAssignAttribute.h"
#include "vtkDoubleArray.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkSmartPointer.h"

vtkStandardNewMacro(vtkVectorOrienter)


vtkVectorOrienter::vtkVectorOrienter()
{
    this->VOI[0] = this->VOI[2] = this->VOI[4] = 0;
    this->VOI[1] = this->VOI[3] = this->VOI[5] = VTK_INT_MAX;

    this->Projection = false;
}


int vtkVectorOrienter::RequestData(
        vtkInformation *vtkNotUsed(request),
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and output
    vtkImageData *input = vtkImageData::SafeDownCast(
                inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *output = vtkImageData::SafeDownCast(
                outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPointData *pd=input->GetPointData();
    vtkPointData *outPD=output->GetPointData();

    vtkDataArray *inVectors;
    double inV[4];

    vtkIdType ptId, numPts;
    vtkDoubleArray *newVectors=NULL;

    // First, copy the input to the output as a starting point
    output->CopyStructure( input );


    // build transposed Matrix
    vtkMatrix4x4 *matrixT = vtkMatrix4x4::New();
    vtkMatrix4x4::Transpose(Matrix, matrixT);

    inVectors = pd->GetVectors();

    numPts    = input->GetNumberOfPoints();

    newVectors = vtkDoubleArray::New();
    newVectors->SetNumberOfComponents (3);
    newVectors->SetNumberOfTuples (numPts);

    if ( !inVectors || numPts < 1 )
    {
        vtkErrorMacro(<<"No data to orient!");
        return 1;
    }

    outPD->CopyAllOn();
    outPD->PassData(pd);

    for (ptId=0; ptId < numPts; ptId++)
    {
        inVectors->GetTuple(ptId, inV);
        inV[3] = 0;

        // As Actor->SetUserMatrix() also applies the matrix to the vectors (expressed in world coordinates)
        // we need to apply matrixT before, so that there are not imapcted by Actor->SetUserMatrix()
        matrixT->MultiplyPoint(inV, inV);

        if(Projection)
        {
            if(ProjMode==ProjectOnYZ)
            {
                inV[0] = 0;
            }
            else if(ProjMode==ProjectOnXZ)
            {
                inV[1] = 0;
            }
            else if(ProjMode==ProjectOnXY)
            {
                inV[2] = 0;
            }
        }

        newVectors->SetTuple (ptId, inV);
    }

    outPD->SetVectors(newVectors);
    newVectors->Delete();
    matrixT->Delete();

    return 1;

}


void vtkVectorOrienter::SetVOI(const int& imin,const int& imax,
                               const int& jmin,const int& jmax,
                               const int& kmin,const int& kmax)
{
    if(imin==imax)
        ProjMode = ProjectOnYZ;
    else if(jmin==jmax)
        ProjMode = ProjectOnXZ;
    else if(kmin==kmax)
        ProjMode = ProjectOnXY;
}

void vtkVectorOrienter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "VOI: \n";
    os << indent << "  Imin,Imax: (" << this->VOI[0] << ", "
       << this->VOI[1] << ")\n";
    os << indent << "  Jmin,Jmax: (" << this->VOI[2] << ", "
       << this->VOI[3] << ")\n";
    os << indent << "  Kmin,Kmax: (" << this->VOI[4] << ", "
       << this->VOI[5] << ")\n";

}
