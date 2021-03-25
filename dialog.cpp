#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->axis_lcdNumber->display("-------");
    ui->axis_lcdNumber_2->display("-------");
    ui->axis_lcdNumber_3->display("-------");
    ui->axis_lcdNumber_7->display("OFF");
    ui->axis_lcdNumber_8->display("OFF");
    arduino_nano = new QSerialPort(this);
    arduino_uno = new QSerialPort(this);
    serialBuffer = "";
    parsed_data1 = "";
    parsed_data2 = "";
    parsed_data3 = "";
    x = 0;
    y = 0;
    z = 0;
    DMS = 0;
    Estop = 0;
    bool arduino_nano_is_available = false;
    QString arduino_nano_port_name;
    bool arduino_uno_is_available = false;
    QString arduino_uno_port_name;


    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
            if(serialPortInfo.hasVendorIdentifier()){
                qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
            }
            qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
            if(serialPortInfo.hasProductIdentifier()){
                qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
            }
        }


    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if((serialPortInfo.productIdentifier() == arduino_nano_product_id) && (serialPortInfo.vendorIdentifier() == arduino_nano_vendor_id))
            {
                    arduino_nano_is_available = true;
                    arduino_nano_port_name = serialPortInfo.portName();
            }
        }
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if((serialPortInfo.productIdentifier() == arduino_uno_product_id) && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id))
            {
                    arduino_uno_is_available = true;
                    arduino_uno_port_name = serialPortInfo.portName();
            }
        }

    }
    if(arduino_nano_is_available)
    {
        qDebug() << "Found the arduino nano port...\n";
        arduino_nano->setPortName(arduino_nano_port_name);
        arduino_nano->open(QSerialPort::ReadOnly);
        arduino_nano->setBaudRate(QSerialPort::Baud9600);
        arduino_nano->setDataBits(QSerialPort::Data8);
        arduino_nano->setFlowControl(QSerialPort::NoFlowControl);
        arduino_nano->setParity(QSerialPort::NoParity);
        arduino_nano->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino_nano, SIGNAL(readyRead()), this, SLOT(readSerial()));

        qDebug() << "Found the arduino uno port...\n";
        arduino_uno->setPortName(arduino_uno_port_name);
        arduino_uno->open(QSerialPort::WriteOnly);
        arduino_uno->setBaudRate(QSerialPort::Baud9600);
        arduino_uno->setDataBits(QSerialPort::Data8);
        arduino_uno->setParity(QSerialPort::NoParity);
        arduino_uno->setStopBits(QSerialPort::OneStop);
        arduino_uno->setFlowControl(QSerialPort::NoFlowControl);
    }
    else
    {
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }

}


Dialog::~Dialog()
{
    if(arduino_nano->isOpen() )
    {
        arduino_nano->close();

    }
    delete ui;
}
void Dialog::readSerial()
{
    QStringList buffer_split = serialBuffer.split(","); //  split the serialBuffer string, parsing with ',' as the separator

    if(buffer_split.length() < 10)
    {
        serialData = arduino_nano->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        qDebug() <<serialBuffer;
        //serialData.clear();
    }
    else
    {
        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        parsed_data1 = buffer_split[0];
        parsed_data2 = buffer_split[1];
        parsed_data3 = buffer_split[2];
        parsed_data4 = buffer_split[3];
        parsed_data5 = buffer_split[4];
        DMS = parsed_data1.toDouble();
        Estop = parsed_data2.toDouble();
        x = (parsed_data3.toDouble() - 512)/2;
        y = (parsed_data4.toDouble() - 512)/2;
        z = (parsed_data5.toDouble() - 512)/2;

        // convert to fahrenheit
        qDebug() << "X: " << x << "\n";
        qDebug() << "Y: " << y << "\n";
        qDebug() << "Z: " << z << "\n";

        parsed_data3 = QString::number(x, 'g', 4);
        parsed_data4 = QString::number(y, 'g', 4);// format precision of temperature_value to 4 digits or fewer
        parsed_data5 = QString::number(z, 'g', 4);

        Dialog::updateStatus1(parsed_data3);
        Dialog::updateStatus2(parsed_data4);
        Dialog::updateStatus3(parsed_data5);




        if(Estop==0)
        {
            ui->axis_lcdNumber_7->display("F");

            if(DMS==0)
            {
                ui->axis_lcdNumber_8->display("F");
            }
            else
            {
                ui->axis_lcdNumber_8->display("O");
            }
        }
        else
        {
            ui->axis_lcdNumber_7->display("O");
            ui->axis_lcdNumber_8->display("-------");
        }

    }

}

void Dialog::updateStatus1(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    if(Estop==0&&DMS==1)
    {
        int value = sensor_reading.toInt();
        ui->axis_lcdNumber->display(value);
        Dialog::updateString(QString("x%1").arg(value));


    }
    else
    {
        ui->axis_lcdNumber->display("-------");
    }

}
void Dialog::updateStatus2(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    if(Estop==0&&DMS==1)
    {
        int value = sensor_reading.toInt();
        ui->axis_lcdNumber_2->display(value);
        Dialog::updateString(QString("y%1").arg(value));

    }
    else
    {
        ui->axis_lcdNumber_2->display("-------");
    }
}
void Dialog::updateStatus3(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    if(Estop==0&&DMS==1)
    {
        int value = sensor_reading.toInt();
        ui->axis_lcdNumber_3->display(value);
        Dialog::updateString(QString("z%1").arg(value));

    }
    else
    {
        ui->axis_lcdNumber_3->display("-------");
    }
}

void Dialog::updateString(QString command)
{
    if(arduino_uno->isWritable())
    {
            arduino_uno->write(command.toStdString().c_str());
            qDebug() << "string: " << command << "\n";
    }
    else
    {
            qDebug() << "Couldn't write to serial!";
    }



}
