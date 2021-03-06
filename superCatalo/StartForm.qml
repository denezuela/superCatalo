import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
    Rectangle {
        gradient: Gradient {
            GradientStop {
                position: 0.02
                color: "#ffffff"
            }

            GradientStop {
                position: 0.5
                color: "#2bb2eb"
            }

            GradientStop {
                position: 0.949
                color: "#1b4251"
            }
        }
        anchors.fill: parent
}
    WindowButton
    {
      // Кнопка закрытия окна
      id: button_close
      anchors.right: parent.right
      anchors.rightMargin: 5
      function callbackClose() {Qt.quit()}
    }
}
