// Version: $Id$
// 
// 

// Commentary: 
// 
// 

// Change Log:
// 
// 

// Code:

#include "medComposerFactoryExtension.h"

//include nodes
#include "medReaderNodeBase.h"
#include "medWriterNodeBase.h"
#include "medAddFilterNode.h"
#include "medCloseFilterNode.h"
#include "medDilateFilterNode.h"
#include "medDivideFilterNode.h"
#include "medErodeFilterNode.h"
#include "medInvertFilterNode.h"
#include "medMedianFilterNode.h"
#include "medMultiplyFilterNode.h"
#include "medNormalizeFilterNode.h"
#include "medOpenFilterNode.h"
#include "medReaderNodeBase.h"
#include "medShrinkFilterNode.h"
#include "medSubstractFilterNode.h"
#include "medWindowingFilterNode.h"
#include "medGaussianFilterNode.h"


// ///////////////////////////////////////////////////////////////////
// 
// ///////////////////////////////////////////////////////////////////

medComposerFactoryExtension::medComposerFactoryExtension(void) : dtkComposerNodeFactoryExtension()
{

}

medComposerFactoryExtension::~medComposerFactoryExtension(void)
{

}

void medComposerFactoryExtension::extend(dtkComposerNodeFactory *factory)
{//record the new nodes

    factory->record(":json/medGenericReaderNode.json", dtkComposerNodeCreator< medGenericReaderNode>);
    factory->record(":json/medImageReaderNode.json"  , dtkComposerNodeCreator< medImageReaderNode  >);
    factory->record(":json/medMeshReaderNode.json"   , dtkComposerNodeCreator< medMeshReaderNode   >);
    factory->record(":json/medGenericWriterNode.json"   , dtkComposerNodeCreator< medWriterNodeBase   >);

    factory->record(":json/medAddFilterNode.json", dtkComposerNodeCreator< medAddFilterNode >);
    factory->record(":json/medCloseFilterNode.json", dtkComposerNodeCreator< medCloseFilterNode >);
    factory->record(":json/medDilateFilterNode.json", dtkComposerNodeCreator< medDilateFilterNode >);
    factory->record(":json/medDivideFilterNode.json", dtkComposerNodeCreator< medDivideFilterNode >);
    factory->record(":json/medErodeFilterNode.json", dtkComposerNodeCreator< medErodeFilterNode >);
    factory->record(":json/medInvertFilterNode.json", dtkComposerNodeCreator< medInvertFilterNode >);
    factory->record(":json/medMedianFilterNode.json", dtkComposerNodeCreator< medMedianFilterNode >);
    factory->record(":json/medMultiplyFilterNode.json", dtkComposerNodeCreator< medMultiplyFilterNode >);
    factory->record(":json/medNormalizeFilterNode.json", dtkComposerNodeCreator< medNormalizeFilterNode >);
    factory->record(":json/medOpenFilterNode.json", dtkComposerNodeCreator< medOpenFilterNode >);
    factory->record(":json/medShrinkFilterNode.json", dtkComposerNodeCreator< medShrinkFilterNode >);
    factory->record(":json/medSubstractFilterNode.json", dtkComposerNodeCreator< medSubstractFilterNode >);
    factory->record(":json/medWindowingFilterNode.json", dtkComposerNodeCreator< medWindowingFilterNode >);

    factory->record(":json/medGaussianFilterNode.json"   , dtkComposerNodeCreator< medGaussianFilterNode >);

}

// 
// medComposerFactoryExtension.cpp ends here
