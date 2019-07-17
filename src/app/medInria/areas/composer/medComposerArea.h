/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkComposer/dtkComposerWriter.h>

#include <QtWidgets>

class dtkComposerSceneNode;

class medComposerAreaPrivate;

class medComposerArea : public QWidget
{
    Q_OBJECT

public:
     medComposerArea(QWidget *parent = 0);
    ~medComposerArea(void);

public slots:
    bool compositionOpen(void);
    bool compositionOpen(const QString& file);
    bool compositionSave(void);
    bool compositionSaveAs(void);
    bool compositionSaveAs(const QString& file, dtkComposerWriter::Type type = dtkComposerWriter::Ascii);
    bool compositionInsert(void);
    bool compositionInsert(const QString& file);

protected slots:
    void switchToCompo(void);
    void switchToDstrb(void);
    void switchToDebug(void);
    void switchToView(void);

protected slots:
    void showControls(void);

protected slots:
    void onComposerNodeFlagged(dtkComposerSceneNode *);

private:
    medComposerAreaPrivate *d;
};
