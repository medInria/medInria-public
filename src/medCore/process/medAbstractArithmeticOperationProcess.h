/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>
#include <medAbstractProcessPlugin.h>

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

class medAbstractSubstractimageProcess: public medAbstractArithmeticOperationProcess
{

};

class medAbstractArithmeticOperationProcessPlugin: public medAsbtractProcessPlugin
{
public:
    medAbstractArithmeticOperationProcessPlugin(QObject *parent = NULL)
    {
        this->addTags(QStringList() << "arithmetic" << "operation");
    }
};

class medAbstractSubstractimageProcessPlugin: public medAbstractArithmeticOperationProcessPlugin
{
public:
    medAbstractSubstractimageProcessPlugin(QObject *parent = NULL)
    {
        this->addTags(QStringList() << "substraction" << "minus");
    }

    medAbstractSubstractimageProcess *process() const = 0;
};
