/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractData.h>

#include <medBoutiquesPluginExport.h>

class medBoutiquesPrivate;
class dtkAbstractData;

class MEDBOUTIQUESPLUGIN_EXPORT medBoutiques : public dtkAbstractProcess
{
    Q_OBJECT

public:
             medBoutiques();
    virtual ~medBoutiques();

    virtual QString description() const;

    static bool registered();

public slots:

    void setInput(dtkAbstractData *data);
    void setParameter(double  data, int channel);
    int update();
    dtkAbstractData *output();

    // bool write(QString file);

private:
    medBoutiquesPrivate *d;
};

dtkAbstractProcess *createmedBoutiques();


