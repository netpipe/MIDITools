/*

    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see http://www.gnu.org/licenses/
 
*/

/********************************************************************************
** Form generated from reading UI file 'preferences.ui'
**
** Created: Sun Sep 8 01:15:30 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCES_H
#define UI_PREFERENCES_H

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreferencesClass
{
public:
    QGridLayout *gridLayout_2;
    QDialogButtonBox *buttonBox;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QLabel *lblNumOctaves;
    QLabel *lblKeyPressColor;
    QPushButton *btnColor;
    QLabel *lblFileInstrument;
    QLineEdit *txtFileInstrument;
    QPushButton *btnInstrument;
    QLabel *lblInstrument;
    QLabel *lblKmap;
    QLineEdit *txtFileKmap;
    QPushButton *btnKmap;
    QLabel *lblRawKmap;
    QLineEdit *txtFileRawKmap;
    QPushButton *btnRawKmap;
    QLabel *lblDrumsChannel;
    QComboBox *cboDrumsChannel;
    QLabel *lblNetworkPort;
    QLineEdit *txtNetworkPort;
    QCheckBox *chkStyledKnobs;
    QCheckBox *chkAlwaysOnTop;
    QCheckBox *chkRawKeyboard;
    QCheckBox *chkVelocityColor;
    QCheckBox *chkGrabKb;
    QLabel *lblNetworkIface;
    QComboBox *cboNetworkIface;
    QLabel *lblMIDIDriver;
    QComboBox *cboMIDIDriver;
    QCheckBox *chkEnforceChannelState;
    QCheckBox *chkEnableTouch;
    QCheckBox *chkEnableMouse;
    QCheckBox *chkEnableKeyboard;
    QComboBox *cboColorPolicy;
    QSpinBox *spinNumOctaves;
    QComboBox *cboInstrument;

    void setupUi(QDialog *PreferencesClass)
    {
        if (PreferencesClass->objectName().isEmpty())
            PreferencesClass->setObjectName(QString::fromUtf8("PreferencesClass"));
        PreferencesClass->resize(401, 437);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/vpiano/vmpk_32x32.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesClass->setWindowIcon(icon);
        gridLayout_2 = new QGridLayout(PreferencesClass);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(3, 3, 3, 3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        buttonBox = new QDialogButtonBox(PreferencesClass);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::RestoreDefaults);

        gridLayout_2->addWidget(buttonBox, 3, 0, 1, 1);

        scrollArea = new QScrollArea(PreferencesClass);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 395, 404));
        scrollAreaWidgetContents->setMinimumSize(QSize(382, 394));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout->setHorizontalSpacing(3);
        gridLayout->setVerticalSpacing(1);
        gridLayout->setContentsMargins(3, 1, 3, 1);
        lblNumOctaves = new QLabel(scrollAreaWidgetContents);
        lblNumOctaves->setObjectName(QString::fromUtf8("lblNumOctaves"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblNumOctaves->sizePolicy().hasHeightForWidth());
        lblNumOctaves->setSizePolicy(sizePolicy);

        gridLayout->addWidget(lblNumOctaves, 0, 0, 1, 1);

        lblKeyPressColor = new QLabel(scrollAreaWidgetContents);
        lblKeyPressColor->setObjectName(QString::fromUtf8("lblKeyPressColor"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lblKeyPressColor->sizePolicy().hasHeightForWidth());
        lblKeyPressColor->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lblKeyPressColor, 2, 0, 1, 1);

        btnColor = new QPushButton(scrollAreaWidgetContents);
        btnColor->setObjectName(QString::fromUtf8("btnColor"));

        gridLayout->addWidget(btnColor, 2, 2, 1, 1);

        lblFileInstrument = new QLabel(scrollAreaWidgetContents);
        lblFileInstrument->setObjectName(QString::fromUtf8("lblFileInstrument"));
        sizePolicy1.setHeightForWidth(lblFileInstrument->sizePolicy().hasHeightForWidth());
        lblFileInstrument->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lblFileInstrument, 3, 0, 1, 1);

        txtFileInstrument = new QLineEdit(scrollAreaWidgetContents);
        txtFileInstrument->setObjectName(QString::fromUtf8("txtFileInstrument"));
        txtFileInstrument->setMinimumSize(QSize(120, 0));
        txtFileInstrument->setReadOnly(true);

        gridLayout->addWidget(txtFileInstrument, 3, 1, 1, 1);

        btnInstrument = new QPushButton(scrollAreaWidgetContents);
        btnInstrument->setObjectName(QString::fromUtf8("btnInstrument"));

        gridLayout->addWidget(btnInstrument, 3, 2, 1, 1);

        lblInstrument = new QLabel(scrollAreaWidgetContents);
        lblInstrument->setObjectName(QString::fromUtf8("lblInstrument"));
        sizePolicy1.setHeightForWidth(lblInstrument->sizePolicy().hasHeightForWidth());
        lblInstrument->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lblInstrument, 4, 0, 2, 1);

        lblKmap = new QLabel(scrollAreaWidgetContents);
        lblKmap->setObjectName(QString::fromUtf8("lblKmap"));

        gridLayout->addWidget(lblKmap, 6, 0, 1, 1);

        txtFileKmap = new QLineEdit(scrollAreaWidgetContents);
        txtFileKmap->setObjectName(QString::fromUtf8("txtFileKmap"));

        gridLayout->addWidget(txtFileKmap, 6, 1, 1, 1);

        btnKmap = new QPushButton(scrollAreaWidgetContents);
        btnKmap->setObjectName(QString::fromUtf8("btnKmap"));

        gridLayout->addWidget(btnKmap, 6, 2, 1, 1);

        lblRawKmap = new QLabel(scrollAreaWidgetContents);
        lblRawKmap->setObjectName(QString::fromUtf8("lblRawKmap"));

        gridLayout->addWidget(lblRawKmap, 7, 0, 1, 1);

        txtFileRawKmap = new QLineEdit(scrollAreaWidgetContents);
        txtFileRawKmap->setObjectName(QString::fromUtf8("txtFileRawKmap"));

        gridLayout->addWidget(txtFileRawKmap, 7, 1, 1, 1);

        btnRawKmap = new QPushButton(scrollAreaWidgetContents);
        btnRawKmap->setObjectName(QString::fromUtf8("btnRawKmap"));

        gridLayout->addWidget(btnRawKmap, 7, 2, 1, 1);

        lblDrumsChannel = new QLabel(scrollAreaWidgetContents);
        lblDrumsChannel->setObjectName(QString::fromUtf8("lblDrumsChannel"));

        gridLayout->addWidget(lblDrumsChannel, 8, 0, 1, 1);

        cboDrumsChannel = new QComboBox(scrollAreaWidgetContents);
        cboDrumsChannel->setObjectName(QString::fromUtf8("cboDrumsChannel"));

        gridLayout->addWidget(cboDrumsChannel, 8, 1, 1, 1);

        lblNetworkPort = new QLabel(scrollAreaWidgetContents);
        lblNetworkPort->setObjectName(QString::fromUtf8("lblNetworkPort"));

        gridLayout->addWidget(lblNetworkPort, 10, 0, 1, 1);

        txtNetworkPort = new QLineEdit(scrollAreaWidgetContents);
        txtNetworkPort->setObjectName(QString::fromUtf8("txtNetworkPort"));

        gridLayout->addWidget(txtNetworkPort, 10, 1, 1, 1);

        chkStyledKnobs = new QCheckBox(scrollAreaWidgetContents);
        chkStyledKnobs->setObjectName(QString::fromUtf8("chkStyledKnobs"));
        chkStyledKnobs->setChecked(true);

        gridLayout->addWidget(chkStyledKnobs, 12, 0, 1, 3);

        chkAlwaysOnTop = new QCheckBox(scrollAreaWidgetContents);
        chkAlwaysOnTop->setObjectName(QString::fromUtf8("chkAlwaysOnTop"));

        gridLayout->addWidget(chkAlwaysOnTop, 13, 0, 1, 3);

        chkRawKeyboard = new QCheckBox(scrollAreaWidgetContents);
        chkRawKeyboard->setObjectName(QString::fromUtf8("chkRawKeyboard"));

        gridLayout->addWidget(chkRawKeyboard, 16, 0, 1, 3);

        chkVelocityColor = new QCheckBox(scrollAreaWidgetContents);
        chkVelocityColor->setObjectName(QString::fromUtf8("chkVelocityColor"));
        chkVelocityColor->setChecked(true);

        gridLayout->addWidget(chkVelocityColor, 17, 0, 1, 3);

        chkGrabKb = new QCheckBox(scrollAreaWidgetContents);
        chkGrabKb->setObjectName(QString::fromUtf8("chkGrabKb"));

        gridLayout->addWidget(chkGrabKb, 15, 0, 1, 3);

        lblNetworkIface = new QLabel(scrollAreaWidgetContents);
        lblNetworkIface->setObjectName(QString::fromUtf8("lblNetworkIface"));

        gridLayout->addWidget(lblNetworkIface, 11, 0, 1, 1);

        cboNetworkIface = new QComboBox(scrollAreaWidgetContents);
        cboNetworkIface->setObjectName(QString::fromUtf8("cboNetworkIface"));

        gridLayout->addWidget(cboNetworkIface, 11, 1, 1, 1);

        lblMIDIDriver = new QLabel(scrollAreaWidgetContents);
        lblMIDIDriver->setObjectName(QString::fromUtf8("lblMIDIDriver"));

        gridLayout->addWidget(lblMIDIDriver, 9, 0, 1, 1);

        cboMIDIDriver = new QComboBox(scrollAreaWidgetContents);
        cboMIDIDriver->setObjectName(QString::fromUtf8("cboMIDIDriver"));

        gridLayout->addWidget(cboMIDIDriver, 9, 1, 1, 1);

        chkEnforceChannelState = new QCheckBox(scrollAreaWidgetContents);
        chkEnforceChannelState->setObjectName(QString::fromUtf8("chkEnforceChannelState"));

        gridLayout->addWidget(chkEnforceChannelState, 18, 0, 1, 3);

        chkEnableTouch = new QCheckBox(scrollAreaWidgetContents);
        chkEnableTouch->setObjectName(QString::fromUtf8("chkEnableTouch"));
        chkEnableTouch->setChecked(true);

        gridLayout->addWidget(chkEnableTouch, 21, 0, 1, 3);

        chkEnableMouse = new QCheckBox(scrollAreaWidgetContents);
        chkEnableMouse->setObjectName(QString::fromUtf8("chkEnableMouse"));
        chkEnableMouse->setChecked(true);

        gridLayout->addWidget(chkEnableMouse, 20, 0, 1, 3);

        chkEnableKeyboard = new QCheckBox(scrollAreaWidgetContents);
        chkEnableKeyboard->setObjectName(QString::fromUtf8("chkEnableKeyboard"));
        chkEnableKeyboard->setChecked(true);

        gridLayout->addWidget(chkEnableKeyboard, 14, 0, 1, 3);

        cboColorPolicy = new QComboBox(scrollAreaWidgetContents);
        cboColorPolicy->setObjectName(QString::fromUtf8("cboColorPolicy"));

        gridLayout->addWidget(cboColorPolicy, 2, 1, 1, 1);

        spinNumOctaves = new QSpinBox(scrollAreaWidgetContents);
        spinNumOctaves->setObjectName(QString::fromUtf8("spinNumOctaves"));
        spinNumOctaves->setMinimum(1);
        spinNumOctaves->setMaximum(9);

        gridLayout->addWidget(spinNumOctaves, 0, 1, 1, 1);

        cboInstrument = new QComboBox(scrollAreaWidgetContents);
        cboInstrument->setObjectName(QString::fromUtf8("cboInstrument"));

        gridLayout->addWidget(cboInstrument, 4, 1, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout_2->addWidget(scrollArea, 0, 0, 1, 1);

#ifndef QT_NO_SHORTCUT
        lblNumOctaves->setBuddy(spinNumOctaves);
        lblKeyPressColor->setBuddy(cboColorPolicy);
        lblFileInstrument->setBuddy(txtFileInstrument);
        lblInstrument->setBuddy(cboInstrument);
        lblKmap->setBuddy(txtFileKmap);
        lblRawKmap->setBuddy(txtFileRawKmap);
        lblDrumsChannel->setBuddy(cboDrumsChannel);
        lblNetworkPort->setBuddy(txtNetworkPort);
        lblNetworkIface->setBuddy(cboNetworkIface);
        lblMIDIDriver->setBuddy(cboMIDIDriver);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(scrollArea, btnColor);
        QWidget::setTabOrder(btnColor, txtFileInstrument);
        QWidget::setTabOrder(txtFileInstrument, btnInstrument);
        QWidget::setTabOrder(btnInstrument, txtFileKmap);
        QWidget::setTabOrder(txtFileKmap, btnKmap);
        QWidget::setTabOrder(btnKmap, txtFileRawKmap);
        QWidget::setTabOrder(txtFileRawKmap, btnRawKmap);
        QWidget::setTabOrder(btnRawKmap, cboDrumsChannel);
        QWidget::setTabOrder(cboDrumsChannel, cboMIDIDriver);
        QWidget::setTabOrder(cboMIDIDriver, txtNetworkPort);
        QWidget::setTabOrder(txtNetworkPort, cboNetworkIface);
        QWidget::setTabOrder(cboNetworkIface, chkStyledKnobs);
        QWidget::setTabOrder(chkStyledKnobs, chkAlwaysOnTop);
        QWidget::setTabOrder(chkAlwaysOnTop, chkGrabKb);
        QWidget::setTabOrder(chkGrabKb, chkRawKeyboard);
        QWidget::setTabOrder(chkRawKeyboard, chkVelocityColor);
        QWidget::setTabOrder(chkVelocityColor, buttonBox);


        QObject::connect(buttonBox, SIGNAL(accepted()), PreferencesClass, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PreferencesClass, SLOT(reject()));
        QObject::connect(chkEnableKeyboard, SIGNAL(toggled(bool)), chkGrabKb, SLOT(setEnabled(bool)));
        QObject::connect(chkEnableKeyboard, SIGNAL(toggled(bool)), chkRawKeyboard, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(PreferencesClass);
    } // setupUi

  
};

namespace Ui {
    class PreferencesClass: public Ui_PreferencesClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCES_H
