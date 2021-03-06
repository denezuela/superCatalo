import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

Item {
    id: item1
    //property int buttonWidth: 200;
    //property int buttonHeight: 100;
    property int shadowOffset: 5

    Rectangle {
        gradient: Gradient {
            GradientStop {
                position: 0.52
                color: "#7bb8d1"
            }

            GradientStop {
                position: 1
                color: "#0e5776"
            }
        }
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.fill: parent
}

    Component {
        id: buttonStyle
        ButtonStyle {
            background: Rectangle{
                color: "#398eb1"
                border.color: "#135083"
                radius: 4
            }
            label: Text {
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.family: "Helvetica"
                font.pointSize: 12
                font.bold: true
                font.italic: true
                color: "black"
                text: control.text
            }
        }  
      }

    Button {
        id:button_rotation
        x: 19
        y: 13
        width: 28
        height: 27
        text:"Поворот"
        style: buttonStyle
        onClicked: { image.rotation +=90 }

    }

//    Button {
//        id:button_delete
//        x: 62
//        y: 13
//        width: 28
//        height: 27
//        //Надо привязать положение кнопки к родителю!
//        //text:"-"
//        style: buttonStyle
//        Image {
//            source: "images/delete.svg"
//            antialiasing: true
//            anchors.fill: parent
//            z:2
//        }
//    }

//    Button {
//        id: button_cut
//        //enabled: true
//        //tooltip: "Выделите изображение"
//        x: 390
//        y: 13
//        width: 83
//        height: 27
//        text: "Обрезать"
//        style: buttonStyle
//    }

    Button {
        id: button_close
        x:604
        y:7
        width: 28
        height: 27
        //iconSource: "close.png"
        style: buttonStyle
        //onTriggered: Qt.quit();
    }

}
