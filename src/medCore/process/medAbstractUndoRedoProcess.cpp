/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractUndoRedoProcess.h>

#include <medToolBox.h>
#include <medUndoRedoToolBox.h>

class medAbstractUndoRedoProcessPrivate
{
public:
    medAbstractUndoRedoProcessWrapper *wrapper;
    medUndoRedoToolBox *toolbox;
};

////////////////////////////////
//
// medAbstractUndoRedoProcess
//
////////////////////////////////

medAbstractUndoRedoProcess::medAbstractUndoRedoProcess(): d(new medAbstractUndoRedoProcessPrivate)
{
    d->wrapper = new medAbstractUndoRedoProcessWrapper(this);
    d->toolbox = new medUndoRedoToolBox;

    QObject::connect(d->toolbox, SIGNAL(undoRequested()), d->wrapper, SLOT(undo()));
    QObject::connect(d->toolbox, SIGNAL(redoRequested()), d->wrapper, SLOT(redo()));
    QObject::connect(d->toolbox, SIGNAL(resetRequested()), d->wrapper, SLOT(reset()));
}

medAbstractUndoRedoProcess::~medAbstractUndoRedoProcess()
{
    delete d->wrapper;
}

medAbstractUndoRedoProcessWrapper * medAbstractUndoRedoProcess::wrapper() const
{
    return d->wrapper;
}

medUndoRedoToolBox* medAbstractUndoRedoProcess::undoRedoToolBox() const
{
    return d->toolbox;
}

////////////////////////////////
//
// medAbstractUndoRedoProcessWrapper
//
////////////////////////////////

medAbstractUndoRedoProcessWrapper::medAbstractUndoRedoProcessWrapper(medAbstractUndoRedoProcess* parent)
{
    wrappee = parent;
}

medAbstractUndoRedoProcessWrapper::~medAbstractUndoRedoProcessWrapper()
{
}

void medAbstractUndoRedoProcessWrapper::undo()
{
    wrappee->undo();
    emit processUndone();
}

void medAbstractUndoRedoProcessWrapper::redo()
{
    wrappee->redo();
    emit processRedone();
}

void medAbstractUndoRedoProcessWrapper::reset()
{
    wrappee->reset();
    emit undoRedoStackReset();
}
