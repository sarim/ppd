#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->devices = new QMap<QString, QString>();
    refresh_devices();

}

void MainWindow::refresh_devices(){
    //TODO: Check platform and call corresponding function;

    ui->comboBox->clear();
    this->devices->clear();

    #ifdef Q_WS_MAC
     refresh_devices_mac();
    #endif

    #ifdef Q_WS_WIN
     refresh_devices_win();
    #endif

    #ifdef Q_WS_X11
     refresh_devices_nix();
    #endif
}

void MainWindow::refresh_devices_win(){
    //TODO: need to implement
    QMessageBox::critical (this,"Not Implemented","Go away. PPD doesn't support your OS");
    qApp->quit();
}

void MainWindow::refresh_devices_mac(){
    //TODO: need to implement
    QMessageBox::critical (this,"Not Implemented","Go away. PPD doesn't support your OS");
    qApp->quit();
}

void MainWindow::refresh_devices_nix(){

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

        this->devices->insert(human_readable_name , real_device );
        ui->comboBox->addItem(human_readable_name);

    }

}

void MainWindow::update_task_list(){
    QString task_string = "Will Write " + ui->lineEdit->text() + " to " + this->devices->value( ui->comboBox->currentText());

    ui->lbl_task_list->setText(task_string);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    qApp->quit();
}

void MainWindow::on_pushButton_4_clicked()
{
    //Refresh device button

    this->refresh_devices();
}

void MainWindow::on_pushButton_clicked()
{
    //open file dialog for ISO selection.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                    tr("ISO Images (*.iso);;ALl Files (*.*)"));
    ui->lineEdit->setText(fileName);
    this->update_task_list();

}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    this->update_task_list();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    this->refresh_devices();
}
