#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkSingleValuedNonLinearOptimizer.h"


namespace itk {

class ITK_EXPORT NewUoaOptimizer : public SingleValuedNonLinearOptimizer
{
 public:

  /** Standard typedefs. */
  typedef NewUoaOptimizer                                           Self;
  typedef SingleValuedNonLinearOptimizer                            SuperClass;
  typedef SmartPointer<Self>                                        Pointer;
  typedef SmartPointer<const Self>                                  ConstPointer;

  typedef SingleValuedNonLinearOptimizer::ParametersType             ParametersType;

  /* Method for creation through the object factory */
  itkNewMacro(Self) ;
  
  /*  Run-time type information (and related methods)  */
  itkTypeMacro( NewUoaOptimizer, SingleValuedNonLinearOptimizer);  

  /* Set/Get space dimension */
  itkSetMacro( SpaceDimension, unsigned int);
  itkGetConstReferenceMacro( SpaceDimension, unsigned int) ;

  /* Set/Get the maximum number of function calls */
  itkSetMacro( MaxFunctionCalls, unsigned int);
  itkGetConstReferenceMacro( MaxFunctionCalls, unsigned int) ;

  /* Get the actual number of function calls */
  itkGetConstReferenceMacro( NbFunctionCalls, unsigned int) ;

  /* Get the best value of the objective function */
  itkGetConstReferenceMacro( BestValue, double) ;

  /* Set/Get rhobeg and rhoend */
  itkSetMacro( RhoBeg, double);
  itkGetConstReferenceMacro( RhoBeg, double);
  
  itkSetMacro( RhoEnd, double);
  itkGetConstReferenceMacro( RhoEnd, double);

  /* Set/Get the number of interpolation conditions */
  itkSetMacro( NbInterp, long int);
  itkGetConstReferenceMacro( NbInterp, long int);

  /* Set/Get the scale on the translation (x,y,z) */
  itkSetMacro( ScaleTranslation, double);
  itkGetConstReferenceMacro( ScaleTranslation, double);

  /* Start the optimizer */
  void StartOptimization();
  
 protected:
  NewUoaOptimizer();
  virtual ~NewUoaOptimizer();
  void PrintSelf(std::ostream& os, Indent indent) const ;
  
  // NewUOA optimization section
  int newuoa(double *w, long int *n, long int *npt, double *x, 
				 double *rhobeg, double *rhoend, long int *iprint, long int * maxfun);

  int newuob(long int *n, long int *npt, double *x, 
				 double *rhobeg, double *rhoend, long int *iprint, long int * maxfun, 
				 double *xbase, double *xopt, double *xnew, 
				 double *xpt, double *fval, double *gq, double *hq, 
				 double *pq, double *bmat, double *zmat, long int *ndim, 
				 double *d__, double *vlag, double *w);
	
  int biglag(long int *, long int *, double *, 
				 double *, double *, double *, long int *, long int *, 
				 long int *, double *, double *, double *, double *,
				 double *, double *, double *, double *);
		
  int bigden(long int *, long int *, double *, double *, double *, 
				 double *, long int *, long int *, long int *, long int *, 
				 double *, double *, double *, double *, 
				 double *, double *, double *);
	
  int update(long int *, long int *, 
				 double *, double *, long int *, long int *, double *, 
				 double *, long int *, double *);
	
  int trsapp(long int *, long int *, double *, 
				 double *, double *, double *, double *, 
				 double *, double *, double *, double *, 
				 double *, double *, double *);

 private:
  NewUoaOptimizer(const Self&);  // Purposely not implemented
  void operator=(const Self&)  ; // Purposely not implemented

  /* Space dimension */
  unsigned int                   m_SpaceDimension;

  /* Maximum number of function calls */
  unsigned int                  m_MaxFunctionCalls;

  /* Number of function calls. */
  unsigned int                  m_NbFunctionCalls;

  /* Best function value. */
  double                        m_BestValue;

  /* rhobeg and rhoend. */
  double                        m_RhoBeg;
  double                        m_RhoEnd;

  /* Number of interpolation conditions. */
  long int                     m_NbInterp;

  /* Scaling on the translation (x,y,z). */
  double                      m_ScaleTranslation;



} ; // end of class

} // end of namespace itk
