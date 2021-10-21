#include "widget.h"
#include "ui_widget.h"
#include "wff.h"


//#define __DEBUG__

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    input_string = std::string();
    variables = std::vector<char>();
    formats = std::list<WFF>();

    //下面的代码用于添加输入框、计算按钮
    calculate_button = new QPushButton;
    calculate_button->setText("calculate!");
    calculate_button->setParent(this);
    calculate_button->move(250, 2);

    input_window = new QLineEdit;
    input_window->setParent(this);
    QPalette palette;
    palette.setColor(QPalette::Base,Qt::red);
    input_window->setPalette(palette);
    input_window->move(60,5);

    label = new QLabel;
    label->setParent(this);
    label->setText("Expr:");
    label->move(10,10);

    //让输入框每发生一次变动（增删），Widget类的string都会更新

    connect(input_window, &QLineEdit::textChanged, this, &Widget::readin_text);

    //为输入按钮添加计算事件

    connect(calculate_button, &QPushButton::clicked, this, &Widget::Calculate);



}

Widget::~Widget()
{
    delete ui;
    delete input_window;
    delete calculate_button;
    delete label;
}

void Widget::readin_text(){
    input_string = input_window->text().toStdString();
#ifdef __DEBUG__
    qDebug()<<"read_in_text\n";
#endif
    //如果输入的字符串是合法的，那么文本框为白色，按钮可按。否则为红色，按钮不可按
    if(validation(input_string)){
        calculate_button->setEnabled(true);
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        input_window->setPalette(palette);
    }else{
        calculate_button->setEnabled(false);
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        input_window->setPalette(palette);

    }
}

void Widget::Calculate(){
#ifndef __DEBUG__
    Calculate_table(input_string, variables, formats);
#endif
#ifdef __DEBUG__
    qDebug()<<"Calculate\n";
#endif
    print_table();
    re_start();
}

void Widget::print_table(){
    // 利用variables和formats打印真值表
#ifdef __DEBUG__
    variables.push_back('p');
    variables.push_back('s');
    variables.push_back('r');
    variables.push_back('q');

    formats = list<WFF>(2);
    list<WFF>::iterator it = formats.begin();
    it->GetVariables().insert('r');
    it->GetVariables().insert('p');
    it->GetTruthVector().push_back(false);
    it->GetTruthVector().push_back(false);
    it->GetTruthVector().push_back(true);
    it->GetTruthVector().push_back(false);

    it++;
    it->GetVariables().insert('q');
    it->GetVariables().insert('s');
    it->GetTruthVector().push_back(false);
    it->GetTruthVector().push_back(true);
    it->GetTruthVector().push_back(true);
    it->GetTruthVector().push_back(false);

#endif
    sort(variables.begin(), variables.end());
    int n = variables.size() + formats.size();
    int m = pow(2, variables.size());
    //下面的代码初始化表格
    ui->truth_table->setColumnCount(n);
    ui->truth_table->setRowCount(m);
    ui->truth_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->truth_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    //下面的代码设置表头，表头包含所有的变量字母和所有的公式
    QStringList label_list;
    for(int i = 0; i < variables.size(); i++){
        label_list << QString(1,variables[i]);
    }
    for(list<WFF>::iterator it = formats.begin(); it != formats.end(); it++){
#ifndef __DEBUG__
         QString label_qstring = QString::fromStdString(ToString(*it));
#endif
#ifdef __DEBUG__
         QString label_qstring = "format";
#endif
         label_list << label_qstring;

    }
    ui->truth_table->setHorizontalHeaderLabels(label_list);

    //下面给variable列各个元素赋予真值
    for(int i = 0; i < m; i++){
        for(int j = 0; j < variables.size(); j++){
            if(int(i / pow(2, variables.size()-j-1)) % 2 == 0){ //满足if里边的条件，那么赋予0值
                QTableWidgetItem* item = new QTableWidgetItem("0");
                ui->truth_table->setItem(i, j, item);
            }else{
                QTableWidgetItem* item = new QTableWidgetItem("1");
                ui->truth_table->setItem(i, j, item);

            }
        }
    }
    //下面给各个公式列赋予真值
    int cnt = 0;
    for(list<WFF>::iterator it = formats.begin(); it != formats.end(); it++, cnt++){
        //找到对应的列
        int truth_table_column = cnt + variables.size();
        //扩充该列公式的真值向量
        vector<bool> format_truth_vector = ExpandTruthTable(*it, variables);
        //将真值向量逐个插入到表格中.
        for(int i = 0; i < m; i++){
            if (format_truth_vector[i]){
                QTableWidgetItem* item = new QTableWidgetItem("1");
                ui->truth_table->setItem(i, truth_table_column, item);
            }else{
                QTableWidgetItem* item = new QTableWidgetItem("0");
                ui->truth_table->setItem(i, truth_table_column, item);
            }
        }

    }
}

bool Widget::validation(const string& s){ //鉴定input_string是否合法
    if(s.empty()){
        return false;
    }

    bool expected_alpha = true;
    int i = 0;
    while(i < s.size()){
        if(expected_alpha){
            //如果expected_alpha为true, 那么期望接受到字母或者!(
            if(isalpha(s[i])){
                i++;
                expected_alpha = false;
            }else if(s[i] == '!'){
                //如果接收到!, 那么后面应该跟着字母或者!(
                i++;
            }else if(s[i] == '('){
                //如果读到(, 那么递归处理()之间的所有内容
                int bracket_cnt = 1;
                bool ret = false;
                int j = i + 1;
                for(j = i + 1; j < s.size(); j++){
                    if(s[j] == '('){
                        bracket_cnt ++;
                    }else if(s[j] == ')'){
                        bracket_cnt --;
                    }
                    if(bracket_cnt == 0){
                        //此时j指向匹配的')'
                        //取出括号中字串，递归进行判断
                        string inbracket_str = string(s, i+1, j-i-1);
                        ret = validation(inbracket_str);
                        break;
                    }
                }
                //有两种情况：要么没找到匹配的括号，要么括号内的式子不正确
                if(!ret || bracket_cnt != 0){
                    return false;
                }
                //其他的是正确情况，此时将指针i移动到右括号)后边，然后应该期待再读到操作符
                i = j + 1;
                expected_alpha = false;
            }else{
                return false;
            }
        }else{
            //如果为false, 那么期望接受到除了!()以外的运算符
            switch (s[i]) {
            case '&':
                break;
            case '|':
                break;
            case '-':
                if(s[i+1] != '>'){
                    return false;
                }else{
                    i += 1;
                    break;
                }
            case '<':
                if(s[i+1] != '-' && s[i+2] != '>'){
                    return false;
                }else{
                    i += 2;
                    break;
                }
            default:
                return false;
            }
            i += 1;
            expected_alpha = true;
        }
    }
    if(s[s.size() - 1] == ')' || isalpha(s[s.size() - 1])){
        return true;
    }else{
        return false;
    }


}

void Widget::re_start(){
    //清空variables, formats, input_string, 初始化input_window和button
    vector<char>temp_v = vector<char>();
    variables.swap(temp_v);
    formats.clear();
//    input_string = string();
//    calculate_button->setEnabled(false);
//    QPalette palette;
//    palette.setColor(QPalette::Base,Qt::red);
//    input_window->setPalette(palette);
}

