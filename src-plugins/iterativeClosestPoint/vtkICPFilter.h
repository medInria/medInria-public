#pragma once

#include "vtkSmartPointer.h" // compiler errors if this is forward declared

#include "vtkPolyDataAlgorithm.h" //superclass
#include "vtkExecutive.h"

class vtkPolyData;
class vtkTransform;
class vtkInformation;
class vtkInformationVector;
class vtkIterativeClosestPointTransform;

#include "vtkMath.h"
#include <vtkLinearTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include "iterativeClosestPointPluginExport.h"

class ITERATIVECLOSESTPOINTPLUGIN_EXPORT vtkICPFilter : public vtkPolyDataAlgorithm
{
public:
    static vtkICPFilter *New();
    vtkTypeRevisionMacro(vtkICPFilter, vtkPolyDataAlgorithm);
    void PrintSelf(ostream &os, vtkIndent indent);

    // Description:
    // Specify the source object. This is the object that will be moved during the transformation.
    vtkPolyData *GetSource();
    void SetSource(vtkPolyData *);

    // Description:
    // Specify the target object. This is the object that will stay in place.
    vtkPolyData *GetTarget();
    void SetTarget(vtkPolyData *);


    //void SetStartByMatchingCentroids(const int start) {this->bStartByMatchingCentroids = start;}
    vtkSetMacro(bStartByMatchingCentroids, int);
    vtkSetMacro(bTransformation, int);
    vtkSetMacro(bCheckMeanDistance, int);

    vtkSetMacro(ScaleFactor, double);
    vtkSetMacro(MaxNumIterations, int);
    vtkSetMacro(MaxNumLandmarks, int);
    vtkSetMacro(MaxMeanDistance, double);

    vtkSmartPointer<vtkIterativeClosestPointTransform> GetICPTransform();

    vtkLinearTransform* GetLinearTransform();

    void Update();

protected:
    vtkICPFilter();
    ~vtkICPFilter();


private:
    //BTX
    // BTX-ETX comment is to hide these variable declarations from
    // bin/vtkWrapClientServer. If omitted, we'd get a
    // *** SYNTAX ERROR found in parsing the header file vtkICPFilter.h before line 34 ***

    vtkSmartPointer<vtkIterativeClosestPointTransform> ICPTransform;
    //ETX

    vtkPolyData* Source;
    vtkPolyData* Target;
    int bStartByMatchingCentroids;
    int bTransformation;
    vtkSmartPointer<vtkTransformPolyDataFilter> TransformFilter2;

    int bCheckMeanDistance;
    double ScaleFactor;
    int MaxNumIterations;
    int MaxNumLandmarks;
    double MaxMeanDistance;
};

