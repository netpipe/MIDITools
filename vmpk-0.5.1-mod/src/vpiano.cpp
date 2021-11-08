/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "vpiano.h"
#include "instrument.h"
#include "mididefs.h"
#include "knob.h"
#include "classicstyle.h"
#include "RtMidi.h"
#include "constants.h"
#include "riffimportdlg.h"
#include "extracontrols.h"
#include "about.h"
#include "preferences.h"
#include "midisetup.h"
#include "events.h"
#include "colordialog.h"

#if !defined(SMALL_SCREEN)
#include "kmapdialog.h"
#include "shortcutdialog.h"
#endif

#if ENABLE_DBUS
#include "vmpk_adaptor.h"
#include <QtDBus/QDBusConnection>
#endif

#if defined(NETWORK_MIDI)
#include "netsettings.h"
#include "udpmidi.h"
#endif

#include <QDesktopServices>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QDial>
#include <QToolButton>
#include <QToolTip>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QDialog>
#include <QUrl>
#include <QString>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMapIterator>
#include <QDebug>

VPiano::VPiano( QWidget * parent, Qt::WindowFlags flags )
    : QMainWindow(parent, flags),
    m_midiout(0),
    m_midiin(0),
    m_currentOut(-1),
    m_currentIn(-1),
    m_inputActive(false),
    m_midiThru(false),
    m_midiOmni(false),
    m_initialized(false),
    m_dlgAbout(0),
    m_dlgPreferences(0),
    m_dlgMidiSetup(0),
    m_dlgKeyMap(0),
    m_dlgExtra(0),
    m_dlgRiffImport(0),
    m_dlgColorPolicy(0)
{
#if ENABLE_DBUS
    new VmpkAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/", this);
    dbus.registerService("net.sourceforge.vmpk");
#endif
    m_trq = new QTranslator(this);
    m_trp = new QTranslator(this);
    m_trq->load( QSTR_QTPX + configuredLanguage(),
                 QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
    m_trp->load( QSTR_VMPKPX + configuredLanguage(),
                 VPiano::localeDirectory() );
    QCoreApplication::installTranslator(m_trq);
    QCoreApplication::installTranslator(m_trp);
    ui.setupUi(this);
    initLanguages();
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(slotAbout()));
    connect(ui.actionAboutQt, SIGNAL(triggered()), SLOT(slotAboutQt()));
    connect(ui.actionConnections, SIGNAL(triggered()), SLOT(slotConnections()));
    connect(ui.actionPreferences, SIGNAL(triggered()), SLOT(slotPreferences()));
    connect(ui.actionEditKM, SIGNAL(triggered()), SLOT(slotEditKeyboardMap()));
    connect(ui.actionContents, SIGNAL(triggered()), SLOT(slotHelpContents()));
    connect(ui.actionWebSite, SIGNAL(triggered()), SLOT(slotOpenWebSite()));
    connect(ui.actionImportSoundFont, SIGNAL(triggered()), SLOT(slotImportSF()));
    connect(ui.actionEditExtraControls, SIGNAL(triggered()), SLOT(slotEditExtraControls()));
    connect(ui.actionNoteNames, SIGNAL(triggered()), SLOT(slotShowNoteNames()));
    connect(ui.actionShortcuts, SIGNAL(triggered()), SLOT(slotShortcuts()));
    connect(ui.actionKeyboardInput, SIGNAL(toggled(bool)), SLOT(slotKeyboardInput(bool)));
    connect(ui.actionMouseInput, SIGNAL(toggled(bool)), SLOT(slotMouseInput(bool)));
    connect(ui.actionTouchScreenInput, SIGNAL(toggled(bool)), SLOT(slotTouchScreenInput(bool)));
    connect(ui.actionColorPalette, SIGNAL(triggered()), SLOT(slotColorPolicy()));
    connect(ui.actionColorScale, SIGNAL(toggled(bool)), SLOT(slotColorScale(bool)));
    // Toolbars actions: toggle view
    connect(ui.toolBarNotes->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionNotes, SLOT(setChecked(bool)));
    connect(ui.toolBarControllers->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionControllers, SLOT(setChecked(bool)));
    connect(ui.toolBarBender->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionBender, SLOT(setChecked(bool)));
    connect(ui.toolBarPrograms->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionPrograms, SLOT(setChecked(bool)));
    connect(ui.toolBarExtra->toggleViewAction(), SIGNAL(toggled(bool)),
            ui.actionExtraControls, SLOT(setChecked(bool)));
#if defined(SMALL_SCREEN)
    ui.toolBarControllers->hide();
    ui.toolBarBender->hide();
    ui.toolBarExtra->hide();
    //ui.toolBarNotes->hide();
    //ui.toolBarPrograms->hide();
    ui.actionEditKM->setVisible(false);
    ui.actionShortcuts->setVisible(false);
    ui.actionStatusBar->setVisible(false);
    setWindowTitle("VMPK " + PGM_VERSION);
#endif
    currentPianoScene()->setPianoHandler(this);
    initialization();
}

VPiano::~VPiano()
{
    //qDebug() << Q_FUNC_INFO;
    try {
        if (m_midiout != 0) {
            m_midiout->closePort();
            delete m_midiout;
        }
        if (m_midiin != 0) {
            if (m_inputActive) {
                m_midiin->cancelCallback();
                m_inputActive = false;
            }
            if (m_currentIn > -1)
                m_midiin->closePort();
            delete m_midiin;
        }
    } catch (RtError& err) {
        qWarning() << "XXX" << QString::fromStdString(err.getMessage());
    }
}

void VPiano::initialization()
{
    readSettings();
    if ((m_initialized = initMidi())) {
        refreshConnections();
        readConnectionSettings();
        readMidiControllerSettings();
        initToolBars();
        applyPreferences();
        applyConnections();
        applyInitialSettings();
        initExtraControllers();
        enforceMIDIChannelState();
        activateWindow();
    }
}

void midiCallback( double /*deltatime*/,
                   std::vector< unsigned char > *message,
                   void *userData )
{
    QEvent* ev = 0;
    VPiano* instance = static_cast<VPiano*>(userData);
    instance->midiThru(message);
    unsigned char channel = message->at(0) & MASK_CHANNEL;
    unsigned char status = message->at(0) & MASK_STATUS;
    bool messageAccepted = (instance->baseChannel() == channel) ||
                           (instance->omniMode() &&
                             (status == STATUS_NOTEON || status == STATUS_NOTEOFF));
    if (messageAccepted) {
        switch( status ) {
        case STATUS_NOTEOFF:
        case STATUS_NOTEON: {
                unsigned char midi_note = message->at(1);
                unsigned char vel = message->at(2);
                if ((status == STATUS_NOTEOFF) || (vel == 0))
                    ev = new NoteOffEvent(channel, midi_note, vel);
                else
                    ev = new NoteOnEvent(channel, midi_note, vel);
            }
            break;
        case STATUS_POLYAFT: {
                unsigned char note = message->at(1);
                unsigned char value = message->at(2);
                ev = new PolyKeyPressEvent(note, value);
            }
            break;
        case STATUS_CTLCHG: {
                unsigned char ctl = message->at(1);
                unsigned char val = message->at(2);
                ev = new ControlChangeEvent(ctl, val);
            }
            break;
        case STATUS_PROGRAM: {
                unsigned char value = message->at(1);
                ev = new ProgramChangeEvent(value);
            }
            break;
        case STATUS_CHANAFT: {
                unsigned char value = message->at(1);
                ev = new ChannelKeyPressEvent(value);
            }
            break;
        case STATUS_BENDER: {
                int value = (message->at(1) + 0x80 * message->at(2)) - BENDER_MID;
                ev = new PitchWheelEvent(value);
            }
            break;
        }
    }
    if (ev != 0)
        QApplication::postEvent(instance, ev);
}

RtMidiOut *
VPiano::MIDIOutDriverFactory(const QString driverName, const QString clientName)
{
    RtMidiOut *driver = 0;
#if defined(__LINUX_ALSASEQ__)
    if (driverName == QSTR_DRIVERNAMEALSA)
        driver = new RtMidiOutAlsa(clientName.toStdString());
#endif
#if defined(__LINUX_JACK__)
    if (driverName == QSTR_DRIVERNAMEJACK)
        driver = new RtMidiOutJack(clientName.toStdString());
#endif
#if defined(__IRIX_MD__)
    if (driverName == QSTR_DRIVERNAMEIRIX)
        driver = new RtMidiOutIrix(clientName.toStdString());
#endif
#if defined(__MACOSX_CORE__)
    if (driverName == QSTR_DRIVERNAMEMACOSX)
        driver = new RtMidiOutCoreMidi(clientName.toStdString());
#endif
#if defined(__WINDOWS_MM__)
    if (driverName == QSTR_DRIVERNAMEWINMM)
        driver = new RtMidiOutWinMM(clientName.toStdString());
#endif
#if defined(NETWORK_MIDI)
    if (driverName == QSTR_DRIVERNAMENET)
        driver = new NetMidiOut(clientName.toStdString());
#endif
    if (driver == 0 && driverName != QSTR_DRIVERDEFAULT)
        driver = MIDIOutDriverFactory(QSTR_DRIVERDEFAULT, clientName);
    return driver;
}

RtMidiIn *
VPiano::MIDIInDriverFactory(const QString driverName, const QString clientName)
{
    RtMidiIn *driver = 0;
#if defined(__LINUX_ALSASEQ__)
    if (driverName == QSTR_DRIVERNAMEALSA)
        driver = new RtMidiInAlsa(clientName.toStdString());
#endif
#if defined(__LINUX_JACK__)
    if (driverName == QSTR_DRIVERNAMEJACK)
        driver = new RtMidiInJack(clientName.toStdString());
#endif
#if defined(__IRIX_MD__)
    if (driverName == QSTR_DRIVERNAMEIRIX)
        driver = new RtMidiInIrix(clientName.toStdString());
#endif
#if defined(__MACOSX_CORE__)
    if (driverName == QSTR_DRIVERNAMEMACOSX)
        driver = new RtMidiInCoreMidi(clientName.toStdString());
#endif
#if defined(__WINDOWS_MM__)
    if (driverName == QSTR_DRIVERNAMEWINMM)
        driver = new RtMidiInWinMM(clientName.toStdString());
#endif
#if defined(NETWORK_MIDI)
    if (driverName == QSTR_DRIVERNAMENET)
        driver = new NetMidiIn(clientName.toStdString());
#endif
    if (driver == 0 && driverName != QSTR_DRIVERDEFAULT)
        driver = MIDIInDriverFactory(QSTR_DRIVERDEFAULT, clientName);
    return driver;
}

