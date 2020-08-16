#ifndef LOGSHELPERS_H
#define LOGSHELPERS_H

#include <QObject>

class LogsHelpers : public QObject
{
    Q_OBJECT
public:
    explicit LogsHelpers(QObject *parent = nullptr);

    void logsUserSucess(QWidget *wid, QString Tipo);

    void logsUserFail(QWidget *wid, QString Tipo);

    void logsInternos(QString desc);
};

#endif // LOGSHELPERS_H
