#include "musicdata2downloadthread.h"

#ifdef MUSIC_QT_5
#   include <QJsonParseError>
#   include <QJsonObject>
#else
#   include <QtScript/QScriptEngine>
#   include <QtScript/QScriptValue>
#endif

MusicData2DownloadThread::MusicData2DownloadThread(const QString &url, const QString &save,
                                                   Download_Type type, QObject *parent)
    : MusicDataDownloadThread(url, save, type, parent)
{
    m_dataReply = nullptr;
    m_dataManager = new QNetworkAccessManager(this);
#ifndef QT_NO_SSL
    connect(m_dataManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                           SLOT(dataSslErrors(QNetworkReply*,QList<QSslError>)));
    M_LOGGER_INFO(QString("MusicData2DownloadThread Support ssl: %1").arg(QSslSocket::supportsSsl()));
#endif
}

void MusicData2DownloadThread::startToDownload()
{
    m_timer.start(1000);
    QNetworkRequest request;
    request.setUrl(m_url);
#ifndef QT_NO_SSL
    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);
#endif
    m_dataReply = m_dataManager->get( request );
    connect(m_dataReply, SIGNAL(finished()), SLOT(dataGetFinished()));
    connect(m_dataReply, SIGNAL(error(QNetworkReply::NetworkError)),
                         SLOT(dataReplyError(QNetworkReply::NetworkError)) );
    connect(m_dataReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(downloadProgress(qint64, qint64)));
}

void MusicData2DownloadThread::deleteAll()
{
    if(m_dataManager)
    {
        m_dataManager->deleteLater();;
        m_dataManager = nullptr;
    }
    if(m_dataReply)
    {
        m_dataReply->deleteLater();
        m_dataReply = nullptr;
    }
    MusicDataDownloadThread::deleteAll();
}

void MusicData2DownloadThread::dataGetFinished()
{
    if(!m_dataReply)
    {
        return;
    }

    m_timer.stop();
    if(m_dataReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = m_dataReply->readAll();
#ifdef MUSIC_QT_5
        QJsonParseError jsonError;
        QJsonDocument parseDoucment = QJsonDocument::fromJson(bytes, &jsonError);
        if(jsonError.error != QJsonParseError::NoError || !parseDoucment.isObject())
        {
            deleteAll();
            return ;
        }

        QJsonObject jsonObject = parseDoucment.object();
        if(jsonObject.value("code").toVariant().toInt() == 1)
        {
            m_url = jsonObject.value("data").toObject().value("singerPic").toString();
#else
        QScriptEngine engine;
        QScriptValue sc = engine.evaluate("value=" + QString(bytes));
        if(sc.property("code").toInt32() == 1)
        {
            m_url = sc.property("data").property("singerPic").toString();
#endif
            emit data2urlHasChanged(m_url);
            MusicDataDownloadThread::startToDownload();
        }
    }
    deleteAll();
}

void MusicData2DownloadThread::dataReplyError(QNetworkReply::NetworkError)
{
    emit musicDownLoadFinished("The data2 create failed");
    deleteAll();
}

#ifndef QT_NO_SSL
void MusicData2DownloadThread::dataSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    QString errorString;
    foreach(const QSslError &error, errors)
    {
        if(!errorString.isEmpty())
        {
            errorString += ", ";
        }
        errorString += error.errorString();
    }

    M_LOGGER_ERROR(QString("sslErrors: %1").arg(errorString));
    reply->ignoreSslErrors();
    emit musicDownLoadFinished("The data2 create failed");
    deleteAll();
}
#endif
