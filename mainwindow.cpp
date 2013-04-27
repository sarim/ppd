#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    refresh_devices();

}

void MainWindow::refresh_devices(){
    //TODO: Check platform and call corresponding function;
    refresh_devices_nix();
}

void MainWindow::refresh_devices_mac(){
    //TODO: need to implement
}

void MainWindow::refresh_devices_nix(){

    ui->comboBox->clear();
    this->devices->clear();

    QDir dev_id_dir = QDir("/dev/disk/by-id/");
    QStringList dev_id_lists;
    dev_id_lists = dev_id_dir.entryList(QStringList("usb-*"),QDir::Files);

    if (ui->checkBox->isChecked())
    {
        QStringList dev_id_lists_ata = dev_id_dir.entryList(QStringList("ata-*"),QDir::Files);
        dev_id_lists += dev_id_lists_ata;
    }

    for (int i = 0; i < dev_id_lists.size(); ++i) {
        QFileInfo symlink_checker = QFileInfo(dev_id_dir,dev_id_lists[i]);
        QString real_device = symlink_checker.symLinkTarget();
        if ( !ui->checkBox->isChecked() && real_device.contains(QRegExp("[0-9]$")) )
            continue;

        QStringList name_parts = dev_id_lists[i].split("-");
        name_parts.pop_back();
        name_parts.pop_front();
        name_parts.prepend("(" + real_device + ") ");
        QString human_readable_name = name_parts.join("");
        ui->comboBox->addItem(human_readable_name);

        this->devices[real_device] = human_readable_name;

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    QApplication::quit();
}

void MainWindow::on_pushButton_4_clicked()
{
    this->refresh_devices();
}
