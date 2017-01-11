import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Rectangle {
    width: 700
    height: 550
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
