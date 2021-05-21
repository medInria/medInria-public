#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreLegacyExport.h>
#include <medToolBox.h>

class medBrowserPacsNodesToolBoxPrivate;

class MEDCORELEGACY_EXPORT medBrowserPacsNodesToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserPacsNodesToolBox(QWidget *parent = nullptr);
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
