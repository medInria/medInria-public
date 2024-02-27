/*
    QtDcm is a C++ Qt based library for communication and conversion of Dicom images.
    Copyright (C) 2011  Alexandre Abadie <Alexandre.Abadie@univ-rennes1.fr>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <QMap>
#include <QObject>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QTemporaryDir>
#include <QNetworkReply>
#include <medAbstractSource.h>

class AnnotationRequest : public QObject
{
    Q_OBJECT
public:
    explicit AnnotationRequest(int requestId);
    ~AnnotationRequest() = default;
    void getRequest(QNetworkAccessManager *manager, const QString &requestUrl);
    void postRequest(QNetworkAccessManager *manager, QString &requestUrl, const QString &name, QVariant dataset);

public slots:
    void onErrorOccured(QNetworkReply::NetworkError error);
    void finishDownload(QNetworkReply *reply);
    void finishUpload(QNetworkReply *reply);
    void inProgress(qint64 bytesReceived, qint64 bytesTotal);
    void abort();

signals:
    void progress(int requestId, medAbstractSource::eRequestStatus status, QVariant data = QVariant());

public:
    int m_requestId;
    QString m_path;

private:
    QNetworkReply *m_Reply;
    QTemporaryDir m_TemporaryDir;
    QString m_AnnotationUid;
    QFile m_File;
    QHttpMultiPart m_PostMultiPart;
};


