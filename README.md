# Upload GUI 
Uploader tool for OpenStreetView

## Description:
This application is used to easily upload photos and videos, taken with the iOS/Android OSV application or any other device 
which saves GPS location information onto the photos. In the desktop application the user can quickly multiple folder into 
the app. The user will have to login and grant access so the files will be uploaded correctly to their OSV account. 
After that just start the upload. While the upload is running the user has the possibility to pause/resume it and if he the 
computer crashes, shuts down because of the battery ( in case of laptops ) or simply the user wants to shut it down, don’t 
worry! The progress is saved and the next time the application is run the user can start it from where is stopped.


## Compatibility: 
The application is compatible with :
- Windows
- Mac OSX
- Linux (new)
    
## Requirements:
- Qt 5.6
- open SSL

Windows:
- Visual C++ Redistributable Packages (for Windows)
- OpenSSL

## Build :
  In UploadComponent.pro file you have to specify "CONFIG += deploy", so it will copy the needed files for the build.
  
  On Windows you have to copy this additional from their build folder :
-	HttpRequest.dll
-	KQOAuth.dll
-	Libeay32.dll
-	Ssleay.dll
