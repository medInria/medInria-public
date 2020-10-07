/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageView.h>

#include <%1PluginExport.h>

class %1Private;
    
class %2PLUGIN_EXPORT %1 : public medAbstractImageView
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

    virtual QPointF mapWorldToDisplayCoordinates( const QVector3D & worldVec );
    virtual QVector3D mapDisplayToWorldCoordinates( const QPointF & scenePoint );
    virtual QVector3D viewCenter();
    virtual QVector3D viewPlaneNormal();
    virtual QVector3D viewUp();
    virtual bool is2D();
    virtual qreal sliceThickness();
    virtual qreal scale();

public slots:
    virtual void reset();
    virtual void render();

private:
    virtual QImage buildThumbnail(const QSize &size);
    
private:
    %1Private *d;
};