bool VPiano::initMidi()
{
    try {
        m_midiDriver = dlgPreferences()->getDriver();
        m_midiout = MIDIOutDriverFactory(m_midiDriver, QSTR_VMPKOUTPUT);
        m_midiin = MIDIInDriverFactory(m_midiDriver, QSTR_VMPKINPUT);
        if (m_midiDriver != QSTR_DRIVERNAMEALSA && m_midiDriver != QSTR_DRIVERNAMEMACOSX && m_midiDriver != QSTR_DRIVERNAMEJACK)
        {
            int nOutPorts = m_midiout->getPortCount();
            if (nOutPorts == 0) {
                delete m_midiout;
                m_midiout = 0;
                QMessageBox::critical( this, tr("Error"),
                    tr("No MIDI output ports available. Aborting") );
                return false;
            }
            int nInPorts = m_midiin->getPortCount();
            if (nInPorts == 0) {
                delete m_midiin;
                m_midiin = 0;
            }
        }
        if (m_midiDriver == QSTR_DRIVERNAMEALSA || m_midiDriver == QSTR_DRIVERNAMEMACOSX || m_midiDriver == QSTR_DRIVERNAMEJACK)
        {
            if (m_midiout != 0)
                m_midiout->openVirtualPort(QSTR_VMPKOUTPUT.toStdString());
            if (m_midiin != 0)
                m_midiin->openVirtualPort(QSTR_VMPKINPUT.toStdString());
        }
        else
        {
            m_midiout->openPort( m_currentOut = 0 );
        }
        if (m_midiin != 0) {
            // ignore SYX, clock and active sense
            m_midiin->ignoreTypes(true,true,true);
            m_midiin->setCallback( &midiCallback, this );
            m_inputActive = true;
        }
    } catch (RtError& err) {
        QMessageBox::critical( this, tr("Error. Aborting"),
                               QString::fromStdString(err.getMessage()));
        return false;
    }
    return (m_midiout != 0);
}

void VPiano::switchMIDIDriver()
{
    try {
        if (m_midiout != 0) {
            m_midiout->closePort();
            delete m_midiout;
            m_midiout = 0;
        }
        if (m_midiin != 0) {
            if (m_inputActive) {
                m_midiin->cancelCallback();
                m_inputActive = false;
            }
            if (m_currentIn > -1) {
                m_midiin->closePort();
                delete m_midiin;
                m_midiin = 0;
            }
        }
    } catch (RtError& err) {
        qWarning() << "XXX" << QString::fromStdString(err.getMessage());
    }
    if ((m_initialized = initMidi())) {
        refreshConnections();
        applyConnections();
        enforceMIDIChannelState();
    }
}

void VPiano::initToolBars()
{
    m_dialStyle = new ClassicStyle();
    m_dialStyle->setParent(this);
    // Notes tool bar
    QWidget *w = ui.toolBarNotes->widgetForAction(ui.actionPanic);
    w->setMaximumWidth(120);
    m_lblChannel = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblChannel);
    m_lblChannel->setMargin(TOOLBARLABELMARGIN);
    m_sboxChannel = new QSpinBox(this);
    m_sboxChannel->setMinimum(1);
    m_sboxChannel->setMaximum(MIDICHANNELS);
    m_sboxChannel->setValue(m_baseChannel + 1);
    m_sboxChannel->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_sboxChannel);
    m_lblBaseOctave = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblBaseOctave);
    m_lblBaseOctave->setMargin(TOOLBARLABELMARGIN);
    m_sboxOctave = new QSpinBox(this);
    m_sboxOctave->setMinimum(0);
    m_sboxOctave->setMaximum(9);
    m_sboxOctave->setValue(m_baseOctave);
    m_sboxOctave->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_sboxOctave);
    m_lblTranspose = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblTranspose);
    m_lblTranspose->setMargin(TOOLBARLABELMARGIN);
    m_sboxTranspose = new QSpinBox(this);
    m_sboxTranspose->setMinimum(-11);
    m_sboxTranspose->setMaximum(11);
    m_sboxTranspose->setValue(m_transpose);
    m_sboxTranspose->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_sboxTranspose);
    m_lblVelocity = new QLabel(this);
    ui.toolBarNotes->addWidget(m_lblVelocity);
    m_lblVelocity->setMargin(TOOLBARLABELMARGIN);
    m_Velocity = new Knob(this);
    m_Velocity->setFixedSize(32, 32);
    m_Velocity->setStyle(dlgPreferences()->getStyledWidgets()? m_dialStyle : 0);
    m_Velocity->setMinimum(0);
    m_Velocity->setMaximum(127);
    m_Velocity->setDefaultValue(MIDIVELOCITY);
    m_Velocity->setDialMode(Knob::LinearMode);
    m_Velocity->setValue(m_velocity);
    m_Velocity->setToolTip(QString::number(m_velocity));
    m_Velocity->setFocusPolicy(Qt::NoFocus);
    ui.toolBarNotes->addWidget(m_Velocity);
    connect( m_sboxChannel, SIGNAL(valueChanged(int)),
             SLOT(slotChannelValueChanged(int)) );
    connect( m_sboxOctave, SIGNAL(valueChanged(int)),
             SLOT(slotBaseOctaveValueChanged(int)) );
    connect( m_sboxTranspose, SIGNAL(valueChanged(int)),
             SLOT(slotTransposeValueChanged(int)) );
    connect( m_Velocity, SIGNAL(valueChanged(int)),
             SLOT(slotVelocityValueChanged(int)) );
    connect( ui.actionChannelUp, SIGNAL(triggered()),
             m_sboxChannel, SLOT(stepUp()) );
    connect( ui.actionChannelDown, SIGNAL(triggered()),
             m_sboxChannel, SLOT(stepDown()) );
    connect( ui.actionOctaveUp, SIGNAL(triggered()),
             m_sboxOctave, SLOT(stepUp()) );
    connect( ui.actionOctaveDown, SIGNAL(triggered()),
             m_sboxOctave, SLOT(stepDown()) );
    connect( ui.actionTransposeUp, SIGNAL(triggered()),
             m_sboxTranspose, SLOT(stepUp()) );
    connect( ui.actionTransposeDown, SIGNAL(triggered()),
             m_sboxTranspose, SLOT(stepDown()) );
    connect( ui.actionVelocityUp, SIGNAL(triggered()),
             SLOT(slotVelocityUp()) );
    connect( ui.actionVelocityDown, SIGNAL(triggered()),
             SLOT(slotVelocityDown()) );
    // Controllers tool bar
    m_lblControl = new QLabel(this);
    ui.toolBarControllers->addWidget(m_lblControl);
    m_lblControl ->setMargin(TOOLBARLABELMARGIN);
    m_comboControl = new QComboBox(this);
    m_comboControl->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboControl->setFocusPolicy(Qt::NoFocus);
    ui.toolBarControllers->addWidget(m_comboControl);
    m_lblValue = new QLabel(this);
    ui.toolBarControllers->addWidget(m_lblValue);
    m_lblValue->setMargin(TOOLBARLABELMARGIN);
    m_Control= new Knob(this);
    m_Control->setFixedSize(32, 32);
    m_Control->setStyle(dlgPreferences()->getStyledWidgets()? m_dialStyle : 0);
    m_Control->setMinimum(0);
    m_Control->setMaximum(127);
    m_Control->setValue(0);
    m_Control->setToolTip("0");
    m_Control->setDefaultValue(0);
    m_Control->setDialMode(Knob::LinearMode);
    m_Control->setFocusPolicy(Qt::NoFocus);
    ui.toolBarControllers->addWidget(m_Control);
    connect( m_comboControl, SIGNAL(currentIndexChanged(int)),
             SLOT(slotComboControlCurrentIndexChanged(int)) );
    connect( m_Control, SIGNAL(sliderMoved(int)),
             SLOT(slotControlSliderMoved(int)) );
    // Pitch bender tool bar
    m_lblBender = new QLabel(this);
    ui.toolBarBender->addWidget(m_lblBender);
    m_lblBender->setMargin(TOOLBARLABELMARGIN);
    m_bender = new QSlider(this);
    m_bender->setOrientation(Qt::Horizontal);
    m_bender->setMaximumWidth(200);
    m_bender->setMinimum(BENDER_MIN);
    m_bender->setMaximum(BENDER_MAX);
    m_bender->setValue(0);
    m_bender->setToolTip("0");
    m_bender->setFocusPolicy(Qt::NoFocus);
    ui.toolBarBender->addWidget(m_bender);
    connect( m_bender, SIGNAL(sliderMoved(int)),
             SLOT(slotBenderSliderMoved(int)) );
    connect( m_bender, SIGNAL(sliderReleased()),
             SLOT(slotBenderSliderReleased()) );
    // Programs tool bar
    m_lblBank = new QLabel(this);
    ui.toolBarPrograms->addWidget(m_lblBank);
    m_lblBank->setMargin(TOOLBARLABELMARGIN);
    m_comboBank = new QComboBox(this);
    m_comboBank->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboBank->setFocusPolicy(Qt::NoFocus);
    ui.toolBarPrograms->addWidget(m_comboBank);
    m_lblProgram = new QLabel(this);
    ui.toolBarPrograms->addWidget(m_lblProgram);
    m_lblProgram->setMargin(TOOLBARLABELMARGIN);
    m_comboProg = new QComboBox(this);
    m_comboProg->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboProg->setFocusPolicy(Qt::NoFocus);
    ui.toolBarPrograms->addWidget(m_comboProg);
    connect( m_comboBank, SIGNAL(activated(int)),
             SLOT(slotComboBankActivated(int)) );
    connect( m_comboProg, SIGNAL(activated(int)),
             SLOT(slotComboProgActivated(int)) );
    // Toolbars actions: buttons
    connect( ui.actionPanic, SIGNAL(triggered()),
             SLOT(slotPanic()));
    connect( ui.actionResetAll, SIGNAL(triggered()),
             SLOT(slotResetAllControllers()));
    connect( ui.actionReset, SIGNAL(triggered()),
             SLOT(slotResetBender()));
    connect( ui.actionEditExtra, SIGNAL(triggered()),
             SLOT(slotEditExtraControls()));
    // Tools actions
    connect( ui.actionNextBank, SIGNAL(triggered()),
             SLOT(slotBankNext()) );
    connect( ui.actionPreviousBank, SIGNAL(triggered()),
             SLOT(slotBankPrev()) );
    connect( ui.actionNextProgram, SIGNAL(triggered()),
             SLOT(slotProgramNext()) );
    connect( ui.actionPreviousProgram, SIGNAL(triggered()),
             SLOT(slotProgramPrev()) );
    connect( ui.actionNextController, SIGNAL(triggered()),
             SLOT(slotControllerNext()) );
    connect( ui.actionPreviousController, SIGNAL(triggered()),
             SLOT(slotControllerPrev()) );
    connect( ui.actionControllerDown, SIGNAL(triggered()),
             SLOT(slotControllerDown()) );
    connect( ui.actionControllerUp, SIGNAL(triggered()),
             SLOT(slotControllerUp()) );
    /* connect( ui.actionEditPrograms, SIGNAL(triggered()),
             SLOT(slotEditPrograms())); */
    retranslateToolbars();
}

