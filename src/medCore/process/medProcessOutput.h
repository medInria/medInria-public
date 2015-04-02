/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medProcessPort.h>

template <typename T>
class medProcessOutput : public medProcessPort
{

public:
    medProcessOutput(QString name, T* output = NULL);
    virtual ~medProcessOutput();

public:
    T* output() const;
    void setOutput(T* output);

    QObject* content() const;
    bool setContent(QObject* obj);
};

#include <medProcessOutput.txx>
