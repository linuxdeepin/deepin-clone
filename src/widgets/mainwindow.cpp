#include "mainwindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class TitleLabel : public QWidget
{
public:
    explicit TitleLabel(QWidget *parent = 0);
};

TitleLabel::TitleLabel(QWidget *parent)
    : QWidget(parent)
{

}

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    m_title = new TitleLabel();
    m_contentWidget = new QStackedWidget();
    m_bottomButton = new QPushButton();
    m_bottomButton->setFixedSize(width() * 0.36, height() * 0.055);

    QWidget *main_widget = new QWidget(this);

    setCentralWidget(main_widget);

    QVBoxLayout *layout = new QVBoxLayout(main_widget);

    layout->addWidget(m_title, 0, Qt::AlignHCenter);
    layout->addWidget(m_contentWidget, 0, Qt::AlignHCenter);
    layout->addWidget(m_bottomButton, 0, Qt::AlignHCenter);
}

void MainWindow::setStatus(MainWindow::Status status)
{
    switch (status) {
    case SelectAction:

        break;
    case SelectFile:
        break;
    case WaitConfirm:
        break;
    case Working:
        break;
    case End:
        break;
    default:
        break;
    }
}

void MainWindow::next()
{
    if (m_currentStatus < End) {
        setStatus(Status(m_currentStatus + 1));
    }
}
