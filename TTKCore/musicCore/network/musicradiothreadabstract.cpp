#include "musicradiothreadabstract.h"

MusicRadioThreadAbstract::MusicRadioThreadAbstract(QObject *parent, QNetworkCookieJar *cookie)
    : QObject(parent), m_reply(nullptr), m_manager(nullptr)
{
    m_cookJar = cookie;
}

MusicRadioThreadAbstract::~MusicRadioThreadAbstract()
{
    deleteAll();
}

void MusicRadioThreadAbstract::deleteAll()
{
    if(m_reply)
    {
        m_reply->deleteLater();;
        m_reply = nullptr;
    }
    if(m_manager)
    {
        m_manager->deleteLater();;
        m_manager = nullptr;
    }
}

void MusicRadioThreadAbstract::replyError(QNetworkReply::NetworkError)
{
    emit networkReplyFinished("The file create failed");
    deleteAll();
}

#ifndef QT_NO_SSL
void MusicRadioThreadAbstract::sslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
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
    emit networkReplyFinished("The file create failed");
    deleteAll();
}
#endif
