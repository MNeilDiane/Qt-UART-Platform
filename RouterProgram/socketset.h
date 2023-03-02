#ifndef SOCKETSET_H
#define SOCKETSET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
namespace Ui {
class Socketset;
}

class Socketset : public QWidget
{
    Q_OBJECT

public:
    explicit Socketset(QWidget *parent = nullptr);
    ~Socketset();
    void pushButton_add();
    void pushButton_deleteself();

private:
    Ui::Socketset *ui;
};

#endif // SOCKETSET_H
