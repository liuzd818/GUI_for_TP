#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QByteArray>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void readSerial();
    void updateStatus1(QString);
    void updateStatus2(QString);
    void updateStatus3(QString);
    void updateString(QString);

private:
    Ui::Dialog *ui;

    QSerialPort *arduino_nano;
    QSerialPort *arduino_uno;
    static const quint16 arduino_nano_vendor_id = 6790;
    static const quint16 arduino_nano_product_id = 29987;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data1;
    QString parsed_data2;
    QString parsed_data3;
    QString parsed_data4;
    QString parsed_data5;
    int x;
    int y;
    int z;
    int DMS;
    int Estop;
    int value;

};

#endif // DIALOG_H
