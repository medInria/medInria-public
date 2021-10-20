//
// Created by Julien Castelneau on 12/10/2021.
//

#ifndef MEDINRIA_DOWNLOADER_H
#define MEDINRIA_DOWNLOADER_H


#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>

class Downloader : public QObject
        {
    Q_OBJECT
        public:
            explicit Downloader(QObject *parent = 0);

    void doDownload();

    signals:

            public slots:
            void replyFinished (QNetworkReply *reply);

        private:
            QNetworkAccessManager *manager;

        };


#endif //MEDINRIA_DOWNLOADER_H
