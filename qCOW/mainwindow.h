#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#ifdef RTMIDI
#include "./rtmidi/RtMidi.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   void sendMessage2();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();

    void on_listPortsBTN_clicked();

    void on_virtInBTN_clicked();



    void on_getlastmessage_clicked();

private:
    Ui::MainWindow *ui;
    #ifdef RTMIDI
    RtMidiIn  *midiin = 0;
    RtMidiOut *midiout = 0;
    #endif
};
#endif // MAINWINDOW_H
