%apply medAbstractData* { med::pipeline::PipelineInstance* };
%apply dtkSmartPointer<medAbstractData> { dtkSmartPointer<med::pipeline::PipelineInstance> };
%feature("novaluewrapper") dtkSmartPointer<med::pipeline::PipelineInstance>;
%template() dtkSmartPointer<med::pipeline::PipelineInstance>;
