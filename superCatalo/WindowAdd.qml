import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: item
    anchors.centerIn: parent
    width: 377
    height: 132
    visible: false
    color: "#398eb1"
    border.width: 1
    border.color: "#3877a8"
    radius: 8

    Button {
        id:buttonOk
        x: 151
        y: 82
        width: 80
        height: 27
        text: qsTr("ОК")
        style:
            ButtonStyle {
            background: Rectangle{
                color: "#23637a"
                border.color: "#23637a"
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
        MouseArea
        {
          // Действует в пределах всего элемента Image
          anchors.fill: parent
          id: mouseAreabutton
          onClicked: callbackOK()
        }
        states:[
          State
          {
            name: "hovered";
            // Указание на то, когда элемент переходит в это состояние
            // в данном случае когда нажата левая кнопка мыши
            when: mouseAreabutton.pressed;
            // Какие свойства будут изменяться в этом состоянии (прозрачность)
            PropertyChanges { target: buttonOk; opacity: 0.4;}
          },
          State
          {
            name: "normal"
            // В это состояние элемент будет переходить когда левая кнопка мыши не нажата
            when: mouseAreabutton.pressed == false;
            PropertyChanges { target: buttonOk; opacity: 1; }
          }
         ]
    }

    WindowButton
    {
      anchors.right: parent.right
      anchors.rightMargin: 3
      anchors.top: parent.top
      anchors.topMargin: 3
    }

}
