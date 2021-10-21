#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<vector>
#include<list>
#include<string>
#include<QPushButton>
#include<QLabel>
#include<QLineEdit>
#include<QDebug>
#include<QTableWidget>
#include<math.h>
#include<algorithm>


#include"calculate_table.h"
#include"wff.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void Calculate();
    void readin_text();
    void print_table();
    bool validation(const string& s);
    void re_start();



private:
    Ui::Widget *ui;
    std::string input_string;
    QPushButton* calculate_button;
    QLineEdit* input_window;
    QLabel* label;

    //真值表相关变量
    std::vector<char> variables; //变量向量，无重复
    std::list<WFF> formats; // 公式list

};
#endif // WIDGET_H
