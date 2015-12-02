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

#include "medCommandLineExecutorFactory.h"
#include "morphomath/medOpeningOperationCommandLineExecutor.h"
#include "morphomath/medClosingOperationCommandLineExecutor.h"
#include "morphomath/medDilateOperationCommandLineExecutor.h"
#include "morphomath/medErodeOperationCommandLineExecutor.h"

#include "arithmetic/medArithmeticCommandLineExecutor.h"
#include "arithmetic/medAddImageCommandLineExecutor.h"
#include "arithmetic/medSubstractImageCommandLineExecutor.h"
#include "arithmetic/medDivideImageCommandLineExecutor.h"
#include "arithmetic/medMultiplyImageCommandLineExecutor.h"

medCommandLineExecutorFactory* medCommandLineExecutorFactory::s_instance=NULL;

medCommandLineExecutorFactory* medCommandLineExecutorFactory::instance()
{
    if(!s_instance)
        s_instance=new medCommandLineExecutorFactory();
    return s_instance;
}

medCommandLineExecutorFactory::medCommandLineExecutorFactory(void)
{
    record("opening",new medOpeningOperationCommandLineExecutor());
    record("closing",new medClosingOperationCommandLineExecutor());
    record("erode",new medErodeOperationCommandLineExecutor());
    record("dilate",new medDilateOperationCommandLineExecutor());

    record("subtract",new medSubstractImageCommandLineExecutor());
    record("add",new medAddImageCommandLineExecutor());
    record("multiply",new medMultiplyImageCommandLineExecutor());
    record("divide",new medDivideImageCommandLineExecutor());

}

medCommandLineExecutorFactory::~medCommandLineExecutorFactory(void)
{

}

// /////////////////////////////////////////////////////////////////
// Type creator registration
// /////////////////////////////////////////////////////////////////

void medCommandLineExecutorFactory::record(const QString& key, medCommandLineExecutor* widget)
{
    if (this->executors.contains(key)) {
        qDebug() << Q_FUNC_INFO << "Factory already contains key" << key << ". Nothing is done";
        return;
    }

    this->executors.insert(key, widget);
}

// /////////////////////////////////////////////////////////////////
// Type creator invokation
// /////////////////////////////////////////////////////////////////

medCommandLineExecutor *medCommandLineExecutorFactory::get(const QString& key) const
{
    if(!this->executors.contains(key))
        return NULL;

    return this->executors.value(key);
}

// /////////////////////////////////////////////////////////////////
// Type creator inspection
// /////////////////////////////////////////////////////////////////

QStringList medCommandLineExecutorFactory::keys(void) const
{
    return this->executors.keys();
}
