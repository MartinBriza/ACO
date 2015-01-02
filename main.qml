/**
 * @project Ant colony optimization
 * @brief Main UI file
 * @author Martin Bříza <xbriza00@stud.fit.vutbr.cz>
 */

import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Particles 2.0

import fit.sfc.aco 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 800
    title: qsTr("Ant Colony Optimization")

    Aco {
        id: aco
    }

    toolBar: ToolBar {
        Row {
            spacing: 2
            FileDialog {
                property string previous
                id: fileDialog
                selectExisting: false
                onAccepted: {
                    if (selectExisting == false) {
                        previous = fileUrl
                        aco.saveTo(fileUrl)
                    }
                    else {
                        aco.loadFrom(fileUrl)
                    }
                }
            }

            ToolButton {
                id: newButton
                text: "New"
                onClicked: aco.clear()
            }

            ToolButton {
                id: loadButton
                text: "Load"
                onClicked: {
                    fileDialog.selectExisting = true
                    fileDialog.open()
                }
            }
            ToolButton {
                id: saveButton
                text: "Save"
                enabled: fileDialog.previous.length > 0 ? true : false
                onClicked: {
                    aco.saveTo(fileDialog.previous)
                }
            }
            ToolButton {
                id: saveAsButton
                text: "Save As"
                onClicked: {
                    fileDialog.selectExisting = false
                    fileDialog.open()
                }
            }
            Rectangle {
                color: "#888888"
                width: 1
                y: 2
                height: parent.height - 4
            }

            ToolButton {
                id: resetButton
                text: "Reset"
                onClicked: aco.algorithm.reset()
            }
            ToolButton {
                id: initButton
                text: "Init " + (aco.algorithm.initialized ? "✔" : "✖")
                onClicked: aco.algorithm.roundInit()
            }
            ToolButton {
                id: stepButton
                text: "Step"
                onClicked: aco.algorithm.step()
            }
            TextField {
                text: aco.algorithm.s
                enabled: false
                readOnly: true
                width: 50
            }
            TextField {
                text: aco.algorithm.t
                enabled: false
                readOnly: true
                width: 50
            }

            ToolButton {
                id: cycleButton
                text: "Cycle"
                Timer {
                    id: cycleTimer
                    interval: aco.animationSpeed * 1.1
                    repeat: true
                    onTriggered:  {
                        aco.algorithm.step();
                        if (aco.algorithm.s == 0)
                            cycleTimer.stop()
                    }
                }

                onClicked: {
                    aco.algorithm.step()
                    if (aco.s != 0)
                        cycleTimer.running = true
                }
            }
            TextField {
                text: aco.algorithm.c
                enabled: false
                readOnly: true
                width: 50
            }
            ToolButton {
                id: playButton
                text: "Play (Until)"
                checkable: true
                Timer {
                    id: playTimer
                    interval: aco.animationSpeed * 1.1
                    repeat: true
                    running: playButton.checked
                    onTriggered:  {
                        aco.algorithm.step();
                        if (aco.algorithm.c >= parseInt(cycleLimit.text))
                            playTimer.stop()
                    }
                }
            }
            TextField {
                width: 50
                id: cycleLimit
                text: "cycles"
            }
        }
    }

    Item {
        id: centralItem
        anchors.fill: parent
        Rectangle {
            id: info
            clip: true
            color: "transparent"
            width: 270
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            GroupBox {
                clip: true
                id: uiOptions
                title: "User Interface Options"
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.left: parent.left
                Grid {
                    height: childrenRect.height
                    clip: true
                    verticalItemAlignment: Grid.AlignVCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    columns: 2
                    spacing: 6
                    Text {
                        width: antCountText.width
                        text: "Distance"
                        horizontalAlignment: Text.AlignRight
                        MouseArea {
                            anchors.fill: parent
                            onClicked: displayDistance.checked = !displayDistance.checked
                        }
                    }
                    CheckBox {
                        id: displayDistance
                        checked: true
                    }
                    Text {
                        width: antCountText.width
                        text: "Names"
                        horizontalAlignment: Text.AlignRight
                        MouseArea {
                            anchors.fill: parent
                            onClicked: displayNames.checked = !displayNames.checked
                        }
                    }
                    CheckBox {
                        id: displayNames
                        checked: true
                    }
                    Text {
                        width: antCountText.width
                        text: "Pheromone Trails"
                        horizontalAlignment: Text.AlignRight
                        MouseArea {
                            anchors.fill: parent
                            onClicked: displayTrails.checked = !displayTrails.checked
                        }
                    }
                    CheckBox {
                        id: displayTrails
                        checked: true
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Shortest Path:"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: displayShortestPath.checked = !displayShortestPath.checked
                        }
                    }
                    CheckBox {
                        id: displayShortestPath
                        checked: true
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Animation Duration:"
                    }
                    Slider {
                        id: animationSpeedSlider
                        minimumValue: 10
                        value: 100
                        maximumValue: 1000
                        updateValueWhileDragging: false
                        onValueChanged: {
                            aco.animationSpeed = value
                        }
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Canvas Width:"
                    }
                    SpinBox {
                        value: scene.width
                        width: antCountInput.width
                        onValueChanged: scene.width = value
                        maximumValue: 999999
                        stepSize: 16
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Canvas Height:"
                    }
                    SpinBox {
                        value: scene.height
                        width: antCountInput.width
                        onValueChanged: scene.height = value
                        maximumValue: 999999
                        stepSize: 16
                    }
                }
            }
            GroupBox {
                id: simOptions
                clip: true
                title: "Simulation Options"
                anchors.left: uiOptions.left
                anchors.right: uiOptions.right
                anchors.top: uiOptions.bottom
                height: simOptionsGrid.height + (buttonRow.height + 6)* 2
                Grid {
                    id: simOptionsGrid
                    verticalItemAlignment: Grid.AlignVCenter
                    columns: 2
                    spacing: 6
                    Text {
                        id: antCountText
                        width: 120
                        horizontalAlignment: Text.AlignRight
                        text: "Ant Count:"
                    }
                    SpinBox {
                        width: 128
                        id: antCountInput
                        value: aco.algorithm.antCount
                        minimumValue: 1
                        maximumValue: 9999999
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Initial τ:"
                    }
                    SpinBox {
                        id: initialTauInput
                        value: aco.initialTau
                        width: antCountInput.width
                        stepSize: 0.1
                        decimals: 2
                        minimumValue: -9999999
                        maximumValue: 9999999
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "α:"
                    }
                    SpinBox {
                        id: alphaInput
                        width: antCountInput.width
                        value: aco.algorithm.alpha
                        stepSize: 0.1
                        decimals: 2
                        minimumValue: -9999999
                        maximumValue: 9999999
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "β:"
                    }
                    SpinBox {
                        id: betaInput
                        width: antCountInput.width
                        value: aco.algorithm.beta
                        stepSize: 0.1
                        decimals: 2
                        minimumValue: -9999999
                        maximumValue: 9999999
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Q:"
                    }
                    SpinBox {
                        id: qInput
                        width: antCountInput.width
                        value: aco.algorithm.q
                        stepSize: 0.1
                        decimals: 2
                        minimumValue: -9999999
                        maximumValue: 9999999
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "ρ:"
                    }
                    SpinBox {
                        id: roInput
                        width: antCountInput.width
                        value: aco.algorithm.ro
                        stepSize: 0.01
                        decimals: 2
                        minimumValue: 0.0
                        maximumValue: 1.0
                    }
                    Text {
                        width: antCountText.width
                        horizontalAlignment: Text.AlignRight
                        text: "Algorithm:"
                    }
                    ComboBox {
                        id: algoCB
                        width: antCountInput.width
                        model: [ "Ant Cycle", "Ant-Density", "Ant-Quantity" ]
                        currentIndex: aco.chosenAlgo
                    }
                }
                Row {
                    id: buttonRow
                    anchors.right: parent.right
                    anchors.topMargin: 6
                    anchors.top: simOptionsGrid.bottom
                    spacing: 6
                    height: childrenRect.height
                    Button {
                        text: "Cancel"
                        onClicked: {
                            alphaInput.value = aco.algorithm.alpha
                            betaInput.value = aco.algorithm.beta
                            initialTauInput.value = aco.initialTau
                            qInput.value = aco.algorithm.q
                            antCountInput.value = aco.algorithm.antCount
                            roInput.value = aco.algorithm.ro
                            algoCB.currentIndex = aco.chosenAlgo
                        }
                    }
                    Button {
                        text: "Accept"
                        onClicked: {
                            aco.algorithm.alpha = alphaInput.value
                            aco.algorithm.beta = betaInput.value
                            aco.initialTau = initialTauInput.value
                            aco.algorithm.q = qInput.value
                            aco.algorithm.antCount = antCountInput.value
                            aco.algorithm.ro = roInput.value
                            aco.chosenAlgo = algoCB.currentIndex
                        }
                    }

                }
            }
            GroupBox {
                clip: true
                title: "Controls"
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                Text {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    text: "<b>Clicking</b> changes focus and enables editable fields.<br><b>Dragging</b> moves the towns<br><b>Double clicking</b> adds a new town.<br><b><font color='grey'>Right click and dragging</font></b><font color='grey'> adds paths between towns or deletes them if they are already present.</font><br><b>Middle button</b> deletes existing towns and their paths."
                }
            }
        }

        ScrollView {
            focus: true
            frameVisible: true
            anchors.left: info.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            Rectangle {
                id: scene
                height: 2000
                width: 2000
                color: "#aaffffff"

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onDoubleClicked: {
                        if (!mouse.wasHeld && mouse.button == Qt.LeftButton && !aco.townAt(mouse.x, mouse.y)) {
                            aco.newTown(mouse.x - aco.townSize / 2, mouse.y - aco.townSize / 2)
                            mouse.accepted = true
                        }
                    }
                    onPressed: {
                        var town = aco.townAt(mouse.x, mouse.y)
                        if (town && mouse.button == Qt.RightButton) {
                            drawingLine.x1 = mouse.x
                            drawingLine.y1 = mouse.y
                            mouse.accepted = true
                            drawingLine.visible = true
                        }
                    }

                    onReleased: {
                        if (mouse.button == Qt.RightButton && drawingLine.visible) {
                            aco.togglePath(aco.townAt(drawingLine.x1, drawingLine.y1), aco.townAt(mouse.x, mouse.y))
                        }

                        drawingLine.visible = false

                    }

                    Repeater {
                        model: displayTrails.checked ? aco.paths : null
                        delegate: Line {
                            height: aco.townSize / 2
                            gradient: Gradient {
                                GradientStop { position: 0.5 - Math.sqrt(trail - aco.initialTau) / 10.0; color: "transparent" }
                                GradientStop { position: 0.5; color: "#44dd44" }
                                GradientStop { position: 0.5 + Math.sqrt(trail - aco.initialTau) / 10.0; color: "transparent" }
                            }
                            x1: a.pos_x + aco.townSize / 2
                            y1: a.pos_y + aco.townSize / 2
                            x2: b.pos_x + aco.townSize / 2
                            y2: b.pos_y + aco.townSize / 2
                        }
                    }
                    Repeater {
                        model: displayDistance.checked ? aco.paths : null
                        delegate: Line {
                            height: 2
                            color: "#cccccc"
                            x1: a.pos_x + aco.townSize / 2
                            y1: a.pos_y + aco.townSize / 2
                            x2: b.pos_x + aco.townSize / 2
                            y2: b.pos_y + aco.townSize / 2
                            Rectangle {
                                anchors.fill: distanceInput
                                anchors.margins: -2
                                opacity: distanceInput.focus ? 1 : 0.7
                                color: distanceInput.focus ? "#8888dd" : "#cccccc"
                                Behavior on color {
                                    ColorAnimation {
                                        duration: 200
                                    }
                                }
                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                }
                            }
                            TextInput {
                                id: distanceInput
                                color: "#444444"
                                x: (parent.width - width) / 2
                                y: 3
                                rotation: parent.x2 < parent.x1 ? 180: 0
                                text: distance.toFixed(2)
                                onAccepted: {
                                    distance = text
                                    focus = false
                                }
                                validator: DoubleValidator {
                                    bottom: 0.0
                                }
                            }
                        }
                    }
                    Repeater {
                        model: displayShortestPath.checked ? aco.algorithm.shortestTrip : null
                        delegate: Line {
                            height: 2
                            color: "#ff8888"
                            x1: a.pos_x + aco.townSize / 2
                            y1: a.pos_y + aco.townSize / 2
                            x2: b.pos_x + aco.townSize / 2
                            y2: b.pos_y + aco.townSize / 2
                        }
                    }
                    Repeater {
                        anchors.fill: parent
                        model: aco.towns
                        delegate: Rectangle {
                            id: townCircle
                            color: focus ? "#aa0000aa" : "#88ffffff"
                            border.color: townCircleMouseArea.containsMouse ? "#cccccc" : "#bbbbbb"
                            border.width: 3
                            Behavior on color {
                                ColorAnimation {
                                    duration: 200
                                }
                            }

                            focus: false
                            x: pos_x
                            y: pos_y
                            width: aco.townSize
                            height: aco.townSize
                            radius: width / 2
                            onXChanged: pos_x = x
                            onYChanged: pos_y = y;

                            Keys.onPressed: {
                                if (event.key == Qt.Key_Delete) {
                                    aco.deleteTown(aco.townAt(x, y))
                                }
                            }

                            Rectangle {
                                visible: displayNames.checked
                                anchors.fill: nameInput
                                anchors.margins: -2
                                opacity: nameInput.focus ? 1 : 0.7
                                color: nameInput.focus ? "#8888dd" : "#cccccc"
                                Behavior on color {
                                    ColorAnimation {
                                        duration: 200
                                    }
                                }
                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                }
                            }
                            TextInput {
                                id: nameInput
                                visible: displayNames.checked
                                color: "#444444"
                                text: name
                                anchors.horizontalCenter: townCircle.horizontalCenter
                                anchors.margins: 2
                                anchors.top: townCircle.bottom
                                onAccepted: {
                                    if (text != "")
                                        name = text
                                    else
                                        text = name
                                    focus = false
                                }
                            }
                            MouseArea {
                                acceptedButtons: Qt.LeftButton | Qt.MiddleButton
                                id: townCircleMouseArea
                                hoverEnabled: true
                                anchors.fill: parent
                                drag.target: parent
                                drag.axis: Drag.XandYAxis
                                onClicked: {
                                    if (mouse.button == Qt.LeftButton)
                                        parent.focus = true
                                    else if (mouse.button == Qt.MiddleButton)
                                        aco.deleteTown(aco.townAt(townCircle.x, townCircle.y))
                                }
                            }
                        }
                    }
                    Repeater {
                        anchors.fill: parent
                        model: aco.algorithm.ants
                        delegate: Image {
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                            source: "qrc:ant.png"
                            x: pos_x
                            y: pos_y
                            width: aco.townSize
                            height: aco.townSize

                            Behavior on x {
                                NumberAnimation {
                                    duration: aco.animationSpeed
                                }
                            }
                            Behavior on y {
                                NumberAnimation {
                                    duration: aco.animationSpeed
                                }
                            }
                        }
                    }
                }
                Line {
                    id: drawingLine
                    x1: 10
                    y1: 10
                    x2: mouseArea.mouseX
                    y2: mouseArea.mouseY
                    height: 2
                    color: "#dd8888"
                    visible: false
                    onX2Changed: setColor()
                    onY2Changed: setColor()
                    function setColor() {
                        if (aco.townAt(x2, y2))
                            color = "#88dd88"
                        else
                            color = "#dd8888"
                    }
                }
            }
        }
    }
}
