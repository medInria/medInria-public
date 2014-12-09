/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QWidget>
#include <QtCore>

#include <medDataIndex.h>

#include <medCoreExport.h>

class medToolBox;
class medDataIndex;
class medAbstractData;

/**
 * @class medAbstractDataSource
 * @brief Abstract base class for dynamic data sources, e.g. plugins that act as database clients
 * This class defines access methods to the following widgets: a mainViewWidget,
 * a source selection widget and several ToolBoxes.
 * All dynamic data source implementation should derive from this class.
 **/
class MEDCORE_EXPORT medAbstractDataSource : public QObject
{
    Q_OBJECT

public:
    medAbstractDataSource(QWidget *parent = 0);
    ~medAbstractDataSource();

    /**
    * Returns the main view widget
    * This widget is used to navigate within the data, e.g. a qTreeWidget
    */
    virtual QWidget *mainViewWidget() = 0;

    virtual QWidget* dialogWidget() {return NULL;}

    // TODO consider if it should be virtual.
    virtual QList<medDataIndex> dataSelected() {return QList<medDataIndex>();}

    /**
    * Returns the source selector widget
    * A widget that let's the user choose between different data locations
    */
    virtual QWidget *sourceSelectorWidget() = 0;

    /** Returns the tab name for the plugin using the data source*/
    virtual QString tabName() = 0;

    /** Returns all ToolBoxes owned by the source data plugin*/
    virtual QList<medToolBox*> getToolBoxes() = 0;

    /** Returns a short description of the data source */
    virtual QString description() const = 0;

signals:
    void openRequest(const medDataIndex &index);
};


