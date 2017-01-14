import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0
//import QtGraphicalEffects 1.0

ApplicationWindow {
    id:window
    visible: true
    width: 700
    height: 550
    title: qsTr("Super Catalo")

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
                visible: false
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
                MenuItem {
                    id: addComment
                    text: qsTr("&Добавить комментарий")
                    onTriggered: item_addcomment.visible=true
                }
                MenuItem {
                    id: addTag
                    text: qsTr("&Добавить тег")
                    onTriggered: item_addtag.visible=true
                }

                MenuItem {
                    text: qsTr("&Удалить элемент")
                    onTriggered: { mymodel.deleteItem();
                                   image.visible=false
                                   textField_comment.visible=false;}
                }
            }
    }
    StartForm {
        id: imageForm
        anchors.fill: parent
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
                        var index_1 = parent.indexAt(mouse.x, mouse.y)
                        if (index_1.valid)
                        { if (parent.isExpanded(index_1)) {
                                mymodel.setCurrentIndex(index_1);
                                var indexes = mymodel.getChildrenIndexes();
                                for (var i = indexes.length - 1; i >= 0; --i)
                                    parent.collapse(indexes[i]);
                            }
                            else parent.expand(index_1)
                        }

                        if(mymodel.data(index_1,1))
                        {
                            image.source = mymodel.data(index_1,1);
                            image.visible=true;
                            //slider_image.visible = true;slider_rotation.visible=true;
                            button_print.visible=true;
                            mymodel.setCurrentIndex(index_1);
                            textField_comment.visible=true;
                            //textField_comment.text="Комментарий: "+mymodel.showComment();
                            textField_comment.text="Тег: "+mymodel.showTags();

                        }
                    }
                    if (mouse.button === Qt.RightButton)
                    {
                        var index_2 = parent.indexAt(mouse.x, mouse.y)
                        if (index_2.valid) {
                            addSemester.visible = mymodel.showMenuItem(index_2,1)
                            addCourse.visible = mymodel.showMenuItem(index_2,1)
                            addTheme.visible = mymodel.showMenuItem(index_2,2)
                            addImage.visible = mymodel.showMenuItem(index_2,3)
                            addComment.visible = mymodel.showMenuItem(index_2,4)
                            addTag.visible = mymodel.showMenuItem(index_2,4)
                            mymodel.setCurrentIndex(index_2)
                            menu_a.popup()
                        }
                }
                }
//                onDoubleClicked:
//                {
//                    var index_image = parent.indexAt(mouse.x, mouse.y);
//                    if(mymodel.data(index_image,1))
//                    {
//                        image.source = mymodel.data(index_image,1);
//                        //slider_image.visible = true;slider_rotation.visible=true;
//                        button_print.visible=true;
//                        mymodel.setCurrentIndex(index_image);
//                        textField_comment.visible=true;
//                        textField_comment.text="Комментарий: "+mymodel.showComment();

//                    }
//                }
            }
        }

    Image {
        id: image
        visible: true
        anchors.left: treeView.right
        anchors.leftMargin: 10
        anchors.top: button_search.bottom
        anchors.topMargin: 5
        anchors.right:parent.right
        anchors.rightMargin: 24
        anchors.bottom: treeView.bottom
        fillMode: Image.PreserveAspectFit //изображение масштабируется равномерно, чтобы соответствовать без кадрирования
        smooth: true

        //scale: slider_image.value //масштаб
        //rotation: slider_rotation.value*360
        MouseArea {
            anchors.fill: parent
            id: mouseArea_image
        }
    }

    Text{
           id: textField_comment
           anchors.left: button_print.right
           anchors.leftMargin: 10
           anchors.top: parent.top
           anchors.topMargin: 2
           width: 250
           height: 20
           visible: false
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

//    Slider {
//        id: slider_image
//        anchors.left:treeView.right
//        anchors.leftMargin: 10
//        anchors.top: treeView.top
//        width: 100
//        height: 20
//        value: 0.5
//        maximumValue: 1
//        visible: false
//    }

Button {
    id: button_print
    anchors.left: button_search.right
    anchors.leftMargin: 10
    anchors.top: parent.top
    anchors.topMargin: 2
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
    onClicked:mymodel.print(image.source)
    visible: false
}

WindowAdd {
   id: item_addsemester
     TextField {
        id: textField_numsemester
        x: 66
        y: 31
        width: 250
        height: 33
        placeholderText: qsTr("Введите номер семестра")
       }
    function callbackOK(){
                        mymodel.addSemester(textField_numsemester.text);
                        item_addsemester.visible=false;
                        textField_numsemester.text=""; }
    function callbackClose() { item_addsemester.visible=false }
}

WindowAdd {
   id: item_addcourse
     TextField {
        id: textField_namecourse
        x: 66
        y: 31
        width: 250
        height: 33
        placeholderText: qsTr("Введите название предмета")
       }
    function callbackOK(){
                        mymodel.addCourse(textField_namecourse.text);
                        item_addcourse.visible=false;
                        textField_namecourse.text=""; }
    function callbackClose() { item_addcourse.visible=false }
}

WindowAdd {
   id: item_addtheme
     TextField {
        id: textField_nametheme
        x: 66
        y: 31
        width: 250
        height: 33
        placeholderText: qsTr("Введите название темы")
       }
    function callbackOK(){
                        mymodel.addTheme(textField_nametheme.text);
                        item_addtheme.visible=false;
                        textField_nametheme.text=""; }
    function callbackClose() { item_addtheme.visible=false }
}

WindowAdd {
   id: item_addimage
   width: 400
   height: 208
   TextField {
       id: textField_path
       anchors.centerIn: parent.Center
       x: 64
       y: 30
       width: 250
       height: 33
       placeholderText: qsTr("Путь к изображению")
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
                       }
                       onRejected: { console.log("Rejected") }
   }
   TextField {
          id: textField_imagecomment
          x: 64
          y: 80
          width: 250
          height: 33
          placeholderText: qsTr("Введите комментарий-необязательно")
   }

   TextField {
          id: textField_imagetads
          x: 64
          y: 130
          width: 250
          height: 33
          placeholderText: qsTr("Введите тег-необязательно")
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
          id: button_ok
          x:150
          y:173
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
          onClicked: {
              mymodel.addImage(textField_path.text,textField_imagecomment.text,textField_imagetads.text);
              item_addimage.visible=false;
              textField_path.text="";
              textField_imagecomment.text="";
              textField_imagetads.text="";
             }
      }
    function callbackClose() { item_addimage.visible=false }
}

