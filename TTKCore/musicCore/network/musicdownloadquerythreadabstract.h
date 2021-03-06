#ifndef MUSICDOWNLOADQUERYTHREADABSTRACT_H
#define MUSICDOWNLOADQUERYTHREADABSTRACT_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (c) 2014 - 2016 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include <QObject>
#include <QSslError>
#include <QSslSocket>
#include <QNetworkReply>
#include "musicglobaldefine.h"
#include "musicobject.h"

class QNetworkAccessManager;

typedef struct DownloadData{
    QString m_songName;
    QString m_songArtist;
    QString m_songUrl;
    QString m_picUrl;
    QString m_time;
    QString m_format;

    void clear()
    {
        m_songName.clear();
        m_songArtist.clear();
        m_songUrl.clear();
        m_picUrl.clear();
        m_time.clear();
        m_format.clear();
    }

    bool isValid()
    {
        return !(m_songName.isEmpty() && m_time.isEmpty() && m_format.isEmpty());
    }
}DownloadData;

/*! @brief The class to abstract query download data from net.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_NETWORK_EXPORT MusicDownLoadQueryThreadAbstract : public QObject
{
    Q_OBJECT
public:
    enum QueryType
    {
        MusicQuery, ///*query music*/
        MovieQuery, ///*query movie*/
        LrcQuery    ///*query lrc*/
    };

    explicit MusicDownLoadQueryThreadAbstract(QObject *parent = 0);
    /*!
     * Object contsructor.
     */
    ~MusicDownLoadQueryThreadAbstract();

    void deleteAll();
    /*!
     * Release the network object.
     */
    virtual void startSearchSong(QueryType type, const QString &text) = 0;
    /*!
     * Start to search data from name and type.
     * Subclass should implement this function.
     */
    inline void setSearchQuality(const QString &qual) { m_searchQuality = qual;}
    /*!
     * Set search data quality.
     */
    inline QString getSearchQuality() const { return m_searchQuality;}
    /*!
     * Get search data quality.
     */
    inline void setQueryAllRecords(bool state) { m_queryAllRecords = state;}
    /*!
     * Set wheather query all quality of records.
     */
    inline bool getQueryAllRecords() const { return m_queryAllRecords;}
    /*!
     * Get query all records flag.
     */
    inline QueryType getQueryType() const { return m_currentType;}
    /*!
     * Return the current song query type.
     */
    inline QString getSearchedText() const { return m_searchText;}
    /*!
     * Return the current song name.
     */
    inline int getSongIdIndex() const { return m_musicSongInfos.size() + 1;}
    /*!
     * Return the current song container size.
     */
    inline const MusicSongInfomations& getMusicSongInfos(){ return m_musicSongInfos;}
    /*!
     * Return the current song container.
     */

Q_SIGNALS:
    void resolvedSuccess();
    /*!
     * Query data finished.
     */
    void clearAllItems();
    /*!
     * Clear all items before the new query start.
     */
    void createSearchedItems(const QString &songname,
                             const QString &artistname, const QString &time);
    /*!
     * Create the current items by song name\ artist name and time.
     */

public Q_SLOTS:
    virtual void searchFinshed() = 0;
    /*!
     * Download data from net finished.
     * Subclass should implement this function.
     */
    void replyError(QNetworkReply::NetworkError error);
    /*!
     * Download reply error.
     */
#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    /*!
     * Download ssl reply error.
     */
#endif

protected:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    MusicSongInfomations m_musicSongInfos;
    QString m_searchText, m_searchQuality;
    QueryType m_currentType;
    bool m_queryAllRecords;

};

#endif // MUSICDOWNLOADQUERYTHREADABSTRACT_H
