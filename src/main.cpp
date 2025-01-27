#include "GuiManager.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QDebug>
#include <QStyleFactory>
#include"CamWindowManager.h"
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	//QTextCodec* codec = QTextCodec::codecForName("GBK");
	//QTextCodec::setCodecForLocale(codec);
    QFont f = a.font();
    f.setFamily("微软雅黑");
    a.setFont(f);
    a.setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    w.showMaximized();
    return (a.exec());
}


