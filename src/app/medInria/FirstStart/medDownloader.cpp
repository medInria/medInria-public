/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDownloader.h>

#include <iostream>



#include <qbytearray.h>
#include <qbytearraylist.h>
#include <qglobal.h>
#include <qlist.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpair.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qurl.h>

#define HTTP_SEP "\r\n" // http separation sequence

class medHttpPart
{
public:
    medHttpPart() = default;
    ~medHttpPart() = default;

    bool setData(QByteArray data);

    using  RawHeaderPair = QPair<QByteArray, QByteArray> ;


    const QList<RawHeaderPair>& rawHeaderPairs() const { m_rawHeaderList; }
    QByteArray payload() const { return m_payload; }


private:
    void rawHeaderExtract(QByteArray rawHead);
    void payloadExtract(QByteArray rawPayload) { m_payload = rawPayload; }

private:
    QList<RawHeaderPair> m_rawHeaderList;
    QByteArray m_payload;
};

medDownloader::medDownloader()
{
    m_qnam = new QNetworkAccessManager(this);
}

medDownloader::~medDownloader()
{
}

int medDownloader::download(QUrl url)
{
    if (!url.isEmpty())
    {
        QNetworkRequest request(url);

        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        QNetworkReply * reply = m_qnam->get(request);

        // Connecting the request manager slots to the signals sent by the reply object
        QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    }
        
    return 0;
}





//--------------------------------------------------------------------------------------------------------------------------

void medDownloader::slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    // retrieving the reply object who sent a download signal
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
    //TODO Log
}

void medDownloader::currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal)
{
    // the response is in progress, therefore, the payload is empty. 
    QByteArray payload = QByteArray();
    // storing the bytes information of the reply in the header
    QList<QPair<QByteArray, QByteArray>> headers;

    // sending the signal for the network classes to handle the reply information 

}










QStringList splitHeaderValues(QByteArray & headerValues)
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

QString subSplitHeaderValues(QString & headerValue, char delim = '=')
{
    // get the value of a header key
    return headerValue.right(headerValue.size() - (headerValue.indexOf(delim) + 1));;
}


QPair<QByteArray, QByteArray> splitByteArray(QByteArray data, QByteArray delimiter)
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

void separateHeaderAndPayload(QByteArray & data, QByteArray & mainPayload, QList<QNetworkReply::RawHeaderPair> & treatedHeaders)
{
    // separate the header to the payload
    auto headerAndData = splitByteArray(data, HTTP_SEP HTTP_SEP);
    mainPayload = headerAndData.second;
    QString headers = headerAndData.first;

    // go through the values in a header to identify fields and values
    QStringList headerList = headers.split(QRegExp("[" HTTP_SEP "]"), QString::SkipEmptyParts);
    for (auto &headerEntry : headerList)
    {
        auto entrySplited = splitByteArray(headerEntry.toLatin1(), ":");
        treatedHeaders.push_back(entrySplited);
    }
}

int getContentSize(QByteArray &data)
{
    QByteArray payload;
    QList<QNetworkReply::RawHeaderPair> headers;

    separateHeaderAndPayload(data, payload, headers);

    // from header parameters, retrieve the value of the "Content-Length" field corresponding to the payload size
    for (auto & head : headers)
    {
        if (head.first == "Content-Length")
        {
            return QString(head.second).trimmed().toInt();
        }
    }

    return 0;
}










void medDownloader::slotFinished()
{
    // retrieving the reply object who sent a finish signal
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());

    //handling the reply finish information
    if (reply)
    {
        int httpCode = static_cast<int>(reply->error());
        int httpCode2 = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        auto payload = reply->readAll();
        auto &headers = reply->rawHeaderPairs();

        auto &headersPairs = reply->rawHeaderPairs();
        auto headersLst   = reply->rawHeaderList();
        auto aHeader      = reply->rawHeader("Content-Disposition");

        QString contentType;
        QByteArray boundary;
        QString type;

        bool multi = false;
        // read response main header for information on the response data
        for (auto head : headers)
        {
            if (head.first == "Content-Type")
            {
                auto values = splitHeaderValues(head.second);
                for (int i = 0; i < values.size(); ++i)
                {
                    if (i == 0)
                    {
                        multi = values[i].startsWith("multipart");
                        contentType = values[i];
                    }

                    if (values[i].startsWith("boundary="))
                    {
                        boundary = subSplitHeaderValues(values[i]).toLatin1();
                    }

                    if (values[i].startsWith("type="))
                    {
                        type = subSplitHeaderValues(values[i]).toLatin1();
                    }
                }
            }
        }

        // if multipart, instances are delimited by boundaries
        if (multi)
        {
            extractMultiPart(boundary, payload);
        }
        else
        {
            extractSinglePart(payload);
        }

        std::cout << httpCode << " " << httpCode2;
    }
}

void medDownloader::extractSinglePart(QByteArray& payload)
{
    //TODO
}

void extractMultiPart(QByteArray &boundary, QByteArray &payload)
{
    auto const sep = "--" + boundary;
    auto pos = payload.indexOf(sep) + sep.size();
    // skip first part who is empty
    QByteArray data = payload.mid(pos);

    // retrieve all the following parts up to the end of the response data (1 part = 1 file)
    while (!data.isEmpty())
    {
        auto dataSplited = splitByteArray(data, sep);
        //addPart(dataSplited.first);

        if (getContentSize(dataSplited.second) != 0)
        {
            data = dataSplited.second;
        }
        else
        {
            data.clear();
        }
    }
}

void extractSinglePart(QByteArray &payload)
{
    if (payload.endsWith(HTTP_SEP))
    {
        payload.chop(2);
    }
}


void medDownloader::slotError(QNetworkReply::NetworkError err)
{
    // retrieving the reply object who sent an error signal
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
    //handling the reply error information
    if (reply)
    {
        int httpCode = static_cast<int>(reply->error());
        int httpCode2 = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        std::cout << httpCode << " " << httpCode2;
        //TODO log error and insert in failed list
    }
}

bool medHttpPart::setData(QByteArray data)
{
    bool bRes = false;

    auto headPayloadBorder = data.indexOf(HTTP_SEP HTTP_SEP);
    auto endBorder = data.lastIndexOf(HTTP_SEP HTTP_SEP);

    if (headPayloadBorder > 0)
    {
        auto payloadIndex = headPayloadBorder + sizeof(HTTP_SEP HTTP_SEP);

        auto head = data.mid(0, headPayloadBorder);
        auto payload = data.mid(payloadIndex, endBorder);

        rawHeaderExtract(head);
        payloadExtract(payload);

        bRes = true;
    }

    return bRes;
}

void medHttpPart::rawHeaderExtract(QByteArray rawHead)
{
    QByteArrayList headersLst;//TODO = rawHead.split(HTTP_SEP);
    for (auto & header : headersLst)
    {
        auto valueSeparatorIndex = header.indexOf(":");
        auto headerName = header.mid(0, valueSeparatorIndex);
        auto headerVal = header.mid(valueSeparatorIndex+1, header.size() - (valueSeparatorIndex + 1));

        m_rawHeaderList.push_back(QPair < QByteArray, QByteArray> (headerName, headerVal));
    }
}
