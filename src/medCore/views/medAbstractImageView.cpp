#include "medAbstractImageView.h"

#include <medAbstractData.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>


void medAbstractImageView::moveToSliceAtPosition (const QVector3D &position)
{   
    foreach (medAbstractData *data, this->data())
    {
        this->primaryInteractor(data)->moveToSliceAtPosition(position);
    }
}

QVector3D medAbstractImageView::positionBeingViewed(void)
{
    return this->primaryNavigator()->positionBeingViewed();
}

void medAbstractImageView::setDataWindowLevel(medAbstractData *data, double &window, double &level)
{
    this->primaryInteractor(data)->setWindowLevel(window, level);
}

void medAbstractImageView::setLayerWindowLevel(unsigned int layer, double &window, double &level)
{
    this->primaryInteractor(layer)->setWindowLevel(window, level);
}

void medAbstractImageView::dataWindowLevel(medAbstractData *data, double &window, double &level)
{
    this->primaryInteractor(data)->windowLevel(window, level);
}

void medAbstractImageView::layerWindowLevel(unsigned int layer, double &window, double &level)
{
    this->primaryInteractor(layer)->windowLevel(window, level);
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
    this->primaryInteractor(layer)->setOpacity(opacity);
}

void medAbstractImageView::setOpacity(medAbstractData *data, double opacity)
{
    this->primaryInteractor(data)->setOpacity(opacity);
}

double medAbstractImageView::opacity(unsigned int layer)
{
    this->primaryInteractor(layer)->opacity();
}

double medAbstractImageView::opacity(medAbstractData *data)
{
    this->primaryInteractor(data)->opacity();
}

