/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

class medVtkView;

class medVtkViewSignalsEmitter : QObject
{
    Q_OBJECT

public:
    medVtkViewSignalsEmitter();

    void setView (medVtkView *view);

    void emitSliceChanged(int);
    void emitWindowLevelChanged(double window, double level);

signals:
    void orientationChanged();
    void sliceChanged(int);
    void windowLevelChanged(double window, double level);

private:
    medVtkView *m_view;


};


