/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QUndoCommand>

class medClearMaskCommand : public QUndoCommand
{
public:
    medClearMaskCommand(QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return 12345; }
};
