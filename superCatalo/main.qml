import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2


import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480

    TreeView {
      model: myModel
      TableViewColumn{
           title: "Name"
           width:500

       }



}

}
