import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    id:window1
    visible: true
    width: 640
    height: 480
    color: "#7bb8d1"
    title: qsTr("Super Catalo")

    StartForm {
        id: startForm;
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent

        function showStartForm() {
           viewForm.visible = false
            addForm.visible = false
           addListForm.visible = false
           mainForm.visible = true
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("Добавить")
            MenuItem {
                text: qsTr("&Добавить семестр")
                onTriggered: item_term.visible=true
            }
            MenuItem {
                text: qsTr("&Добавить предмет")
                onTriggered: item_subject.visible=true
            }
            /*MenuItem {
                text: qsTr("&Добавить тему")
                onTriggered: item_theme.visible=true
            }*/
            MenuItem {
                text: qsTr("&Добавить изображение")
                onTriggered: item_image.visible=true
            }
        }
    }


    TreeView {
            id: treeView
            x: 9
            y: 42
            width: 186
            height: 407
            model: mymodel
            backgroundVisible: true
            frameVisible: true
            style: TreeViewStyle {
              alternateBackgroundColor: "#398eb1" //цвет полосок
              backgroundColor: "#398eb1" //цвет фона
            }
            TableViewColumn{
                        title:"Структура"
                        role: "display"
                        width:300 //появляется прокрутка

            }
            onDoubleClicked: {
                if(mymodel.data(index,1)){
                    image.source=mymodel.data(index,1);}
            }
        }

    Image {
        id: image
        x: 207
        y: 42
        width: 394
        height: 381

        MouseArea {
            // действуем в пределах всего элемента Image
            anchors.fill: parent
            id: mouseArea_2
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: 0
        }
    }

    Rectangle {
        id: item_term
        x: 132
        y: 160
        width: 377
        height: 132
        visible: false
        color: "#7bb8d1"
        border.width: 1

        Button {
            id: button_ok
            x: 151
            y: 82
            text: qsTr("ОК")
            onClicked: {
                mymodel.addSemester(textField_term.text);
                item_term.visible=false;
               }
        }

        Button {
            id: button_close
            x: 348
            y: 1
            width: 28
            height: 27
            iconSource: "images/close.png"
            //iconSource: "close.png"
            onClicked: item_term.visible=false
        }

        TextField {
            x: 66
            y: 31
            width: 250
            height: 33
            id: textField_term
            placeholderText: qsTr("Введите номер семестра")
        }
    }

   /* Rectangle {
        id: item_subject
        x: 109
        y: 121
        width: 429
        height: 238
        visible: false
        color: "#7bb8d1"
        border.width: 1

     TextField {
            x: 66
            y: 31
            width: 250
            height: 33
            id: textField_subject
            placeholderText: qsTr("Введите название предмета")

        }

        Button {
            id: button_ok1
            x: 172
            y: 192
            text: qsTr("ОК")
            //onClicked: label1.text=textField_subject.text //Берет текст и выводит на label
//            onClicked: {
//                mymodel.addSemester(textField_term.text);
//                item_term.visible=false;
//               }
        }

        Button {
            id: button_close1
            x: 398
            y: 3
            width: 28
            height: 27
            iconSource: "close.png"
            onClicked: item_subject.visible=false
        }

        Label {
            id: label1
            x: 29
            y: 182
            width: 79
            height: 32

        }
    }*/



}

