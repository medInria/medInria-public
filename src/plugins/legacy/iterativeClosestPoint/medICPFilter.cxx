/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 *
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the
 * root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "medICPFilter.h"

#include "vtkCellLocator.h"
#include "vtkDataSet.h"
#include "vtkLandmarkTransform.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkTransform.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <QList>
#include <medUtilities.h>

vtkStandardNewMacro(medICPFilter)

//----------------------------------------------------------------------------

medICPFilter::medICPFilter()
  : vtkLinearTransform()
{
  this->Source = nullptr;
  this->Target = nullptr;
  this->Locator = nullptr;
  this->LandmarkTransform = vtkLandmarkTransform::New();
  this->MaximumNumberOfIterations = 50;
  this->CheckMeanDistance = 0;
  this->MeanDistanceMode = VTK_ICP_MODE_RMS;
  this->MaximumMeanDistance = 0.01;
  this->MaximumNumberOfLandmarks = 200;
  this->StartByMatchingCentroids = 0;
  this->ScaleFactor = 1.0;

  this->NumberOfIterations = 0;
  this->MeanDistance = 0.0;

  this->meanFRE = this->varianceFRE = this->medianFRE = 0.0;
}

//----------------------------------------------------------------------------

const char *medICPFilter::GetMeanDistanceModeAsString()
{
  if ( this->MeanDistanceMode == VTK_ICP_MODE_RMS )
  {
    return "RMS";
  }
  else
  {
    return "AbsoluteValue";
  }
}

//----------------------------------------------------------------------------

medICPFilter::~medICPFilter()
{
  ReleaseSource();
  ReleaseTarget();
  ReleaseLocator();
  this->LandmarkTransform->Delete();
}

//----------------------------------------------------------------------------

void medICPFilter::SetSource(vtkDataSet *source)
{
  if (this->Source == source)
  {
    return;
  }

  if (this->Source)
  {
    this->ReleaseSource();
  }

  if (source)
  {
    source->Register(this);
  }

  this->Source = source;
  this->Modified();
}

//----------------------------------------------------------------------------

void medICPFilter::ReleaseSource(void) {
  if (this->Source)
  {
    this->Source->UnRegister(this);
    this->Source = nullptr;
  }
}

//----------------------------------------------------------------------------

void medICPFilter::SetTarget(vtkDataSet *target)
{
  if (this->Target == target)
  {
    return;
  }

  if (this->Target)
  {
    this->ReleaseTarget();
  }

  if (target)
  {
    target->Register(this);
  }

  this->Target = target;
  this->Modified();
}

//----------------------------------------------------------------------------

void medICPFilter::ReleaseTarget(void) {
  if (this->Target)
  {
    this->Target->UnRegister(this);
    this->Target = nullptr;
  }
}

//----------------------------------------------------------------------------

void medICPFilter::SetLocator(vtkCellLocator *locator)
{
  if (this->Locator == locator)
  {
    return;
  }

  if (this->Locator)
  {
    this->ReleaseLocator();
  }

  if (locator)
  {
    locator->Register(this);
  }

  this->Locator = locator;
  this->Modified();
}

//----------------------------------------------------------------------------

void medICPFilter::ReleaseLocator(void) {
  if (this->Locator)
  {
    this->Locator->UnRegister(this);
    this->Locator = nullptr;
  }
}

//----------------------------------------------------------------------------

void medICPFilter::CreateDefaultLocator() {
  if (this->Locator)
  {
    this->ReleaseLocator();
  }

  this->Locator = vtkCellLocator::New();
}

//------------------------------------------------------------------------

