/* medComposerArea.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Aug  3 17:38:47 2009 (+0200)
 * Version: $Id: 736dba4f5968cf2d506d986ea3c6cc7431d3c49f $
 * Last-Updated: Tue Jun 25 09:44:56 2013 (+0200)
 *           By: Selim Kraria
 *     Update #: 70
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef medComposerArea_H
#define medComposerArea_H

#include <dtkComposer/dtkComposerWriter.h>

#include <QtWidgets>

class dtkComposerSceneNode;

class medComposerAreaPrivate;

class medComposerArea : public QMainWindow
{
    Q_OBJECT

public:
     medComposerArea(QWidget *parent = 0);
    ~medComposerArea(void);

    void readSettings(void);
    void writeSettings(void);

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

protected:
    void closeEvent(QCloseEvent *event);

private:
    medComposerAreaPrivate *d;
};

#endif
