#include "medAbstractAnnotationRepresentation.h"

#include <medCore/medAbstractView.h>

#include "medAnnotationData.h"

#include <dtkCore/dtkSmartPointer.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QVector2D>
#include <QVector3D>


class medAbstractAnnotationRepresentation::Impl
{
public:
    dtkSmartPointer<const medAnnotationData> m_data;
    medAbstractView * view;
};

medAbstractAnnotationRepresentation::medAbstractAnnotationRepresentation( dtkAbstractData * parent )
    : BaseClass( parent ), 
    d( new Impl )
{
}

medAbstractAnnotationRepresentation::~medAbstractAnnotationRepresentation()
{
    delete d;
    d = NULL;
}


medAbstractView * medAbstractAnnotationRepresentation::view() const
{
    return d->view;
}

void medAbstractAnnotationRepresentation::setAnnotationData( const medAnnotationData * annotationData )
{
    if ( d->m_data ) {
        disconnect( d->m_data, SIGNAL(dataModified()), this, SLOT(onDataModified()) );
    }
    d->m_data = annotationData;
    if ( d->m_data ) {
        connect( d->m_data, SIGNAL(dataModified()), this, SLOT(onDataModified()) );
    }
}

const medAnnotationData * medAbstractAnnotationRepresentation::annotationData() const
{
    return d->m_data;
}




void medAbstractAnnotationRepresentation::onDataModified()
{

}