vtkMTimeType medICPFilter::GetMTime()
{
  vtkMTimeType result = this->vtkLinearTransform::GetMTime();
  vtkMTimeType mtime;

  if (this->Source)
  {
    mtime = this->Source->GetMTime();
    if (mtime > result)
    {
      result = mtime;
    }
  }

  if (this->Target)
  {
    mtime = this->Target->GetMTime();
    if (mtime > result)
    {
      result = mtime;
    }
  }

  if (this->Locator)
  {
    mtime = this->Locator->GetMTime();
    if (mtime > result)
    {
      result = mtime;
    }
  }

  if (this->LandmarkTransform)
  {
    mtime = this->LandmarkTransform->GetMTime();
    if (mtime > result)
    {
      result = mtime;
    }
  }

  return result;
}

//----------------------------------------------------------------------------

std::vector<vtkIdType> medICPFilter::GetSourceLandmarkIds()
{
    return this->SourceLandmarkIds;
}

//----------------------------------------------------------------------------

void medICPFilter::GetFREStats(double &mean, double &stdDev, double &median)
{
     mean   = this->meanFRE;
     stdDev = sqrt(this->varianceFRE);
     median = this->medianFRE;
}

//----------------------------------------------------------------------------

void medICPFilter::Inverse()
{
  vtkDataSet *tmp1 = this->Source;
  this->Source = this->Target;
  this->Target = tmp1;
  this->Modified();
}

//----------------------------------------------------------------------------

vtkAbstractTransform *medICPFilter::MakeTransform()
{
  return medICPFilter::New();
}

//----------------------------------------------------------------------------

void medICPFilter::InternalDeepCopy(vtkAbstractTransform *transform)
{
  medICPFilter *t = (medICPFilter *)transform;

  this->SetSource(t->GetSource());
  this->SetTarget(t->GetTarget());
  this->SetLocator(t->GetLocator());
  this->SetMaximumNumberOfIterations(t->GetMaximumNumberOfIterations());
  this->SetCheckMeanDistance(t->GetCheckMeanDistance());
  this->SetMeanDistanceMode(t->GetMeanDistanceMode());
  this->SetMaximumMeanDistance(t->GetMaximumMeanDistance());
  this->SetMaximumNumberOfLandmarks(t->GetMaximumNumberOfLandmarks());
  this->SetScaleFactor(t->GetScaleFactor());

  this->Modified();
}

//----------------------------------------------------------------------------

