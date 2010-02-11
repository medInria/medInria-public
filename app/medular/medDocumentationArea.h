/* medDocumentationArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:15 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb 11 10:46:10 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDOCUMENTATIONAREA_H
#define MEDDOCUMENTATIONAREA_H

#include <QtCore>
#include <QtGui>

class medDocumentationAreaPrivate;

class medDocumentationArea : public QWidget
{
    Q_OBJECT

public:
     medDocumentationArea(QWidget *parent = 0);
    ~medDocumentationArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

protected slots:
    void onRegisterClicked(void);
    void onUnregisterClicked(void);
    void onUrlChanged(const QUrl& url);
    void onLinksActivated(const QMap<QString, QUrl>& urls, const QString& keyword);
    void onAddressFocusTriggered(void);
    void onSearchFocusTriggered(void);

private:
    medDocumentationAreaPrivate *d;
};

#endif // MEDDOCUMENTATIONAREA_H
