///*=========================================================================

// medInria

// Copyright (c) INRIA 2013 - 2014. All rights reserved.
// See LICENSE.txt for details.

//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.

//=========================================================================*/

//#include <medTestWorkspace.h>

//#include <medTabbedViewContainers.h>

//#include <medCore.h>
//#include <medAbstractAddImageProcess.h>

//#include <medToolBox.h>

//class medTestWorkspacePrivate
//{
//public:
//  medAbstractAddImageProcess *process;

//};

//medTestWorkspace::medTestWorkspace(QWidget *parent): medAbstractWorkspaceLegacy (parent), d(new medTestWorkspacePrivate)
//{
//    for(QString const& key : medCore::arithmeticalOperation::addImage::pluginFactory().keys())
//        d->process = medCore::morphomathOperation::erodeImage::pluginFactory().create(key);

//    qDebug() << "CREATE PROCESS:" << d->process->caption() << d->process->description();
//}

//medTestWorkspace::~medTestWorkspace()
//{
//    delete d;
//    d = NULL;
//}

//void medTestWorkspace::setupTabbedViewContainer()
//{

//}

//bool medTestWorkspace::isUsable()
//{
//    return true;
//}