void medICPFilter::InternalUpdate()
{
  // Check source, target

  if (this->Source == nullptr || !this->Source->GetNumberOfPoints())
  {
    vtkErrorMacro(<<"Can't execute with nullptr or empty input");
    return;
  }

  if (this->Target == nullptr || !this->Target->GetNumberOfPoints())
  {
    vtkErrorMacro(<<"Can't execute with nullptr or empty target");
    return;
  }

  // Apply scale factor to Source mesh
  double m[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
  m[0]  = this->ScaleFactor;
  m[5]  = this->ScaleFactor;
  m[10] = this->ScaleFactor;

  vtkSmartPointer<vtkMatrixToLinearTransform> linearTransform = vtkSmartPointer<vtkMatrixToLinearTransform>::New();
  linearTransform->GetMatrix()->DeepCopy(m);

  // Bring the source to the target
  vtkSmartPointer<vtkTransformPolyDataFilter> TransformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  TransformFilter1->SetInputData(this->GetSource());
  TransformFilter1->SetTransform(linearTransform);
  TransformFilter1->Update();

  vtkSmartPointer<vtkPolyData> newSource = TransformFilter1->GetOutput();

  // Create locator

  this->CreateDefaultLocator();
  this->Locator->SetDataSet(this->Target);
  this->Locator->SetNumberOfCellsPerBucket(1);
  this->Locator->BuildLocator();

  // Create two sets of points to handle iteration

  int step = 1;
  if (newSource->GetNumberOfPoints() > this->MaximumNumberOfLandmarks)
  {
    step = newSource->GetNumberOfPoints() / this->MaximumNumberOfLandmarks;
    vtkDebugMacro(<< "Landmarks step is now : " << step);
  }

  vtkIdType nb_points = newSource->GetNumberOfPoints() / step;

  this->SourceLandmarkIds.resize(nb_points);

  for (int i = 0, j = 0; i < nb_points; i++, j += step)
  {
     this->SourceLandmarkIds[i] = j;
  }

  // Allocate some points.
  // - closestp is used so that the internal state of LandmarkTransform remains
  //   correct whenever the iteration process is stopped (hence its source
  //   and landmark points might be used in a vtkThinPlateSplineTransform).
  // - points2 could have been avoided, but do not ask me why
  //   InternalTransformPoint is not working correctly on my computer when
  //   in and out are the same pointer.

  vtkPoints *points1 = vtkPoints::New();
  points1->SetNumberOfPoints(nb_points);

  vtkPoints *closestp = vtkPoints::New();
  closestp->SetNumberOfPoints(nb_points);

  vtkPoints *points2 = vtkPoints::New();
  points2->SetNumberOfPoints(nb_points);

  // Fill with initial positions (sample dataset using step)

  vtkTransform *accumulate = vtkTransform::New();
  accumulate->PostMultiply();
  accumulate->Concatenate(linearTransform->GetMatrix());

  vtkIdType i;
  int j;
  double p1[3], p2[3];

  if (StartByMatchingCentroids)
  {
    double source_centroid[3] = {0,0,0};
    for (i = 0; i < newSource->GetNumberOfPoints(); i++)
    {
      newSource->GetPoint(i, p1);
      source_centroid[0] += p1[0];
      source_centroid[1] += p1[1];
      source_centroid[2] += p1[2];
    }
    source_centroid[0] /= newSource->GetNumberOfPoints();
    source_centroid[1] /= newSource->GetNumberOfPoints();
    source_centroid[2] /= newSource->GetNumberOfPoints();

    double target_centroid[3] = {0,0,0};
    for (i = 0; i < this->Target->GetNumberOfPoints(); i++)
    {
      this->Target->GetPoint(i, p2);
      target_centroid[0] += p2[0];
      target_centroid[1] += p2[1];
      target_centroid[2] += p2[2];
    }
    target_centroid[0] /= this->Target->GetNumberOfPoints();
    target_centroid[1] /= this->Target->GetNumberOfPoints();
    target_centroid[2] /= this->Target->GetNumberOfPoints();

    vtkMatrix4x4 *translateMatrix = vtkMatrix4x4::New();
    translateMatrix->Identity();

    translateMatrix->SetElement(0, 3, target_centroid[0] - source_centroid[0]);
    translateMatrix->SetElement(1, 3, target_centroid[1] - source_centroid[1]);
    translateMatrix->SetElement(2, 3, target_centroid[2] - source_centroid[2]);

    accumulate->Concatenate(translateMatrix);
    accumulate->Update();

    vtkSmartPointer<vtkTransform> translateTransform = vtkSmartPointer<vtkTransform>::New();
    translateTransform->SetMatrix(translateMatrix);
    translateTransform->Update();

    double outPoint[3];
    for (i = 0, j = 0; i < nb_points; i++, j += step)
    {
      translateTransform->TransformPoint(newSource->GetPoint(j), outPoint);
      points1->SetPoint(i, outPoint);
    }
  }
  else
  {
    for (i = 0, j = 0; i < nb_points; i++, j += step)
    {
      points1->SetPoint(i, newSource->GetPoint(j));
    }
  }

  // Go

  vtkIdType cell_id;
  int sub_id;
  double dist2, totaldist = 0;
  double outPoint[3];

  vtkPoints *temp, *a = points1, *b = points2;

  this->NumberOfIterations = 0;

  do
  {
    // Fill points with the closest points to each vertex in input

    for(i = 0; i < nb_points; i++)
    {
      this->Locator->FindClosestPoint(a->GetPoint(i),
                                      outPoint,
                                      cell_id,
                                      sub_id,
                                      dist2);
      closestp->SetPoint(i, outPoint);
    }

    // Build the landmark transform

    this->LandmarkTransform->SetSourceLandmarks(a);
    this->LandmarkTransform->SetTargetLandmarks(closestp);
    this->LandmarkTransform->Update();

    // Concatenate (can't use this->Concatenate directly)

    accumulate->Concatenate(this->LandmarkTransform->GetMatrix());

    this->NumberOfIterations++;
    vtkDebugMacro(<< "Iteration: " << this->NumberOfIterations);
    if (this->NumberOfIterations >= this->MaximumNumberOfIterations)
    {
      break;
    }

    // Move mesh and compute mean distance if needed

    if (this->CheckMeanDistance)
    {
      totaldist = 0.0;
    }

    for(i = 0; i < nb_points; i++)
    {
      a->GetPoint(i, p1);
      this->LandmarkTransform->InternalTransformPoint(p1, p2);
      b->SetPoint(i, p2);
      if (this->CheckMeanDistance)
      {
        if (this->MeanDistanceMode == VTK_ICP_MODE_RMS)
        {
          totaldist += vtkMath::Distance2BetweenPoints(p1, p2);
        }
        else
        {
          totaldist += sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
        }
      }
    }

    if (this->CheckMeanDistance)
    {
      if (this->MeanDistanceMode == VTK_ICP_MODE_RMS)
      {
        this->MeanDistance = sqrt(totaldist / (double)nb_points);
      }
      else
      {
        this->MeanDistance = totaldist / (double)nb_points;
      }
      vtkDebugMacro("Mean distance: " << this->MeanDistance);
      if (this->MeanDistance <= this->MaximumMeanDistance)
      {
        break;
      }
    }

    temp = a;
    a = b;
    b = temp;

  }
  while (1);

  // Now recover accumulated result

  this->Matrix->DeepCopy(accumulate->GetMatrix());

  double transformSourcePoint[3];
  QList<double> distances;

  for(i = 0; i < nb_points; i++)
  {
      accumulate->TransformPoint(this->Source->GetPoint(this->SourceLandmarkIds[i]), transformSourcePoint);
      distances.append(sqrt(vtkMath::Distance2BetweenPoints(transformSourcePoint,
                                                        closestp->GetPoint(i) )));

  }

  medUtilities::computeMeanAndVariance(distances, &this->meanFRE, &this->varianceFRE);
  medUtilities::computeMedian(distances, &this->medianFRE);

  accumulate->Delete();
  points1->Delete();
  closestp->Delete();
  points2->Delete();
}

//----------------------------------------------------------------------------

void medICPFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->Source )
  {
    os << indent << "Source: " << this->Source << "\n";
  }
  else
  {
    os << indent << "Source: (none)\n";
  }

  if ( this->Target )
  {
    os << indent << "Target: " << this->Target << "\n";
  }
  else
  {
    os << indent << "Target: (none)\n";
  }

  if ( this->Locator )
  {
    os << indent << "Locator: " << this->Locator << "\n";
  }
  else
  {
    os << indent << "Locator: (none)\n";
  }

  os << indent << "MaximumNumberOfIterations: " << this->MaximumNumberOfIterations << "\n";
  os << indent << "CheckMeanDistance: " << this->CheckMeanDistance << "\n";
  os << indent << "MeanDistanceMode: " << this->GetMeanDistanceModeAsString() << "\n";
  os << indent << "MaximumMeanDistance: " << this->MaximumMeanDistance << "\n";
  os << indent << "MaximumNumberOfLandmarks: " << this->MaximumNumberOfLandmarks << "\n";
  os << indent << "StartByMatchingCentroids: " << this->StartByMatchingCentroids << "\n";
  os << indent << "NumberOfIterations: " << this->NumberOfIterations << "\n";
  os << indent << "MeanDistance: " << this->MeanDistance << "\n";
  if(this->LandmarkTransform)
  {
    os << indent << "LandmarkTransform:\n";
    this->LandmarkTransform->PrintSelf(os, indent.GetNextIndent());
  }
}
