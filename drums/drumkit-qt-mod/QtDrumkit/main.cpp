/**
 * Copyright (c) 2011 Nokia Corporation.
 * All rights reserved.
 *
 * Part of the QtDrumkit
 *
 * For the applicable distribution terms see the license text file included in
 * the distribution.
 */

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
//#include <QGLFormat>

#include "drumengine.h"
#include "toucheventfilter.h"
//#include "touchevents.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //QGLFormat fmt = QGLFormat::defaultFormat();
    //fmt.setDirectRendering(true);
    //fmt.setDoubleBuffer(true);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("me.lduboeuf.drumkit");
    app.setApplicationName("drumkit");





    // Register QML bindings fo DrumEngine and TouchEvents
//#ifdef Q_OS_UBUNTU_TOUCH
 //   qmlRegisterType<DrumEngine>("DrumEngine", 1,0, "DrumEngine");
//#endif
    //qmlRegisterType<TouchEvents>("TouchEvents", 1,0, "TouchEvents");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();


// Provide information whether running in simulator. Used in Pad.qml.
#ifdef QT_SIMULATOR
    context->setContextProperty("simulator", true);
#else
    context->setContextProperty("simulator", false);
#endif


    TouchEventFilter touchEventFilter;
    context->setContextProperty("touchEventFilter", &touchEventFilter);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    //intercept mouse event and propagate to ApplicationWindow
    QObject *root = engine.rootObjects()[0];
    root->installEventFilter(&touchEventFilter);

    //viewer.setMainQmlFile(QLatin1String("qml/harmattan/main.qml"));

    // Enable OpenGL rendering
    /*
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setDirectRendering(true);
    fmt.setDoubleBuffer(true);
    QGLWidget *glWidget = new QGLWidget(fmt);
    viewer.setViewport(glWidget);
    viewer.showFullScreen();
    */

    return app.exec();
}

