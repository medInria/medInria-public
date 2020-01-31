/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <medPacsExport.h>

class MEDPACS_EXPORT medAbstractPacsStoreScp : public QObject
{
    Q_OBJECT

public:
    medAbstractPacsStoreScp() = default;
    ~medAbstractPacsStoreScp() override = default;

    virtual  int start(const char* ourTitle, const char* ourIP, unsigned int ourPort);

    virtual  void stop();

    virtual  bool setStorageDirectory(const char* directory);

signals:
    void endOfStudy(QString);
};
