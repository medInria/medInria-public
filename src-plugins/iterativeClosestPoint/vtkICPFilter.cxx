#include <vtkFieldData.h>
#include "vtkObjectFactory.h" //for new() macro
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkMatrixToLinearTransform.h"

#include "vtkICPFilter.h"

#include <QString>

vtkStandardNewMacro(vtkICPFilter);
vtkCxxRevisionMacro(vtkICPFilter, "$Revision$");


//-----------------------------------------------------------------------------
vtkICPFilter::vtkICPFilter()
{
    // Mandatory!
    this->Source=nullptr;
    this->Target=nullptr;

    this->bStartByMatchingCentroids=1;
    this->bTransformation=0;
    this->bCheckMeanDistance=0;

    this->ScaleFactor=10;
    this->MaxNumIterations=50;
    this->MaxNumLandmarks=200;
    this->MaxMeanDistance=0.01;

    this->TransformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
}


//-----------------------------------------------------------------------------
vtkICPFilter::~vtkICPFilter()
{

}


//----------------------------------------------------------------------------
vtkPolyData *vtkICPFilter::GetTarget()
{
    return this->Target;
}


//-----------------------------------------------------------------------------
vtkPolyData *vtkICPFilter::GetSource()
{
    return this->Source;
}

//-----------------------------------------------------------------------------
void vtkICPFilter::SetSource(vtkPolyData *source)
{
    this->Source=source;
}

//-----------------------------------------------------------------------------
void vtkICPFilter::SetTarget(vtkPolyData *source)
{
    this->Target=source;
}



//----------------------------------------------------------------------------
void vtkICPFilter::Update()
{
    //  // get the info objects
    //  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    //  vtkInformation *outInfo = outputVector->GetInformationObject(0);
    //
    //  // get the input and output
    //  vtkPolyData *input = vtkPolyData::SafeDownCast(
    //                                                 inInfo->Get(vtkDataObject::DATA_OBJECT()));
    //  vtkPolyData *output = vtkPolyData::SafeDownCast(
    //                                                  outInfo->Get(vtkDataObject::DATA_OBJECT()));

    double m[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    m[0]=this->ScaleFactor;
    m[5]=this->ScaleFactor;
    m[10]=this->ScaleFactor;

    vtkSmartPointer<vtkMatrixToLinearTransform> linearTransform = vtkSmartPointer<vtkMatrixToLinearTransform>::New();
    linearTransform->GetMatrix()->DeepCopy( m );

    //bring the source to the target
    vtkSmartPointer<vtkTransformPolyDataFilter> TransformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    TransformFilter1->SetInput(this->GetSource());
    TransformFilter1->SetTransform(linearTransform);
    TransformFilter1->Update();

    //ICP Transform
    this->ICPTransform = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
    this->ICPTransform->SetSource(TransformFilter1->GetOutput());
    this->ICPTransform->SetTarget(this->GetTarget());

    if(this->bCheckMeanDistance)
        this->ICPTransform->CheckMeanDistanceOn();


    this->ICPTransform->SetMaximumNumberOfIterations(MaxNumIterations);
    //icp->SetThresh(Thresh);
    if(this->bStartByMatchingCentroids)
        this->ICPTransform->StartByMatchingCentroidsOn(); //align center of masses as first transformation

    //all points are used if this is not done, the idea is to use a subset of the points if they are very dense
    //icp->SetMaximumNumberOfLandmarks(1000);
    this->ICPTransform->SetMaximumNumberOfLandmarks(this->MaxNumLandmarks);


    //icp->SetMaximumMeanDistance(accuracy); //the test distance for convergence
    this->ICPTransform->SetMaximumMeanDistance(this->MaxMeanDistance);

    //Set the number of degrees of freedom to constrain the solution to.
    //Rigidbody (VTK_LANDMARK_RIGIDBODY): rotation and translation only.
    //Similarity (VTK_LANDMARK_SIMILARITY): rotation, translation and isotropic scaling.
    //Affine (VTK_LANDMARK_AFFINE): collinearity is preserved. Ratios of distances along a line are preserved.
    //The default is similarity.
    switch (this->bTransformation)
    {
    case 0:
        this->ICPTransform->GetLandmarkTransform()->SetMode(VTK_LANDMARK_RIGIDBODY);
        break;
    case 1:
        this->ICPTransform->GetLandmarkTransform()->SetMode(VTK_LANDMARK_SIMILARITY);
        break;
    case 2:
        this->ICPTransform->GetLandmarkTransform()->SetMode(VTK_LANDMARK_AFFINE);
        break;
    default:
        break;
    }

    //icp->Modified(); //is this necessary?
    this->ICPTransform->Update();

    //bring the source to the target
    TransformFilter2->SetInput(TransformFilter1->GetOutput());
    TransformFilter2->SetTransform(this->ICPTransform);
    TransformFilter2->Update();

    this->GetOutput()->DeepCopy(TransformFilter2->GetOutput());

    this->Source=nullptr;
    this->Target=nullptr;
}

vtkSmartPointer<vtkIterativeClosestPointTransform> vtkICPFilter::GetICPTransform()
{
    return ICPTransform;
}

vtkLinearTransform* vtkICPFilter::GetLinearTransform()
{
    return vtkLinearTransform::SafeDownCast(this->TransformFilter2->GetTransform());
}

////////// External Operators /////////////

void vtkICPFilter::PrintSelf(ostream &os, vtkIndent indent)
{
}
