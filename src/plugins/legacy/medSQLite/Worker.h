#pragma once

#include <QArgument>

/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <QObject>
#include <medAbstractSource.h>

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(){};

public slots:
    void sendSignalWithDelay(const int iRequestId);

signals:
    void sendProgress(int po_iRequest, int status);
};


