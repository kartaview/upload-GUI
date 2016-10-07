import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import "UtilFunctions.js" as UtilFunctions

Dialog {
    id: dialogUploadComplete
    title: qsTr("Upload Complete!")
    onAccepted : {
        uploadController.resetUploadValues();
        close();
    }
    standardButtons: StandardButton.Ok
    visible: uploadController.isUploadComplete

    contentItem: Rectangle {
        implicitWidth: 275
        implicitHeight: 275
        ColumnLayout {
            anchors.fill:parent
            spacing: 2
            Text {
                text: qsTr("Number of tracks: ") + persistentController.sequences.count
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                text: qsTr("Total size: ") + UtilFunctions.editFolderSize(persistentController.totalSize)
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                text: elapsedTime.text
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                text: uploadSpeed.text
                Layout.alignment: Qt.AlignCenter
            }

            TableView {
                Layout.alignment: Qt.AlignCenter
                height:100
                model : persistentController.sequences
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

                TableViewColumn {
                    role : "name"
                    title : qsTr("Name")
                    width : 100
                    horizontalAlignment: Text.AlignLeft
                }
                TableViewColumn {
                    title : qsTr("Track ID")
                    width : 100
                    delegate : Text {
                        text: model.sequenceId
                        horizontalAlignment: Text.AlignCenter;
                    }
                }
            }
            CustomButton {
                Layout.alignment: Qt.AlignCenter
                anchors.margins:10
                text: qsTr("OK")
                onClicked: {
                    dialogUploadComplete.click(StandardButton.Ok);
                }
            }
        }
    }
}
