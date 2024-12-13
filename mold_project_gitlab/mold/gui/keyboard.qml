import QtQuick 2.7
import QtQuick.VirtualKeyboard 2.1
import QtQuick.VirtualKeyboard.Settings 2.0

InputPanel {
    function setLanguage(language) {
        VirtualKeyboardSettings.locale = language;
    }
}
