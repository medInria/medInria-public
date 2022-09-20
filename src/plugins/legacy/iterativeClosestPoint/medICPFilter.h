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
#ifndef medICPFilter_h
#define medICPFilter_h

#include <iterativeClosestPointPluginExport.h>

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkLinearTransform.h"
#include <vector>

#define VTK_ICP_MODE_RMS 0
#define VTK_ICP_MODE_AV 1

class vtkCellLocator;
class vtkLandmarkTransform;
class vtkDataSet;

class ITERATIVECLOSESTPOINTPLUGIN_EXPORT medICPFilter : public vtkLinearTransform
{
public:
  static medICPFilter *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

  //@{
  /**
   * Specify the source and target data sets.
   */
  void SetSource(vtkDataSet *source);
  void SetTarget(vtkDataSet *target);
  vtkGetObjectMacro(Source, vtkDataSet);
  vtkGetObjectMacro(Target, vtkDataSet);
  //@}

  //@{
  /**
   * Set/Get a spatial locator for speeding up the search process.
   * An instance of vtkCellLocator is used by default.
   */
  void SetLocator(vtkCellLocator *locator);
  vtkGetObjectMacro(Locator, vtkCellLocator);
  //@}

  //@{
  /**
   * Set/Get the scale factor
   */
  vtkSetMacro(ScaleFactor, double);
  vtkGetMacro(ScaleFactor, double);
  //@}

  //@{
  /**
   * Set/Get the  maximum number of iterations. Default is 50.
   */
  vtkSetMacro(MaximumNumberOfIterations, int);
  vtkGetMacro(MaximumNumberOfIterations, int);
  //@}

  //@{
  /**
   * Get the number of iterations since the last update
   */
  vtkGetMacro(NumberOfIterations, int);
  //@}

  //@{
  /**
   * Force the algorithm to check the mean distance between two iterations.
   * Default is Off.
   */
  vtkSetMacro(CheckMeanDistance, int);
  vtkGetMacro(CheckMeanDistance, int);
  vtkBooleanMacro(CheckMeanDistance, int);
  //@}

  //@{
  /**
   * Specify the mean distance mode. This mode expresses how the mean
   * distance is computed. The RMS mode is the square root of the average
   * of the sum of squares of the closest point distances. The Absolute
   * Value mode is the mean of the sum of absolute values of the closest
   * point distances. The default is VTK_ICP_MODE_RMS
   */
  vtkSetClampMacro(MeanDistanceMode, int, VTK_ICP_MODE_RMS, VTK_ICP_MODE_AV);
  vtkGetMacro(MeanDistanceMode, int);
  void SetMeanDistanceModeToRMS() {
    this->SetMeanDistanceMode(VTK_ICP_MODE_RMS);
  }
  void SetMeanDistanceModeToAbsoluteValue() {
    this->SetMeanDistanceMode(VTK_ICP_MODE_AV);
  }
  const char *GetMeanDistanceModeAsString();
  //@}

  //@{
  /**
   * Set/Get the maximum mean distance between two iteration. If the mean
   * distance is lower than this, the convergence stops. The default
   * is 0.01.
   */
  vtkSetMacro(MaximumMeanDistance, double);
  vtkGetMacro(MaximumMeanDistance, double);
  //@}

  //@{
  /**
   * Get the mean distance between the last two iterations.
   */
  vtkGetMacro(MeanDistance, double);
  //@}

  //@{
  /**
   * Set/Get the maximum number of landmarks sampled in your dataset.
   * If your dataset is dense, then you will typically not need all the
   * points to compute the ICP transform. The default is 200.
   */
  vtkSetMacro(MaximumNumberOfLandmarks, int);
  vtkGetMacro(MaximumNumberOfLandmarks, int);
  //@}

  //@{
  /**
   * Starts the process by translating source centroid to target centroid.
   * The default is Off.
   */
  vtkSetMacro(StartByMatchingCentroids, int);
  vtkGetMacro(StartByMatchingCentroids, int);
  vtkBooleanMacro(StartByMatchingCentroids, int);
  //@}

  //@{
  /**
   * Get the internal landmark transform. Use it to constrain the number of
   * degrees of freedom of the solution (i.e. rigid body, similarity, etc.).
   */
  vtkGetObjectMacro(LandmarkTransform, vtkLandmarkTransform);
  //@}

  /**
   * Invert the transformation.  This is done by switching the
   * source and target.
   */
  void Inverse() override;

  /**
   * Make another transform of the same type.
   */
  vtkAbstractTransform *MakeTransform() override;

  std::vector<vtkIdType> GetSourceLandmarkIds();

  void GetFREStats(double &mean, double &variance, double &median);

protected:
  //@{
  /**
   * Release source and target
   */
  void ReleaseSource(void);
  void ReleaseTarget(void);
  //@}

  /**
   * Release locator
   */
  void ReleaseLocator(void);

  /**
   * Create default locator. Used to create one when none is specified.
   */
  void CreateDefaultLocator(void);

  /**
   * Get the MTime of this object also considering the locator.
   */
  vtkMTimeType GetMTime() override;

  medICPFilter();
  ~medICPFilter() override;

  void InternalUpdate() override;

  /**
   * This method does no type checking, use DeepCopy instead.
   */
  void InternalDeepCopy(vtkAbstractTransform *transform) override;

  vtkDataSet *Source;
  vtkDataSet *Target;
  vtkCellLocator *Locator;
  int MaximumNumberOfIterations;
  int CheckMeanDistance;
  int MeanDistanceMode;
  double MaximumMeanDistance;
  int MaximumNumberOfLandmarks;
  int StartByMatchingCentroids;
  double ScaleFactor;
  std::vector<vtkIdType> SourceLandmarkIds;
  double meanFRE, varianceFRE, medianFRE;

  int NumberOfIterations;
  double MeanDistance;
  vtkLandmarkTransform *LandmarkTransform;

private:
  medICPFilter(const medICPFilter &) = delete;
  void operator=(const medICPFilter &) = delete;
};

#endif
