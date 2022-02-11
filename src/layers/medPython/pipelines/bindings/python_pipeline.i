%rename(Pipeline) med::pipeline::PythonPipeline;

// Allow PythonPipeline to be derived in Python
%feature("director") med::pipeline::PythonPipeline;

%{
#include "medPythonPipeline.h"
%}

%include "medAbstractPipeline.h"
%include "medPythonPipeline.h"
  
%extend med::pipeline::PythonPipeline
{
    %pythoncode
    {
        def getPythonClass(self):
            return self.__class__

        def getTitle(self):
            return self.title

        def getShortTitle(self):
            return self.shortTitle

        def getInitialDataIdsAndNames(self):
            return self.initialData

        def getMeshOrder(self):
            return self.meshOrder if hasattr(self, 'meshOrder') else ()

        def createSteps(self):
            if isinstance(self.steps, list):
                steps = self.steps
            else:
                steps = [self.steps]
            return steps

        def startStep(self, name, state):
            name = "start" + name + "Step"
            try:
                if hasattr(self, name):
                    getattr(self, name)(state)
            except Exception as e:
                state.reject("Python script error: " + e.__str__())
        
        def endStep(self, name, state):
            name = "end" + name + "Step"
            try:
                if hasattr(self, name):
                    getattr(self, name)(state)
            except Exception as e:
                state.reject("Python script error: " + e.__str__())
    }
}

%pythoncode
{
    displayResultsStep = AbstractPipeline.createDelegateDisplayResultsStep
    customDisplayResultsStep = AbstractPipeline.createDisplayResultsStep
}

