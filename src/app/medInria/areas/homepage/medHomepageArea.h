/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#pragma once

#include <QtGui>
#include <QtWidgets>

class medHomepageAreaPrivate;

class medHomepageArea : public QWidget
{
Q_OBJECT
public:
    medHomepageArea(QWidget * parent = nullptr);
    virtual ~medHomepageArea();

    /**
     * @brief Display the workspace buttons on homepage
     */
    void initPage();

    void resizeEvent( QResizeEvent * event );

    QPixmap getApplicationLogoPixmap();

public slots:
    void onShowBrowser();

    /**
     * @brief Switch the application to the asked workspace
     */
    void onShowWorkspace(QString workspace);

signals:
    void showBrowser();
    void showWorkspace(QString workspace);

private:
    medHomepageAreaPrivate * d;
};