WindowAdd {
   id: item_addcomment
     TextField {
        id: textField_newcomment
        x: 66
        y: 31
        width: 250
        height: 33
        placeholderText: qsTr("Введите комментарий")
       }
    function callbackOK(){
                        mymodel.setComment(textField_newcomment.text);
                        item_addcomment.visible=false;
                        textField_newcomment.text="";
                        textField_comment.text="Комментарий: "+mymodel.showComment() }
    function callbackClose() { item_addcomment.visible=false }
}

WindowAdd {
   id: item_addtag
     TextField {
        id: textField_newtag
        x: 66
        y: 31
        width: 250
        height: 33
        placeholderText: qsTr("Введите тег")
       }
    function callbackOK(){
                        mymodel.addTags(textField_newtag.text);
                        item_addtag.visible=false;
                        textField_newtag.text="";
    }
    function callbackClose() { item_addtag.visible=false }
}

Button
{
    id:button_search
    anchors.left: button_add.right
    anchors.leftMargin: 10
    anchors.top: parent.top
    anchors.topMargin: 2
    width: 24
    height: 24
    style:
     ButtonStyle {
        background:
            Rectangle {
                     color: "#4ea9cc"
                     border.color: "#3877a8"
                     radius: 4
                     Image {
                         width: 24;height: 24
                         source: "image/search.png"
                     }
        }
}
    onClicked:{searchForm.visible=true;
    }
}
Button
{
    id:button_add
    anchors.left: treeView.right
    anchors.leftMargin: 10
    anchors.top: parent.top
    anchors.topMargin: 2
    width: 24
    height: 24
    style:
     ButtonStyle {
        background:
            Rectangle {
                     color: "#4ea9cc"
                     border.color: "#3877a8"
                     radius: 4
                     Image {
                         width: 24;height: 24
                         source: "image/plus.png"
                     }
        }
}
    onClicked:{item_addsemester.visible=true}
}

Rectangle{
    id:searchForm
    visible: false
    anchors.fill: parent
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
    Button {
        id:button_ago
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.top: parent.top
        anchors.topMargin: 5
        width: 27
        height: 27
        style:
         ButtonStyle {
            background:
                Rectangle {
                         color: "#4ea9cc"
                         border.color: "#3877a8"
                         radius: 4
                         Image {
                             width: 27;height: 27
                             source: "image/ago.png"
                         }
            }
        }
        onClicked:{searchForm.visible=false}
    }

    Button {
        id: button_search1
        anchors.left: textField_search.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 5
        width: 92
        height: 27
        text: qsTr("Поиск")
        style:
         ButtonStyle {
           background:
               Rectangle {
                        color: "#4ea9cc"
                        border.color: "#3877a8"
                        radius: 4
           }
       }
        onClicked: { var imageList = mymodel.findByTags(textField_search.text)
        image_sourse.source=imageList[1] }

    }
    TextField {
            id: textField_search
            anchors.left: button_ago.right
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 5
            width: parent.width - button_ago.width - button_search1.width - 50
            height: 27
            placeholderText: qsTr("Введите теги для поиска")
        }

Image{
 id:image_sourse
 anchors.top: textField_search.bottom
 anchors.topMargin: 15
 anchors.left: button_ago.right
 anchors.leftMargin: 10
 anchors.right: textField_search.right
}

    }
  }
}


