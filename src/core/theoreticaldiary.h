#ifndef THEORETICALDIARY_H
#define THEORETICALDIARY_H

#include <QtWidgets>

class TheoreticalDiary : public QApplication {
    Q_OBJECT
public:
    // argc must be a reference to an integer otherwise the program will crash
    TheoreticalDiary(int &argc, char *argv[]);
    ~TheoreticalDiary();

    static TheoreticalDiary *instance();
};

#endif // THEORETICALDIARY_H
