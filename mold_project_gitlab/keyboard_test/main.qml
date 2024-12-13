import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.VirtualKeyboard 2.1

Rectangle {
    color: "red"
    TextField {
        width: parent.width
        placeholderText: "One line field"
    }
    InputPanel {
        x: 0;
        y: 40;
        width: parent.width
        height: 200
    }
}
