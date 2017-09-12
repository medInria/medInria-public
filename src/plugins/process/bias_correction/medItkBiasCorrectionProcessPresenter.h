/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractBiasCorrectionProcessPresenter.h>
#include <medItkBiasCorrectionProcess.h>
#include <medProcessPresenterFactory.h>

#include <medItkBiasCorrectionProcessPluginExport.h>

class medIntParameterPresenter;
class medDoubleParameterPresenter;

class MEDITKBIASCORRECTIONPROCESSPLUGIN_EXPORT medItkBiasCorrectionProcessPresenter: public medAbstractBiasCorrectionProcessPresenter
{
    Q_OBJECT
public:
    medItkBiasCorrectionProcessPresenter(medItkBiasCorrectionProcess *parent);
    virtual QWidget *buildToolBoxWidget();
    virtual medItkBiasCorrectionProcess* process() const {return m_process;}

private:
    medItkBiasCorrectionProcess *m_process;
    medIntParameterPresenter    *m_poUIPresenterThreadNb;                     //Number of threads to run on (default: all cores)
    medIntParameterPresenter    *m_poUIPresenterShrinkFactors;                //Shrink factor, default=4
    medIntParameterPresenter    *m_poUIPresenterSplineOrder;                  //BSpline Order, default=3
    medIntParameterPresenter    *m_poUIPresenterMaxNumbersIterationsVector1;  //B - Spline grid resolution
    medIntParameterPresenter    *m_poUIPresenterMaxNumbersIterationsVector2;  //B - Spline grid resolution
    medIntParameterPresenter    *m_poUIPresenterMaxNumbersIterationsVector3;  //B - Spline grid resolution
    medDoubleParameterPresenter *m_poFPresenterWienerFilterNoise;              //Wiener Filter Noise, default=0.01
    medDoubleParameterPresenter *m_poFPresenterbfFWHM;                         //Bias field Full Width at Half Maximum, default=0.15
    medDoubleParameterPresenter *m_poFPresenterConvergenceThreshold;           //Convergence Threshold, default=0.0001
    medDoubleParameterPresenter *m_poFPresenterSplineDistance;                 //B-Spline distance, default=0.0
    medDoubleParameterPresenter *m_poFPresenterInitialMeshResolutionVect1;     //B - Spline grid resolution. Exist only if SplineDistance == 0. \ 
    medDoubleParameterPresenter *m_poFPresenterInitialMeshResolutionVect2;     //B - Spline grid resolution. Exist only if SplineDistance == 0. |--default=1x1x1
    medDoubleParameterPresenter *m_poFPresenterInitialMeshResolutionVect3;     //B - Spline grid resolution. Exist only if SplineDistance == 0. / 

};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractBiasCorrectionProcess, medItkBiasCorrectionProcess)
