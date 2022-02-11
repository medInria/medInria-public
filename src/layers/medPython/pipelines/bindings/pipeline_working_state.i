%ignore importsFinished;

%{
#include "medPipelineWorkingState.h"
%}

%include "medPipelineState.h"
%include "medPipelineWorkingState.h"

//%qVariantMapSetter(setValue, medPipelineWorkingState, setValue)
//%qVariantMapGetter(getValueFromQVariant, medPipelineWorkingState, getValue)
//%qVariantMapGetter(getStepOption, medPipelineWorkingState, getStepOption)
//%qVariantMapSetter(setResultValue, medPipelineWorkingState, setResultValue)
//%qVariantMapGetter(getResultValue, medPipelineWorkingState, getResultValue)

/* This code permits read/write access to state values as if they were
 * attributes. Instead of writing state.setValue('foo', 2) you simply write
 * state.foo = 2
 */
%extend med::pipeline::PipelineWorkingState
{
    %pythoncode
    {
        def __getattr__(self, name):
            if self.isMedData(name):
                return self.getValue(name)
            else:
                return self.getValueFromQVariant(name)

        def __setattr__(self, name, value):
          self.setValue(name, value)
    }
}
