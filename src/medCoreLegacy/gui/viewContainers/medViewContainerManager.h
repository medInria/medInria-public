/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include <medCoreExport.h>

struct QUuid;
class medViewContainer;

class medViewContainerManagerPrivate;

class MEDCORE_EXPORT medViewContainerManager : public QObject
{
    Q_OBJECT

public:
    static medViewContainerManager *instance();

    medViewContainer* container(QUuid uuid) const;

public slots:
    void registerNewContainer(medViewContainer* newContainer);
    void unregisterContainer(medViewContainer *container);

protected:
     medViewContainerManager();
    ~medViewContainerManager();

protected:
    static medViewContainerManager *s_instance;

signals:
    void containerAboutToBeDestroyed(QUuid uuid);

private:
    medViewContainerManagerPrivate *d;
};


