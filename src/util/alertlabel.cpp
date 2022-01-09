#include "alertlabel.h"

AlertLabel::AlertLabel(QWidget *parent) : QLabel(parent)
{
    //    connect(InternalManager::instance(), &InternalManager::update_theme, this, &AlertLabel::update_theme,
    //        Qt::QueuedConnection);

    // update_theme cannot be called here as this constructor is called before the constructor of QLabel. Therefore, a
    // flag needs to be setup so that it only runs the first time set_text is called.
    first_time_stylesheet_set = false;

    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, &AlertLabel::timeout);
}

void AlertLabel::update_theme()
{
    QFile file(":/global/alertlabel.qss");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());

    first_time_stylesheet_set = false;
}

void AlertLabel::set_text(const QString &&t, bool const auto_hide)
{
    if (!first_time_stylesheet_set)
        update_theme();

    if (!t.size()) {
        setVisible(false);
    }
    else {
        setText(t);
        setVisible(true);

        if (auto_hide) {
            timer->setInterval(5000);
            timer->start();
        }
        else {
            timer->stop();
        }
    }

    update();
}

void AlertLabel::timeout()
{
    setVisible(false);
}
