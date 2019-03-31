#ifndef SIGNEDCOMMAND_H
#define SIGNEDCOMMAND_H

#include <QObject>
#include "command.h"

class SignedCommand : public Command
{
public:
    explicit SignedCommand(QString code, int divider = 1, quint8 interval = 1, QObject* parent = nullptr);
    bool isSignedValue() override;
};

#endif // SIGNEDCOMMAND_H
