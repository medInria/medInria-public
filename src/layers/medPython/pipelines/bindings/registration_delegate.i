%{
#include "medPipelineRegistrationDelegate.h"
%}

/* This rules allow SWIG to replace C++ output arguments by multiple return values in Python.
 * (Python does not support output arguments)
 * To avoid side effects the rules are cleared after use.
 */
%apply dtkSmartPointer<medAbstractData>* OUTPUT { dtkSmartPointer<medAbstractData>* output };

%include "medPipelineRegistrationDelegate.h"

%clear dtkSmartPointer<medAbstractData>* output;

%forwardStepCreationFunction(manualRegistrationStep, RegistrationDelegate, createDelegateManualRegistrationStep)
%forwardStepCreationFunction(customManualRegistrationStep, RegistrationDelegate, createManualRegistrationStep)

%extend med::pipeline::PythonPipeline
{
    %forwardMethod(startManualRegistrationStep, getRegistrationDelegate())
    %forwardMethod(endManualRegistrationStep, getRegistrationDelegate())
}
