#ifndef TESTPRECISION_H
#define TESTPRECISION_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class TestPrecision;
}

class TestPrecision : public QDialog
{
    Q_OBJECT

public:
    explicit TestPrecision(QWidget *parent = Q_NULLPTR);
    ~TestPrecision();

private slots:
    void startTest();
    void stopTest();
    void resetLights();

private:
    Ui::TestPrecision *ui;
    QTimer m_timerTest;
    QTimer m_timerReset;
};

#endif // TESTPRECISION_H
