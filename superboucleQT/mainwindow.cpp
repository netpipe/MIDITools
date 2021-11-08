#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollArea>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(ui->groupBox);
  //  scrollArea->sizePolicy() = QSizePolicy::Expanding;
  // scrollArea->setAutoFillBackground(1);
   //        ui->groupBox->setAutoFillBackground(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

