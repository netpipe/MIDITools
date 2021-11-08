/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -m -a vmpk_adaptor -i vpiano.h -l VPiano /home/clayton/Desktop/vmpk-0.5.1/src/net.sourceforge.vmpk.xml
 *
 * qdbusxml2cpp is Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef VMPK_ADAPTOR_H_1378624530
#define VMPK_ADAPTOR_H_1378624530

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "vpiano.h"
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface net.sourceforge.vmpk
 */
class VmpkAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "net.sourceforge.vmpk")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"net.sourceforge.vmpk\">\n"
"    <!-- window/widget interface controls -->\n"
"    <method name=\"close\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"hide\"/>\n"
"    <method name=\"lower\"/>\n"
"    <method name=\"raise\"/>\n"
"    <method name=\"repaint\"/>\n"
"    <method name=\"setDisabled\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"disable\"/>\n"
"    </method>\n"
"    <method name=\"setEnabled\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"enable\"/>\n"
"    </method>\n"
"    <method name=\"setFocus\"/>\n"
"    <method name=\"setHidden\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"hidden\"/>\n"
"    </method>\n"
"    <method name=\"setStyleSheet\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"styleSheet\"/>\n"
"    </method>\n"
"    <method name=\"setVisible\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"visible\"/>\n"
"    </method>\n"
"    <method name=\"setWindowModified\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"modified\"/>\n"
"    </method>\n"
"    <method name=\"setWindowTitle\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"title\"/>\n"
"    </method>\n"
"    <method name=\"show\"/>\n"
"    <method name=\"showFullScreen\"/>\n"
"    <method name=\"showMaximized\"/>\n"
"    <method name=\"showMinimized\"/>\n"
"    <method name=\"showNormal\"/>\n"
"    <method name=\"update\"/>\n"
"    <method name=\"resize\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"width\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"height\"/>\n"
"    </method>\n"
"    <method name=\"move\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
"    </method>\n"
"    <!-- basic program interface controls -->\n"
"    <method name=\"quit\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"panic\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"reset_controllers\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"    </method>\n"
"    <method name=\"channel\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"octave\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"transpose\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"velocity\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"connect_in\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"connect_out\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"connect_thru\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"b\" name=\"value\"/>\n"
"    </method>\n"
"    <!-- standard MIDI channel event generators -->\n"
"    <method name=\"noteoff\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"note\"/>\n"
"    </method>\n"
"    <method name=\"noteon\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"note\"/>\n"
"    </method>\n"
"    <method name=\"polykeypress\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"note\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"controlchange\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"control\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"programchange\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"programnamechange\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"chankeypress\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <method name=\"pitchwheel\">\n"
"      <annotation value=\"true\" name=\"org.freedesktop.DBus.Method.NoReply\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"value\"/>\n"
"    </method>\n"
"    <!-- standard MIDI channel events -->\n"
"    <signal name=\"event_noteoff\">\n"
"      <arg type=\"i\" name=\"note\"/>\n"
"    </signal>\n"
"    <signal name=\"event_noteon\">\n"
"      <arg type=\"i\" name=\"note\"/>\n"
"    </signal>\n"
"    <signal name=\"event_polykeypress\">\n"
"      <arg type=\"i\" name=\"note\"/>\n"
"      <arg type=\"i\" name=\"value\"/>\n"
"    </signal>\n"
"    <signal name=\"event_controlchange\">\n"
"      <arg type=\"i\" name=\"control\"/>\n"
"      <arg type=\"i\" name=\"value\"/>\n"
"    </signal>\n"
"    <signal name=\"event_programchange\">\n"
"      <arg type=\"i\" name=\"value\"/>\n"
"    </signal>\n"
"    <signal name=\"event_chankeypress\">\n"
"      <arg type=\"i\" name=\"value\"/>\n"
"    </signal>\n"
"    <signal name=\"event_pitchwheel\">\n"
"      <arg type=\"i\" name=\"value\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    VmpkAdaptor(VPiano *parent);
    virtual ~VmpkAdaptor();

    inline VPiano *parent() const
    { return static_cast<VPiano *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    Q_NOREPLY void chankeypress(int value);
    Q_NOREPLY void channel(int value);
    Q_NOREPLY void close();
    Q_NOREPLY void connect_in(const QString &value);
    Q_NOREPLY void connect_out(const QString &value);
    Q_NOREPLY void connect_thru(bool value);
    Q_NOREPLY void controlchange(int control, int value);
    void hide();
    void lower();
    void move(int x, int y);
    Q_NOREPLY void noteoff(int note);
    Q_NOREPLY void noteon(int note);
    Q_NOREPLY void octave(int value);
    Q_NOREPLY void panic();
    Q_NOREPLY void pitchwheel(int value);
    Q_NOREPLY void polykeypress(int note, int value);
    Q_NOREPLY void programchange(int value);
    Q_NOREPLY void programnamechange(const QString &value);
    Q_NOREPLY void quit();
    void raise();
    void repaint();
    Q_NOREPLY void reset_controllers();
    void resize(int width, int height);
    void setDisabled(bool disable);
    void setEnabled(bool enable);
    void setFocus();
    void setHidden(bool hidden);
    void setStyleSheet(const QString &styleSheet);
    void setVisible(bool visible);
    void setWindowModified(bool modified);
    void setWindowTitle(const QString &title);
    void show();
    void showFullScreen();
    void showMaximized();
    void showMinimized();
    void showNormal();
    Q_NOREPLY void transpose(int value);
    void update();
    Q_NOREPLY void velocity(int value);
Q_SIGNALS: // SIGNALS
    void event_chankeypress(int value);
    void event_controlchange(int control, int value);
    void event_noteoff(int note);
    void event_noteon(int note);
    void event_pitchwheel(int value);
    void event_polykeypress(int note, int value);
    void event_programchange(int value);
};

#endif
