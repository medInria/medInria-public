/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>

class medAbstractDataSource;

class medBrowserAreaPrivate;
class medBrowserArea : public QWidget
{
    Q_OBJECT

public:
     medBrowserArea(QWidget *parent = 0);
    ~medBrowserArea();

public slots:
    void changeSource(int index);


protected:
    void addDataSource(medAbstractDataSource* dataSource);

private:
    medBrowserAreaPrivate *d;
};

class medDatabasePreview;
class medBrowserPannelFramePrivate;
class medBrowserPannelFrame: public QFrame
{
    Q_OBJECT

public:
    medBrowserPannelFrame(QWidget *parent = NULL);
    virtual ~medBrowserPannelFrame();

public:
    void setWidgetAndPreview(QWidget *widget, QWidget *preview);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    medBrowserPannelFramePrivate *d;
};
