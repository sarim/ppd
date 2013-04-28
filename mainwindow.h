#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QMap<QString, QString> *devices;
    void refresh_devices();
    void refresh_devices_win();
    void refresh_devices_mac();
    void refresh_devices_nix();
    void update_task_list();

};

#endif // MAINWINDOW_H
