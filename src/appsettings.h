#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QPoint>
#include <QSize>

#include "commondefines.h"

class AppSettings : public QObject
{
    Q_OBJECT
public:
    explicit AppSettings(QObject *parent = nullptr);
    quint32 getComBaudrate();
    QString getComPort();
    uint getComAutoconnectFlag(bool flag);
    uint getComCommandsDelay();
    bool getComAutoconnectFlag();
    QString getTemperatureSymbol();
    bool getCompactModeFlag();
    const QList<QVariant> getRecentOpenFiles();
    QString getLastSaveDirectory();
    quint32 getLastSelectedDeviceId();
    bool getHideControlsFlag();
    QPoint getWindowPosition();
    int getComStopBits();
    bool getKeepCheckboxesFlag();

private:
    QSettings *settings;

signals:

public slots:
    void setComBaudrate(quint32 BR);
    void setComPort(QString port);
    void setComAutoconnectFlag(bool flag);
    void setComCommandsDelay(uint delayMs);
    void setTemperatureSymbol(QString tempSymbol);
    void setCompactModeFlag(bool flag);
    void setRecentOpenFiles(const QList<QVariant> &list);
    void setLastSaveDirectory(QString dir = QDir::homePath());
    void setLastSelectedDeviceId(quint32 id);
    void setHideControlsFlag(bool flag);
    void removeRecentOpenFiles(QString str);
    void setWindowPosition(QPoint);
    void setComStopBits(int);
    void setKeepCheckboxesFlag(bool flag);
};

#endif // APPSETTINGS_H
