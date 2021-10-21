#ifndef CALCULATE_TABLE_H
#define CALCULATE_TABLE_H
#include<string>
#include<vector>
#include<list>
#include"wff.h"
#include<stack>
#include<ctype.h>

using namespace std;


class Calculate_table
{
    //是一个函数对象
public:
    Calculate_table(string& s, vector<char>& variables, list<WFF>& formats);
};

#endif // CALCULATE_TABLE_H
