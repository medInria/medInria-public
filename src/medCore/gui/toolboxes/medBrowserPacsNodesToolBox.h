/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medBrowserPacsNodesToolBoxPrivate;

class MEDCORE_EXPORT medBrowserPacsNodesToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsNodesToolBox(QWidget *parent = 0);
    ~medBrowserPacsNodesToolBox();

    void readSettings();

    void writeSettings();

signals:
    void nodesUpdated();
    void echoRequest();

public slots:
    void addNode();
    void remNode();
    void onEchoResponse(QVector<bool>);

private:

    void fillWidget( QList<QVariant> nodes );

    medBrowserPacsNodesToolBoxPrivate *d;
};


