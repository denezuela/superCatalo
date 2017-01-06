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
            id: menu_a
            title: qsTr("Добавить")
            MenuItem {
                id:addSemester
                text: qsTr("&Добавить семестр")
                onTriggered: item_addsemester.visible=true
            }
            MenuItem {
                id:addCourse
                text: qsTr("&Добавить предмет")
                onTriggered: item_addcourse.visible=true
            }
            MenuItem {
                id: addImage
                text: qsTr("&Добавить изображение")
                onTriggered: item_addimage.visible=true
            }
        }
        Menu {
            title: qsTr("Удалить")
            MenuItem {
                text: qsTr("&Удалить семестр")
                onTriggered: item_delsemester.visible=true
            }
            MenuItem {
                text: qsTr("&Удалить предмет")
                onTriggered: item_delcourse.visible=true
            }
            MenuItem {
                text: qsTr("&Удалить изображение")
                onTriggered: item_delimage.visible=true
            }
        }

        Menu {
            title: qsTr("Обработка")
            MenuItem {
                text: qsTr("&Оттенки серого")
            }
            MenuItem {
                text: qsTr("&Размытие по Гаусу")
            }
            MenuItem {
                text: qsTr("&Деление")

            }
        }
        Menu {
            title: qsTr("Печать")
            MenuItem {
                text: qsTr("&Печать изображения")
            }
            MenuItem {
                text: qsTr("&Печать со свойствами") //печать нужных тем,номер изобр. на странице
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
            MouseArea {
                id: mouseArea_tree
                anchors.bottomMargin: 21
                anchors.fill: parent
                acceptedButtons: Qt.RightButton | Qt.LeftButton
                onClicked:
                {
                    if (mouse.button === Qt.LeftButton)
                    {
                        var index_1 = parent.indexAt(mouse.x, mouse.y);
                        if (index_1.valid)
                        { parent.isExpanded(index_1) ? parent.collapse(index_1) : parent.expand(index_1);}
                    }
                    if (mouse.button === Qt.RightButton)
                    {
                        var index_2 = parent.indexAt(mouse.x, mouse.y);
                        if (index_2.valid) menu_a.popup();
                }
                }
                onDoubleClicked:
                {
                    var index_image = parent.indexAt(mouse.x, mouse.y);
                    if(mymodel.data(index_image,1))
                    {
                        image.source = mymodel.data(index_image,1);
                        slider_image.visible = true;
                    }
                }
            }
        }

    Image {
        id: image
        x: 211
        y: 42
        width: 394
        height: 392

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

    Slider {
        id: slider_rotation
        //anchors.left: button_turn.right
        anchors.leftMargin: 5
        width: 107
        height: 27
        stepSize: 0.001
        visible: false
        onEnabledChanged: {
            image.rotation = slider_rotation.value;
        }
    }

    Slider {
        id: slider_image
        anchors.left: slider_rotation.right
        anchors.leftMargin: 5
        width: 107
        height: 27
        value: 0.5
        maximumValue: 1
        visible: false
    }

    Rectangle {
        id: item_addsemester
        x: 132
        y: 160
        width: 377
        height: 132
        visible: false
        color: "#7bb8d1"
        border.width: 1

        Button {
            id: button1_ok
            x: 151
            y: 82
            text: qsTr("ОК")
            onClicked: {
                mymodel.addSemester(textField_numsemester1.text);
                item_addsemester.visible=false;
               }
        }

        Button {
            id: button1_close
            x: 348
            y: 1
            width: 28
            height: 27
            iconSource: "images/close.png"
            //iconSource: "close.png"
            onClicked: item_semester.visible=false
        }

        TextField {
            x: 66
            y: 31
            width: 250
            height: 33
            id: textField_numsemester1
            placeholderText: qsTr("Введите номер семестра")
        }
    }

    Rectangle {
        id: item_addcourse
        x: 132
        y: 160
        width: 377
        height: 184
        visible: false
        color: "#7bb8d1"
        border.width: 1

        Button {
            id: button2_ok
            x: 149
            y: 130
            text: qsTr("ОК")
            onClicked: {
                mymodel.addCourse(textField_numsemester2.text,textField_namecourse2.text);
                item_addcourse.visible=false;
               }
        }

        Button {
            id: button2_close
            x: 341
            y: 8
            width: 28
            height: 27
            onClicked: item_addcourse.visible=false
        }

        TextField {
            x: 64
            y: 31
            width: 250
            height: 33
            id: textField_numsemester2
            placeholderText: qsTr("Введите номер семестра")
        }
        TextField {
            x: 64
            y: 81
            width: 250
            height: 33
            id: textField_namecourse2
            placeholderText: qsTr("Введите название курса")
        }

    }

    Rectangle {
        id: item_addimage
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        width: 402
        height: 264
        border.width: 1
        visible: false
        color: "#55aaff"

        TextField {
            id: textField_path
            x: 76
            y: 122
            width: 250
            height: 33
            placeholderText: qsTr("Путь к изображению")

            MouseArea {
                anchors.fill: parent
                id: mouseArea_path

                onDoubleClicked: {
                    fileDialog.visible = true;
                }
            }
        }

        Button {
            id: button3_ok
            x: 161
            y: 229
            text: qsTr("ОК")
//            onClicked: {
//            //mymodel.cutPath(textField_numsemester3.text,textField_namecourse3.text,);
//            //item_addimage.visible=false;
//           }
        }

        TextField {
            id: textField_numsemester3
            x: 76
            y: 18
            width: 250
            height: 33
            placeholderText: qsTr("Введите номер семестра")
        }
        Button {
            id: button3_close
            x: 367
            y: 8
            width: 27
            height: 27
            iconSource: "images/close.png"
            onClicked: item_addimage.visible=false
        }

        TextField {
            id: textField_namecourse3
            x: 76
            y: 70
            width: 250
            height: 33
            placeholderText: qsTr("Введите название курса")
        }

        FileDialog {
                            id: fileDialog
                            title: "Выбор изображения"
                            folder: shortcuts.home
                            visible: false
                            nameFilters: [ "Изображения (*.jpg *.png *.bmp *gif)", "Все файлы (*)" ]
                            selectedNameFilter: "Изображения (*.jpg *.png *.bmp *gif)"
                            onAccepted: {
                                textField_path.text = mymodel.cutPath(fileDialog.fileUrl.toString());
                            }
        }

        TextField {
               id: textField_image_comment
               x: 76
               y: 174
               width: 250
               height: 33
               placeholderText: qsTr("Введите комментарий")
        }
    }


    Rectangle {
        id: item_delsemester
        x: 132
        y: 160
        width: 377
        height: 132
        visible: false
        color: "#7bb8d1"
        border.width: 1

        Button {
            id: button11_ok
            x: 151
            y: 82
            text: qsTr("ОК")
            onClicked: {
                mymodel.deleteSemester(textField_numsemester11.text);
                item_delsemester.visible=false;
               }
        }

        Button {
            id: button11_close
            x: 348
            y: 1
            width: 28
            height: 27
            iconSource: "images/close.png"
            //iconSource: "close.png"
            onClicked: item_delsemester.visible=false
        }

        TextField {
            x: 66
            y: 31
            width: 250
            height: 33
            id: textField_numsemester11
            placeholderText: qsTr("Введите номер семестра")
        }
    }
    Rectangle {
        id: item_delcourse
        x: 132
        y: 160
        width: 377
        height: 184
        visible: false
        color: "#7bb8d1"
        border.width: 1

        Button {
            id: button22_ok
            x: 149
            y: 130
            text: qsTr("ОК")
            onClicked: {
                mymodel.deleteCourse(textField_numsemester22.text,textField_namecourse22.text);
                item_delcourse.visible=false;
               }
        }

        Button {
            id: button22_close
            x: 341
            y: 8
            width: 28
            height: 27
            onClicked: item_delcourse.visible=false
        }

        TextField {
            x: 64
            y: 31
            width: 250
            height: 33
            id: textField_numsemester22
            placeholderText: qsTr("Введите номер семестра")
        }
        TextField {
            x: 64
            y: 81
            width: 250
            height: 33
            id: textField_namecourse22
            placeholderText: qsTr("Введите название курса")
        }

    }


}


