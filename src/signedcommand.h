#ifndef SIGNEDCOMMAND_H
#define SIGNEDCOMMAND_H

#include <QObject>
#include "command.h"
#include <QDebug>

class SignedCommand : public Command
{
public:
    explicit SignedCommand(QString code, double divider = 1, quint8 interval = 1, QObject* parent = nullptr);
    bool isSignedValue() override;
    double getConvertedValue() override;

public slots:
    void setRawValue(quint16 rawValue) override;
};

#endif // SIGNEDCOMMAND_H
