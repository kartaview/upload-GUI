import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import "UtilFunctions.js" as UtilFunctions

Window {
    title: qsTr("Upload OpenStreetView")
    visible: true
    width: 530
    minimumWidth: 530
    maximumWidth: 640
    height: 430
    minimumHeight: 430
    maximumHeight: 480

    Column {
        spacing : 10
        anchors.fill : parent
        anchors.leftMargin : 10
        anchors.topMargin : 10

        // 1st group of elements : Drag and Drop, list of folders, buttons (add/remove/upload photos)
        Rectangle {
            id : folderSelectionRectangle
            width : parent.width
            height : 230

            Row {
                spacing : 10
                anchors.fill : parent

                // Drag And Drop Area and TableView of folders to upload
                Rectangle {
                    id : background
                    width : 400
                    height : parent.height
                    enabled: !uploadController.isUploadStarted

                    Rectangle {
                        id: dropAreaText
                        width : parent.width
                        height : parent.height
                        visible: persistentController.sequences.count == 0 || dropArea.containsDrag
                        color : "lightsteelblue"

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            text : qsTr("Drop files here")
                        }
                    }

                    // drag and drop area
                    DropArea {
                        id : dropArea
                        anchors.fill: parent
                        onEntered: {
                            for (var i = 0; i < drag.urls.length; i++)
                            {
                                persistentController.addPreviewPath(drag.urls[i]); //add preview path

                            }
                            drag.accept(Qt.CopyAction);
                        }
                        onDropped: {
                            persistentController.onDropped();
                        }
                        onExited: {
                            persistentController.onExitedDropArea();
                        }

                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: {
                            if(mouse.button == Qt.RightButton)
                            {
                                 selectContextMenu.popup();
                            }
                        }
                    }

                    // view of the list of folders
                    TableView {
                        id: addedFolders
                        visible: persistentController.sequences.count > 0 && !dropArea.containsDrag
                        anchors.fill: parent
                        model : persistentController.sequences
                        selectionMode: SelectionMode.MultiSelection
                        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

                        Menu {
                            id: selectContextMenu
                            MenuItem {
                                text: qsTr("Select All")
                                shortcut: "Ctrl+A"
                                enabled: persistentController.sequences.count
                                onTriggered: addedFolders.selection.selectAll()
                            }
                            MenuItem {
                                text: qsTr("Deselect All")
                                shortcut: "Ctrl+D"
                                enabled: persistentController.sequences.count
                                onTriggered: addedFolders.selection.clear()
                            }
                        }

                        TableViewColumn {
                            role : "name"
                            title : qsTr("Name")
                            width : 115
                            horizontalAlignment: Text.AlignLeft
                        }
                        TableViewColumn {
                            id : folderSize
                            title : qsTr("Size")
                            width : 90
                            delegate : Text {
                                text: UtilFunctions.editFolderSize(model.size);
                                horizontalAlignment: Text.AlignLeft;
                            }
                        }
                        TableViewColumn {
                            title : qsTr("Files")
                            width : 50
                            horizontalAlignment: Text.AlignLeft
                            delegate : Text {
                                text: model.sequence.photo_no
                                horizontalAlignment: Text.AlignLeft;
                            }
                        }
                        TableViewColumn {
                            title : qsTr("Type")
                            width : 60
                            horizontalAlignment: Text.AlignLeft
                            delegate : Text {
                                text : UtilFunctions.sequenceTypeToString(model.type);
                                horizontalAlignment: Text.AlignLeft;
                            }
                        }

                        TableViewColumn {
                            title : qsTr("Track ID")
                            width : 90
                            horizontalAlignment: Text.AlignLeft
                            delegate : Text {
                                text : model.sequence.id < 0 ? "-" : model.sequence.id
                                horizontalAlignment: Text.AlignLeft;
                            }
                        }

                        FileDialog {
                            id : fileDialog
                            title : qsTr("Please choose a folder")
                            folder: shortcuts.home
                            selectExisting: true
                            selectFolder: true
                            onAccepted: {
                                persistentController.onFileDialogButton(fileDialog.fileUrl) //add the 2 methods
                            }
                            onRejected: {
                                console.log("Canceled")
                            }
                            Component.onCompleted: visible = false
                        }
                    }
                }

                // area for buttons related to folders
                Rectangle {
                    id : folderButtonsRectangle
                    width : 100
                    height : parent.height

                    Column {
                        id : folderRelatedButtons
                        spacing : 10

                        // button user for adding folders to the list
                        CustomButton {
                            id : addFolderButton
                            text : qsTr("Add folder")
                            enabled: !uploadController.isUploadStarted

                            MouseArea {
                                anchors.fill: addFolderButton
                                onClicked: {
                                    fileDialog.visible = true;
                                }
                            }
                        }

                        // button user for removing selected folder from the list
                        CustomButton {
                            id : removeFolderButton
                            text : qsTr("Remove folder")
                            enabled: persistentController.sequences.count > 0 && addedFolders.selection.count
                            onClicked: {
                                var indexes = [];
                                addedFolders.selection.forEach(function (rowIndex) {
                                    indexes.push(rowIndex);
                                });
                                persistentController.removeFolders(indexes);
                                indexes = [];
                                addedFolders.selection.clear();
                            }
                        }
                        // button used to upload photos to the server ( request token )
                        CustomButton
                        {
                            id: uploadButton
                            text: qsTr("Start")
                            enabled: loginController.isLoggedIn && persistentController.sequences.count > 0 && !uploadController.isUploadStarted
                            visible: !uploadController.isUploadStarted
                            onClicked: uploadController.startUpload();
                        }

                        CustomButton
                        {
                            id: pauseUploadButton
                            text: qsTr("Pause Upload")
                            visible: !uploadController.isUploadPaused && uploadController.isUploadStarted
                            onClicked: uploadController.pauseUpload();
                        }

                        CustomButton
                        {
                            id: resumeUploadButton
                            text: qsTr("Resume Upload")
                            visible: uploadController.isUploadPaused && uploadController.isUploadStarted
                            onClicked: uploadController.resumeUpload();
                        }

                        CustomButton
                        {
                            id: loginButton
                            text: qsTr("Login")
                            visible: !loginController.isLoggedIn
                            onClicked: loginController.login();
                        }

                        CustomButton
                        {
                            id: logutButton
                            text: qsTr("Logout")
                            enabled: !uploadController.isUploadStarted
                            visible: loginController.isLoggedIn
                            onClicked: loginController.logout();

                        }
                    }
                }
            }
        }

        // 2nd group of elements : details about uploaded photos
        Rectangle
        {
            id : uploadDetailsRectangle
            width : 410
            height : 65
            Column
            {
                id: column
                spacing : 10
                anchors.fill: parent

                ProgressBar
                {
                    id : uploadPhotosProgressBar
                    width : 400
                    height : 25
                    minimumValue : 0
                    maximumValue : 100
                    value : uploadController.percentage

                    Text
                    {
                        id : percentageUploaded
                        anchors.centerIn: parent
                        text: uploadController.percentage + " %"
                        font.pixelSize: 15
                    }
                }

                // uploaded photos / total photos
                Label {
                    id: currentUploadingPhotoLabel
                    text: qsTr("Uploaded files: ") + uploadController.uploadedNoFiles + " / " + persistentController.totalFiles
                }

                // uploaded : size / total size
                Label {
                    id: uploadedSizeLabel
                    text: qsTr("Uploaded: ")+UtilFunctions.editFolderSize(uploadController.uploadedSize)+" / " + UtilFunctions.editFolderSize(persistentController.totalSize)
                }

                // time estimation until finish
                Label {
                    id : estimationTimeFinish
                    text: qsTr("Time estimation until finish: ") + UtilFunctions.convertSeconds(uploadController.remainingTime)
                }

                // upload speed label
                Label {
                    id: uploadSpeed
                    text: qsTr("Upload speed: ") + UtilFunctions.editFolderSize(uploadController.uploadSpeed) + "/s"
                }

                // elapsed time label
                Label {
                    id : elapsedTime
                    text: qsTr("Elapsed time: ") + UtilFunctions.convertSeconds(uploadController.elapsedTime)
                }
            }
        }
    }

    //message dialog
    UploadCompleteDialog { }

    //error dialog
    ErrorDialog { }

}
