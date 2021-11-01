/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 2.6

// Info text component.
Item {
    id: container

    property bool show: false
    property alias textPointSize: text.font.pointSize
    property string url: "https://github.com/lduboeuf/drumkit-qt"

    opacity: show ? 1.0 : 0

    Behavior on opacity {
        PropertyAnimation { duration: 700; easing {type: Easing.OutQuad} }
    }

    // Dim background while showing info.
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.6
    }

    MouseArea {
        anchors.fill: parent
        enabled: show
        onPressed: show = false
    }

    Column {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width

        Text {
            id: text
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            font.pointSize: 18
            font.bold: true
            text: "Drumkit v1.0\n\n"+
                  "Let your fills fly by tapping the pads.\n"+
                  "You may also record your beats and play them afterwards.\n"+
                  "Long tap on a pad lets you select different instruments.\n"+
                  "Tap the screen to continue! \n\n"+
                  "Drumkit is initially a Nokia Developer example application hosted now at:"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            id: url
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            font.pointSize: text.font.pointSize - 1
            text: "\n" + container.url
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            MouseArea {
                anchors.fill: url
                onClicked: {
                    if (container.show){
                        Qt.openUrlExternally(container.url)
                    }
                }
            }
        }
    }
}
