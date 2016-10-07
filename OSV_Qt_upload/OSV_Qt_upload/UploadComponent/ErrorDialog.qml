import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import "UtilFunctions.js" as UtilFunctions

Dialog {
    id: errorDialog
    onAccepted: {
        uploadController.errorAknowledged()
        close();
    }
    visible: uploadController.isError
    standardButtons: StandardButton.Ok

    contentItem: Rectangle {
        implicitWidth: 100
        implicitHeight: 50
        ColumnLayout {
            anchors.fill:parent
            spacing: 2
            Text {
                text: qsTr("Error Login")
            }

            CustomButton {
                Layout.alignment: Qt.AlignCenter
                anchors.margins:10
                text: qsTr("OK")
                onClicked: {
                    errorDialog.click(StandardButton.Ok);
                }
            }
        }
    }
}
