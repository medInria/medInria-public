/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QUndoStack>

class medPaintCommandStack : public QUndoStack
{
    Q_OBJECT

public:
    medPaintCommandStack(QObject *parent = 0);

public slots:
    virtual void redoAllUntilIndex(int index);

private:
    int currentIndex;
};