//void VPiano::slotDebugDestroyed(QObject *obj)
//{
//    qDebug() << Q_FUNC_INFO << obj->metaObject()->className();
//}

void VPiano::clearExtraControllers()
{
    QList<QAction*> allActs = ui.toolBarExtra->actions();
    foreach(QAction* a, allActs) {
        if (a != ui.actionEditExtra) {
            ui.toolBarExtra->removeAction(a);
            delete a;
        }
    }
    ui.toolBarExtra->clear();
    ui.toolBarExtra->addAction(ui.actionEditExtra);
    ui.toolBarExtra->addSeparator();
}

QByteArray VPiano::readSysexDataFile(const QString& fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QByteArray res = file.readAll();
    file.close();
    return res;
}

void VPiano::initExtraControllers()
{
    QWidget *w = 0;
    QCheckBox *chkbox = 0;
    Knob *knob = 0;
    QSpinBox *spin = 0;
    QSlider *slider = 0;
    QToolButton *button = 0;
    foreach(const QString& s, m_extraControls) {
        QString lbl;
        int control = 0;
        int type = 0;
        int minValue = 0;
        int maxValue = 127;
        int defValue = 0;
        int value = 0;
        int size = 100;
        QString fileName;
        ExtraControl::decodeString( s, lbl, control, type,
                                    minValue, maxValue, defValue,
                                    size, fileName );
        if (m_ctlState[m_baseChannel].contains(control))
            value = m_ctlState[m_baseChannel][control];
        else
            value = defValue;
        switch(type) {
        case 0:
            chkbox = new QCheckBox(this);
            chkbox->setStyle(dlgPreferences()->getStyledWidgets() ? m_dialStyle : 0);
            chkbox->setProperty(MIDICTLONVALUE, maxValue);
            chkbox->setProperty(MIDICTLOFFVALUE, minValue);
            chkbox->setChecked(bool(value));
            connect(chkbox, SIGNAL(clicked(bool)), SLOT(slotControlClicked(bool)));
            w = chkbox;
            break;
        case 1:
            knob = new Knob(this);
            knob->setFixedSize(32, 32);
            knob->setStyle(dlgPreferences()->getStyledWidgets() ? m_dialStyle : 0);
            knob->setMinimum(minValue);
            knob->setMaximum(maxValue);
            knob->setValue(value);
            knob->setToolTip(QString::number(value));
            knob->setDefaultValue(defValue);
            knob->setDialMode(Knob::LinearMode);
            connect(knob, SIGNAL(sliderMoved(int)), SLOT(slotExtraController(int)));
            w = knob;
            break;
        case 2:
            spin = new QSpinBox(this);
            spin->setMinimum(minValue);
            spin->setMaximum(maxValue);
            spin->setValue(value);
            connect(spin, SIGNAL(valueChanged(int)), SLOT(slotExtraController(int)));
            w = spin;
            break;
        case 3:
            slider = new QSlider(this);
            slider->setOrientation(Qt::Horizontal);
            slider->setFixedWidth(size);
            slider->setMinimum(minValue);
            slider->setMaximum(maxValue);
            slider->setToolTip(QString::number(value));
            slider->setValue(value);
            connect(slider, SIGNAL(sliderMoved(int)), SLOT(slotExtraController(int)));
            w = slider;
            break;
        case 4:
            button = new QToolButton(this);
            button->setText(lbl);
            button->setProperty(MIDICTLONVALUE, maxValue);
            button->setProperty(MIDICTLOFFVALUE, minValue);
            connect(button, SIGNAL(clicked(bool)), SLOT(slotControlClicked(bool)));
            w = button;
            break;
        case 5:
            control = 255;
            button = new QToolButton(this);
            button->setText(lbl);
            button->setProperty(SYSEXFILENAME, fileName);
            button->setProperty(SYSEXFILEDATA, readSysexDataFile(fileName));
            connect(button, SIGNAL(clicked(bool)), SLOT(slotControlClicked(bool)));
            w = button;
            break;
        default:
            w = 0;
        }
        if (w != 0) {
            if (!lbl.isEmpty() && type < 4) {
                QLabel *qlbl = new QLabel(lbl, this);
                qlbl->setMargin(TOOLBARLABELMARGIN);
                ui.toolBarExtra->addWidget(qlbl);
                //connect(qlbl, SIGNAL(destroyed(QObject*)), SLOT(slotDebugDestroyed(QObject*)));
            }
            w->setProperty(MIDICTLNUMBER, control);
            w->setFocusPolicy(Qt::NoFocus);
            ui.toolBarExtra->addWidget(w);
            //connect(w, SIGNAL(destroyed(QObject*)), SLOT(slotDebugDestroyed(QObject*)));
        }
    }
}

void VPiano::readSettings()
{
    QSettings settings;
    settings.beginGroup(QSTR_WINDOW);
    restoreGeometry(settings.value(QSTR_GEOMETRY).toByteArray());
    restoreState(settings.value(QSTR_STATE).toByteArray());
    settings.endGroup();

    settings.beginGroup(QSTR_PREFERENCES);
    m_baseChannel = settings.value(QSTR_CHANNEL, 0).toInt();
    m_velocity = settings.value(QSTR_VELOCITY, MIDIVELOCITY).toInt();
    m_baseOctave = settings.value(QSTR_BASEOCTAVE, 3).toInt();
    m_transpose = settings.value(QSTR_TRANSPOSE, 0).toInt();
    int num_octaves = settings.value(QSTR_NUMOCTAVES, DEFAULTNUMBEROFOCTAVES).toInt();
    QString insFileName = settings.value(QSTR_INSTRUMENTSDEFINITION).toString();
    QString insName = settings.value(QSTR_INSTRUMENTNAME).toString();
    bool grabKb = settings.value(QSTR_GRABKB, false).toBool();
    bool styledKnobs = settings.value(QSTR_STYLEDKNOBS, true).toBool();
    bool alwaysOnTop = settings.value(QSTR_ALWAYSONTOP, false).toBool();
    bool showNames = settings.value(QSTR_SHOWNOTENAMES, false).toBool();
    bool showStatusBar = settings.value(QSTR_SHOWSTATUSBAR, false).toBool();
    bool velocityColor = settings.value(QSTR_VELOCITYCOLOR, true).toBool();
    bool enforceChanState = settings.value(QSTR_ENFORCECHANSTATE, false).toBool();
    bool enableKeyboard = settings.value(QSTR_ENABLEKEYBOARDINPUT, true).toBool();
    bool enableMouse = settings.value(QSTR_ENABLEMOUSEINPUT, true).toBool();
    bool enableTouch = settings.value(QSTR_ENABLETOUCHINPUT, true).toBool();
    int drumsChannel = settings.value(QSTR_DRUMSCHANNEL, MIDIGMDRUMSCHANNEL).toInt();
    m_midiDriver = settings.value(QSTR_MIDIDRIVER, QSTR_DRIVERDEFAULT).toString();
#if defined(NETWORK_MIDI)
    int udpPort = settings.value(QSTR_NETWORKPORT, NETWORKPORTNUMBER).toInt();
    NetworkSettings::instance().setPort(udpPort);
    QString iface = settings.value(QSTR_NETWORKIFACE).toString();
    NetworkSettings::instance().setIface(QNetworkInterface::interfaceFromName(iface));
#endif
    m_currentPalette = settings.value(QSTR_CURRENTPALETTE, PAL_SINGLE).toInt();
    bool colorScale = settings.value(QSTR_SHOWCOLORSCALE, false).toBool();
    settings.endGroup();

    dlgColorPolicy()->loadPalette(m_currentPalette);
    dlgPreferences()->setDriver(m_midiDriver);
#if defined(NETWORK_MIDI)
    dlgPreferences()->setNetworkPort(udpPort);
    dlgPreferences()->setNetworkIfaceName(iface);
#endif
    dlgPreferences()->setNumOctaves(num_octaves);
    dlgPreferences()->setDrumsChannel(drumsChannel);
    //dlgPreferences()->setKeyPressedColor(keyColor);
    dlgPreferences()->setGrabKeyboard(grabKb);
    dlgPreferences()->setStyledWidgets(styledKnobs);
    dlgPreferences()->setAlwaysOnTop(alwaysOnTop);
    dlgPreferences()->setVelocityColor(velocityColor);
    dlgPreferences()->setEnforceChannelState(enforceChanState);
    dlgPreferences()->setEnabledKeyboard(enableKeyboard);
    dlgPreferences()->setEnabledMouse(enableMouse);
    dlgPreferences()->setEnabledTouch(enableTouch);
    ui.actionNoteNames->setChecked(showNames);
    ui.actionStatusBar->setChecked(showStatusBar);
    ui.pianokeybd->setNumOctaves(num_octaves);
    currentPianoScene()->setVelocity(velocityColor ? m_velocity : MIDIVELOCITY);
    ui.pianokeybd->setTranspose(m_transpose);
    ui.pianokeybd->setBaseOctave(m_baseOctave);
    currentPianoScene()->setKeyboardEnabled(enableKeyboard);
    currentPianoScene()->setMouseEnabled(enableMouse);
    currentPianoScene()->setTouchEnabled(enableTouch);
    currentPianoScene()->setChannel(m_baseChannel);
    ui.actionColorScale->setChecked(colorScale);
    slotShowNoteNames();
    if (!insFileName.isEmpty()) {
        dlgPreferences()->setInstrumentsFileName(insFileName);
        if (!insName.isEmpty()) {
            dlgPreferences()->setInstrumentName(insName);
        }
    }

    settings.beginGroup(QSTR_KEYBOARD);
    bool rawKeyboard = settings.value(QSTR_RAWKEYBOARDMODE, false).toBool();
    QString mapFile = settings.value(QSTR_MAPFILE, QSTR_DEFAULT).toString();
    QString rawMapFile = settings.value(QSTR_RAWMAPFILE, QSTR_DEFAULT).toString();
    settings.endGroup();
    dlgPreferences()->setRawKeyboard(rawKeyboard);

    settings.beginGroup(QSTR_SHORTCUTS);
    //qDebug() << "settings.childKeys:" << settings.childKeys().count();
    bool savedShortcuts = (settings.childKeys().count() > 0);
    QList<QAction *> actions = findChildren<QAction *> ();
    foreach(QAction* pAction, actions)
    {
        if (pAction->objectName().isEmpty())
            continue;
        const QString& sKey = '/' + pAction->objectName();
        QList<QKeySequence> sShortcuts = pAction->shortcuts();
        m_defaultShortcuts.insert(sKey, sShortcuts);
        if (savedShortcuts)
        {
            const QString& sValue = settings.value('/' + sKey).toString();
            if (sValue.isEmpty())
            {
                if(sShortcuts.count() == 0)
                {
                    continue;
                }
                else
                {
                    pAction->setShortcuts(QList<QKeySequence>());
                }
            }
            else
            {
                pAction->setShortcut(QKeySequence(sValue));
            }
        }
    }
    settings.endGroup();

    currentPianoScene()->getKeyboardMap()->setRawMode(false);
    ui.pianokeybd->getRawKeyboardMap()->setRawMode(true);
    if (!mapFile.isEmpty() && mapFile != QSTR_DEFAULT) {
        dlgPreferences()->setKeyMapFileName(mapFile);
        currentPianoScene()->setKeyboardMap(dlgPreferences()->getKeyboardMap());
    }
    if (!rawMapFile.isEmpty() && rawMapFile != QSTR_DEFAULT) {
        dlgPreferences()->setRawKeyMapFileName(rawMapFile);
        ui.pianokeybd->setRawKeyboardMap(dlgPreferences()->getKeyboardMap());
    }
}

