/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "httpResponse.h"

#include <QDebug>
#include <QRegExp>


#define HTTP_SEP "\r\n" // http separation sequence


class httpResponsePrivate
{
public:
    QByteArray payload;
    QList<QNetworkReply::RawHeaderPair> headers;

    QString contentType; // "Content-Type" field value
    QString type; // "type" key value
    bool multi; // multipart response or not

    QByteArray boundary; // limit separating instances in the reply body
    QVector<partChunck> parts; // series instances
};


httpResponse::httpResponse(QNetworkReply &reply)
{
    d = new httpResponsePrivate;
    d->payload = reply.readAll();
    d->headers = reply.rawHeaderPairs();

    init();
}

httpResponse::httpResponse(QByteArray &data)
{
    d = new httpResponsePrivate;
    separateHeaderAndPayload(data, d->payload, d->headers);

    init();
}

httpResponse::httpResponse(QByteArray &payload, QList<QPair<QByteArray, QByteArray>> &headers)
{
    d = new httpResponsePrivate;
    d->payload = payload;
    d->headers = headers;

    init();
}

httpResponse::~httpResponse()
{
}

bool httpResponse::isMutlipart()
{
    return false;
}

QVector<partChunck> httpResponse::getParts()
{
    return d->parts;
}

void httpResponse::init()
{
    d->multi = false;
    // read response main header for information on the response data
    for (auto head : d->headers) 
    {
        if (head.first == "Content-Type")
        {
            auto values = splitHeaderValues(head.second);
            for (int i =0; i< values.size(); ++i)
            {
                if (i == 0)
                {
                    d->multi = values[i].startsWith("multipart");
                    d->contentType = values[i];
                }

                if (values[i].startsWith("boundary="))
                {
                    d->boundary = subSplitHeaderValues(values[i]).toLatin1();
                }
                
                if (values[i].startsWith("type="))
                {
                    d->type = subSplitHeaderValues(values[i]).toLatin1();
                }
            }
        }
    }

    // if multipart, instances are delimited by boundaries
    if (d->multi)
    {
        auto const sep = "--" + d->boundary;
        auto pos = d->payload.indexOf(sep)+sep.size();
        // skip first part who is empty
        QByteArray data = d->payload.mid(pos);
        
        // retrieve all the following parts up to the end of the response data (1 part = 1 file)
        while (! data.isEmpty())
        {
            auto dataSplited = splitByteArray(data, sep);
            addPart(dataSplited.first);

            if(getContentSize(dataSplited.second) != 0)
            {
                data = dataSplited.second;
            }
            else
            {
                data.clear();
            }
        }
    }
    else
    {
        if (d->payload.endsWith(HTTP_SEP))
        {
            d->payload.chop(2);
        }
    }
}

void httpResponse::addPart(QByteArray & part)
{
    partChunck p;
    
    // make a vector containing each of the parts (instances), keeping only the payload
    separateHeaderAndPayload(part, p.payload, p.headers);
    if (p.payload.endsWith(HTTP_SEP))
    {
        p.payload.chop(2);
    }

    d->parts.push_back(p);
}




void httpResponse::separateHeaderAndPayload(QByteArray & data, QByteArray & mainPayload, QList<QNetworkReply::RawHeaderPair> & treatedHeaders)
{
    // separate the header to the payload
    auto headerAndData = splitByteArray(data, HTTP_SEP HTTP_SEP);
    mainPayload = headerAndData.second;
    QString headers = headerAndData.first;

    // go through the values in a header to identify fields and values
    QStringList headerList = headers.split(QRegExp("[" HTTP_SEP "]"), QString::SkipEmptyParts);
    for (auto headerEntry : headerList)
    {
        auto entrySplited = splitByteArray(headerEntry.toLatin1(), ":");
        treatedHeaders.push_back(entrySplited);
    }
}

int httpResponse::getContentSize(QByteArray &data)
{
    QByteArray payload;
    QList<QNetworkReply::RawHeaderPair> headers;
    
    separateHeaderAndPayload(data, payload, headers);

    // from header parameters, retrieve the value of the "Content-Length" field corresponding to the payload size
    for (auto head : headers) 
    {
        if (head.first == "Content-Length")
        {
            return QString(head.second).trimmed().toInt();
        }
    }

    return 0;
}

QStringList httpResponse::splitHeaderValues(QByteArray & headerValues)
{
    QStringList listRes;

    // identifies all key-value pairs from a main header field
    listRes = QString(headerValues).split(";", QString::SplitBehavior::SkipEmptyParts);

    for (auto & str : listRes)
    {
        str = str.trimmed();
    }

    return listRes;
}

QString httpResponse::subSplitHeaderValues(QString & headerValue, char delim)
{
    // get the value of a header key
    return headerValue.right(headerValue.size() - (headerValue.indexOf(delim) + 1));;
}

QPair<QByteArray, QByteArray> httpResponse::splitByteArray(QByteArray data, QByteArray delimiter)
{
    QPair<QByteArray, QByteArray> res;

    int delimLen = delimiter.size();

    // identify the separation position according to the delimiter before splitting
    int pos = data.indexOf(delimiter);

    if (pos != -1)
    {
        res.first = data.left(pos);
        if (pos + delimLen < data.size() - 1)
        {
            res.second = data.mid(pos + delimLen);
        }
    }
    else
    {
        res.first = data;
    }

    return res;
}