#include <medVtkViewToolBox.h>

#include <medAbstractData.h>
#include <medToolBoxFactory.h>
#include <medVtkView.h>

#include <medToolBoxHeader.h>
#include <medSettingsManager.h>
#include <medMetaDataKeys.h>

#include <medLayerItemWidget.h>


class medVtkViewToolBoxPrivate
{
public:

    medVtkView                      * vtkView;

};

medVtkViewToolBox::medVtkViewToolBox(QWidget *parent)
    : medToolBox(parent), d(new medVtkViewToolBoxPrivate)
{
    d->vtkView = 0;

    this->header()->hide();
}


medVtkViewToolBox::~medVtkViewToolBox()
{

}

bool medVtkViewToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medVtkViewToolBox>("medVtkViewToolBox","medVtkViewToolBox",
                                                                                "View management toolbox", QStringList() << "view");
}

void medVtkViewToolBox::update(dtkAbstractView * view)
{
    // Hide this toolbox when no views
    if(!view)
    {
        this->hide();
        d->vtkView = 0;
        return;
    }
    else
        this->show();

    // this is not the current view
    if(d->vtkView != qobject_cast<medVtkView*>(view))
    {
        // the privous view is not null
        if( d->vtkView != 0 )
        {
          d->vtkView->toolBox()->hide();
        }
        d->vtkView = qobject_cast<medVtkView*>(view);
        this->addWidget(d->vtkView->toolBox());
        d->vtkView->toolBox()->show();

    }
}