void VPiano::readConnectionSettings()
{
    QString in_port, out_port;
    QSettings settings;
    settings.beginGroup(QSTR_CONNECTIONS);
    bool inEnabled = settings.value(QSTR_INENABLED, true).toBool();
    bool thruEnabled = settings.value(QSTR_THRUENABLED, false).toBool();
    bool omniEnabled = settings.value(QSTR_OMNIENABLED, false).toBool();
    if (m_midiDriver != QSTR_DRIVERNAMENET) {
        in_port = settings.value(QSTR_INPORT).toString();
        out_port = settings.value(QSTR_OUTPORT).toString();
    }
    settings.endGroup();
    if ( m_midiDriver == QSTR_DRIVERNAMEALSA ||
         m_midiDriver == QSTR_DRIVERNAMEMACOSX ||
         m_midiDriver == QSTR_DRIVERNAMEJACK )
        inEnabled = true;

    if (m_midiin == 0) {
        dlgMidiSetup()->inputNotAvailable();
    } else {
        dlgMidiSetup()->setInputEnabled(inEnabled);
        dlgMidiSetup()->setThruEnabled(thruEnabled);
        dlgMidiSetup()->setOmniEnabled(omniEnabled);
        if (m_midiDriver != QSTR_DRIVERNAMENET)
            dlgMidiSetup()->setCurrentInput(in_port);
    }
    if (m_midiDriver != QSTR_DRIVERNAMENET)
        dlgMidiSetup()->setCurrentOutput(out_port);
}

void VPiano::readMidiControllerSettings()
{
    QSettings settings;
    for (int chan=0; chan<MIDICHANNELS; ++chan) {
        QString group = QSTR_INSTRUMENT + QString::number(chan);
        settings.beginGroup(group);
        m_lastBank[chan] = settings.value(QSTR_BANK, -1).toInt();
        m_lastProg[chan] = settings.value(QSTR_PROGRAM, 0).toInt();
        m_lastCtl[chan] = settings.value(QSTR_CONTROLLER, 1).toInt();
        settings.endGroup();

        group = QSTR_CONTROLLERS + QString::number(chan);
        settings.beginGroup(group);
        foreach(const QString& key, settings.allKeys()) {
            int ctl = key.toInt();
            int val = settings.value(key, 0).toInt();
            m_ctlSettings[chan][ctl] = val;
        }
        settings.endGroup();
    }

    settings.beginGroup(QSTR_EXTRACONTROLLERS);
    m_extraControls.clear();
    QStringList keys = settings.allKeys();
    keys.sort();
    foreach(const QString& key, keys) {
        m_extraControls << settings.value(key, QString()).toString();
    }
    settings.endGroup();
}

void VPiano::writeSettings()
{
    QSettings settings;
    settings.clear();

    settings.beginGroup(QSTR_WINDOW);
    settings.setValue(QSTR_GEOMETRY, saveGeometry());
    settings.setValue(QSTR_STATE, saveState());
    settings.endGroup();

    settings.beginGroup(QSTR_PREFERENCES);
    settings.setValue(QSTR_CHANNEL, m_baseChannel);
    settings.setValue(QSTR_VELOCITY, m_velocity);
    settings.setValue(QSTR_BASEOCTAVE, m_baseOctave);
    settings.setValue(QSTR_TRANSPOSE, m_transpose);
    settings.setValue(QSTR_LANGUAGE, m_language);
    settings.setValue(QSTR_NUMOCTAVES, dlgPreferences()->getNumOctaves());
    settings.setValue(QSTR_INSTRUMENTSDEFINITION, dlgPreferences()->getInstrumentsFileName());
    settings.setValue(QSTR_INSTRUMENTNAME, dlgPreferences()->getInstrumentName());
    settings.setValue(QSTR_GRABKB, dlgPreferences()->getGrabKeyboard());
    settings.setValue(QSTR_STYLEDKNOBS, dlgPreferences()->getStyledWidgets());
    settings.setValue(QSTR_ALWAYSONTOP, dlgPreferences()->getAlwaysOnTop());
    settings.setValue(QSTR_SHOWNOTENAMES, ui.actionNoteNames->isChecked());
    settings.setValue(QSTR_SHOWSTATUSBAR, ui.actionStatusBar->isChecked());
    settings.setValue(QSTR_DRUMSCHANNEL, dlgPreferences()->getDrumsChannel());
    settings.setValue(QSTR_VELOCITYCOLOR, dlgPreferences()->getVelocityColor());
    settings.setValue(QSTR_ENFORCECHANSTATE, dlgPreferences()->getEnforceChannelState());
    settings.setValue(QSTR_ENABLEKEYBOARDINPUT, dlgPreferences()->getEnabledKeyboard());
    settings.setValue(QSTR_ENABLEMOUSEINPUT, dlgPreferences()->getEnabledMouse());
    settings.setValue(QSTR_ENABLETOUCHINPUT, dlgPreferences()->getEnabledTouch());
    settings.setValue(QSTR_MIDIDRIVER, dlgPreferences()->getDriver());
#if defined(NETWORK_MIDI)
    settings.setValue(QSTR_NETWORKPORT, dlgPreferences()->getNetworkPort());
    settings.setValue(QSTR_NETWORKIFACE, dlgPreferences()->getNetworkInterfaceName());
#endif
    settings.setValue(QSTR_CURRENTPALETTE, m_currentPalette);
    settings.setValue(QSTR_SHOWCOLORSCALE, ui.actionColorScale->isChecked());
    settings.endGroup();

    settings.beginGroup(QSTR_CONNECTIONS);
    settings.setValue(QSTR_INENABLED, dlgMidiSetup()->inputIsEnabled());
    settings.setValue(QSTR_THRUENABLED, dlgMidiSetup()->thruIsEnabled());
    settings.setValue(QSTR_OMNIENABLED, dlgMidiSetup()->omniIsEnabled());
    if (m_midiDriver != QSTR_DRIVERNAMENET) {
        settings.setValue(QSTR_INPORT,  dlgMidiSetup()->selectedInputName());
        settings.setValue(QSTR_OUTPORT, dlgMidiSetup()->selectedOutputName());
    }
    settings.endGroup();

    settings.beginGroup(QSTR_KEYBOARD);
    settings.setValue(QSTR_RAWKEYBOARDMODE, dlgPreferences()->getRawKeyboard());
    settings.setValue(QSTR_MAPFILE, currentPianoScene()->getKeyboardMap()->getFileName());
    settings.setValue(QSTR_RAWMAPFILE, ui.pianokeybd->getRawKeyboardMap()->getFileName());
    settings.endGroup();

    for (int chan=0; chan<MIDICHANNELS; ++chan) {

        QString group = QSTR_CONTROLLERS + QString::number(chan);
        settings.beginGroup(group);
        QMap<int,int>::const_iterator it, end;
        it = m_ctlState[chan].constBegin();
        end = m_ctlState[chan].constEnd();
        for (; it != end; ++it)
            settings.setValue(QString::number(it.key()), it.value());
        settings.endGroup();

        group = QSTR_INSTRUMENT + QString::number(chan);
        settings.beginGroup(group);
        settings.setValue(QSTR_BANK, m_lastBank[chan]);
        settings.setValue(QSTR_PROGRAM, m_lastProg[chan]);
        settings.setValue(QSTR_CONTROLLER, m_lastCtl[chan]);
        settings.endGroup();
    }

    settings.beginGroup(QSTR_EXTRACONTROLLERS);
    int i = 0;
    foreach(const QString& ctl, m_extraControls)  {
        QString key = QString("%1").arg(i++, 2, 10, QChar('0'));
        settings.setValue(key, ctl);
    }
    settings.endGroup();

    settings.beginGroup(QSTR_SHORTCUTS);
    QList<QAction *> actions = findChildren<QAction *> ();
    foreach(QAction *pAction, actions)
    {
        if (pAction->objectName().isEmpty())
            continue;
        const QString& sKey = '/' + pAction->objectName();
        const QString& sValue = pAction->shortcut().toString();
        QList<QKeySequence> defShortcuts = m_defaultShortcuts.value(sKey);
        if (sValue.isEmpty() && defShortcuts.count() == 0)
        {
            if (settings.contains(sKey))
                settings.remove(sKey);
        }
        else
        {
            settings.setValue(sKey, sValue);
        }
    }
    settings.endGroup();

    dlgColorPolicy()->saveCurrentPalette();

    settings.sync();
}

void VPiano::closeEvent( QCloseEvent *event )
{
    //qDebug() << "closeEvent:" << event->type();
    if (m_initialized)
        writeSettings();
    event->accept();
}

QColor VPiano::getColorFromPolicy(NoteOnEvent *ev)
{
    PianoPalette *palette = dlgColorPolicy()->currentPalette();
    switch (palette->paletteId()) {
    case PAL_SINGLE:
        return palette->getColor(0);
    case PAL_DOUBLE:
        return palette->getColor(ev->getType());
    case PAL_CHANNELS:
        return palette->getColor(ev->getChannel());
    case PAL_SCALE:
        return palette->getColor(ev->getDegree());
    }
    return QColor();
}

