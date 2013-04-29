#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>

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

    void when_receive_stdout();

    void when_process_finished( int exitCode, QProcess::ExitStatus exitStatus );

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    void refresh_devices();
    void refresh_devices_win();
    void refresh_devices_mac();
    void refresh_devices_nix();
    void update_task_list();
    void run_async_process(QString cmd);
    void toogle_ui_lock(bool islock);
    QMap<QString, QString> *devices;
	QString run_and_get_stdout(QStringList arguments);
	QList<QStringList> regex_helper(QString pattern, QString text);
    QProcess *dd_process;

};

#endif // MAINWINDOW_H
