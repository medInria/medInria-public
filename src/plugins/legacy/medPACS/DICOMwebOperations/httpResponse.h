/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once


#include <QObject>
#include <QNetworkReply>
#include <QByteArray>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QJsonObject>


struct partChunck
{
    QList<QPair<QByteArray, QByteArray>> headers; // headers information
    QByteArray payload; // DICOM data
};

class httpResponsePrivate;

class httpResponse : public QObject
{
    Q_OBJECT

public:
    explicit httpResponse(QNetworkReply & reply);
    explicit httpResponse(QByteArray &data);
    explicit httpResponse(QByteArray &payload, QList<QPair<QByteArray, QByteArray>> &headers);
    ~httpResponse();

    bool isMutlipart();
    QVector<partChunck> getParts();

    // possible ajout ici

private:
    void init();
    void addPart(QByteArray & part);
    int getContentSize(QByteArray &data);

    QPair<QByteArray, QByteArray> splitByteArray(QByteArray data, QByteArray delimiter);
    QStringList splitHeaderValues(QByteArray & headerValues);
    QString subSplitHeaderValues(QString & headerValue, char delim = '=');
    void separateHeaderAndPayload(QByteArray & data, QByteArray & mainPayload, QList<QNetworkReply::RawHeaderPair> & treatedHeaders);

private:
    httpResponsePrivate *d;
};


/**
 * @fn bool httpResponse::isMutlipart()
 * @brief find out whether data are in multipart mode
 * @return true if it is, false otherwise
*/

/**
 * @fn QVector<partChunck> httpResponse::getParts()
 * @brief retrieve QVector of parts (d->parts) extracted from the content of a query response 
 * @return for each part, structure contains header and payload (DICOM data)
*/

/**
 * @fn void httpResponse::init()
 * @brief read the main response header. From these information, create parts of each instance identified in the response data
 * @details instances are separated by the 'boudary' field, mentioned in the main header response
*/

/**
 * @fn void httpResponse::addPart(QByteArray & part)
 * @brief set a new partChunck structure with the header and the payload (DICOM data) of an instance
 * @details filled the QVector d->parts
*/

/**
 * @fn void httpResponse::separateHeaderAndPayload(QByteArray & data, QByteArray & mainPayload, QList<QNetworkReply::RawHeaderPair> & treatedHeaders)
 * @brief differentiate header information from payload of a single instance of the reponse data
 * @param data [in] data to be separated
 * @param mainPayload [out] will contain payload (DICOM data) of an instance
 * @param treatedHeaders [out] will contain instance header pairs information
*/

/**
 * @fn int httpResponse::getContentSize(QByteArray &data)
 * @brief obtain the payload content size of an instance from its header
 * @param data [in] data of a specific instance 
*/

/**
 * @fn QStringList httpResponse::splitHeaderValues(QByteArray & headerValues)
 * @brief split the values contained in the "Content-Type" field of the main header response
 * @details split with ';' delimiter
 * @param headerValues [in] the value of the "Content-Type" field
 * @return a QStringList containing all sub-fields
*/

/**
 * @fn QString httpResponse::subSplitHeaderValues(QString & headerValue, char delim)
 * @brief retrieve value of a subfield, from the delimiter+1 to the end
 * @param headerValue [in] the sub-field whose value is to be retrieved
 * @param delim [in] delimiter that indicates how to separate the key and the value
 * @return the value of the sub-field
*/

/**
 * @fn QPair<QByteArray, QByteArray> httpResponse::splitByteArray(QByteArray data, QByteArray delimiter)
 * @brief separate data into two parts defined by a delimiter
 * @param data [in] the data to be devided
 * @param delim [in] delimiter that indicates, by its first occurrence, the position from which the data must be separated
 * @return a pair in which each element corresponds respectively to the beginning and the end of the data
*/