void VPiano::customEvent ( QEvent *event )
{
    //qDebug() << "customEvent:" << event->type();
    if ( event->type() == NoteOnEventType ) {
        NoteOnEvent *ev = static_cast<NoteOnEvent*>(event);
        int n = ev->getNote();
        QColor c = getColorFromPolicy(ev);
        int v = (dlgPreferences()->getVelocityColor() ? ev->getValue() : MIDIVELOCITY );
        currentPianoScene()->showNoteOn(n, c, v);
#ifdef ENABLE_DBUS
        emit event_noteon(n);
#endif
    }
    else if ( event->type() == NoteOffEventType ) {
        NoteOffEvent *ev = static_cast<NoteOffEvent*>(event);
        int n = ev->getNote();
        currentPianoScene()->showNoteOff(n);
#ifdef ENABLE_DBUS
        emit event_noteoff(n);
#endif
    }
    else if ( event->type() == PolyKeyPressEventType ) {
#ifdef ENABLE_DBUS
        PolyKeyPressEvent *ev = static_cast<PolyKeyPressEvent*>(event);
        int n = ev->getNote();
        int v = ev->getValue();
        emit event_polykeypress(n, v);
#endif
    }
    else if ( event->type() ==  ControlChangeEventType ) {
        ControlChangeEvent *ev = static_cast<ControlChangeEvent*>(event);
        int ctl = ev->getController();
        int val = ev->getValue();
        switch (ctl) {
        case CTL_ALL_SOUND_OFF:
        case CTL_ALL_NOTES_OFF:
            currentPianoScene()->allKeysOff();
            break;
        case CTL_RESET_ALL_CTL:
            initializeAllControllers();
            break;
        default:
            updateController(ctl, val);
            updateExtraController(ctl, val);
        }
#ifdef ENABLE_DBUS
        emit event_controlchange(ctl, val);
#endif
    }
    else if ( event->type() ==  ProgramChangeEventType) {
        ProgramChangeEvent *ev = static_cast<ProgramChangeEvent*>(event);
        int val = ev->getValue();
        updateProgramChange(val);
#ifdef ENABLE_DBUS
        emit event_programchange(val);
#endif
    }
    else if ( event->type() ==  ChannelKeyPressEventType ) {
#ifdef ENABLE_DBUS
        ChannelKeyPressEvent *ev = static_cast<ChannelKeyPressEvent*>(event);
        int val = ev->getValue();
        emit event_chankeypress(val);
#endif
    }
    else if ( event->type() ==  PitchWheelEventType ) {
        PitchWheelEvent *ev = static_cast<PitchWheelEvent*>(event);
        int val = ev->getValue();
        m_bender->setValue(val);
        m_bender->setToolTip(QString::number(val));
#ifdef ENABLE_DBUS
        emit event_pitchwheel(val);
#endif
    }
    event->accept();
}

void VPiano::showEvent ( QShowEvent *event )
{
    //qDebug() << "showEvent:" << event->type();
    QMainWindow::showEvent(event);
#if !defined(SMALL_SCREEN)
    if (m_initialized) {
        ui.pianokeybd->setFocus();
        grabKb();
    }
#endif
}

void VPiano::hideEvent( QHideEvent *event )
{
    //qDebug() << "hideEvent:" << event->type();
#if !defined(SMALL_SCREEN)
    releaseKb();
#endif
    QMainWindow::hideEvent(event);
}

void VPiano::midiThru(std::vector<unsigned char> *message) const
{
    if (m_midiThru) {
        try {
            m_midiout->sendMessage( message );
        } catch (RtError& err) {
            qWarning() << QString::fromStdString(err.getMessage());
        }
    }
}

void VPiano::sendMessageWrapper(std::vector<unsigned char> *message) const
{
    try {
        m_midiout->sendMessage( message );
    } catch (RtError& err) {
        ui.statusBar->showMessage(QString::fromStdString(err.getMessage()));
    }
}

void VPiano::sendNoteOn(const int midiNote, const int vel)
{
    std::vector<unsigned char> message;
    if ((midiNote & MASK_SAFETY) == midiNote) {
        unsigned char chan = static_cast<unsigned char>(m_baseChannel);
        //unsigned char vel = static_cast<unsigned char>(m_velocity);
        // Note On: 0x90 + channel, note, vel
        message.push_back(STATUS_NOTEON + (chan & MASK_CHANNEL));
        message.push_back(midiNote & MASK_SAFETY);
        message.push_back(vel & MASK_SAFETY);
        sendMessageWrapper( &message );
    }
}

void VPiano::noteOn(const int midiNote, const int vel)
{
    sendNoteOn(midiNote, vel);
#ifdef ENABLE_DBUS
    emit event_noteon(midiNote);
#endif
}

void VPiano::sendNoteOff(const int midiNote, const int vel)
{
    std::vector<unsigned char> message;
    if ((midiNote & MASK_SAFETY) == midiNote) {
        unsigned char chan = static_cast<unsigned char>(m_baseChannel);
        //unsigned char vel = static_cast<unsigned char>(m_velocity);
        // Note Off: 0x80 + channel, note, vel
        message.push_back(STATUS_NOTEOFF + (chan & MASK_CHANNEL));
        message.push_back(midiNote & MASK_SAFETY);
        message.push_back(vel & MASK_SAFETY);
        sendMessageWrapper( &message );
    }
}

void VPiano::noteOff(const int midiNote, const int vel)
{
    sendNoteOff(midiNote, vel);
#ifdef ENABLE_DBUS
    emit event_noteoff(midiNote);
#endif
}

void VPiano::sendController(const int controller, const int value)
{
    std::vector<unsigned char> message;
    unsigned char chan = static_cast<unsigned char>(m_baseChannel);
    unsigned char ctl  = static_cast<unsigned char>(controller);
    unsigned char val  = static_cast<unsigned char>(value);
    // Controller: 0xB0 + channel, ctl, val
    message.push_back(STATUS_CTLCHG + (chan & MASK_CHANNEL));
    message.push_back(ctl & MASK_SAFETY);
    message.push_back(val & MASK_SAFETY);
    sendMessageWrapper( &message );
}

void VPiano::resetAllControllers()
{
    sendController(CTL_RESET_ALL_CTL, 0);
    initializeAllControllers();
}

void VPiano::initializeAllControllers()
{
    int index = m_comboControl->currentIndex();
    int ctl = m_comboControl->itemData(index).toInt();
    int val = m_ctlState[m_baseChannel][ctl];
    initControllers(m_baseChannel);
    m_comboControl->setCurrentIndex(index);
    m_Control->setValue(val);
    m_Control->setToolTip(QString::number(val));
    // extra controllers
    QList<QWidget *> allWidgets = ui.toolBarExtra->findChildren<QWidget *>();
    foreach(QWidget *w, allWidgets) {
        QVariant c = w->property(MIDICTLNUMBER);
        if (c.isValid()) {
            ctl = c.toInt();
            if (m_ctlState[m_baseChannel].contains(ctl)) {
                val = m_ctlState[m_baseChannel][ctl];
                QVariant p = w->property("value");
                if (p.isValid()) {
                    w->setProperty("value", val);
                    w->setToolTip(QString::number(val));
                    continue;
                }
                p = w->property("checked");
                if (p.isValid()) {
                    QVariant on = w->property(MIDICTLONVALUE);
                    w->setProperty("checked", (val >= on.toInt()));
                }
            }
        }
    }
}

void VPiano::allNotesOff()
{
    sendController(CTL_ALL_NOTES_OFF, 0);
    currentPianoScene()->allKeysOff();
}

void VPiano::sendProgramChange(const int program)
{
    std::vector<unsigned char> message;
    unsigned char chan = static_cast<unsigned char>(m_baseChannel);
    unsigned char pgm  = static_cast<unsigned char>(program);
    // Program: 0xC0 + channel, pgm
    message.push_back(STATUS_PROGRAM + (chan & MASK_CHANNEL));
    message.push_back(pgm & MASK_SAFETY);
    sendMessageWrapper( &message );
}

void VPiano::sendBankChange(const int bank)
{
    int method = (m_ins != 0) ? m_ins->bankSelMethod() : 0;
    int lsb, msb;
    switch (method) {
    case 0:
        lsb = CALC_LSB(bank);
        msb = CALC_MSB(bank);
        sendController(CTL_MSB, msb);
        sendController(CTL_LSB, lsb);
        break;
    case 1:
        sendController(CTL_MSB, bank);
        break;
    case 2:
        sendController(CTL_LSB, bank);
        break;
    default: /* if method is 3 or above, do nothing */
        break;
    }
    m_lastBank[m_baseChannel] = bank;
}

void VPiano::sendPolyKeyPress(const int note, const int value)
{
    std::vector<unsigned char> message;
    unsigned char chan = static_cast<unsigned char>(m_baseChannel);
    unsigned char midi_note  = static_cast<unsigned char>(note);
    unsigned char val  = static_cast<unsigned char>(value);
    // Polyphonic After-touch: 0xA0 + channel, note, value
    message.push_back(STATUS_POLYAFT + (chan & MASK_CHANNEL));
    message.push_back(midi_note & MASK_SAFETY);
    message.push_back(val & MASK_SAFETY);
    sendMessageWrapper( &message );
}

void VPiano::sendChanKeyPress(const int value)
{
    std::vector<unsigned char> message;
    unsigned char chan = static_cast<unsigned char>(m_baseChannel);
    unsigned char val  = static_cast<unsigned char>(value);
    // Channel After-touch: 0xD0 + channel, value
    message.push_back(STATUS_CHANAFT + (chan & MASK_CHANNEL));
    message.push_back(val & MASK_SAFETY);
    sendMessageWrapper( &message );
}

void VPiano::sendBender(const int value)
{
    std::vector<unsigned char> message;
    int v = value + BENDER_MID; // v >= 0, v <= 16384
    unsigned char chan = static_cast<unsigned char>(m_baseChannel);
    unsigned char lsb  = static_cast<unsigned char>(CALC_LSB(v));
    unsigned char msb  = static_cast<unsigned char>(CALC_MSB(v));
    // Program: 0xE0 + channel, lsb, msb
    message.push_back(STATUS_BENDER + (chan & MASK_CHANNEL));
    message.push_back(lsb);
    message.push_back(msb);
    sendMessageWrapper( &message );
}

void VPiano::slotPanic()
{
    allNotesOff();
}

void VPiano::slotResetAllControllers()
{
    resetAllControllers();
}

void VPiano::slotResetBender()
{
    m_bender->setValue(0);
    sendBender(0);
}

void VPiano::sendSysex(const QByteArray& data)
{
    std::vector<unsigned char> message;
    foreach(const char byte, data) {
        message.push_back(byte);
    }
    sendMessageWrapper( &message );
}

void VPiano::slotControlClicked(const bool boolValue)
{
    QObject *s = sender();
    QVariant p = s->property(MIDICTLNUMBER);
    if (p.isValid()) {
        int controller = p.toInt();
        if (controller < 128) {
            QVariant on = s->property(MIDICTLONVALUE);
            QVariant off = s->property(MIDICTLOFFVALUE);
            int value = boolValue ? on.toInt() : off.toInt();
            sendController( controller, value );
            updateController( controller, value );
        } else {
            QVariant data = s->property(SYSEXFILEDATA);
            sendSysex(data.toByteArray());
        }
    }
}

void VPiano::slotVelocityValueChanged(int value)
{
    m_velocity = value;
    setWidgetTip(m_Velocity, value);
    currentPianoScene()->setVelocity(dlgPreferences()->getVelocityColor() ? value : MIDIVELOCITY);
}

void VPiano::slotExtraController(const int value)
{
    QWidget *w = static_cast<QWidget *>(sender());
    QVariant p = w->property(MIDICTLNUMBER);
    if (p.isValid()) {
        int controller = p.toInt();
        sendController( controller, value );
        updateController( controller, value );
        setWidgetTip(w, value);
    }
}

void VPiano::slotControlSliderMoved(const int value)
{
    int index = m_comboControl->currentIndex();
    int controller = m_comboControl->itemData(index).toInt();
    sendController( controller, value );
    updateExtraController( controller, value );
    m_ctlState[m_baseChannel][controller] = value;
    setWidgetTip(m_Control, value);
}

