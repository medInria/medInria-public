%ignore FilteringDelegate::endSizeThresholdingStep(dtkSmartPointer<medAbstractData>*);
%ignore FilteringDelegate::endMorphologicalFilteringStep(dtkSmartPointer<medAbstractData>*);
%ignore FilteringDelegate::endMorphologicalFilteringStep(dtkSmartPointer<medAbstractData>*, int*);

%{
#include "medPipelineFilteringDelegate.h"
%}

/* These rules allow SWIG to replace C++ output arguments by multiple return values in Python.
 * (Python does not support output arguments)
 * To avoid side effects the rules are cleared after use.
 */
%apply dtkSmartPointer<medAbstractData>* OUTPUT { dtkSmartPointer<medAbstractData>* output };
%apply int* OUTPUT { int* chosenRadius };
%apply int* OUTPUT { med::pipeline::RadiusType* chosenRadiusType };
%apply double* OUTPUT { double* chosenThreshold };

%include "medPipelineFilteringDelegate.h"

%clear dtkSmartPointer<medAbstractData>* output;
%clear int* chosenRadius;
%clear med::pipeline::RadiusType* chosenRadiusType;
%clear double* chosenThreshold;

%forwardStepCreationFunction(sizeThresholdingStep, FilteringDelegate, createDelegateSizeThresholdingStep)
%forwardStepCreationFunction(customSizeThresholdingStep, FilteringDelegate, createSizeThresholdingStep)

%forwardStepCreationFunction(thresholdingStep, FilteringDelegate, createDelegateThresholdingStep)
%forwardStepCreationFunction(customThresholdingStep, FilteringDelegate, createThresholdingStep)

%forwardStepCreationFunction(binaryThresholdingStep, FilteringDelegate, createDelegateBinaryThresholdingStep)
%forwardStepCreationFunction(customBinaryThresholdingStep, FilteringDelegate, createBinaryThresholdingStep)

%forwardStepCreationFunction(morphologicalFilteringStep, FilteringDelegate, createDelegateMorphologicalFilteringStep)
%forwardStepCreationFunction(customMorphologicalFilteringStep, FilteringDelegate, createMorphologicalFilteringStep)

%extend med::pipeline::PythonPipeline
{
    %forwardMethod(startSizeThresholdingStep, getFilteringDelegate())
    %forwardMethod(endSizeThresholdingStep, getFilteringDelegate())

    %forwardMethod(startThresholdingStep, getFilteringDelegate())
    %forwardMethod(endThresholdingStep, getFilteringDelegate())

    %forwardMethod(startBinaryThresholdingStep, getFilteringDelegate())
    %forwardMethod(endBinaryThresholdingStep, getFilteringDelegate())

    %forwardMethod(startMorphologicalFilteringStep, getFilteringDelegate())
    %forwardMethod(endMorphologicalFilteringStep, getFilteringDelegate())
}
