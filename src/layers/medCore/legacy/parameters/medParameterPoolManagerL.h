#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore/QObject>
#include <QtCore/QHash>

#include <medCoreExport.h>

#include <memory>

class medParameterPoolManagerLPrivate;

class medParameterPoolManagerLToolBox;
class medAbstractParameterL;
class medParameterPoolL;
class medAbstractView;

class MEDCORE_EXPORT medParameterPoolManagerL : public QObject
{
    Q_OBJECT

public:
    ~medParameterPoolManagerL();

    static medParameterPoolManagerL *instance();

    QList<std::shared_ptr<medParameterPoolL> > pools();
    std::shared_ptr<medParameterPoolL> pool(QString poolId);
    QStringList pools(medAbstractParameterL *param);

public slots:
    void linkParameter(medAbstractParameterL*, QString poolId);
    void unlinkParameter(medAbstractParameterL*);
    void removePool(QString);

protected:
    medParameterPoolManagerL();

    static std::shared_ptr<medParameterPoolManagerL> s_instance;

private:
    const std::unique_ptr<medParameterPoolManagerLPrivate> d;
};

