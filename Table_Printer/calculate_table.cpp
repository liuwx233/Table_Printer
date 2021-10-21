#include "calculate_table.h"
#include"wff.h"

using namespace std;

#define N_OPR 6

const char pri[N_OPR][N_OPR] = { //运算符优先等级 [栈顶] [当前]
   /*              |-------------------- 当 前 运 算 符 ------------| */
   /*              !      &      |      ->    <->     (   */
 /* --  ! */    {'>',   '>',   '>',   '>',   '>',   ' '},
 /* |   & */    {'<',   '>',   '>',   '>',   '>',   ' '},
 /* 栈  | */    {'<',   '>',   '>',   '>',   '>',   ' '},
 /* 顶  -> */    {'<',   '<',   '<',   '>',   '>',   ' '},
 /* 运 <-> */    {'<',   '<',   '<',   '>',   '>',   ' '},
 /* 算  ( */    {'<',   '<',   '<',   '<',   '<',   ' '},

};

int opr_t0_int(const string& opr){
    if(opr == string("!")){
        return 0;
    }else if(opr == string("&")){
        return 1;
    }else if(opr == string("|")){
        return 2;
    }else if(opr == string("->")){
        return 3;
    }else if(opr == string("<->")){
        return 4;
    }else if(opr == string("(")){
        return 5;
    }else{
        printf("warning, detect unexpected oprand\n");
        return -1;
    }
}

bool opr_compare(const string& opr1, const string& opr2){
    //比较两个运算符优先级。
    //前一个是栈顶运算符，后一个是现有运算符。现有运算符不可能是"("
    //如果比栈顶元素优先级高，返回true
    int o1 = opr_t0_int(opr1);
    int o2 = opr_t0_int(opr2);
    if(pri[o1][o2] == '>'){ //栈顶大于当前
        return false;
    }else if(pri[o1][o2] == '<'){//栈顶小于当前
        return true;
    }else{
        printf("In opr_compare, not expected comparison\n");
        return true;
    }

}

void Calculation(const string& opr, stack<WFF>& wff_stack, list<WFF>& formats){
    //根据操作符opr，在wff_stack中取数进行运算，运算结束后
    //把结果push到wff_stack里边
    //同时将运算过的中间公式储存到formats里边
    WFF temp_wff('g');
    if(opr == string("!")){
        temp_wff = Not(wff_stack.top());
        wff_stack.pop();
    }else if(opr == string("&")){
        WFF w2 = wff_stack.top(); //注意：栈顶元素是后读到的，所以是w2
        wff_stack.pop();
        WFF w1 = wff_stack.top();
        wff_stack.pop();
        temp_wff = And(w1, w2);
    }else if(opr == string("|")){
        WFF w2 = wff_stack.top();
        wff_stack.pop();
        WFF w1 = wff_stack.top();
        wff_stack.pop();
        temp_wff = Or(w1, w2);
    }else if(opr == string("->")){
        WFF w2 = wff_stack.top();
        wff_stack.pop();
        WFF w1 = wff_stack.top();
        wff_stack.pop();
        temp_wff = Contain(w1, w2);
    }else if(opr == string("<->")){
        WFF w2 = wff_stack.top();
        wff_stack.pop();
        WFF w1 = wff_stack.top();
        wff_stack.pop();
        temp_wff = Biconditional(w1, w2);
    }else{
        printf("warning, detect unexpected oprand\n");
    }
    wff_stack.push(temp_wff);
    formats.push_back(temp_wff);
}

void handle_opr(string& now_opr, stack<string>& opr_stack, stack<WFF>& wff_stack, list<WFF>& formats){
    //当读取到操作符时候的处理方式
    if(now_opr == string("(")){
        opr_stack.push(now_opr);
        return;
    }
    if(now_opr == string(")")){
        //读到右括号，不断弹出知道直到左括号，最后把左括号弹出
        while(opr_stack.top() != string("(")){
            string pop_opr = opr_stack.top();
            opr_stack.pop();
            Calculation(pop_opr, wff_stack, formats);
        }
        opr_stack.pop();
        return;
    }
    if(opr_stack.empty()){
        opr_stack.push(now_opr);
    }else{
        if(opr_compare(opr_stack.top(), now_opr)){ //如果是true，栈顶小于当前
            opr_stack.push(now_opr);
        }else{
            while(!opr_stack.empty() && !opr_compare(opr_stack.top(), now_opr)){
                string pop_opr = opr_stack.top();
                opr_stack.pop();
                Calculation(pop_opr, wff_stack, formats);
            }
            opr_stack.push(now_opr);
        }
    }
}

Calculate_table::Calculate_table(string& s, vector<char>& variables, list<WFF>& formats){
//根据输入字符串，填补variables和formats
    stack<string> opr_stack;
    stack<WFF> wff_stack;
    for(int i = 0; i < s.length(); i++){
        //如果读到原子命题变量，直接入栈, 同时计入variables
        if(isalpha(s[i])){
            wff_stack.push(WFF(s[i]));
            if(find(variables.begin(), variables.end(), s[i]) == variables.end()){
                variables.push_back(s[i]);
            }
        }else{
            string now_opr;
            switch (s[i]) {
            case '!':
                now_opr = string("!");
                break;
            case '&':
                now_opr = string("&");
                break;
            case '|':
                now_opr = string("|");
                break;
            case '-':
                if(s[i+1] == '>'){
                    now_opr = string("->");
                    i += 1;
                }else{
                    printf("Format Error Detected!\n");
                    //TO DO: 错误处理
                }
                break;
            case '<':
                if(s[i+1] == '-' && s[i+2] == '>'){
                    now_opr = string("<->");
                    i += 2;
                }else{
                    printf("Format Error Detected!\n");
                    //TO DO: 错误处理
                }
                break;
            case '(':
                now_opr = string("(");
                break;
            case ')':
                now_opr = string(")");
                break;
            default:
                printf("Format Error Detected!\n");
                //TO DO: 错误处理
            } //switch
            handle_opr(now_opr, opr_stack, wff_stack, formats);
        }//else
    }//for

    //最后把操作符栈pop至空
    while(!opr_stack.empty()){
        string now_opr = opr_stack.top();
        opr_stack.pop();
        Calculation(now_opr, wff_stack, formats);

    }

}
