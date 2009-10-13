/* medViewerAreaViewContainer.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Sep 22 15:57:54 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERAREAVIEWCONTAINER_H
#define MEDVIEWERAREAVIEWCONTAINER_H

#include <QtGui/QWidget>

class QGridLayout;

class medViewerAreaViewContainer : public QWidget
{

    Q_OBJECT
      
public:
     medViewerAreaViewContainer(QWidget *parent = 0);
    ~medViewerAreaViewContainer(void);

    medViewerAreaViewContainer *current(void);

    void split(int rows, int cols);

    void addWidget (QWidget *widget, int row, int col);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QGridLayout *m_layout;

    static medViewerAreaViewContainer *s_current;
};

#endif // MEDVIEWERAREAVIEWCONTAINER_H
