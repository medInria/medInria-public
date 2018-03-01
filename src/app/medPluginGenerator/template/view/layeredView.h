/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredView.h>

#include <%1PluginExport.h>

class %1Private;
    
class %2PLUGIN_EXPORT %1 : public medAbstractLayeredView
{
    Q_OBJECT
    
public:
    %1(QObject* parent = 0);
    virtual ~%1();

    virtual QString  identifier() const;
    virtual QString description() const;
    
    static bool registered();

    virtual QWidget *viewWidget();

    virtual medViewBackend * backend() const;

public slots:
    virtual void reset();
    virtual void render();

private:
    virtual QImage buildThumbnail(const QSize &size);
    
private:
    %1Private *d;
};



