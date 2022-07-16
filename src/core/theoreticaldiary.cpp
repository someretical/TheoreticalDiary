#include "theoreticaldiary.h"
#include "../util/Util.h"

TheoreticalDiary::TheoreticalDiary(int &argc, char *argv[]) : QApplication(argc, argv)
{


TheoreticalDiary::~TheoreticalDiary() {}

TheoreticalDiary *TheoreticalDiary::instance()
{
    return static_cast<TheoreticalDiary *>(QApplication::instance());
}
