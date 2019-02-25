#ifndef ParameterForm_H
#define ParameterForm_H

#include "commondefines.h"
//#include "globals.h"

extern AppSettings settings;

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
//#include <QLocale>
//#include <QRegExpValidator>
#include <QGroupBox>
#include <QTimer>

class ParameterForm : public QWidget
{
    Q_OBJECT

public:
    //explicit ParameterForm(const paramControls_t &inParam, QWidget *parent = 0);
    explicit ParameterForm(QWidget *parent = 0);
    ~ParameterForm();
    //~ParameterForm();
    bool getPinState();
    bool getEnableState();
    QWidget* loadWidget();
    QWidget* loadCompactWidget();
    QWidget* loadTextWidget();
//    QLabel* getTitleLabel();
//    QLabel* getValueLabel();
//    QLabel* getUnitLabel();
//    void deleteLabels();
    int getCommValue();
    QString getMinComm();
    QString getMaxComm();
    QString getRealComm();
    QString getValueComm();
    bool isTemperature();
//    void updateWidget();

public slots:
    void setMinComm(QString str);
    void setMaxComm(QString str);
    void setRealComm(QString str);
    void setValueComm(QString str);
    void setTitle(QString str);
    void setMax(double val);
    void setMin(double val);
    void setUnit(QString str);
    void setType(QString str);
    void setRealValue(double val);
    void setSentValue(double val);
    void setDivider(double val);
    void setIsTemperatureFlag(bool state = false);
    void hideRealValue(bool state = false);
    void setEnableState(bool state);
    void temperatureIsChanged(QString);

private:
    void setPinState(bool val);
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
    QProgressBar *ui_readedValueBar;
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
    QString unit;
    QString type;
    QString title;
    //QString valueComm;
    bool pinState;
    bool isCelsius;
    bool hideReal;
    bool isTemperatureFlag;
    //const paramControls_t *param;
    double min;
    double max;
    double currValue;
    double realValue;
    double divider;
    //QHBoxLayout *labelLayout;
    QLabel* titleLabel;
    QLabel* valueLabel;
    QLabel* unitLabel;
    QString realComm;
    QString valueComm;
    QString minComm;
    QString maxComm;
    QString preparedCommand;
    bool isUserEdited;

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

signals:
    //void changePinState(bool);
    void changeValue(QString);
};

#endif // ParameterForm_H
