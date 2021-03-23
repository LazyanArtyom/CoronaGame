#include <QApplication>
#include <QScreen>
#include "frontend/covidgameui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CCovidGame* pGame = new CCovidGame(qApp->screens()[0]->size());
    pGame->showMaximized();
    pGame->resize(qApp->screens()[0]->size().width(), qApp->screens()[0]->size().height());
    //pGame->onRun();
    //pGame->StartMenu();

    return a.exec();
}
