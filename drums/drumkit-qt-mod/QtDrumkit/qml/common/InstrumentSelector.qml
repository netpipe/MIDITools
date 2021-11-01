/**
 * Copyright (c) 2011 Nokia Corporation.
 */

import QtQuick 2.0

// Instrument selector component.
// Arranges InstrumentButtons on a circle and performs transitions.
Item {
    id: container

    // Set to true to show.
    property bool show: false

    // Radius of the instrument selection circle
    property int radius: 100

    // The selected sample is stored here.
    property string selectedSample

    // The starting coordinate for the selector. This is set from each Pad.
    property int originX: width/2
    property int originY: height/2

    // Emitted when a sample has been selected.
    signal selected

    // Helper functions to calculate (x,y) for a given angle and radius.
    function xPos(angle) {
        return Math.cos(2*Math.PI/360*angle) * radius
    }

    function yPos(angle) {
        return Math.sin(2*Math.PI/360*angle) * radius
    }

    state: show ? "end" : "start"
    opacity: show ? 1.0 : 0

    onShowChanged: {
        show ? z=2: z=-2
    }

    Behavior on opacity {
        PropertyAnimation { duration: 700; easing {type: Easing.OutQuad} }
    }

    // Dim background.
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.5
    }

    // Block mouse events.
    MouseArea
    {
        anchors.fill: parent
    }

    // Close button is in the middle.
    Button {
        image: "selector/close.png"
        imagePressed: "selector/close.png"
        anchors.centerIn: parent
        onPressed: parent.selected()
    }

    // InstrumenButtons within a FocusScope so that only one gets
    // a selection highlight.
    FocusScope {
        id: buttons

        // The center of the InstrumentButton circle is animated.
        // The buttons are positioned around the origo. Translation
        // and rotation around the circle center are required.

        property int centerX
        property int centerY
        transform: [ Translate { x: buttons.centerX; y: buttons.centerY },
                     Rotation  { id: rotation; origin.x: buttons.centerX; origin.y: buttons.centerY} ]

        InstrumentButton {
            id: button1
            image: "selector/china.png"
            imagePressed: "selector/china_highlight.png"
            sample: "china"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button2
            image: "selector/crash.png"
            imagePressed: "selector/crash_highlight.png"
            sample: "crash"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button3
            image: "selector/splash.png"
            imagePressed: "selector/splash_highlight.png"
            sample: "splash"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button4
            image: "selector/hihat1.png"
            imagePressed: "selector/hihat1_highlight.png"
            sample: "hihat1"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button5
            image: "selector/hihat2.png"
            imagePressed: "selector/hihat2_highlight.png"
            sample: "hihat2"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button6
            image: "selector/ride1.png"
            imagePressed: "selector/ride1_highlight.png"
            sample: "ride1"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button7
            image: "selector/ride2.png"
            imagePressed: "selector/ride2_highlight.png"
            sample: "ride2"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button8
            image: "selector/kick.png"
            imagePressed: "selector/kick_highlight.png"
            sample: "kick"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button9
            image: "selector/snare.png"
            imagePressed: "selector/snare_highlight.png"
            sample: "snare"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button10
            image: "selector/tom1.png"
            imagePressed: "selector/tom1_highlight.png"
            sample: "tom1"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button11
            image: "selector/tom2.png"
            imagePressed: "selector/tom2_highlight.png"
            sample: "tom2"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button12
            image: "selector/tom3.png"
            imagePressed: "selector/tom3_highlight.png"
            sample: "tom3"
            onPressed: container.selectedSample = sample
        }
        InstrumentButton {
            id: button13
            image: "selector/cowbell.png"
            imagePressed: "selector/cowbell_highlight.png"
            sample: "cowbell"
            onPressed: container.selectedSample = sample
        }

    }

    // Initial state:
    // Center of the button circle is at specified originX and originY.
    // All buttons are in the middle of the circle at (0,0).
    // Initial rotation is at 180 degrees.
    states: [
        State {
            name: "start"
            PropertyChanges { target: buttons; centerX: originX; centerY: originY }
            PropertyChanges { target: rotation; angle: 180 }
            PropertyChanges { target: button1; x: 0; y: 0 }
            PropertyChanges { target: button2; x: 0; y: 0 }
            PropertyChanges { target: button3; x: 0; y: 0 }
            PropertyChanges { target: button4; x: 0; y: 0 }
            PropertyChanges { target: button5; x: 0; y: 0 }
            PropertyChanges { target: button6; x: 0; y: 0 }
            PropertyChanges { target: button7; x: 0; y: 0 }
            PropertyChanges { target: button8; x: 0; y: 0 }
            PropertyChanges { target: button9; x: 0; y: 0 }
            PropertyChanges { target: button10; x: 0; y: 0 }
            PropertyChanges { target: button11; x: 0; y: 0 }
            PropertyChanges { target: button12; x: 0; y: 0 }
            PropertyChanges { target: button13; x: 0; y: 0 }
        },
        // End state:
        // Center of the button circle is at the middle of the screen.
        // All buttons are positioned on the circle, divided equally.
        State {
            name: "end"
            PropertyChanges { target: buttons; centerX: container.width/2; centerY: container.height/2 }
            PropertyChanges { target: rotation; angle: 0 }
            PropertyChanges { target: button1; x: xPos(360/13*0); y: yPos(360/13*0) }
            PropertyChanges { target: button2; x: xPos(360/13*1); y: yPos(360/13*1) }
            PropertyChanges { target: button3; x: xPos(360/13*2); y: yPos(360/13*2) }
            PropertyChanges { target: button4; x: xPos(360/13*3); y: yPos(360/13*3) }
            PropertyChanges { target: button5; x: xPos(360/13*4); y: yPos(360/13*4) }
            PropertyChanges { target: button6; x: xPos(360/13*5); y: yPos(360/13*5) }
            PropertyChanges { target: button7; x: xPos(360/13*6); y: yPos(360/13*6) }
            PropertyChanges { target: button8; x: xPos(360/13*7); y: yPos(360/13*7) }
            PropertyChanges { target: button9; x: xPos(360/13*8); y: yPos(360/13*8) }
            PropertyChanges { target: button10; x: xPos(360/13*9); y: yPos(360/13*9) }
            PropertyChanges { target: button11; x: xPos(360/13*10); y: yPos(360/13*10) }
            PropertyChanges { target: button12; x: xPos(360/13*11); y: yPos(360/13*11) }
            PropertyChanges { target: button13; x: xPos(360/13*12); y: yPos(360/13*12) }
        }
    ]

    transitions: Transition {
        PropertyAnimation { duration: 700; target: buttons; properties: "centerX,centerY"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: rotation; properties: "angle"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button1; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button2; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button3; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button4; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button5; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button6; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button7; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button8; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button9; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button10; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button11; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button12; properties: "x,y"; easing {type: Easing.OutQuad} }
        PropertyAnimation { duration: 700; target: button13; properties: "x,y"; easing {type: Easing.OutQuad} }
    }
}
