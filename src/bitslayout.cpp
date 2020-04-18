#include "bitslayout.h"

BitsLayout::BitsLayout(QWidget *parent) : QWidget(parent), layout(nullptr)
{
    setup();
}

BitsLayout::~BitsLayout() {
//    if(layout != nullptr) delete layout;
}

void BitsLayout::setup() {
    if(layout == nullptr) {
        layout = new QGridLayout();
        layout->setSpacing(0);
        cols = 0;
        addLink();
    }
}

QPointer<QGridLayout> BitsLayout::getLayout() {
    return layout;
}

int BitsLayout::findElement(QString name) {
    if(bitsLabels.isEmpty()) return -1;

    for(int i = 0; i < bitsLabels.size(); i++) {
        if(name.compare(bitsLabels.at(i)->text(), Qt::CaseInsensitive) == 0) return i;
    }
    return -1;
}

QColor BitsLayout::getElementColor(int ID) {
    if(bitsLabels.size() < ID) return LEDS_DEFAULT_COLOR;

    return currColor[ID];
}

/*
 * slots
 */

void BitsLayout::addElement(leds_t led) {
    QLabel* newLabel = new QLabel(led.label);
    newLabel->setFont(QFont("Share Tech Mono"));
    newLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    newLabel->setAlignment(Qt::AlignHCenter);
    newLabel->setFont(APPLICATION_DEFAULT_FONT);

    layout->addWidget(newLabel, 0, cols++, 1,1);

    bitsLabels.insert(bitsLabels.count(), newLabel);
    currColor.insert(currColor.count(), LEDS_DEFAULT_COLOR);
    setElementColor(bitsLabels.size()-1, LEDS_DEFAULT_COLOR);
}

void BitsLayout::addFromList(const QList<leds_t>& list) {
    foreach (leds_t led, list) {
        addElement(led);
    }
}

void BitsLayout::setElementColor(int ID, QColor color) {
    if(bitsLabels.isEmpty()) return;
    if(bitsLabels.size() < ID || ID == -1) return;

    currColor[ID] = color;
    bitsLabels.at(ID)->setStyleSheet("QLabel {font-family: \"Share Tech Mono\"; position: relative;  margin: 1px 3px; padding: 3px 5px 0 5px; color: #fff; background: " + LEDS_DEFAULT_COLOR.name() + "; border-top: 2px solid " + currColor[ID].name() + QString("; border-radius: 0; }"));
}

void BitsLayout::resetColors() {
    for (int i = 0; i < bitsLabels.count(); i++)
        setElementColor(i, LEDS_DEFAULT_COLOR);
}

void BitsLayout::clear() {
    for(int i = 0; i < bitsLabels.count(); i++) {
        QLabel* labelPtr = bitsLabels.at(i);
//        currColor.removeAt(i);
        layout->removeWidget(labelPtr);
        delete labelPtr;
    }
    bitsLabels.clear();
    currColor.clear();
    cols = 0;
    addLink();
}

void BitsLayout::addLink() {
    leds_t linkLed;
    linkLed.label = "Link";
    addElement(linkLed);
}
