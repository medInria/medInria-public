%{
#include "medPipelineRoiToolBoxDelegate.h"
%}

/* These rules allow SWIG to replace C++ output arguments by multiple return values in Python.
 * (Python does not support output arguments)
 * To avoid side effects the rules are cleared after use.
 */
%apply dtkSmartPointer<medAbstractData>* OUTPUT { dtkSmartPointer<medAbstractData>* output };

%include "medPipelineRoiToolBoxDelegate.h"

%clear dtkSmartPointer<medAbstractData>* output;

%forwardStepCreationFunction(roiToolboxStep, RoiToolboxDelegate, createDelegateRoiToolboxStep)
%forwardStepCreationFunction(customRoiToolboxStep, RoiToolboxDelegate, createRoiToolboxStep)

%extend med::pipeline::PythonPipeline
{
    %forwardMethod(startRoiToolboxStep, getRoiToolboxDelegate())
    %forwardMethod(endRoiToolboxStep, getRoiToolboxDelegate())

}