void VPiano::slotBenderSliderMoved(const int pos)
{
    sendBender(pos);
    setWidgetTip(m_bender, pos);
}

void VPiano::slotBenderSliderReleased()
{
    m_bender->setValue(0);
    sendBender(0);
    setWidgetTip(m_bender, 0);
}

void VPiano::slotAbout()
{
    releaseKb();
    dlgAbout()->exec();
    grabKb();
}

void VPiano::slotAboutQt()
{
    releaseKb();
    QApplication::aboutQt();
    grabKb();
}

void VPiano::refreshConnections()
{
    int i = 0, nInPorts = 0, nOutPorts = 0;
    try {
        dlgMidiSetup()->clearCombos();
        // inputs
        if (m_midiin == 0) {
            dlgMidiSetup()->inputNotAvailable();
            dlgMidiSetup()->setInputEnabled(false);
        } else {
#if !defined(__LINUX_ALSASEQ__) && !defined(__MACOSX_CORE__)
            dlgMidiSetup()->setInputEnabled(m_currentIn != -1);
#endif
            dlgMidiSetup()->addInputPortName(QString::null, -1);
            nInPorts = m_midiin->getPortCount();
            for ( i = 0; i < nInPorts; i++ ) {
                QString name = QString::fromStdString(m_midiin->getPortName(i));
                if (!name.startsWith(QSTR_VMPK))
                    dlgMidiSetup()->addInputPortName(name, i);
            }
        }
        // outputs
        nOutPorts = m_midiout->getPortCount();
        for ( i = 0; i < nOutPorts; i++ ) {
            QString name = QString::fromStdString(m_midiout->getPortName(i));
            if (!name.startsWith(QSTR_VMPK))
                dlgMidiSetup()->addOutputPortName(name, i);
        }
    } catch (RtError& err) {
        ui.statusBar->showMessage(QString::fromStdString(err.getMessage()));
    }
}

void VPiano::slotConnections()
{
    refreshConnections();
    dlgMidiSetup()->setCurrentInput(m_currentIn);
    dlgMidiSetup()->setCurrentOutput(m_currentOut);
    releaseKb();
    if (dlgMidiSetup()->exec() == QDialog::Accepted) {
        applyConnections();
        enforceMIDIChannelState();
    }
    grabKb();
}

void VPiano::applyConnections()
{
    int i, nInPorts = 0, nOutPorts = 0;
    try {
        nOutPorts = m_midiout->getPortCount();
        i = dlgMidiSetup()->selectedOutput();
        if ((i >= 0) && (i < nOutPorts) && (i != m_currentOut)) {
            m_midiout->closePort();
            m_midiout->openPort(i);
        }
        m_currentOut = i;
        if (m_midiin != 0) {
            nInPorts = m_midiin->getPortCount();
            i = dlgMidiSetup()->selectedInput();
            if (m_inputActive && (i != m_currentIn)) {
                m_midiin->cancelCallback();
                m_inputActive = false;
                if (m_currentIn > -1)
                    m_midiin->closePort();
            }
            if ((i >= 0) && (i < nInPorts) && (i != m_currentIn) &&
                dlgMidiSetup()->inputIsEnabled()) {
                m_midiin->openPort(i);
                m_midiin->setCallback( &midiCallback, this );
                m_inputActive = true;
            }
            m_currentIn = i;
            m_midiThru = dlgMidiSetup()->thruIsEnabled();
            m_midiOmni = dlgMidiSetup()->omniIsEnabled();
        }
    } catch (RtError& err) {
        ui.statusBar->showMessage(QString::fromStdString(err.getMessage()));
    }
}

void VPiano::initControllers(int channel)
{
    if (m_ins != 0) {
        InstrumentData controls = m_ins->control();
        InstrumentData::ConstIterator it, end;
        it = controls.constBegin();
        end = controls.constEnd();
        for( ; it != end; ++it ) {
            int ctl = it.key();
            switch (ctl) {
            case CTL_VOLUME:
                m_ctlState[channel][CTL_VOLUME] = MIDIVOLUME;
                break;
            case CTL_PAN:
                m_ctlState[channel][CTL_PAN] = MIDIPAN;
                break;
            case CTL_EXPRESSION:
                m_ctlState[channel][CTL_EXPRESSION] = MIDIEXPRESSION;
                break;
            default:
                m_ctlState[channel][ctl] = 0;
            }
        }
    }
}

void VPiano::populateControllers()
{
    m_comboControl->blockSignals(true);
    m_comboControl->clear();
    if (m_ins != 0) {
        InstrumentData controls = m_ins->control();
        InstrumentData::ConstIterator it, end = controls.constEnd();
        for( it = controls.constBegin(); it != end; ++it )
            m_comboControl->addItem(it.value(), it.key());
    }
    m_comboControl->blockSignals(false);
}

void VPiano::applyPreferences()
{
    currentPianoScene()->allKeysOff();

    if (ui.pianokeybd->numOctaves() != dlgPreferences()->getNumOctaves()) {
        ui.pianokeybd->setNumOctaves(dlgPreferences()->getNumOctaves());
    }
    currentPianoScene()->setRawKeyboardMode(dlgPreferences()->getRawKeyboard());
    currentPianoScene()->setVelocity(dlgPreferences()->getVelocityColor() ? m_velocity : MIDIVELOCITY );
    bool enableKeyboard = dlgPreferences()->getEnabledKeyboard();
    bool enableMouse = dlgPreferences()->getEnabledMouse();
    bool enableTouch = dlgPreferences()->getEnabledTouch();
    currentPianoScene()->setKeyboardEnabled(enableKeyboard);
    currentPianoScene()->setMouseEnabled(enableMouse);
    currentPianoScene()->setTouchEnabled(enableTouch);
    ui.actionKeyboardInput->setChecked(enableKeyboard);
    ui.actionMouseInput->setChecked(enableMouse);
    ui.actionTouchScreenInput->setChecked(enableTouch);

#if defined(NETWORK_MIDI)
    int udpPort = dlgPreferences()->getNetworkPort();
    NetworkSettings::instance().setPort(udpPort);
    NetworkSettings::instance().setIface(dlgPreferences()->getNetworkInterface());
#endif

    KeyboardMap* map = dlgPreferences()->getKeyboardMap();
    if (!map->getFileName().isEmpty() && map->getFileName() != QSTR_DEFAULT )
        currentPianoScene()->setKeyboardMap(map);
    else
        ui.pianokeybd->resetKeyboardMap();

    map = dlgPreferences()->getRawKeyboardMap();
    if (!map->getFileName().isEmpty() && map->getFileName() != QSTR_DEFAULT )
        ui.pianokeybd->setRawKeyboardMap(map);
    else
        ui.pianokeybd->resetRawKeyboardMap();

    m_currentPalette = dlgColorPolicy()->currentPalette()->paletteId();
    currentPianoScene()->setPianoPalette(dlgColorPolicy()->currentPalette());
    currentPianoScene()->setColorScalePalette(dlgColorPolicy()->getPalette(PAL_SCALE));
    currentPianoScene()->setShowColorScale(ui.actionColorScale->isChecked());

    populateInstruments();
    populateControllers();

    QPoint wpos = pos();
    Qt::WindowFlags flags = windowFlags();
    if (dlgPreferences()->getAlwaysOnTop())
        flags |= Qt::WindowStaysOnTopHint;
    else
        flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags( flags );
    move(wpos);

    slotShowNoteNames();
    updateStyles();
    show();
}

void VPiano::populateInstruments()
{
    m_ins = 0;
    m_comboBank->clear();
    m_comboProg->clear();
    if (!dlgPreferences()->getInstrumentsFileName().isEmpty() &&
         dlgPreferences()->getInstrumentsFileName() != QSTR_DEFAULT) {
        if (m_baseChannel == dlgPreferences()->getDrumsChannel())
            m_ins = dlgPreferences()->getDrumsInstrument();
        else
            m_ins = dlgPreferences()->getInstrument();
        if (m_ins != 0) {
            //qDebug() << "Instrument Name:" << m_ins->instrumentName();
            //qDebug() << "Bank Selection method: " << m_ins->bankSelMethod();
            InstrumentPatches patches = m_ins->patches();
            InstrumentPatches::ConstIterator j;
            for( j = patches.constBegin(); j != patches.constEnd(); ++j ) {
                //if (j.key() < 0) continue;
                InstrumentData patch = j.value();
                m_comboBank->addItem(patch.name(), j.key());
                //qDebug() << "---- Bank[" << j.key() << "]=" << patch.name();
            }
            updateBankChange(m_lastBank[m_baseChannel]);
        }
    }
}

void VPiano::applyInitialSettings()
{
    int idx, ctl;
    for ( int ch=0; ch<MIDICHANNELS; ++ch) {
        initControllers(ch);
        QMap<int,int>::Iterator i, j, end;
        i = m_ctlSettings[ch].begin();
        end = m_ctlSettings[ch].end();
        for (; i != end; ++i) {
            j = m_ctlState[ch].find(i.key());
            if (j != m_ctlState[ch].end())
                m_ctlState[ch][i.key()] = i.value();
        }
    }
    ctl = m_lastCtl[m_baseChannel];
    idx = m_comboControl->findData(ctl);
    if (idx != -1)
        m_comboControl->setCurrentIndex(idx);
    //slotControlSliderMoved(m_ctlState[m_channel][ctl]);
    updateBankChange(m_lastBank[m_baseChannel]);
    idx = m_comboProg->findData(m_lastProg[m_baseChannel]);
    m_comboProg->setCurrentIndex(idx);
    //slotComboProgActivated(idx);
}

void VPiano::slotPreferences()
{
#if defined(NETWORK_MIDI)
    int old_udpPort = NetworkSettings::instance().port();
    QString old_iface = NetworkSettings::instance().iface().name();
#endif
    QString old_driver = dlgPreferences()->getDriver();
    releaseKb();
    if (dlgPreferences()->exec() == QDialog::Accepted) {
        applyPreferences();
        if (old_driver != dlgPreferences()->getDriver()) {
            switchMIDIDriver();
        }
#if defined(NETWORK_MIDI)
        if (old_udpPort != NetworkSettings::instance().port() ||
            old_iface != NetworkSettings::instance().iface().name() ) {
            applyConnections();
        }
#endif
    }
    grabKb();
}

QString VPiano::dataDirectory()
{
#if defined(Q_OS_WIN32)
    return QApplication::applicationDirPath() + "/";
#elif defined(Q_OS_DARWIN)
    return QApplication::applicationDirPath() + "/../Resources/";
#elif defined(Q_OS_SYMBIAN)
    return QApplication::applicationDirPath() + "/";
#elif defined(Q_OS_UNIX)
    return QApplication::applicationDirPath() + "/../share/vmpk/";
#endif
    return QString();
}

