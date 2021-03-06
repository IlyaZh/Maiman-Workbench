#include "comport.h"

ComPort::ComPort(AppSettings *appSettings, QObject *parent) : QObject(parent), settings(appSettings), serialportinfo(nullptr)
{
    if(appSettings == nullptr)  {
        qDebug() << "ComPort" << "appSettings pointer is NULL";
    }
    serialPort = new QSerialPort();
//    qsettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, ORG_NAME, APP_NAME);

//    isTimeout = false;
//    bufferSize = 0;
    portIsBusy = false;
    checkStopWritten = 0;
    stopCommandDelay = STOP_COMMAND_DELAY_DEFAULT;

    // Таймер таймаута (отсчёт идёт с момента фактической пересылки всей посылки в порт и до прихода ответа от устройства)
    timer = new QTimer();
    timer->setInterval(COM_PORT_TIMEOUT);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    // Таймер ошибки порта (отсчёт идёт с момента вызова функции записи в порт и до момента фактической пересылки данных)
    // Если связи нет - данные не будут отправлены и сработает таймаут ошибки
    errorTimer = new QTimer();
    errorTimer->setInterval(COM_PORT_TIMEOUT);
    errorTimer->setSingleShot(true);
    connect(errorTimer, SIGNAL(timeout()), this, SLOT(errorTimeout()));

    connect(serialPort, SIGNAL(bytesWritten(qint64)), this, SLOT(dataIsWritten(qint64)));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(needToRead()));
    connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(errorSlot(QSerialPort::SerialPortError)));
//    connect(serialPort, SIGNAL())
}

ComPort::~ComPort() {
    close();
    if(timer) timer->deleteLater();
    if(errorTimer) errorTimer->deleteLater();
    emit portNewState(serialPort->isOpen());
}

bool ComPort::isOpen() {
    return serialPort->isOpen();
}

//void ComPort::stopAndDisconnect() {
//    clearQueue();

//    putIntoQueue(DEVICE_STOP_COMMAND);
//    putIntoQueue(TEC_STOP_COMMAND);
//    checkStopWritten = 2;
//}

void ComPort::setPortState(bool state) {
    serialPort->setPortName(settings->getComPort());
    if(serialportinfo) delete serialportinfo;
    serialportinfo = new QSerialPortInfo(settings->getComPort());
    portIsBusy = false;

    if (state) {
        if (serialPort->open(QIODevice::ReadWrite)) {
            QSerialPort::StopBits stopBits;
            switch(settings->getComStopBits()) {
            case 1:
                stopBits = QSerialPort::OneStop;
                break;
            default:
            case 2:
                stopBits = QSerialPort::TwoStop;
                break;
            }
            if (serialPort->setStopBits(stopBits) &&
                    serialPort->setParity(QSerialPort::NoParity) &&
                    serialPort->setBaudRate(settings->getComBaudrate())) {
            } else {
                emit errorOccuredSignal(serialPort->errorString());
            }
        } else {
            emit errorOccuredSignal(serialPort->errorString());
        }
        emit portNewState(serialPort->isOpen());
    } else {
        this->close();
    }
}

void ComPort::changeBaudRate(int BR) {
    settings->setComBaudrate(BR);
    if(serialPort->isOpen()) {
        // restart the port
        setPortState(false);
        setPortState(true);
    }
}

void ComPort::close() {
    if(serialPort->isOpen()) {
        serialPort->close();
    }
    timer->stop();
    errorTimer->stop();
//    isTimeout = false;
    portIsBusy = false;
    clearQueue();
    emit portNewState(serialPort->isOpen());
}

void ComPort::putIntoQueue(QString str) {
    // Добавляем символ конца строки \r = 0x13
    str.append(COM_END_OF_LINE);

    // Удаляем дубликаты
//    queue2send.removeAll(str);
    // Удаляем запросы той же команды, но с другим значением параметра.
//    for(int i =0; i < queue2send.size(); i++) {
//        if(queue2send.at(i).startsWith(str.left(5), Qt::CaseInsensitive)) {
//            queue2send.removeAt(i);
//        }
//    }

    // Команды установки параметров добавляем в начало очереди
    if(str.at(0) == COM_WRITE_PREFIX) {
        // Добавляем в начало очереди
        queue2send.prepend(str);
    } else {
        // В конец очереди
        queue2send.append(str);
    }

    if(queue2send.length() == 1 && !portIsBusy) {
        // Если только что добавленная в очередь команда является одной единственной в ней, то запускаем процесс передачи.
        writeToPort();
    }
}

