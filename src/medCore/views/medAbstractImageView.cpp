#include <medAbstractImageView.h>

#include <medAbstractData.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>


medAbstractImageView::medAbstractImageView(QObject *parent) : medAbstractLayeredView(parent)
{

}

medAbstractImageView::~medAbstractImageView()
{

}

void medAbstractImageView::moveToSliceAtPosition (const QVector3D &position)
{   
    foreach (medAbstractData *data, this->data())
    {
        medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
        if(!inter)
            break;

        inter->moveToSliceAtPosition(position);
    }
}

void medAbstractImageView::setOrientation(medImageView::Orientation orientation)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setOrientation(orientation);
}

QVector3D medAbstractImageView::positionBeingViewed(void)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return QVector3D(0.0,0.0,0.0);

    return nav->positionBeingViewed();
}

void medAbstractImageView::setDataWindowLevel(medAbstractData *data, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return;

    inter->setWindowLevel(window, level);
}

void medAbstractImageView::setLayerWindowLevel(unsigned int layer, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->setWindowLevel(window, level);
}

void medAbstractImageView::dataWindowLevel(medAbstractData *data, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return;

    inter->windowLevel(window, level);
}

void medAbstractImageView::layerWindowLevel(unsigned int layer, double &window, double &level)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->windowLevel(window, level);
}

void medAbstractImageView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setCamera(position, viewup, focal, parallelScale);
}

void medAbstractImageView::camera (QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->camera(position, viewup, focal, parallelScale);
}

void medAbstractImageView::setOpacity(unsigned int layer, double opacity)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return;

    inter->setOpacity(opacity);
}

void medAbstractImageView::setOpacity(medAbstractData *data, double opacity)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return;

    inter->setOpacity(opacity);
}

double medAbstractImageView::opacity(unsigned int layer)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(layer);
    if(!inter)
        return 1;

    return inter->opacity();
}

double medAbstractImageView::opacity(medAbstractData *data)
{
    medAbstractImageViewInteractor* inter = this->primaryInteractor(data);
    if(!inter)
        return 1;

    return inter->opacity();
}

medImageView::Orientation medAbstractImageView::orientation()
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return medImageView::VIEW_ORIENTATION_AXIAL;

    return nav->orientaion();
}
