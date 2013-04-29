#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->devices = new QMap<QString, QString>();
    this->dd_process = new QProcess(this);
    //this->dd_process->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(this->dd_process,SIGNAL(readyReadStandardError()),this,SLOT(when_receive_stdout()));
    connect(this->dd_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(when_process_finished(int, QProcess::ExitStatus)));

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
    throw "Platform not supported" ;
}

void MainWindow::refresh_devices_mac(){
    
    QDir dev_id_dir = QDir("/dev/");
    QStringList dev_id_lists;
    dev_id_lists = dev_id_dir.entryList(QStringList("rdisk*"),QDir::System);
    
    QString usb_list_raw = this->run_and_get_stdout((QStringList() << "system_profiler" << "SPUSBDataType"));
    
    QList<QStringList> usb_devices = this->regex_helper("BSD Name: (\\w+)\n\\s+Product ID: (\\w+)\n\\s+Vendor ID: (\\w+)([^\n]+)",usb_list_raw);
    
    for (int p = 0; p < usb_devices.size(); ++p){
        QStringList usb_device = usb_devices[p];
        QString real_device = "/dev/r" + usb_device[1];
        QString human_readable_name = "(" + usb_device[1] + ") " + usb_device[3] + ":" + usb_device[2] + usb_device[4];
        this->devices->insert(real_device, human_readable_name);
        ui->comboBox->addItem(human_readable_name);
    }
    

    //TODO: implement all device in OSX. Need to change regex to xml output of system_profiler with SPSerialATADataType
    if (ui->checkBox->isChecked())
    {
        QMessageBox::information (this,"Not Implemented","Showing all devices is not supported in Mac OSX");
    }
    
    
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

        this->devices->insert(real_device, human_readable_name);
        ui->comboBox->addItem(human_readable_name);

    }

}

QList<QStringList> MainWindow::regex_helper(QString pattern, QString str){
    QRegExp rx(pattern);
    QList<QStringList> list;
    int pos = 0;

    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.capturedTexts();
        pos += rx.matchedLength();
    }
    
    return list;
}

QString MainWindow::run_and_get_stdout(QStringList arguments){
    QProcess exec;
    QStringList sh_arg;
    sh_arg << "-c" << arguments.join(" ");
    exec.start("/bin/sh",sh_arg);
    exec.waitForFinished();
    return QString(exec.readAllStandardOutput());
    
}

void MainWindow::update_task_list(){
    QString task_string = "Will Write " + ui->lineEdit->text() + " to " + this->devices->key( ui->comboBox->currentText());
    this->toogle_ui_lock(false);
    ui->lbl_task_list->setText(task_string);

}

void MainWindow::run_async_process(QString cmd){
    QStringList sh_arg;
    sh_arg << "-c" << cmd;
    this->dd_process->start("/bin/sh",sh_arg);
}

void MainWindow::toogle_ui_lock(bool islock){
    ui->pushButton->setEnabled(!islock);
    ui->pushButton_2->setEnabled(!islock);
    ui->pushButton_4->setEnabled(!islock);
    ui->pushButton_5->setEnabled(!islock);
    ui->comboBox->setEnabled(!islock);
    ui->checkBox->setEnabled(!islock);

    if (!islock && !(ui->lineEdit->text().length() > 0 && ui->comboBox->currentText().length() > 0)){
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_5->setEnabled(false);

    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::when_process_finished( int exitCode, QProcess::ExitStatus exitStatus){
    qDebug() << this->dd_process->readAllStandardError();
    qDebug() << this->dd_process->readAllStandardOutput();
    ui->lbl_task_list->setText("All Done");
    this->toogle_ui_lock(false);
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

void MainWindow::when_receive_stdout(){
    QString progress_str = QString(this->dd_process->readAllStandardError());
    QList<QStringList> progress_vars_list = this->regex_helper("(\\d+:\\d+:\\d+)\\s\\[([^\\]]+)\\].* (\\d+)\\%(.+ETA.+\\d+:\\d+:\\d+)?",progress_str);
    QString eta_status_str = "Time Elapsed : %1        Estimated Time Remaining : %2          %3";
    if (progress_vars_list.size() > 0){
        QStringList progress_vars = progress_vars_list[0];
        progress_vars.pop_front();
        qDebug() << progress_vars;
        ui->progressBar->setValue(progress_vars[2].toInt());

        QString time_e = progress_vars[0].trimmed();
        QString speed_p_s = progress_vars[1].trimmed();
        QString time_eta = progress_vars[3].contains("ETA") ? progress_vars[3].trimmed() : "0:00:00";

        eta_status_str = eta_status_str.arg(time_e,time_eta,speed_p_s);

        ui->lbl_eta_status->setText(eta_status_str);

    }
}

void MainWindow::on_pushButton_2_clicked()
{
    //write to device

    QString dd_command = QString("pv -f < \"%1\" | dd of=\"%2\" bs=2m").arg(ui->lineEdit->text(), this->devices->key( ui->comboBox->currentText()));

    this->toogle_ui_lock(true);

    this->run_async_process(dd_command);
}
