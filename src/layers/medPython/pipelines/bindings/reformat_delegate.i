%ignore ReformatDelegate::endCroppingStep(dtkSmartPointer<medAbstractData>*);

%{
#include "medPipelineReformatDelegate.h"
%}

/* These rules allow SWIG to replace C++ output arguments by multiple return values in Python.
 * (Python does not support output arguments)
 * To avoid side effects the rules are cleared after use.
 */
%apply dtkSmartPointer<medAbstractData>* OUTPUT { dtkSmartPointer<medAbstractData>* output };
%apply QList<medAbstractData*>* OUTPUT { QList<dtkSmartPointer<medAbstractData> >* outputs };

%include "medPipelineReformatDelegate.h"

%clear dtkSmartPointer<medAbstractData>* output;
%clear QList<dtkSmartPointer<medAbstractData> >* outputs;

%forwardStepCreationFunction(resliceStep, ReformatDelegate, createDelegateResliceStep)
%forwardStepCreationFunction(customResliceStep, ReformatDelegate, createResliceStep)

%forwardStepCreationFunction(frameExtractionStep, ReformatDelegate, createDelegateFrameExtractionStep)
%forwardStepCreationFunction(customFrameExtractionStep, ReformatDelegate, createFrameExtractionStep)

%forwardStepCreationFunction(croppingStep, ReformatDelegate, createDelegateCroppingStep)
%forwardStepCreationFunction(customCroppingStep, ReformatDelegate, createCroppingStep)

%forwardStepCreationFunction(mergeVolumesStep, ReformatDelegate, createDelegateMergeVolumesStep)
%forwardStepCreationFunction(customMergeVolumesStep, ReformatDelegate, createMergeVolumesStep)

%extend med::pipeline::PythonPipeline
{
    %forwardMethod(startResliceStep, getReformatDelegate())
    %forwardMethod(endResliceStep, getReformatDelegate())
    
    %forwardMethod(startFrameExtractionStep, getReformatDelegate())
    %forwardMethod(endFrameExtractionStep, getReformatDelegate())

    %forwardMethod(startCroppingStep, getReformatDelegate())
    %forwardMethod(endCroppingStep, getReformatDelegate())

    %forwardMethod(startMergeVolumesStep, getReformatDelegate())
    %forwardMethod(endMergeVolumesStep, getReformatDelegate())
}
