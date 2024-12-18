#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QEventLoop>
#include <QString>
#include <QStringList>

enum asyncRequestType { getRqstType = 1, addRqstType = 2 };
struct asyncRequest
{
    asyncRequest();
    asyncRequest(const asyncRequest &rqst);
    asyncRequest & operator=(asyncRequest const & rqst) { type = rqst.type; tmpId = rqst.tmpId; uri = rqst.uri; stampTimeout = rqst.stampTimeout;  needMedAbstractConversion = rqst.needMedAbstractConversion; noLongerValid = rqst.noLongerValid; return *this; }
    asyncRequestType type;
    QString tmpId;
    QStringList uri;

    QString dataName;

    qint64 stampTimeout;
    QEventLoop waiter;
    bool needMedAbstractConversion;
    bool noLongerValid;

public:
    friend bool operator< (asyncRequest const & a, asyncRequest const & b);
    friend bool operator> (asyncRequest const & a, asyncRequest const & b);
    friend bool operator==(asyncRequest const & a, asyncRequest const & b);
};

inline bool operator< (asyncRequest const & a, asyncRequest const & b) { return a.uri < b.uri; };
inline bool operator> (asyncRequest const & a, asyncRequest const & b) { return a.uri > b.uri; };
inline bool operator==(asyncRequest const & a, asyncRequest const & b) { return a.uri == b.uri && a.tmpId == b.tmpId && a.type == b.type; };

