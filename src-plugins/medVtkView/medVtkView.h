#pragma once

#include <medAbstractImageView.h>

class medVtkViewPrivate;
class medAbstractData;
class medAbstractDataImage;


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

    virtual QWidget *receiverWidget();

    virtual QWidget* toolBar();
    virtual QWidget* toolBox();

    virtual medAbstractImageViewInteractor* primaryInteractor(medAbstractData* data) ;
    virtual QList<medAbstractExtraInteractor*> extraInteractor(medAbstractData* data);
    virtual medAbstractImageViewInteractor* primaryInteractor(unsigned int layer);
    virtual QList<medAbstractExtraInteractor*> extraInteractor(unsigned int layer);

    virtual medAbstractImageViewNavigator* primaryNavigator();
    virtual QList<medAbstractExtraNavigator*> extraNavigator();

    virtual QPointF mapWorldToDisplayCoordinates( const QVector3D & worldVec );
    virtual QVector3D mapDisplayToWorldCoordinates( const QPointF & scenePoint );
    virtual QVector3D viewCenter();
    virtual QVector3D viewPlaneNormal();
    virtual QVector3D viewUp();
    virtual bool is2D();
    virtual qreal sliceThickness();
    virtual qreal scale();

protected:
    virtual void removeInteractors(medAbstractData *data);
    virtual void retreiveNavigators();
    virtual void retreiveInteractors(medAbstractData *data);


protected slots:



private:
    medVtkViewPrivate *d;
};

