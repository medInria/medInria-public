/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPaintCommand.h>

#include <medAbstractImageView.h>


class medAbstractPaintCommandPrivate
{
public:
    medAbstractPaintCommand* previousCommand;
    medPaintCommandOptions *option;
};


medAbstractPaintCommand::medAbstractPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent):
    QUndoCommand(parent), d(new medAbstractPaintCommandPrivate)
{
    d->option = options;
}


void medAbstractPaintCommand::undo()
{
    this->unpaint();

    d->option->view->render();
}

void medAbstractPaintCommand::redo()
{
    this->paint();

    d->option->view->render();
}

medPaintCommandOptions* medAbstractPaintCommand::options() const
{
    return d->option;
}

void medAbstractPaintCommand::setPreviousCommand(medAbstractPaintCommand *cmd)
{
    d->previousCommand = cmd;
}

medAbstractPaintCommand * medAbstractPaintCommand::previousCommand() const
{
    return d->previousCommand;
}
