/*
    Copyright (c) 2011,  Alexandre Abadie <Alexandre.Abadie@irisa.fr>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Alexandre <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Alexandre <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#pragma once

#include <QtGui>

class medHomepageAreaPrivate;

class medHomepageArea : public QWidget
{
Q_OBJECT
public:
    medHomepageArea(QWidget * parent = 0);
    virtual ~medHomepageArea();

    void initPage();

    QParallelAnimationGroup * getAnimation();

    void resizeEvent( QResizeEvent * event );

public slots:
    void onShowBrowser();
    void onShowWorkspace(QString workspace);
    void onShowSettings();
    void onShowAbout();
    void onShowPlugin();
    void onShowHelp();
    void onShowInfo();

signals:
    /**
     * @deprecated Doesn't use the medMainWindow's slots anymore.
     */
    void showSettings();
    /**
     * @deprecated Doesn't use the medMainWindow's slots anymore.
     */
    void showAbout();
    /**
     * @deprecated Doesn't use the medMainWindow's slots anymore.
     */
    void showHelp();

    void showBrowser();
    void showViewer();
    void showWorkspace(QString workspace);

private:
    medHomepageAreaPrivate * d;
};


