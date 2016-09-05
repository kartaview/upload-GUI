#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <logincontroller.h>
#include "persistentcontroller.h"
#include "uploadcontroller.h"
#include <osmlogin.h>
#include <QtQml>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("kQOAuth");
    QGuiApplication::setApplicationName("telenavOSV");

    QQmlApplicationEngine engine;
    OSMLogin *osmLogin = new OSMLogin();
    LoginController *loginController = new LoginController(osmLogin);
    PersistentController *persistentController = new PersistentController();
    UploadController *uploadController = new UploadController(loginController, persistentController);

    engine.rootContext()->setContextProperty("persistentController", persistentController);
    engine.rootContext()->setContextProperty("loginController", loginController);
    engine.rootContext()->setContextProperty("uploadController", uploadController);
    engine.load(QUrl(QStringLiteral("qrc:/UploadComponent.qml")));
    return app.exec();
}
