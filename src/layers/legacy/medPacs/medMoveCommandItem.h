/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

struct medMoveCommandItem
{
    int group;
    int elem;
    QString query;
    QString sourceTitle;
    QString sourceIp;
    int sourcePort;
    QString targetTitle;
    QString targetIp;
    int targetPort;

    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(medMoveCommandItem);


