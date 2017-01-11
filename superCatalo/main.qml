import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0


ApplicationWindow {
    id:window
    visible: true
    width: 700
    height: 550
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
        style: MenuBarStyle
        {
            background:
                Rectangle {
                  color: "#ffffff";
                  border.color: "#ffffff" // "#2bb2eb"
                  border.width: 0.5
              }
        }
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
                id:addTheme
                text: qsTr("&Добавить тему")
                onTriggered: item_addtheme.visible=true
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
                text: qsTr("&Удалить тему")
                onTriggered: item_deltheme.visible=true
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
    }

TreeView {
        id: treeView
        x: 3
        y: 2
        width: parent.width/3.8 //ширина
        height: parent.height/1.05+7 // высота
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
                    width:500 //появляется прокрутка
        }
            MouseArea {
                id: mouseArea_tree
                anchors.topMargin: 0
                anchors.bottomMargin: 30 //нижнее поле
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
                        mymodel.setCurrentIndex(index_2); //!!!
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
                        button_print.visible=true;
                        //slider_rotation.visible=true;
                    }
                }
            }
        }

    Image {
        id: image
        anchors.left: treeView.right
        anchors.leftMargin: 10
        anchors.top: slider_image.bottom
        anchors.right:parent.right
        anchors.rightMargin: 24
        anchors.bottom: treeView.bottom

        fillMode: Image.PreserveAspectFit //изображение масштабируется равномерно, чтобы соответствовать без кадрирования
        smooth: true

        ///scale: slider_image.value //масштаб
        //rotation: slider_rotation.value*360

        MouseArea {
            // действуем в пределах всего элемента Image
            anchors.fill: parent
            id: mouseArea_image
        }
    }

//    Slider {
//        id: slider_rotation
//        anchors.left:treeView.right
//        anchors.leftMargin: 10
//        anchors.top: treeView.top
//        width: 100
//        height: 20
//        stepSize: 0.001
//        visible: false
//        onEnabledChanged: {
//            image.rotation = slider_rotation.value;
//        }
//    }

    Slider {
        id: slider_image
        anchors.left:treeView.right
        anchors.leftMargin: 10
        anchors.top: treeView.top
        width: 100
        height: 20
        value: 0.5
        maximumValue: 1
        visible: false
    }

