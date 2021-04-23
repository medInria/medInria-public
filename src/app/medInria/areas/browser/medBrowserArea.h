/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <QtWidgets>

class medBrowserAreaPrivate;
class medAbstractDataSource;
class medToolBox;
class medJobItemL;

class medBrowserArea : public QWidget
{
    Q_OBJECT

public:
     medBrowserArea(QWidget *parent = nullptr);
    ~medBrowserArea();

public slots:
    void onSourceIndexChanged(int index);

    /**
    * @brief Adds a medToolBox to the medToolBoxContainer.
    *
    * @param toolbox
    */
    void addToolBox(medToolBox *toolbox);
    
    /**
    * @brief Removes a medToolBox from the medToolBoxContainer.
    *
    * @param toolbox
    */
    void removeToolBox(medToolBox *toolbox);

    /**
    * @brief Change the current tab in the Browser area
    *
    * @param index
    */
    void switchToIndexTab(int index);

protected:
    void setToolBoxesVisible(int index, bool visible);
    void addDataSource(medAbstractDataSource* dataSource);

private:
    medBrowserAreaPrivate *d;
};
