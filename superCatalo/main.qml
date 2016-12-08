import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Super Catalo")

    menuBar: MenuBar {
        Menu {
            title: qsTr("Add...")
            MenuItem {
                text: qsTr("Semester...")
                onTriggered: console.log("Semester");
            }
            MenuItem {
                text: qsTr("Course...")
                onTriggered: console.log("Course");
            }
            MenuItem {
                text: qsTr("Lecture...")
                onTriggered: console.log("Lecture");
            }
        }
    }

    TreeView {
        anchors.fill: parent
        id: window
        width: 640; height: 480
        model: ip

        TableViewColumn {
            title: "Semester -> Course -> Lecture";
            width: 600;
            role: "display"
        }

    }
}
