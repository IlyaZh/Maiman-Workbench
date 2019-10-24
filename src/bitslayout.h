#ifndef BITSLAYOUT_H
#define BITSLAYOUT_H

#include <QWidget>
//#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSet>
#include <QColor>
#include <QDebug>
#include <QFont>

#include "commondefines.h"
#include "globals.h"

class BitsLayout : public QWidget
{
    Q_OBJECT
public:
    explicit BitsLayout(QWidget *parent = 0);
    ~BitsLayout();
    QPointer<QGridLayout> getLayout();
    int findElement(QString name);
    QColor getElementColor(int ID);

signals:

public slots:
    void addElement(leds_t led);
    void addFromList(const QList<leds_t>&);
    void setElementColor(int ID, QColor color);
    void resetColors();
    void clear();
    void setup();


private:
    QPointer<QGridLayout> layout;
    QList<QLabel*> bitsLabels;
    QList<QColor> currColor;
    void addLink();
    int cols;
};

#endif // BITSLAYOUT_H
