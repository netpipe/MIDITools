import QtQuick 2.0
import QtMultimedia 5.2

Item {
    id: root

    // api

    property bool isPlaying: false
    property bool isRecording: false
    property bool emptyTrack: track.length == 0

    function startRecording() {
        track = [];
        isRecording = true;
        startTime = new Date().getTime();
    }

    function stopRecording() {
        if (track.length > 0) {
            recordHit(undefined);
        }

        isRecording = false;
        startTime = 0;
    }

    function playTrack() {
        isPlaying = true;
        index = 0;

        playHit();
        timer.start();
    }

    function stopPlaying() {
        timer.stop();

        isPlaying = false;
    }

    function playSample(sample) {
        if (isRecording) {
            recordHit(sample);
        }

        if (sample === "cowbell") {
            playSound(cowbell)
            //cowbell.play();
        } else if (sample === "crash") {
            playSound(crash)
            //crash.play();
        } else if (sample === "hihat1") {
            playSound(hihat1)
            //hihat1.play();
        } else if (sample === "hihat2") {
            playSound(hihat2)
            //hihat2.play();
        } else if (sample === "kick") {
            playSound(kick)
            //kick.play();
        } else if (sample === "ride1") {
            playSound(ride1)
            //ride1.play();
        } else if (sample === "ride2") {
            playSound(ride2)
            //ride2.play();
        } else if (sample === "snare") {
            playSound(snare)
            //snare.play();
        } else if (sample === "splash") {
            playSound(splash)
           // splash.play();
        } else if (sample === "tom1") {
            playSound(tom1)
            //tom1.play();
        } else if (sample === "tom2") {
            playSound(tom2)
            //tom2.play();
        } else if (sample === "tom3") {
            playSound(tom3)
            //tom3.play();
        } else if (sample === "china") {
            playSound(china)
            //china.play();
        } else {
            console.debug("oops!");
        }

    }

    function playSound(soundId){
        soundId.play()
        console.log("oki sound")
    }

    // internal

    property variant startTime
    property int index: 0
    property variant track: []

    function playHit() {
        var now = track[index];
        var next = track[index + 1];
        var isLast = index > track.length - 3;

        playSample(now.sample);

        if (next && !isLast) {
            index++;
        } else {
            index = 0;
        }

        timer.interval = next.time - now.time;
    }

    function recordHit(sample) {
        var t = track;
        t.push( { "time": new Date().getTime() - startTime, "sample": sample } );
        track = t;
    }

    SoundEffect { id: cowbell; source: "qrc:/samples/cowbell.wav" }
    SoundEffect { id: crash; source: "qrc:/samples/crash.wav" }
    SoundEffect { id: hihat1; source: "qrc:/samples/hihat1.wav" }
    SoundEffect { id: hihat2; source: "qrc:/samples/hihat2.wav" }
    SoundEffect { id: kick; source: "qrc:/samples/kick.wav" }
    SoundEffect { id: ride1; source: "qrc:/samples/ride1.wav" }
    SoundEffect { id: ride2; source: "qrc:/samples/ride2.wav" }
    SoundEffect { id: snare; source: "qrc:/samples/snare.wav" }
    SoundEffect { id: splash; source: "qrc:/samples/splash.wav" }
    SoundEffect { id: tom1; source: "qrc:/samples/tom1.wav" }
    SoundEffect { id: tom2; source: "qrc:/samples/tom2.wav" }
    SoundEffect { id: tom3; source: "qrc:/samples/tom3.wav" }
    SoundEffect { id: china; source: "qrc:/samples/china.wav" }

    Timer {
        id: timer

        repeat: true
        onTriggered: root.playHit()
    }
}
