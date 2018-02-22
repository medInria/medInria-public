/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>

#include <medViewFactory.h>

// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1::%1(medAbstractView* parent) : medAbstractLayeredViewInteractor(parent), d(new %1Private)
{
    
}

%1::~%1()
{
    
}

QString %1::identifier() const
{
    return "%1";
}

QString %1::description() const
{
    return "%1";
}

QStringList  %1::handled(void) const
{
    return %1::dataHandled();
}

QStringList  %1::dataHandled(void)
{
    QStringList d = QStringList() << "TODO: add your supported data identifier";
    return  d;
}

bool %1::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<%1>("%1", QStringList () << "TODO: view identifier"
                                           << %1::dataHandled());
}

QList<medAbstractParameter*> %1::linkableParameters()
{
    // TODO: return the parameters that you want to be linkable
    return QList<medAbstractParameter*>();
}

QList<medBoolParameter*> %1::mouseInteractionParameters()
{
    // TODO: return the parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

QWidget* %1::buildToolBoxWidget()
{
    // TODO: construct and return the widget displayed in the
    // view settings toolBox when the container of the parent view is single selected.
    return new QWidget;
}

QWidget* %1::buildToolBarWidget()
{
    // TODO: construct and return the widget displayed in the
    // toolbar of the container where the parent view is displayed.
    return new QWidget;
}

QWidget* %1::buildLayerWidget()
{
    // TODO: construct and return the widget displayed in the
    // layer toolbox when the container of the parent view is selected.
    return new QWidget;
}

void %1::setUpViewForThumbnail()
{
    // TODO: Implement it to set up your view correctly before generating thumbnail
}

void %1::removeData()
{
    // TODO: remove the data from the view
}

void %1::setVisibility(bool)
{
    // TODO: show/hide data in the view
}
