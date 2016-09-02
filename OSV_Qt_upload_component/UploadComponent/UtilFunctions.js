// converts bytes into kb/mb/gb
function editFolderSize(param) {
    var aux;
    var result;

    if (param > 1024000000)
    {
        aux = param / 1000000000;
        result = aux.toFixed(2) + " GB";
    }
    else if (param > 1024000)
    {
        aux = param / 1000000;
        result = aux.toFixed(2) + " MB";
    }
    else if (param > 1024)
    {
        aux = param / 1000;
        result = aux.toFixed(2) + " KB";
    }
    else
        result = param + " B";
    return result;
}

// returns total number of photos
function getNumberOfPhotos(sequence) {
    var i;
    var numberOfPhotos = 0;

    for (i = 0; i < sequence.size(); i++) {
        numberOfPhotos += sequence.get(i).noFiles;
    }
    return numberOfPhotos;
}

// returns total size of photos to upload
function getUploadSize(sequence) {
    var i;
    var sizeOfPhotos = 0;

    if (sequence.size() === 0)
        return 0;
    for(i = 0; i < sequence.size(); i++) {
        sizeOfPhotos += sequence.get(i).size;
    }
    if (sizeOfPhotos === 0)
        return 0;
    sizeOfPhotos = editFolderSize(sizeOfPhotos);
    return sizeOfPhotos;
}

// converts seconds to hours, minutes, seconds
function convertSeconds(numberOfSeconds) {
    var hours = 0;
    var minutes = 0;
    var seconds = 0;

    hours = numberOfSeconds / 3600;
    numberOfSeconds %= 3600;
    minutes = Math.floor(numberOfSeconds / 60);
    numberOfSeconds %= 60;

    seconds = numberOfSeconds;

    return (hours.toFixed(0) + " h : " +  minutes.toFixed(0) + " m : " + seconds.toFixed(0) + " s");
}

function sequenceTypeToString(type) {
    if(type === 1)
        return "Photo";
    else if(type === 2)
        return "Video";
}
