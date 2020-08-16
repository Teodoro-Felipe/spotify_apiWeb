#include "logshelpers.h"

#include <QMessageBox>
#include <QDebug>

LogsHelpers::LogsHelpers(QObject *parent) :
    QObject(parent)
{}

void LogsHelpers::logsUserSucess(QWidget *wid, QString Tipo)
{
    QMessageBox::information(wid,"Sucesso!", Tipo);
}

void LogsHelpers::logsUserFail(QWidget *wid, QString Tipo)
{
    QMessageBox::warning(wid,"Erro", Tipo);
    qWarning() << "Erro: "+Tipo;
}

void LogsHelpers::logsInternos(QString desc)
{
    qWarning() << "Erro: "+desc;
}
