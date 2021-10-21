#ifndef WFF_H
#define WFF_H

#include<string>
#include<vector>
#include<list>
#include<set>
#include<algorithm>
#include<math.h>

using namespace std;


class WFF
{
public:
    list<char> char_list; //顺序存储命题中的字符
    set<char> variable_set; //所有原子命题构成的集合
    vector<bool> truth_table; //该命题对象的真值表

    //构造、析构等函数
    WFF(char P);
    WFF(const WFF& w);
    ~WFF(){}
};

vector<bool> ExpandTruthTable(WFF& w, vector<char>& new_variables);
void SingleExpand(vector<bool>& before_vec, vector<bool>& after_vec, int pos, int before_var_num, int insert_var_num);

WFF Not(WFF& w);
WFF And(WFF& w1, WFF& w2);
WFF Or(WFF& w1, WFF& w2);
WFF Contain(WFF& w1, WFF& w2);
WFF Biconditional(WFF& w1, WFF& w2);
string ToString(WFF& w);



#endif // WFF_H
