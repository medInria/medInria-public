%{
#include "medPipelineStepProperties.h"
%}

%include "medPipelineStepProperties.h"

//%qVariantMapSetter(setOption, medPipelineStepProperties, setOption)
%qListTypemaps(med::pipeline::PipelineStepProperties, SWIG_TYPECHECK_OBJECT_ARRAY)

%extend med::pipeline::PipelineStepProperties
{
    %pythoncode
    {
        def setParameter(self, parameter, value):
            if parameter == "name":
                return self.setName(value)
            elif parameter == "title":
                return self.setTitle(value)
            elif parameter == "titlePrefix":
                return self.prependTitle(value)
            elif parameter == "titleSuffix":
                return self.appendTitle(value)
            elif parameter == "description":
                return self.setDescription(value)
            elif parameter == "descriptionPrefix":
                return self.prependDescription(value)
            elif parameter == "descriptionSuffix":
                return self.appendDescription(value)
            elif parameter == "requires":
                if isinstance(value, tuple):
                    for item in value:
                        self.requires(item)
                else:
                    self.requires(value)
                return self
            elif parameter == "delegateClass":
                return self.setDelegateClass(value.__name__)
            elif parameter == "automatic":
                return self.setAutomatic(value)
            elif parameter == "resultDropMode":
                return self.setResultDropMode(value)
            else:
                return self.setOption(parameter, value)
    }
}

/* Allows step paramters and options to be set directly on step creation
 * using Python's named parameter syntax.
 * ex: customStep('Foo', description = "...", requires = ('ctData', 'leftEndoMask'))
 */
%define %forwardStepCreationFunction(name, targetClass, targetMethod)
%pythoncode
{
    def name(*positional, **keywords):
        step = targetClass.targetMethod(*positional)
        for key in keywords.keys():
            step.setParameter(key, keywords[key])
        return step
}
%enddef

// To create a generic custom step
%pythoncode
{
    def customStep(name, **keywords):
        step = medPipelineStepProperties()
        step.setName(name)
        for key in keywords.keys():
            step.setParameter(key, keywords[key])
        return step
}
