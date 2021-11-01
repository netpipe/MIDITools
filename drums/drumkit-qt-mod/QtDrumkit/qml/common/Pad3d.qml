/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 2.0

// Pad for the 3d view, provides a bouncy animation when pad is hit.
Item {
    width: img.width
    height: img.height

    property alias sourceImage: img.source
    property alias sample: pad.sample

    function play() {
        pad.play()
    }

    Image {
        id: img
        scale: pad.pressed ? 0.9 : 1.0
        Behavior on scale { SpringAnimation { spring: 5; damping: 0.2 } }
    }

    Pad {
        id: pad
        anchors.fill: img
        showSplash: false
    }
}
