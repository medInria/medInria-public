/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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

%1::%1(QObject* parent) : medAbstractLayeredView(parent), d(new %1Private)
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

bool %1::registered()
{
    return medViewFactory::instance()->registerView<%1>(QString("%1"), QStringList()<<"TODO: add the supported data identifier here");
}

QWidget* %1::viewWidget()
{
    //TODO: Return the widget containing the actual view object.
    return new QWidget;
}

medViewBackend* %1::backend() const
{
    //TODO: Return the backend of the view
    return new medViewBackend;

}

void %1::reset()
{
    //TODO: Implement your reset method
}

void %1::render()
{
     //TODO: Implement your render method
}

QImage %1::buildThumbnail(const QSize &size)
{
    //TODO: Build and return a thumbnail of the view of the specified size.
    return QImage();
}
