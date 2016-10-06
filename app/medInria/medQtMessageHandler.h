/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2014. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <QtGui>

class medQtMessageHandler: public QObject
{
    Q_OBJECT
public :
    medQtMessageHandler();
    static medQtMessageHandler & instance();
    static void msgHandler(QtMsgType type, const char *msg);

signals :
    void newQtMsg(QtMsgType type, const char *msg);

};
