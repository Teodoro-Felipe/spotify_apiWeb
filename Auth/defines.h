#ifndef DEFINES_H
#define DEFINES_H

#include <QtCore>

constexpr qint16 port = 8080;

constexpr const char *UrlAutorization = "https://accounts.spotify.com/authorize";

constexpr const char *UrlToken = "https://accounts.spotify.com/api/token";

constexpr const char *scope = "user-read-private user-top-read playlist-read-private playlist-modify-public playlist-modify-private";

namespace Structs
{
    struct PlayList
    {
        QString name;
        bool isPublic{false};
        QString descript;
    };
}

#endif // DEFINES_H
