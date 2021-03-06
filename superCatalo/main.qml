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
    width: 800
    height: 600
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
                id: menu
                visible: false
                MenuItem {
                    id:addSemester
                    text: qsTr("&Добавить семестр...")
                    onTriggered: item_addsemester.visible=true
                }
                MenuItem {
                    id:addCourse
                    text: qsTr("&Добавить предмет...")
                    onTriggered: item_addcourse.visible=true
                }
                MenuItem {
                    id:addTheme
                    text: qsTr("&Добавить тему...")
                    onTriggered: item_addtheme.visible=true
                }
                MenuItem {
                    id: addImage
                    text: qsTr("&Добавить изображение...")
                    onTriggered: item_addimage.visible=true
                }
                MenuItem {
                    id: addComment
                    text: qsTr("&Изменить комментарий...")
                    onTriggered: item_addcomment.visible=true
                }
                MenuItem {
                    id: addTag
                    text: qsTr("&Добавить тег...")
                    onTriggered: item_addtag.visible=true
                }

                MenuItem {
                    text: qsTr("&Удалить...")
                    onTriggered: { ip.deleteItem();
                                   image.visible=false
                                   textField_comment.visible=false;
                                    textField_tag.visible=false}
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
                model: ip
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
                                var leftButtonIndex = parent.indexAt(mouse.x, mouse.y)
                                if (leftButtonIndex.valid)
                                { if (parent.isExpanded(leftButtonIndex)) {
                                        ip.setCurrentIndex(leftButtonIndex);
                                        var indexes = ip.getChildrenIndexes();
                                        for (var i = indexes.length - 1; i >= 0; --i)
                                            parent.collapse(indexes[i]);
                                    }
                                    else parent.expand(leftButtonIndex)
                                }

                                if(ip.data(leftButtonIndex,1))
                                {
                                    image.source = ip.data(leftButtonIndex,1);
                                    redactor.loadImage(ip.data(leftButtonIndex,1));
                                    imagesource.text=image.source;
                                    botton_row.visible=true
                                    image.visible=true;
                                    rotation_image.visible=true;
                                    redactor.saveImage("../Pictures/temp.jpg");
                                    mouseArea_under_image.rotation=0;
                                    //slider_image.visible = true;slider_rotation.visible=true;
                                    button_print.visible=true;
                                    ip.setCurrentIndex(leftButtonIndex);
                                    textField_comment.visible=true;
                                    textField_comment.text="Комментарий: "+ip.showComment();
                                    textField_tag.visible=true;
                                    var str = ip.showTags();
                                    if (str[0] === ',')
                                        str = str.slice(1);
                                    textField_tag.text="Тег: "+str;
                                }
                            }
                            if (mouse.button === Qt.RightButton)
                            {
                                var rightButtonIndex = parent.indexAt(mouse.x, mouse.y)
                                if (rightButtonIndex.valid) {
                                    addSemester.visible = ip.showMenuItem(rightButtonIndex,1)
                                    addCourse.visible = ip.showMenuItem(rightButtonIndex,1)
                                    addTheme.visible = ip.showMenuItem(rightButtonIndex,2)
                                    addImage.visible = ip.showMenuItem(rightButtonIndex,3)
                                    addComment.visible = ip.showMenuItem(rightButtonIndex,4)
                                    addTag.visible = ip.showMenuItem(rightButtonIndex,4)
                                    ip.setCurrentIndex(rightButtonIndex)
                                    menu.popup()
                                }
                            }
                        }
                    }
                }


    Item{
        id: redactor_for_image
        visible: true
        anchors.left: treeView.right
        anchors.leftMargin: 10
        anchors.top: button_search.bottom
        anchors.topMargin: 0
        anchors.right:parent.right
        anchors.rightMargin: 24
        anchors.bottom: treeView.bottom

        Item{
            id: rect_for_image
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.width
            height: parent.height*0.95

            Image {
                id: rotation_image
                anchors.left: parent.left
                visible: false
                anchors.verticalCenter: parent.verticalCenter
                width: 25
                height: parent.height*0.8
                source: "image/line.png"

            }
         MouseArea {
             anchors.fill: parent
             id: mouseArea_under_image
             property int firstX: image.width/2
             property int firstY: image.height/2
             property int lastX: image.width/2
             property int lastY: image.height/2
             property int rotation: 0
             onPressed: {
                 if(canvas.visible===false)
                 if(rotation_image.visible===true)
                 {
                        firstY=mouseY
                 }
             }
             onPositionChanged: {
                if(canvas.visible===false)
                    if(rotation_image.visible===true)
                {
                    lastY=firstY;
                    firstY=mouseY
                    mouseArea_under_image.rotation-=(firstY-lastY)/mouseArea_under_image.height*360;
                    redactor.loadImage("../Pictures/temp.jpg");
                    redactor.rotate(rotation);
                    redactor.saveImage("../Pictures/temp_rotation.jpg");
                    image.source="file:"
                    image.source="file:../Pictures/temp_rotation.jpg"

                }
             }

            Image {
                // Positioner: parent.Center
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 50
                id: image
                scale:Math.min(parent.height/height, parent.width/width)*0.95;
                fillMode: Image.PreserveAspectFit
                cache: false
                MouseArea {
                    anchors.fill: parent
                    id: mouseArea_image
                    onPressed: {
                        if(canvas.visible===true){
                        canvas.firstX = mouseX
                        canvas.firstY = mouseY
                        canvas.lastX=mouseX
                        canvas.lastY=mouseY
                        canvas.requestPaint()}
                    }
                    onPositionChanged: {
                       if(canvas.visible===true){
                       canvas.lastX = Math.max(0,Math.min(mouseX, canvas.width))
                       canvas.lastY = Math.max(0,Math.min(mouseY, canvas.height))
                       canvas.requestPaint()
                       }
                    }
                }
                Text{
                 id: imagesource
                 text: " "
                 visible: false
                }
                Canvas{
                    id:canvas
                    visible: false
                    height: image.height
                    width: image.width
                    property int firstX: 0
                    property int firstY: 0
                    property int lastX: canvas.width
                    property int lastY: canvas.height
                    onPaint:{
                         var ctx = canvas.getContext('2d');
                         ctx = getContext("2d")
                         ctx.lineWidth = Math.max(2,height/200);
                         ctx.strokeStyle=Qt.rgba(0.921, 0.478, 0.137, 1);
                         ctx.clearRect(0,0,canvas.width,canvas.height)
                         ctx.beginPath()
                         ctx.rect(firstX,firstY,Math.min(lastX - firstX, image.width),Math.min(lastY - firstY,image.height))
                         ctx.stroke()
                     }
                }

                    }

         }
        }
       Item{
           anchors.left: parent.left
           anchors.bottom: parent.bottom
           anchors.right: parent.right
           width: parent.width
           height: parent.height*0.05
           Row{
               id: botton_row
               anchors.horizontalCenter: parent.horizontalCenter
               anchors.verticalCenter: parent.verticalCenter
               height: Math.min(parent.height*0.8, 50)
                visible: false
               spacing:10//min(10,(parent.width/2-5*parent.height*0.8)/4)


               Button {
                   height: parent.height*0.8
                   width: height
                   style:
                    ButtonStyle {
                       background: Rectangle {
                                    color: "#4ea9cc"
                                    border.color: "#3877a8"
                                    radius: 4
                                    Image {
                                        anchors.fill: parent
                                        source: "image/undo.png"
                                    }
                       }
                    }
                   onClicked:
                   {
                       image.source=imagesource.text
                       redactor.loadImage(imagesource.text);
                       redactor.saveImage("../Pictures/temp.jpg")
                       canvas.firstX=0;
                      canvas.firstY=0;
                      canvas.lastY=image.height;
                      canvas.lastX=image.width;
                       if(canvas.visible===true){
                           canvas.visible=false
                       }
                   }

                   }


               Button {
                   id: left_rotate
                   height: parent.height*0.8
                   width: height
                   style:
                    ButtonStyle {
                       background: Rectangle {
                                    color: "#4ea9cc"
                                    border.color: "#3877a8"
                                    radius: 4
                                    Image {
                                        anchors.fill: parent
                                        source: "image/left.png"
                                    }
                       }
                    }
                   onClicked:
                   {
                       redactor.rotate(270);
                       redactor.saveImage("../Pictures/temp.jpg");
                       image.source="file:"
                       image.source="file:../Pictures/temp.jpg"
                       canvas.firstX=0;
                      canvas.firstY=0;
                      canvas.lastY=image.height;
                      canvas.lastX=image.width;
                       if(canvas.visible===true){
                           canvas.requestPaint()
                       }
                   }

                   }
               Button {
                   height: parent.height*0.8
                   width: height
                   style:
                       ButtonStyle {
                      background: Rectangle {
                                   color: "#4ea9cc"
                                   border.color: "#3877a8"
                                   radius: 4
                                   Image {
                                       anchors.fill: parent
                                       source: "image/blackwhite.png"
                                   }
                      }
                   }
                   onClicked:
                   {
                        redactor.blackAndWhite();
                        redactor.saveImage("../Pictures/temp.jpg");
                        image.source="file:"
                        image.source="file:../Pictures/temp.jpg"
                   }
                   }
               Button {
                   height: parent.height*0.8
                   width: height
                   style:
                    ButtonStyle {
                       background:
                           Rectangle {
                           function color()
                           {
                           return canvas.visible===false ? "#4ea9cc" : "#ec7c26";
                           }
                                    color: color()
                                    border.color: "#3877a8"
                                    radius: 4
                                    Image {
                                        anchors.fill: parent
                                        source: "image/crop.png"
                                    }
                       }
                    }
                   onClicked: {
                       if(canvas.visible===true){
                       redactor.crop(canvas.firstX,canvas.firstY,Math.max(0,Math.min(canvas.lastX,image.width)) ,Math.max(0,Math.min(canvas.lastY,image.height)));
                       redactor.saveImage("../Pictures/temp.jpg");
                       image.source="file:"
                       image.source="file:../Pictures/temp.jpg"
                       canvas.visible=false
                       canvas.lastX=image.width
                       canvas.lastY=image.height
                       canvas.firstX=0;
                       canvas.firstY=0;

                       }
                       else {
                           canvas.firstX=0;
                          canvas.firstY=0;
                          canvas.lastY=image.height;
                          canvas.lastX=image.width;
                           canvas.visible=true;
                          canvas.requestPaint();
                       }
                   }
                   }
               Button {
                   height: parent.height*0.8
                   width: height
                   style:
                       ButtonStyle {
                      background: Rectangle {
                                   color: "#4ea9cc"
                                   border.color: "#3877a8"
                                   radius: 4
                                   Image {
                                       anchors.fill: parent
                                       source: "image/gauss.png"
                                   }
                      }
                   }
                   onClicked:
                   {
                       redactor.blackAndWhite();
                       redactor.blur(35);
                       redactor.division();
                       redactor.saveImage("../Pictures/temp.jpg");
                       image.source="file:"
                       image.source="file:../Pictures/temp.jpg"
                   }
                   }
               Button {
                   height: parent.height*0.8
                   width: height
                   style:
                    ButtonStyle {
                       background: Rectangle {
                                    color: "#4ea9cc"
                                    border.color: "#3877a8"
                                    radius: 4
                                    Image {
                                        anchors.fill: parent
                                        source: "image/right.png"
                                    }
                       }
                    }
                   onClicked:
                   {
                       redactor.rotate(90);
                       redactor.saveImage("../Pictures/temp.jpg");
                       image.source="file:"
                       image.source="file:../Pictures/temp.jpg"
                       canvas.firstX=0;
                      canvas.firstY=0;
                      canvas.lastY=image.height;
                      canvas.lastX=image.width;
                       if(canvas.visible===true){
                           canvas.requestPaint()
                       }
                   }
                   }

               Button {
                   height: parent.height*0.8
                   width: height
                   style:
                    ButtonStyle {
                       background: Rectangle {
                                    color: "#4ea9cc"
                                    border.color: "#3877a8"
                                    radius: 4
                                    Image {
                                        anchors.fill: parent
                                        source: "image/save.png"
                                    }
                       }
                    }
                   onClicked:
                   {
                       redactor.saveImage(imagesource.text);
                       image.source=imagesource.text


                   }
                   }


           }
       }
}
    Text{
           id: textField_comment
           anchors.left: button_print.right
           anchors.leftMargin: 10
           anchors.top: parent.top
           anchors.topMargin: 2
           width: 150
           height: 20
           visible: false
    }
    Text{
           id: textField_tag
           anchors.top: textField_comment.bottom
           anchors.topMargin: 2
           anchors.left: textField_comment.left
           width: 150
           height: 20
           visible: false
    }

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
    onClicked:ip.print(image.source)
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
                        ip.addSemester(textField_numsemester.text);
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
                        ip.addCourse(textField_namecourse.text);
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
                        ip.addTheme(textField_nametheme.text);
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
              ip.addImage(textField_path.text,textField_imagecomment.text,textField_imagetads.text);
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
                        ip.setComment(textField_newcomment.text);
                        item_addcomment.visible=false;
                        textField_newcomment.text="";
                        textField_comment.text="Комментарий: "+ip.showComment() }
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
                        ip.addTags(textField_newtag.text);
                        item_addtag.visible=false;
                        textField_newtag.text="";
                        var str = ip.showTags();
                        if (str[0] === ',')
                            str = str.slice(1);
                        textField_tag.text="Тег: "+str;
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
    property int max: 0
    property int i: 0

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
        anchors.leftMargin: 5
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
        onClicked:{searchForm.visible=false; textField_search.text="";
            textField_search.text="";
            image_sourse.source="file:"
            text_comment.text="";
            searchForm.max=0;}
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
        onClicked: {
            searchForm.max = ip.findByTags(textField_search.text)
            var str=ip.fetchImage(0);
            var str1="file:"
            console.log(str.slice(0, str.indexOf('+')));
            image_sourse.source=str1+str.slice(0, str.indexOf('+'));
            text_comment.text="Комментарий: "+str.slice(str.indexOf('+')+1)
            button_left.visible=true;
            button_right.visible=true;
        }

    }

    Button {
        id: button_left
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: parent.verticalCenter
       //anchors.topMargin: 200
        visible: false
        text: qsTr("<")
        width: 27
        height: 27
        style:
            ButtonStyle {
              background:
                  Rectangle {
                           color: "#4ea9cc"
                           border.color: "#3877a8"
                           radius: 4
              }
          }
        onClicked: {
            searchForm.i=(searchForm.i-1+searchForm.max)%searchForm.max;
            var str=ip.fetchImage(searchForm.i);
            var str1="file:"
            console.log(str.slice(0, str.indexOf('+')));
            image_sourse.source=str1+str.slice(0, str.indexOf('+'));
            text_comment.text="Комментарий: " + str.slice(str.indexOf('+')+1)
        }

    }

    Button {
        id: button_right
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.top: parent.verticalCenter//parent.top
        //anchors.topMargin: 200
        visible: false
        width: 27
        height: 27
        text: qsTr(">")
        style:
            ButtonStyle {
              background:
                  Rectangle {
                           color: "#4ea9cc"
                           border.color: "#3877a8"
                           radius: 4
              }
          }
        onClicked: {
            searchForm.i=(searchForm.i+1)%searchForm.max;
            var str=ip.fetchImage(searchForm.i);
            var str1="file:"
            console.log(str.slice(0, str.indexOf('+')));
            image_sourse.source=str1+str.slice(0, str.indexOf('+'));
            text_comment.text="Комментарий: " + str.slice(str.indexOf('+')+1, str.length)

        }

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

    Text{
           id: text_comment
          // x: 618
           anchors.top: textField_search.bottom
           anchors.bottom: parent.bottom
           anchors.topMargin: 3
           width: 20
           height: 20
           anchors.left: textField_search.left
          // anchors.left: parent.horizontalCenter
    }
Image{

 id:image_sourse
 width:parent.width - button_ago.width - button_search1.width - 50
 height: parent.height -button_ago.height - button_search1.height - 20
 anchors.top: textField_search.bottom
 anchors.topMargin: 10
 anchors.left: button_left.right
 anchors.leftMargin: 80
 anchors.right: button_right.left
 anchors.rightMargin: 80
 scale:Math.min(parent.height/height, parent.width/width)*0.8; //масштаб
 fillMode: Image.PreserveAspectFit
}

    }
  }
}
