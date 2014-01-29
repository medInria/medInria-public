#include "medAbstractImageView.h"

#include <medAbstractData.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>


void medAbstractImageView::moveToSliceAtPosition (const QVector3D &position)
{   
    foreach (medAbstractData *data, this->data())
    {
        this->primaryIntercator(data)->moveToSliceAtPosition(position);
    }
}

QVector3D medAbstractImageView::positionBeingViewed(void)
{
    return this->primaryNavigator()->positionBeingViewed();
}

void medAbstractImageView::setWindowLevel (medAbstractData *data, double &window, double &level)
{
    this->primaryIntercator(data)->setWindowLevel(window, level);
}

void medAbstractImageView::windowLevel(medAbstractData *data, double &window, double &level)
{
    this->primaryIntercator(data)->windowLevel(window, level);
}

void medAbstractImageView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{    
    this->primaryNavigator()->setCamera(position, viewup, focal, parallelScale);
}

void medAbstractImageView::camera (QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale)
{
    this->primaryNavigator()->camera(position, viewup, focal, parallelScale);
}

void medAbstractImageView::setOpacity(unsigned int layer, double opacity)
{
    this->primaryIntercator(layer)->setOpacity(opacity);
}

void medAbstractImageView::setOpacity(medAbstractData *data, double opacity)
{
    this->primaryIntercator(data)->setOpacity(opacity);
}

double medAbstractImageView::opacity(unsigned int layer)
{
    this->primaryIntercator(layer)->opacity();
}

double medAbstractImageView::opacity(medAbstractData *data)
{
    this->primaryIntercator(data)->opacity();
}