QString VPiano::localeDirectory()
{
#if defined(Q_OS_LINUX)
    return VPiano::dataDirectory() + "locale/";
#else
    return VPiano::dataDirectory();
#endif
}

void VPiano::slotEditKeyboardMap()
{
#if !defined(SMALL_SCREEN)
    KeyboardMap* map;
    releaseKb();
    if (dlgPreferences()->getRawKeyboard())
        map = ui.pianokeybd->getRawKeyboardMap();
    else
        map = currentPianoScene()->getKeyboardMap();
    dlgKeyMap()->displayMap(map);
    if (dlgKeyMap()->exec() == QDialog::Accepted) {
        dlgKeyMap()->getMap(map);
        if (dlgPreferences()->getRawKeyboard())
            ui.pianokeybd->setRawKeyboardMap(map);
        else
            currentPianoScene()->setKeyboardMap(map);
    }
    grabKb();
#endif
}

void VPiano::populatePrograms(int bank)
{
    if (bank < 0)
        return;
    m_comboProg->clear();
    if (m_ins != 0) {
        InstrumentData patch = m_ins->patch(bank);
        InstrumentData::ConstIterator k;
        for( k = patch.constBegin(); k != patch.constEnd(); ++k )
            m_comboProg->addItem(k.value(), k.key());
            //qDebug() << "patch[" << k.key() << "]=" << k.value();
    }
}

void VPiano::slotComboBankActivated(const int index)
{
    int idx = index;
    if (idx < 0)
        m_comboBank->setCurrentIndex(idx = 0);
    int bank = m_comboBank->itemData(idx).toInt();
    populatePrograms(bank);
    slotComboProgActivated();
}

void VPiano::slotComboProgActivated(const int index)
{
    int idx = index;
    if (idx < 0)
        m_comboProg->setCurrentIndex(idx = 0);
    int bankIdx = m_comboBank->currentIndex();
    int bank = m_comboBank->itemData(bankIdx).toInt();
    if (bank >= 0) {
        sendBankChange(bank);
        m_lastBank[m_baseChannel] = bank;
    }
    int pgm = m_comboProg->itemData(idx).toInt();
    if (pgm >= 0) {
        sendProgramChange(pgm);
        m_lastProg[m_baseChannel] = pgm;
    }
    updateNoteNames(m_baseChannel == dlgPreferences()->getDrumsChannel());
}

void VPiano::slotBaseOctaveValueChanged(const int octave)
{
    if (octave != m_baseOctave) {
        currentPianoScene()->allKeysOff();
        ui.pianokeybd->setBaseOctave(octave);
        m_baseOctave = octave;
    }
}

void VPiano::slotTransposeValueChanged(const int transpose)
{
    if (transpose != m_transpose) {
        ui.pianokeybd->setTranspose(transpose);
        m_transpose = transpose;
    }
}

void VPiano::updateNoteNames(bool drums)
{
    if (drums && (m_ins != 0)) {
        int b = m_lastBank[m_baseChannel];
        int p = m_lastProg[m_baseChannel];
        const InstrumentData& notes = m_ins->notes(b, p);
        QStringList noteNames;
        for(int n=0; n<128; ++n) {
            if (notes.contains(n))
                noteNames << notes[n];
            else
                noteNames << QString();
        }
        currentPianoScene()->useCustomNoteNames(noteNames);
    } else
        currentPianoScene()->useStandardNoteNames();
}

void VPiano::slotChannelValueChanged(const int channel)
{
    int idx;
    int c = channel - 1;
    if (c != m_baseChannel) {
        int drms = dlgPreferences()->getDrumsChannel();
        bool updDrums = ((c == drms) || (m_baseChannel == drms));
        m_baseChannel = c;
        ui.pianokeybd->getPianoScene()->setChannel(c);
        if (updDrums) {
            populateInstruments();
            populateControllers();
        }
        idx = m_comboControl->findData(m_lastCtl[m_baseChannel]);
        if (idx != -1) {
            int ctl = m_lastCtl[m_baseChannel];
            m_comboControl->setCurrentIndex(idx);
            updateController(ctl, m_ctlState[m_baseChannel][ctl]);
            updateExtraController(ctl, m_ctlState[m_baseChannel][ctl]);
        }
        updateBankChange(m_lastBank[m_baseChannel]);
        updateProgramChange(m_lastProg[m_baseChannel]);
        enforceMIDIChannelState();
        currentPianoScene()->resetKeyPressedColor();
    }
}

void VPiano::updateController(int ctl, int val)
{
    int index = m_comboControl->currentIndex();
    int controller = m_comboControl->itemData(index).toInt();
    if (controller == ctl) {
        m_Control->setValue(val);
        m_Control->setToolTip(QString::number(val));
    }
    m_ctlState[m_baseChannel][ctl] = val;
    if ((ctl == CTL_MSB || ctl == CTL_LSB ) && m_ins != 0) {
        if (m_ins->bankSelMethod() == 0)
            m_lastBank[m_baseChannel] = m_ctlState[m_baseChannel][CTL_MSB] << 7 |
                                    m_ctlState[m_baseChannel][CTL_LSB];
        else
            m_lastBank[m_baseChannel] = val;

        updateBankChange(m_lastBank[m_baseChannel]);
    }
}

void VPiano::updateExtraController(int ctl, int val)
{
    QList<QWidget *> allWidgets = ui.toolBarExtra->findChildren<QWidget *>();
    foreach(QWidget *w, allWidgets) {
        QVariant p = w->property(MIDICTLNUMBER);
        if (p.isValid() && p.toInt() == ctl) {
            QVariant v = w->property("value");
            if (v.isValid() && v.toInt() != val) {
                w->setProperty("value", val);
                w->setToolTip(QString::number(val));
                continue;
            }
            v = w->property("checked");
            if (v.isValid()) {
                QVariant on = w->property(MIDICTLONVALUE);
                bool checked = (val >= on.toInt());
                w->setProperty("checked", checked);
            }
        }
    }
}

void VPiano::updateBankChange(int bank)
{
    int idx;
    if (bank < 0) {
        m_comboBank->setCurrentIndex(idx = 0);
        bank = m_comboBank->itemData(idx).toInt();
        if (bank < 0)
            bank = 0;
    } else {
        idx = m_comboBank->findData(bank);
        if (idx != -1) {
            m_comboBank->setCurrentIndex(idx);
            m_lastBank[m_baseChannel] = bank;
        }
    }
    populatePrograms(bank);
    updateProgramChange();
}

void VPiano::updateProgramChange(int program)
{
    int idx;
    if (program < 0) {
        m_comboProg->setCurrentIndex(idx = 0);
        program = m_comboProg->itemData(idx).toInt();
    } else {
        idx = m_comboProg->findData(program);
        if (idx != -1) {
            m_comboProg->setCurrentIndex(idx);
            m_lastProg[m_baseChannel] = program;
        }
    }
    updateNoteNames(m_baseChannel == dlgPreferences()->getDrumsChannel());
}

void VPiano::slotComboControlCurrentIndexChanged(const int index)
{
    int ctl = m_comboControl->itemData(index).toInt();
    int val = m_ctlState[m_baseChannel][ctl];
    m_Control->setValue(val);
    m_Control->setToolTip(QString::number(val));
    m_lastCtl[m_baseChannel] = ctl;
}

void VPiano::grabKb()
{
#if !defined(SMALL_SCREEN)
    if (dlgPreferences()->getGrabKeyboard()) {
        ui.pianokeybd->grabKeyboard();
    }
    currentPianoScene()->setRawKeyboardMode(dlgPreferences()->getRawKeyboard());
#endif
}

void VPiano::releaseKb()
{
#if !defined(SMALL_SCREEN)
    if (dlgPreferences()->getGrabKeyboard()) {
        ui.pianokeybd->releaseKeyboard();
    }
    currentPianoScene()->setRawKeyboardMode(false);
#endif
}

class HelpDialog : public QDialog
{
public:
    HelpDialog(const QUrl &document, QWidget *parent = 0) : QDialog(parent)
    {
        setWindowState(Qt::WindowMaximized | Qt::WindowActive);
        QVBoxLayout *layout = new QVBoxLayout(this);
        QTextBrowser *browser = new QTextBrowser(this);
        layout->addWidget(browser);
        QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        layout->addWidget(buttonBox);
        browser->setSource(document);
        connect(buttonBox, SIGNAL(accepted()), SLOT(close()));
    }
};

void VPiano::slotHelpContents()
{
    QStringList hlps;
    QLocale loc(configuredLanguage());
    QStringList lc = loc.name().split("_");
    hlps += QSTR_HELPL.arg(loc.name());
    if (lc.count() > 1)
        hlps += QSTR_HELPL.arg(lc[0]);
    hlps += QSTR_HELP;
    QDir hlpDir(VPiano::dataDirectory());
    foreach(const QString& hlp_name, hlps) {
        if (hlpDir.exists(hlp_name)) {
            QUrl url = QUrl::fromLocalFile(hlpDir.absoluteFilePath(hlp_name));
#if defined(SMALL_SCREEN)
            HelpDialog hlpDlg(url, this);
            hlpDlg.exec();
#else
            QDesktopServices::openUrl(url);
#endif
            return;
        }
    }
    QMessageBox::critical(this, tr("Error"), tr("No help file found"));
}

void VPiano::slotOpenWebSite()
{
    QUrl url(QSTR_VMPKURL);
    QDesktopServices::openUrl(url);
}

void VPiano::updateStyles()
{
    QList<Knob *> allKnobs = findChildren<Knob *> ();
    foreach(Knob* knob, allKnobs) {
        knob->setStyle(dlgPreferences()->getStyledWidgets() ? m_dialStyle : 0);
    }
    QList<QCheckBox *> allChkbox = ui.toolBarExtra->findChildren<QCheckBox *> ();
    foreach(QCheckBox* chkbox, allChkbox) {
        chkbox->setStyle(dlgPreferences()->getStyledWidgets() ? m_dialStyle : 0);
    }
}

void VPiano::slotImportSF()
{
    releaseKb();
    if ((dlgRiffImport()->exec() == QDialog::Accepted) &&
        !dlgRiffImport()->getOutput().isEmpty()) {
        dlgRiffImport()->save();
        dlgPreferences()->setInstrumentsFileName(dlgRiffImport()->getOutput());
        dlgPreferences()->setInstrumentName(dlgRiffImport()->getName());
        applyPreferences();
    }
    grabKb();
}

void VPiano::slotEditExtraControls()
{
    dlgExtra()->setControls(m_extraControls);
    releaseKb();
    if (dlgExtra()->exec() == QDialog::Accepted) {
        m_extraControls = dlgExtra()->getControls();
        clearExtraControllers();
        initExtraControllers();
    }
    grabKb();
}

About* VPiano::dlgAbout()
{
    if (m_dlgAbout == 0) {
        m_dlgAbout = new About(this);
    }
    return m_dlgAbout;
}

