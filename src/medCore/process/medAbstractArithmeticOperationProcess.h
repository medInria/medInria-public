/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkCorePluginManager.h>
#include <dtkCore/dtkCorePlugin.h>

#include <medAbstractProcess.h>
#include <medAbstractProcessPresenter.h>

class medAbstractImageData;

class medAbstractArithmeticOperationProcessPrivate;

class medAbstractArithmeticOperationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractArithmeticOperationProcess(QObject *parent = NULL);
    virtual ~medAbstractArithmeticOperationProcess();

    void setInput1(medAbstractImageData* data);
    medAbstractImageData* input1() const;

    void setInput2(medAbstractImageData* data);
    medAbstractImageData* input2() const;

    medAbstractImageData* output() const;

private:
    medAbstractArithmeticOperationProcessPrivate* d;

};


class medAbstractArithmeticOperationProcessPresenter: public medAsbtractProcessPresenter
{
public:
    medAbstractArithmeticOperationProcessPresenter(QObject *parent = NULL)
    {
        this->addTags(QStringList() << "arithmetic" << "operation");
    }
};


class medAbstractSubstractimageProcess: public medAbstractArithmeticOperationProcess
{
public:
    medAbstractSubstractimageProcess(QObject* parent = NULL): medAbstractArithmeticOperationProcess(parent) {}
};

class medAbstractSubstractimageProcessPresenter: public medAbstractArithmeticOperationProcessPresenter
{
public:
    medAbstractSubstractimageProcessPresenter(QObject *parent = NULL)
    {
        this->addTags(QStringList() << "substraction" << "minus");
    }

    medAbstractSubstractimageProcess *process() const = 0;
};


class medAbstractSubstractimageProcessPlugin : public QObject
{
    Q_OBJECT

 public:
             medAbstractSubstractimageProcessPlugin(void) {}
    virtual ~medAbstractSubstractimageProcessPlugin(void) {}

 public:
    virtual void   initialize(void) = 0;
    virtual void uninitialize(void) = 0;
};


Q_DECLARE_INTERFACE(medAbstractSubstractimageProcessPlugin, DTK_DECLARE_PLUGIN_INTERFACE(medAbstractSubstractimageProcessPresenter))

class medAbstractSubstractimageProcessPluginFactory : public dtkCorePluginFactory<medAbstractSubstractimageProcessPresenter>
{
};

class medAbstractSubstractimageProcessPluginManager : public dtkCorePluginManager<medAbstractSubstractimageProcessPlugin>
{
};

namespace process
{
    namespace pluginManager
    {
        void initialize(const QString& path = QString());
    }
    namespace substractImage
    {
        void initialize(const QString& path);
        medAbstractSubstractimageProcessPluginManager& pluginManager(void);
        medAbstractSubstractimageProcessPluginFactory& pluginFactory(void);
    }
}
