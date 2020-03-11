/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractBiasCorrectionProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>
#include <medStringParameter.h>

#include <medItkBiasCorrectionProcessPluginExport.h>

#include <vector>
class medItkBiasCorrectionProcessPrivate;

class MEDITKBIASCORRECTIONPROCESSPLUGIN_EXPORT medItkBiasCorrectionProcess: public medAbstractBiasCorrectionProcess
{
    Q_OBJECT
public:
    medItkBiasCorrectionProcess(QObject* parent = nullptr);
    ~medItkBiasCorrectionProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;


    medIntParameter* getUIThreadNb() { return m_poUIThreadNb; }
    medIntParameter* getUIShrinkFactors() { return m_poUIShrinkFactors; }
    medIntParameter* getUISplineOrder() { return m_poUISplineOrder; }
    medStringParameter* getSMaxIterations() { return m_poSMaxIterations; }

    medDoubleParameter* getFWienerFilterNoise() { return m_poFWienerFilterNoise; }
    medDoubleParameter* getFbfFWHM() { return m_poFbfFWHM; }
    medDoubleParameter* getFConvergenceThreshold() { return m_poFConvergenceThreshold; }
    medDoubleParameter* getFSplineDistance() { return m_poFSplineDistance; }
    medDoubleParameter* getFInitialMeshResolutionVect1() { return m_poFInitialMeshResolutionVect1; }
    medDoubleParameter* getFInitialMeshResolutionVect2() { return m_poFInitialMeshResolutionVect2; }
    medDoubleParameter* getFInitialMeshResolutionVect3() { return m_poFInitialMeshResolutionVect3; }

private:
    template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus _run();
    template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus N4BiasCorrectionCore();

    float updateProgression(float &pio_rfProgression, int pi_iStepLevel = 1);

private:
    medIntParameter    *m_poUIThreadNb;
    medIntParameter    *m_poUIShrinkFactors;
    medIntParameter    *m_poUISplineOrder;
    medStringParameter *m_poSMaxIterations;

    medDoubleParameter *m_poFWienerFilterNoise;
    medDoubleParameter *m_poFbfFWHM;
    medDoubleParameter *m_poFConvergenceThreshold;
    medDoubleParameter *m_poFSplineDistance;
    medDoubleParameter *m_poFInitialMeshResolutionVect1;
    medDoubleParameter *m_poFInitialMeshResolutionVect2;
    medDoubleParameter *m_poFInitialMeshResolutionVect3;

    itk::SmartPointer <itk::ProcessObject> m_filter;
    bool m_bAborting;
};

inline medAbstractBiasCorrectionProcess* medItkBiasCorrectionProcessCreator()
{
    return new medItkBiasCorrectionProcess();
}
