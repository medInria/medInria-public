/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2014. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medQtMessageHandler.h"

medQtMessageHandler::medQtMessageHandler() : QObject()
{
    // important
    qRegisterMetaType<QtMsgType>("QtMsgType");
}

medQtMessageHandler &medQtMessageHandler::instance()
{
    static medQtMessageHandler obj;
    return obj;
}

void medQtMessageHandler::msgHandler(QtMsgType type, const char *msg)
{
    emit instance().newMsg(type, msg);
}