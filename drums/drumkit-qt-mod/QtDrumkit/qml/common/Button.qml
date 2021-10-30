/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 2.0

// A togglable button that can be enabled or disabled.
Item {
    property string image;
    property string imagePressed
    property bool enabled: true
    property bool toggled: false

    signal pressed
    signal released

    width: childrenRect.width
    height: childrenRect.height
    opacity: enabled ? 1 : 0.5

    Image {
        id: img
        source: parent.toggled || mouseArea.pressed ? gfxPath + imagePressed : gfxPath + image
    }

    MouseArea {
        id: mouseArea
        anchors.fill: img
        enabled: parent.enabled
        onPressed: parent.pressed()
        onReleased: parent.released()
    }
}