Button {
    id: button_print
    anchors.left: slider_image.right
    anchors.leftMargin: 10
    width: 27
    height: 27
    style:
     ButtonStyle {
        background: Rectangle {
                     color: "#4ea9cc"
                     border.color: "#3877a8"
                     radius: 4
                     Image {
                         width: 27;height: 27
                         source: "image/print.png"
                     }
        }
                        }
    //iconSource:"image/print.png"
    onClicked:mymodel.print(image.source)
    visible: false
}

    Rectangle {
        id: item_addsemester
        anchors.centerIn: parent
        width: 377
        height: 132
        visible: false
        color: "#398eb1"
        border.width: 1
        border.color: "#3877a8"
        radius: 8

        TextField {
            x: 66
            y: 31
            width: 250
            height: 33
            id: textField_numsemester1
            placeholderText: qsTr("Введите номер семестра")
        }

        Button {
            id: button1_ok
            x: 151
            y: 82
            text: qsTr("ОК")
            onClicked: {
                mymodel.addSemester(textField_numsemester1.text);
                item_addsemester.visible=false;
                textField_numsemester1.text="";
               }
        }
        WindowButton
        {
          anchors.right: parent.right
          anchors.rightMargin: 3
          anchors.top: parent.top
          anchors.topMargin: 3
          function callback() {item_addsemester.visible=false}
        }
    }

    Rectangle {
        id: item_addcourse
        anchors.centerIn: parent
        width: 377
        height: 184
        visible: false
        color: "#398eb1"
        border.width: 1
        border.color: "#3877a8"
        radius: 8

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
            placeholderText: qsTr("Введите название предмета")
        }
        Button {
            id: button2_ok
            x: 149
            y: 130
            text: qsTr("ОК")
            onClicked: {
                mymodel.addCourse(textField_numsemester2.text,textField_namecourse2.text);
                item_addcourse.visible=false;
                textField_numsemester2.text=""; textField_namecourse2.text="";
               }
        }
        WindowButton
        {
          anchors.right: parent.right
          anchors.rightMargin: 3
          anchors.top: parent.top
          anchors.topMargin: 3
          function callback() {item_addcourse.visible=false}
        }
    }

    Rectangle {
        id: item_addtheme
        anchors.centerIn: parent
        width: 377
        height: 208
        visible: false
        color: "#398eb1"
        border.width: 1
        border.color: "#3877a8"
        radius: 8

        TextField {
            x: 64
            y: 30
            width: 250
            height: 33
            id: textField_numsemester3
            placeholderText: qsTr("Введите номер семестра")
        }
        TextField {
            x: 64
            y: 80
            width: 250
            height: 33
            id: textField_namecourse3
            placeholderText: qsTr("Введите название предмета")
        }

        TextField {
            x: 64
            y: 130
            width: 250
            height: 33
            id: textField_nametheme3
            placeholderText: qsTr("Введите название темы")
        }
        Button {
            id: button3_ok
            x: 149
            y: 169
            text: qsTr("ОК")
            onClicked: {
                mymodel.addTheme(textField_numsemester3.text,textField_namecourse3.text,textField_nametheme3.text);
                item_addtheme.visible=false;
                textField_numsemester3.text="";textField_namecourse3.text="";textField_nametheme3.text="";
               }
        }
        WindowButton
        {
          anchors.right: parent.right
          anchors.rightMargin: 3
          anchors.top: parent.top
          anchors.topMargin: 3
          function callback() {item_addtheme.visible=false}
        }
    }

    Rectangle {
        id: item_addimage
        anchors.centerIn: parent
        width: 402
        height: 270
        color: "#398eb1"
        border.width: 1
        border.color: "#3877a8"
        radius: 8
        visible: false

        TextField {
            id: textField_numsemester4
            x: 76
            y: 10
            width: 250
            height: 33
            placeholderText: qsTr("Введите номер семестра")
        }

        TextField {
            id: textField_namecourse4
            x: 76
            y: 55
            width: 250
            height: 33
            placeholderText: qsTr("Введите название предмета")
        }

        TextField {
            id: textField_nametheme4
            x: 76
            y: 100
            width: 250
            height: 33
            placeholderText: qsTr("Введите название темы")
        }

        TextField {
            id: textField_path
            x: 76
            y: 145
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

        FileDialog {
                            id: fileDialog
                            title: "Выбор изображения"
                            folder: shortcuts.home
                            visible: false
                            nameFilters: [ "Изображения (*.jpg *.png *.bmp *gif)", "Все файлы (*)" ]
                            selectedNameFilter: "Изображения (*.jpg *.png *.bmp *gif)"
                            onAccepted: {
                                textField_path.text=(fileDialog.fileUrl.toString()).slice(7);
                                //textField_path.text = "";

                            }
                            onRejected: { console.log("Rejected") }
        }


        TextField {
               id: textField_imagecomment
               x: 76
               y: 190
               width: 250
               height: 33
               placeholderText: qsTr("Введите комментарий")
        }

        Button {
            id:buttonbrowse
            width: 70
            height: 33
            anchors.left: textField_path.right
            anchors.bottom: textField_path.bottom
            anchors.leftMargin: 2
            text:"Browse"
            visible: true
            style:  ButtonStyle {
                background: Rectangle{
                    color: control.pressed ? "#2bb2eb"  : "#398eb1"
                    border.color: "#ffffff"
                    radius: 5
                }
        }
         onClicked: fileDialog.open()
        }

        Button {
            id: button4_ok
            x: 161
            y: 230
            text: qsTr("ОК")
            onClicked: {
            mymodel.addImage(textField_numsemester4.text,textField_namecourse4.text,textField_nametheme4.text,textField_path.text,textField_imagecomment.text);
            item_addimage.visible=false;
           }
        }
        WindowButton
        {
          anchors.right: parent.right
          anchors.rightMargin: 3
          anchors.top: parent.top
          anchors.topMargin: 3
          function callback() {item_addimage.visible=false}
        }
    }


   Rectangle {
        id: item_delsemester
        anchors.centerIn: parent
        width: 377
        height: 132
        visible: false
        color: "#398eb1"
        border.width: 1
        border.color: "#3877a8"
        radius: 8

        TextField {
            x: 66
            y: 31
            width: 250
            height: 33
            id: textField_numsemester11
            placeholderText: qsTr("Введите номер семестра")
        }
        Button {
            id: button11_ok
            x: 151
            y: 82
            text: qsTr("ОК")
            onClicked: {
                mymodel.deleteSemester(textField_numsemester11.text);
                item_delsemester.visible=false;
                textField_numsemester11.text="";
               }
        }
        WindowButton
        {
          anchors.right: parent.right
          anchors.rightMargin: 3
          anchors.top: parent.top
          anchors.topMargin: 3
          function callback() {item_delsemester.visible=false}
        }
    }

   Rectangle {
        id: item_delcourse
        anchors.centerIn: parent
        width: 377
        height: 184
        visible: false
        color: "#398eb1"
        border.width: 1
        border.color: "#3877a8"
        radius: 8

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

        Button {
            id: button22_ok
            x: 149
            y: 130
            text: qsTr("ОК")
            onClicked: {
                mymodel.deleteCourse(textField_numsemester22.text,textField_namecourse22.text);
                item_delcourse.visible=false;
                textField_numsemester22.text="";textField_namecourse22.text="";
               }
        }
        WindowButton
        {
          anchors.right: parent.right
          anchors.rightMargin: 3
          anchors.top: parent.top
          anchors.topMargin: 3
          function callback() {item_delcourse.visible=false}
        }
    }

   Rectangle {
    id: item_deltheme
    anchors.centerIn: parent
    width: 377
    height: 208
    visible: false
    color: "#398eb1"
    border.width: 1
    border.color: "#3877a8"
    radius: 8

    TextField {
        x: 64
        y: 30
        width: 250
        height: 33
        id: textField_numsemester33
        placeholderText: qsTr("Введите номер семестра")
    }
    TextField {
        x: 64
        y: 80
        width: 250
        height: 33
        id: textField_namecourse33
        placeholderText: qsTr("Введите название предмета")
    }

    TextField {
        x: 64
        y: 130
        width: 250
        height: 33
        id: textField_nametheme33
        placeholderText: qsTr("Введите название темы")
    }
    Button {
        id: button33_ok
        x: 149
        y: 169
        text: qsTr("ОК")
        onClicked: {
            mymodel.deleteTheme(textField_numsemester33.text,textField_namecourse33.text,textField_nametheme33.text);
            item_deltheme.visible=false;
            textField_numsemester33.text="";textField_namecourse33.text="";textField_nametheme33.text="";
           }
    }
    WindowButton
    {
      anchors.right: parent.right
      anchors.rightMargin: 3
      anchors.top: parent.top
      anchors.topMargin: 3
      function callback() {item_deltheme.visible=false}
    }
}
Rectangle {
    id: item_delimage
    anchors.centerIn: parent
    width: 402
    height: 270
    color: "#398eb1"
    border.width: 1
    border.color: "#3877a8"
    radius: 8
    visible: false

    TextField {
        id: textField_numsemester44
        x: 76
        y: 10
        width: 250
        height: 33
        placeholderText: qsTr("Введите номер семестра")
    }

    TextField {
        id: textField_namecourse44
        x: 76
        y: 55
        width: 250
        height: 33
        placeholderText: qsTr("Введите название предмета")
    }

    TextField {
        id: textField_nametheme44
        x: 76
        y: 100
        width: 250
        height: 33
        placeholderText: qsTr("Введите название темы")
    }

    TextField {
        id: textField_path2
        x: 76
        y: 145
        width: 250
        height: 33
        placeholderText: qsTr("Путь к изображению")

        MouseArea {
            anchors.fill: parent
            id: mouseArea_path2

            onDoubleClicked: {
                fileDialog.visible = true;
            }
        }
    }

    FileDialog {
                        id: fileDialog2
                        title: "Выбор изображения"
                        folder: shortcuts.home
                        visible: false
                        nameFilters: [ "Изображения (*.jpg *.png *.bmp *gif)", "Все файлы (*)" ]
                        selectedNameFilter: "Изображения (*.jpg *.png *.bmp *gif)"
                        onAccepted: {
                            textField_path2.text=(fileDialog2.fileUrl.toString()).slice(7);
                        }
                        //onRejected: { console.log("Rejected")}


    }
//    TextField {
//           id: textField_imagecomment2
//           x: 76
//           y: 190
//           width: 250
//           height: 33
//           placeholderText: qsTr("Введите комментарий")
//   }

    Button {
        id:buttonbrowse2
        width: 70
        height: 33
        anchors.left: textField_path2.right
        anchors.bottom: textField_path2.bottom
        anchors.leftMargin: 2
        text:"Browse"
        visible: true
        style:  ButtonStyle {
            background: Rectangle{
                color: control.pressed ? "#2bb2eb"  : "#398eb1"
                border.color: "#ffffff"
                radius: 5
            }
    }
     onClicked: fileDialog.open()
    }

    Button {
        id: button44_ok
        x: 161
        y: 230
        text: qsTr("ОК")
        onClicked: {
        mymodel.addImage(textField_numsemester44.text,textField_namecourse44.text,textField_nametheme44.text,textField_path2.text);
        item_delimage.visible=false;
       }
    }
    WindowButton
    {
      anchors.right: parent.right
      anchors.rightMargin: 3
      anchors.top: parent.top
      anchors.topMargin: 3
      function callback() {item_delimage.visible=false}
    }
}

}


