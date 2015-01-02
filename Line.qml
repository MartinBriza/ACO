/**
 * @project Ant colony optimization
 * @brief Line QML type
 * @author Martin Bříza <xbriza00@stud.fit.vutbr.cz>
 */

import QtQuick 2.0

Rectangle {
    id: line
    smooth: true;
    antialiasing: true

    color: "black"
    height: 1

    x: x1
    y: y1 - height / 2
    property real x1
    property real y1
    property real x2
    property real y2

    width: Math.sqrt(Math.pow((x2-x1),2)+Math.pow((y2-y1),2))

    transformOrigin: Item.Left
    rotation: x2 - x1 == 0 ? (y1 < y2 ? 90 : 270) :
              (Math.atan((y2 - y1)/(x2 - x1))*180/Math.PI + (x1 > x2 ? 180 : 0))
}
