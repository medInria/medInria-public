/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractArithmeticOperationProcess.h>

#include <medAbstractImageData.h>

class medAbstractArithmeticOperationProcessPrivate
{
public:
    medAbstractImageData *input1;
    medAbstractImageData *input2;
    medAbstractImageData *output;
};

medAbstractArithmeticOperationProcess::medAbstractArithmeticOperationProcess(QObject *parent): medAbstractProcess(parent),
    d(new medAbstractArithmeticOperationProcessPrivate)
{
    d->input1 = NULL;
    d->input2 = NULL;
    d->output = NULL;
}

medAbstractArithmeticOperationProcess::~medAbstractArithmeticOperationProcess()
{
    delete d;
}

void medAbstractArithmeticOperationProcess::setInput1(medAbstractImageData *data)
{
    d->input1 = data;
}

medAbstractImageData* medAbstractArithmeticOperationProcess::input1() const
{
    return d->input1;
}


void medAbstractArithmeticOperationProcess::setInput2(medAbstractImageData *data)
{
    d->input2 = data;
}

medAbstractImageData* medAbstractArithmeticOperationProcess::input2() const
{
    return d->input2;
}

medAbstractImageData* medAbstractArithmeticOperationProcess::output() const
{
    return d->output;
}



namespace process
{
    namespace pluginManager
    {
        void initialize(const QString& path)
        {
            QString realpath=path;
            if (path.isEmpty())
            {
//                dtkLinearAlgebraSparseSettings linear_algebra_sparse_settings;
//                linear_algebra_sparse_settings.beginGroup("linear-algebra-sparse");
//                realpath = linear_algebra_sparse_settings.value("plugins").toString();
//                linear_algebra_sparse_settings.endGroup();
            }
            process::substractImage::initialize(realpath);
        }
    }

    namespace substractImage
    {

        namespace _private
        {
            medAbstractSubstractimageProcessPluginManager manager;
            medAbstractSubstractimageProcessPluginFactory factory;
        }

        void initialize(const QString& path)
        {
            pluginManager().initialize(path);
        }

        medAbstractSubstractimageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractSubstractimageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
}
