/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageView.h>
#include <medVtkViewPluginExport.h>

class medVtkViewPrivate;
class medAbstractData;
class medAbstractImageData;


class MEDVTKVIEWPLUGIN_EXPORT medVtkView : public medAbstractImageView
{
    Q_OBJECT
    
public:

    medVtkView(QObject* parent = 0);
    virtual ~medVtkView();

    virtual QString  identifier() const;
    static QString  s_identifier();
    static bool registered();
    virtual medViewBackend * backend() const;
    virtual QString description() const;
    virtual QWidget *viewWidget();
    virtual QWidget* toolBarWidget();
    virtual QWidget* navigatorWidget();
    virtual QPointF mapWorldToDisplayCoordinates(const QVector3D & worldVec );
    virtual QVector3D mapDisplayToWorldCoordinates(const QPointF & scenePoint );
    virtual QVector3D viewCenter();
    virtual QVector3D viewPlaneNormal();
    virtual QVector3D viewUp();
    virtual bool is2D();
    virtual qreal sliceThickness();
    virtual qreal scale();

public slots:
    virtual void reset();

private slots:
    void setWindowingInteractionStyle(bool windowing);
    void setZoomInteractionStyle(bool zoom);
    void setSlicingInteractionStyle(bool slicing);
    void displayDataInfo(uint layer);

    void changeCurrentLayer();

    void removeInternNavigatorWidget();
    void removeInternToolBarWidget();
    void removeInternViewWidget();

private:
    virtual QImage buildThumbnail(const QSize &size);

private:
    medVtkViewPrivate *d;
};