Preferences* VPiano::dlgPreferences()
{
    if (m_dlgPreferences == 0) {
        m_dlgPreferences = new Preferences(this);
        m_dlgPreferences->setColorPolicyDialog(dlgColorPolicy());
    }
    return m_dlgPreferences;
}

MidiSetup* VPiano::dlgMidiSetup()
{
    if (m_dlgMidiSetup == 0) {
        m_dlgMidiSetup = new MidiSetup(this);
    }
    return m_dlgMidiSetup;
}

KMapDialog* VPiano::dlgKeyMap()
{
#if !defined(SMALL_SCREEN)
    if (m_dlgKeyMap == 0) {
        m_dlgKeyMap = new KMapDialog(this);
    }
#endif
    return m_dlgKeyMap;
}

DialogExtraControls* VPiano::dlgExtra()
{
    if (m_dlgExtra == 0) {
        m_dlgExtra = new DialogExtraControls(this);
    }
    return m_dlgExtra;
}

RiffImportDlg* VPiano::dlgRiffImport()
{
    if (m_dlgRiffImport == 0) {
        m_dlgRiffImport = new RiffImportDlg(this);
    }
    return m_dlgRiffImport;
}

ColorDialog* VPiano::dlgColorPolicy()
{
    if (m_dlgColorPolicy == 0) {
        m_dlgColorPolicy = new ColorDialog(this);
    }
    return m_dlgColorPolicy;
}

void VPiano::setWidgetTip(QWidget* w, int val)
{
    QString tip = QString::number(val);
    w->setToolTip(tip);
    QToolTip::showText(w->parentWidget()->mapToGlobal(w->pos()), tip);
}

void VPiano::slotShowNoteNames()
{
    ui.pianokeybd->setShowLabels(ui.actionNoteNames->isChecked());
}

//void VPiano::slotEditPrograms()
//{ }

#if ENABLE_DBUS

void VPiano::quit()
{
    close();
}

void VPiano::panic()
{
    allNotesOff();
}

void VPiano::reset_controllers()
{
    resetAllControllers();
}

void VPiano::channel(int value)
{
    if (value >= 0 && value < MIDICHANNELS)
        m_sboxChannel->setValue(value + 1);
}

void VPiano::octave(int value)
{
    m_sboxOctave->setValue(value);
}

void VPiano::transpose(int value)
{
    m_sboxTranspose->setValue(value);
}

void VPiano::velocity(int value)
{
    m_Velocity->setValue(value);
}

void VPiano::connect_in(const QString &value)
{
    if( m_midiin != 0) {
        dlgMidiSetup()->setInputEnabled(true);
        dlgMidiSetup()->setCurrentInput(value);
        applyConnections();
    }
}

void VPiano::connect_out(const QString &value)
{
    if( m_midiout != 0) {
        dlgMidiSetup()->setCurrentOutput(value);
        applyConnections();
    }
}

void VPiano::connect_thru(bool value)
{
    if( m_midiin != 0 && m_midiout != 0) {
        dlgMidiSetup()->setThruEnabled(value);
        applyConnections();
    }
}

void VPiano::noteoff(int note)
{
    sendNoteOff(note, 0);
    NoteOffEvent *ev = new NoteOffEvent(m_baseChannel, note, 0);
    QApplication::postEvent(this, ev);
}

void VPiano::noteon(int note)
{
    sendNoteOn(note, m_velocity);
    NoteOnEvent *ev = new NoteOnEvent(m_baseChannel, note, m_velocity);
    QApplication::postEvent(this, ev);
}

void VPiano::polykeypress(int note, int value)
{
    sendPolyKeyPress(note, value);
    PolyKeyPressEvent *ev = new PolyKeyPressEvent(note, value);
    QApplication::postEvent(this, ev);
}

void VPiano::controlchange(int control, int value)
{
    sendController(control, value);
    ControlChangeEvent *ev = new ControlChangeEvent(control, value);
    QApplication::postEvent(this, ev);
}

void VPiano::programchange(int value)
{
    sendProgramChange(value);
    ProgramChangeEvent *ev = new ProgramChangeEvent(value);
    QApplication::postEvent(this, ev);
}

void VPiano::programnamechange(const QString &value)
{
    int idx = m_comboProg->findText(value, Qt::MatchFixedString);
    if (idx != -1) {
        int prg = m_comboProg->itemData(idx).toInt();
        programchange(prg);
    }
}

void VPiano::chankeypress(int value)
{
    sendChanKeyPress(value);
    ChannelKeyPressEvent *ev = new ChannelKeyPressEvent(value);
    QApplication::postEvent(this, ev);
}

void VPiano::pitchwheel(int value)
{
    sendBender(value);
    PitchWheelEvent *ev = new PitchWheelEvent(value);
    QApplication::postEvent(this, ev);
}

#endif /* ENABLE_DBUS */

void VPiano::slotShortcuts()
{
#if !defined(SMALL_SCREEN)
    ShortcutDialog shcutDlg(findChildren<QAction*>());
    releaseKb();
    shcutDlg.setDefaultShortcuts(m_defaultShortcuts);
    shcutDlg.exec();
    grabKb();
#endif
}

void VPiano::slotBankNext()
{
    int index = m_comboBank->currentIndex();
    if (index < m_comboBank->count()-1) {
        m_comboBank->setCurrentIndex(++index);
        slotComboBankActivated(index);
    }
}

void VPiano::slotBankPrev()
{
    int index = m_comboBank->currentIndex();
    if (index > 0) {
        m_comboBank->setCurrentIndex(--index);
        slotComboBankActivated(index);
    }
}

void VPiano::slotProgramNext()
{
    int index = m_comboProg->currentIndex();
    if (index < m_comboProg->count()-1) {
        m_comboProg->setCurrentIndex(++index);
        slotComboProgActivated(index);
    }
}

void VPiano::slotProgramPrev()
{
    int index = m_comboProg->currentIndex();
    if (index > 0) {
        m_comboProg->setCurrentIndex(--index);
        slotComboProgActivated(index);
    }
}

void VPiano::slotControllerNext()
{
    int index = m_comboControl->currentIndex();
    if (index < m_comboControl->count()-1)
        m_comboControl->setCurrentIndex(++index);
}

void VPiano::slotControllerPrev()
{
    int index = m_comboControl->currentIndex();
    if (index > 0)
        m_comboControl->setCurrentIndex(--index);
}

void VPiano::slotVelocityUp()
{
    m_Velocity->triggerAction(Knob::SliderPageStepAdd);
}

void VPiano::slotVelocityDown()
{
    m_Velocity->triggerAction(Knob::SliderPageStepSub);
}

void VPiano::slotControllerUp()
{
    m_Control->triggerAction(Knob::SliderPageStepAdd);
    slotControlSliderMoved(m_Control->value());
}

void VPiano::slotControllerDown()
{
    m_Control->triggerAction(Knob::SliderPageStepSub);
    slotControlSliderMoved(m_Control->value());
}

QString VPiano::configuredLanguage()
{
    if (m_language.isEmpty()) {
        QSettings settings;
        QString defLang = QLocale::system().name();
        settings.beginGroup(QSTR_PREFERENCES);
        m_language = settings.value(QSTR_LANGUAGE, defLang).toString();
        settings.endGroup();
        //qDebug() << Q_FUNC_INFO << m_language;
    }
    return m_language;
}

void VPiano::initLanguages()
{
    m_supportedLangs.clear();
    m_supportedLangs.insert(QLatin1String("cs"), tr("Czech"));
    m_supportedLangs.insert(QLatin1String("de"), tr("German"));
    m_supportedLangs.insert(QLatin1String("en"), tr("English"));
    m_supportedLangs.insert(QLatin1String("es"), tr("Spanish"));
    m_supportedLangs.insert(QLatin1String("fr"), tr("French"));
    m_supportedLangs.insert(QLatin1String("nl"), tr("Dutch"));
    m_supportedLangs.insert(QLatin1String("ru"), tr("Russian"));
    m_supportedLangs.insert(QLatin1String("sv"), tr("Swedish"));
    m_supportedLangs.insert(QLatin1String("zh_CN"), tr("Chinese"));
}

void VPiano::retranslateToolbars()
{
    m_lblChannel->setText(
#if defined(SMALL_SCREEN)
        tr("Chan:")
#else
        tr("Channel:")
#endif
    );
    m_lblBaseOctave->setText(
#if defined(SMALL_SCREEN)
        tr("Oct:")
#else
        tr("Base Octave:")
#endif
    );
    m_lblTranspose->setText(
#if defined(SMALL_SCREEN)
        tr("Trans:")
#else
        tr("Transpose:")
#endif
    );
    m_lblVelocity->setText(
#if defined(SMALL_SCREEN)
        tr("Vel:")
#else
        tr("Velocity:")
#endif
    );
    m_lblBank->setText(tr("Bank:"));
    m_lblBender->setText(tr("Bender:"));
    m_lblControl->setText(tr("Control:"));
    m_lblProgram->setText(tr("Program:"));
    m_lblValue->setText(tr("Value:"));
}

QMenu * VPiano::createPopupMenu ()
{
#if defined(SMALL_SCREEN)
    return 0;
#else
    return QMainWindow::createPopupMenu();
#endif
}

void VPiano::enforceMIDIChannelState()
{
    if (dlgPreferences()->getEnforceChannelState()) {
        //qDebug() << Q_FUNC_INFO << "channel=" << m_channel << endl;
        QMap<int,int>::Iterator i, end;
        i = m_ctlSettings[m_baseChannel].begin();
        end = m_ctlSettings[m_baseChannel].end();
        for (; i != end; ++i) {
            //qDebug() << "ctl=" << i.key() << "val=" << i.value();
            sendController(i.key(), i.value());
        }
        //qDebug() << "bank=" << m_lastBank[m_channel];
        sendBankChange(m_lastBank[m_baseChannel]);
        //qDebug() << "prog=" << m_lastProg[m_channel];
        sendProgramChange(m_lastProg[m_baseChannel]);
    }
}

void VPiano::slotKeyboardInput(bool value)
{
    dlgPreferences()->setEnabledKeyboard(value);
    currentPianoScene()->setKeyboardEnabled(value);
}

void VPiano::slotMouseInput(bool value)
{
    dlgPreferences()->setEnabledMouse(value);
    currentPianoScene()->setMouseEnabled(value);
}

void VPiano::slotTouchScreenInput(bool value)
{
    dlgPreferences()->setEnabledTouch(value);
    currentPianoScene()->setTouchEnabled(value);
}

void VPiano::slotColorPolicy()
{
    if (dlgColorPolicy()->exec() == QDialog::Accepted) {
        m_currentPalette = dlgColorPolicy()->currentPalette()->paletteId();
        currentPianoScene()->setPianoPalette(dlgColorPolicy()->currentPalette());
    }
}

void VPiano::slotColorScale(bool value)
{
    currentPianoScene()->setShowColorScale(value);
}

PianoScene *VPiano::currentPianoScene()
{
    return ui.pianokeybd->getPianoScene();
}
