import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    width: 640
    height: 480

    TreeView {
        TableViewColumn {
            title: "Semester";
            width: 300;
        }

        TableViewColumn {
            title: "Course";
            width: 300;
        }

        TableViewColumn {
            title: "Lecture";
            width: 300;
        }
    }
}
