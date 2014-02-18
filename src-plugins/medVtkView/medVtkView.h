#pragma once

#include <medAbstractImageView.h>

class medVtkViewPrivate;
class medAbstractData;
class medAbstractImageData;


class medVtkView : public medAbstractImageView
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
    virtual QWidget* toolBar();
    virtual QWidget* toolBox();

    virtual QList<medAbstractParameter*> viewParameters();

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
    void setSLicingInteractionStyle(bool slicing);

    void buildToolBox();
    void buildMouseInteracToolBox();

    void addLayerItem(int layer);
    void removeLayerItem(int layer);

    void removeSelectedLayer();

    void removeLayerData(int layer);

    void updateInteractorsWidget();

    void removeInternToolBox();
    void removeInternToolBar();
    void removeInternView();




private:
    medVtkViewPrivate *d;
};