void ComPort::writeToPort() {
    if(queue2send.isEmpty()) return;
    portIsBusy = true;
//    isTimeout = false;
    buffer.clear();
    msgToSend = queue2send.dequeue();
    sizeOfPackage = msgToSend.size();
    firstSymbol = msgToSend.at(0);
    // Сохраняем номер команды (2-5 символы включительно) - в функции mid нумерация идёт с 0 символа.
    //if(sizeOfPackage > 4) lastWrittenCommand = msgToSend.mid(1,4);

    if(serialPort->isOpen()) {
        lastSentCommand = msgToSend;
        int sendState = serialPort->write(msgToSend.toLocal8Bit(), msgToSend.size());
        if(sendState == -1) {
            errorTimeout();
        }

//        errorTimer->setInterval(COM_PORT_TIMEOUT);
//        errorTimer->start();
    }
}

void ComPort::timeOut() {
//    isTimeout = false; // was true
    portIsBusy = false;
    buffer.clear();
    clearQueue();
    emit portTimeroutOccure();
}

void ComPort::errorTimeout() {
    portIsBusy = false;
    buffer.clear();
    clearQueue();
//    emit portErrorTimeoutOccure();
    emit errorOccuredSignal("Cannot send data to port: " + serialPort->errorString());
}

void ComPort::needToRead() {
   timer->stop();

   buffer.append(serialPort->readAll());
//   bufferSize += buffer.size();

//   if(isTimeout) {
//       return;
//   }

   if(firstSymbol == COM_WRITE_PREFIX) {
       buffer.clear();
       portIsBusy = false;
//       bufferSize = 0;
       return;
   }
   if(buffer.contains(COM_END_OF_LINE) && (buffer.at(0) == COM_ANSWER_PREFIX || buffer.at(0) == COM_ERROR_PREFIX)) {
       buffer.resize(buffer.indexOf(COM_END_OF_LINE)+1);
       emit receivedDataSignal(buffer);

       buffer.clear();
       portIsBusy = false;

           // Если в очереди есть ещё команды - приступаем к передаче
           if(startToSendNextCommand()) return;

   } else {
       timer->setInterval(COM_PORT_TIMEOUT);
       timer->start();
   }
//   buffer.clear();

//   if(buffer.size() > READ_COM_COMMAND_LENGTH*2) {
//       emit errorOccuredSignal("Unexpected length of answer: " + QString(buffer));
//       portIsBusy = false;
//       return;
//   }
}

void ComPort::dataIsWritten(qint64 length) {
    errorTimer->stop();

    if(sizeOfPackage == length) {
        emit writeToConsoleSignal("-> "+msgToSend, CONSOLE_SEND_COLOR);
        if(firstSymbol == COM_WRITE_PREFIX) {
            // emit dataIsSentSignal();
            // startToSendNextCommand();

            // Ожидаем длительную паузу после отправки команды "стоп"
            if(lastSentCommand == DEVICE_STOP_COMMAND+QString(COM_END_OF_LINE) || lastSentCommand == TEC_STOP_COMMAND+QString(COM_END_OF_LINE)) {
                QTimer::singleShot(stopCommandDelay, this, SLOT(waitForNextCommandSlot()));
            } else {
                waitForNextCommandSlot();
                portIsBusy = false;
            }
            return;
        } else {
            timer->setInterval(COM_PORT_TIMEOUT);
            timer->start();
        }
    } else {
//        qDebug() << "Data written is not equal size: " << length << " instead of " << sizeOfPackage;
    }
}

void ComPort::errorSlot(QSerialPort::SerialPortError spe) {
    portIsBusy = false;
    if(spe != QSerialPort::NoError) {// && spe != QSerialPort::UnknownError)
//        if(serialPort->isOpen())
            emit errorOccuredSignal(serialPort->errorString());
        close();
    }
}

void ComPort::clearQueue() {
    queue2send.clear();
}

bool ComPort::startToSendNextCommand() {
    if(portIsBusy) return false;
    if(queue2send.count() > 0) {
        QTimer sendTimer;
        sendTimer.singleShot(settings->getComCommandsDelay(), this, SLOT(writeToPort()));
        return true;
    }
    return false;
}

void ComPort::waitForNextCommandSlot() {
    portIsBusy = false;

    if(checkStopWritten > 0 && (lastSentCommand == DEVICE_STOP_COMMAND+QString(COM_END_OF_LINE) || lastSentCommand == TEC_STOP_COMMAND+QString(COM_END_OF_LINE))) {
        checkStopWritten--;
        if(checkStopWritten == 0) {
            setPortState(false);
            return;
        }
    }

    if(startToSendNextCommand() == false) { // false - команд в очереди нет - даём сигнал об окончании передачи
        emit dataIsSentSignal();
    }
}

void ComPort::setStopCommandDelay(uint value) {
    stopCommandDelay = value;
}

void ComPort::setStopBits(int value) {
    settings->setComStopBits(value);
    if(serialPort->isOpen()) {
        // restart the port
        setPortState(false);
        setPortState(true);
    }
}

