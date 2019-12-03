#ifndef ParameterForm_H
#define ParameterForm_H

#include "commondefines.h"

#include "command.h"

#include <QWidget>
#include <QUiLoader>
#include <QFile>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QTimer>
#include <QtGlobal>

class ParameterController : public QWidget
{
    Q_OBJECT

public:
    static int Count;
    explicit ParameterController(QString title, Command *minComm, Command *maxComm, Command *valueComm, Command *realComm, QWidget *parent = nullptr);
    ~ParameterController();
    bool getPinState();
    bool getEnableState();
    QWidget* loadWidget();
    QWidget* loadCompactWidget();
    QWidget* loadTextWidget();
    int getCommValue();
    bool isOnlyMeasured();
    bool hasRealCommand();

public slots:
    void hideRealValue(bool state = false);
    void setEnableState(bool state);

private:
    void setUnit();
    void setTitle(QString str);
    void setPinState(bool val);
    void prepareBigWidget();
    void prepareCompactViewWidget();
    void prepareTextWidget();

    QUiLoader loader;
    QPointer<QWidget> bigWidget;
    QPointer<QWidget> compactWidget;
    QPointer<QWidget> textWidget;
    QLabel *ui_titleLabel;
    QLabel *ui_minLabel;
    QLabel *ui_maxLabel;
    QLabel *ui_setValueLabel;
    QLabel *ui_typeLabel;
    QLabel *ui_currValueUnitLabel;
    QLineEdit *ui_currValueLine;
    QPushButton *ui_sendValueButton;
    QLabel *ui_currUnitProgressLabel;
    QLabel *ui_minUnitProgressLabel;
    QLabel *ui_maxUnitProgressLabel;
    QProgressBar *ui_measuredValueBar;
    QSlider *ui_valueSlider;
    QPushButton *ui_minusButton;
    QPushButton *ui_plusButton;
    QPushButton *ui_pinButton;
    QLineEdit *ui_currValueCompactLine;
    QLabel *ui_realValueCompactLabel;
    QLabel *ui_minCompactLabel;
    QLabel *ui_maxCompactLabel;
    QLabel *ui_titleCompactLabel;
    QLabel *ui_realValueUnitCompactLabel;
    QPushButton *ui_minusCompactButton;
    QPushButton *ui_plusCompactButton;
    QPushButton *ui_sendValueCompactButton;
    QTimer timer;
    QString title;
    bool pinState;
    bool hideReal;
    double currValue;
    double realValue;
    int precisionOfRealValue, precisionOfValue;
    QLabel* titleLabel;
    QLabel* valueLabel;
    QLabel* unitLabel;
    Command *realComm;
    Command *valueComm;
    Command *minComm;
    Command *maxComm;
    uint iMin, iMax, iReal, iSent;
    QString preparedCommand;
    bool isUserEdited;
    QDoubleValidator validator;

private slots:
    void on_valueSlider_valueChanged(int value);
    void on_valueSlider_sliderPressed();
    void on_valueSlider_sliderMoved(int value);
    void on_valueSlider_sliderReleased();
    void on_currValueLine_returnPressed();
    void on_currValueLine_textEdited();
    void on_minusButton_released();
    void on_plusButton_released();
    void on_pinButton_released();
    void sendValueSlot();
    void on_currValueCompactLine_returnPressed();
    void on_currValueCompactLine_textEdited();
    void on_minusCompactButton_released();
    void on_plusCompactButton_released();
    void sendValueCompactSlot();
    void setEditLineRedColor();
    void setEditLineDefaultState();
    void setMax();
    void setMin();
    void setRealValue();
    void setSentValue();

signals:
    void changeValue(QString);
};

#endif // ParameterForm_H
