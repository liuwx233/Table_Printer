#include "wff.h"

WFF::WFF(char P)
{
    char_list.push_back(P);
    variable_set.insert(P);
    truth_table = { 0, 1 };
}

WFF::WFF(const WFF& w)
{
    char_list = w.char_list;
    variable_set = w.variable_set;
    truth_table = w.truth_table;
}

void SingleExpand(vector<bool>& before_vec, vector<bool>& after_vec, int pos, int before_var_num, int insert_var_num)
{
    for (int i = 0; i < after_vec.size(); i++) {
        //对于一个特定的i，找到在原数组中的位置, 第group组中的第number个
        int group = i / int(round(pow(2, pos + insert_var_num)));
        int number = i % int(round(pow(2, pos)));
        after_vec[i] = before_vec[group * int(round(pow(2, pos))) + number];
    }
}
vector<bool> ExpandTruthTable(WFF& w, vector<char>& new_variables)
{
    //提取w原来的真值表和原来的变量列表
    vector<char> before_variables;
    before_variables.assign(w.variable_set.begin(), w.variable_set.end());
    sort(before_variables.begin(), before_variables.end());

    //提取一个(m+1)维向量insert_info_vec，存储中间各插入了多少个元素
    vector<int> insert_info_vec(before_variables.size() + 1);
    vector<char>::iterator it;
    vector<char>::iterator last_it = new_variables.begin();
    for (int i = 0; i < before_variables.size(); i++) {
        it = find(new_variables.begin(), new_variables.end(), before_variables[i]);
        if (i == 0) {
            insert_info_vec[i] = distance(last_it, it);
        }
        else {
            insert_info_vec[i] = distance(last_it, it) - 1;
        }
        last_it = it;
    }
    insert_info_vec[before_variables.size()] = distance(last_it, new_variables.end()) - 1;

    //根据insert_info_vec，输出最后的扩充vector
    int insert_cnt = 0; // 目前已插入字母数量
    const int before_cnt = before_variables.size();  //插入前的字母数量
    vector<bool> before_vec = w.truth_table;
    vector<bool> last_vec;
    for (int i = insert_info_vec.size() - 1; i >= 0; i--)
    {
        int this_insert_cnt = insert_info_vec[i];  //这一次插入的字母量
        last_vec = vector<bool>(int(round(pow(2, before_cnt + insert_cnt + this_insert_cnt))));

        SingleExpand(before_vec, last_vec, insert_info_vec.size() - 1 - i + insert_cnt, before_cnt + insert_cnt, this_insert_cnt);
        insert_cnt += this_insert_cnt;

        before_vec = last_vec;
    }
    return last_vec;
}

WFF Not(WFF& w)
{
    WFF _w(w); //复制构造
    auto first = w.truth_table.begin();
    auto end = w.truth_table.end();
    //真值表取反
    while (first != end)
    {
        _w.truth_table[*first] = !w.truth_table[*first];
        ++first;
    }
    //修改运算符链表
    if (w.char_list.size() == 1)
    {
        _w.char_list.push_front('~');
    }
    else
    {
        _w.char_list.push_front('(');
        _w.char_list.push_front('~');
        _w.char_list.push_back(')');
    }
    return _w;
}
WFF And(WFF& w1, WFF& w2)
{
    WFF _w(w1);
    WFF _w2(w2);//用于真值表运算
    WFF _w3(w1);//用于缓存
    //修改运算符链表
    if (w1.char_list.size() != 1)
    {
        _w.char_list.push_front('(');
        _w.char_list.push_back(')');
    }
    if (w2.char_list.size() != 1)
    {
        w2.char_list.push_front('(');
        w2.char_list.push_back(')');
        w2.char_list.push_front('\\');
        w2.char_list.push_front('/');
    }
    else
    {
        w2.char_list.push_front('\\');
        w2.char_list.push_front('/');
    }
    list<char>::iterator it = _w.char_list.end();
    _w.char_list.splice(it, w2.char_list);
    //扩展真值表
    _w3.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    vector<char> v;
    v.assign(_w3.variable_set.begin(), _w3.variable_set.end());
    sort(v.begin(), v.end()); //字典序排序
    _w.truth_table = ExpandTruthTable(_w, v);
    _w2.truth_table = ExpandTruthTable(_w2, v);
    //合并字母表集合
    _w.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    //真值表运算
    auto first1 = _w.truth_table.begin();
    auto first2 = _w2.truth_table.begin();
    auto end = _w.truth_table.end();
    while (first1 != end)
    {
        *first1 = *first1 && *first2;
        ++first1;
        ++first2;
    }
    return _w;
}
WFF Or(WFF& w1, WFF& w2)
{
    WFF _w(w1);
    WFF _w2(w2);
    WFF _w3(w1);//用于缓存
    //修改运算符链表
    if (w1.char_list.size() != 1)
    {
        _w.char_list.push_front('(');
        _w.char_list.push_back(')');
    }
    if (w2.char_list.size() != 1)
    {
        w2.char_list.push_front('(');
        w2.char_list.push_back(')');
        w2.char_list.push_front('/');
        w2.char_list.push_front('\\');
    }
    else
    {
        w2.char_list.push_front('/');
        w2.char_list.push_front('\\');
    }
    list<char>::iterator it = _w.char_list.end();
    _w.char_list.splice(it, w2.char_list);
    //扩展真值表
    _w3.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    vector<char> v;
    v.assign(_w3.variable_set.begin(), _w3.variable_set.end());
    sort(v.begin(), v.end()); //字典序排序
    _w.truth_table = ExpandTruthTable(_w, v);
    _w2.truth_table = ExpandTruthTable(_w2, v);
    //合并字母表集合
    _w.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    //真值表运算
    auto first1 = _w.truth_table.begin();
    auto first2 = _w2.truth_table.begin();
    auto end = _w.truth_table.end();
    while (first1 != end)
    {
        *first1 = *first1 || *first2;
        ++first1;
        ++first2;
    }
    return _w;
}
WFF Contain(WFF& w1, WFF& w2)
{
    WFF _w(w1);
    WFF _w2(w2);
    WFF _w3(w1);//用于缓存
    //修改运算符链表
    if (w1.char_list.size() != 1)
    {
        _w.char_list.push_front('(');
        _w.char_list.push_back(')');
    }
    if (w2.char_list.size() != 1)
    {
        w2.char_list.push_front('(');
        w2.char_list.push_back(')');
        w2.char_list.push_front('>');
        w2.char_list.push_front('-');
    }
    else
    {
        w2.char_list.push_front('>');
        w2.char_list.push_front('-');
    }
    list<char>::iterator it = _w.char_list.end();
    _w.char_list.splice(it, w2.char_list);
    //扩展真值表
    _w3.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    vector<char> v;
    v.assign(_w3.variable_set.begin(), _w3.variable_set.end());
    sort(v.begin(), v.end()); //字典序排序
    _w.truth_table = ExpandTruthTable(_w, v);
    _w2.truth_table = ExpandTruthTable(_w2, v);
    //合并字母表集合
    _w.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    //真值表运算
    auto first1 = _w.truth_table.begin();
    auto first2 = _w2.truth_table.begin();
    auto end = _w.truth_table.end();
    while (first1 != end)
    {
        *first1 = !(*first1) || *first2;
        ++first1;
        ++first2;
    }
    return _w;
}
WFF Biconditional(WFF& w1, WFF& w2)
{
    WFF _w(w1);
    WFF _w2(w2);
    WFF _w3(w1);//用于缓存
    //修改运算符链表
    if (w1.char_list.size() != 1)
    {
        _w.char_list.push_front('(');
        _w.char_list.push_back(')');
    }
    if (w2.char_list.size() != 1)
    {
        w2.char_list.push_front('(');
        w2.char_list.push_back(')');
        w2.char_list.push_front('>');
        w2.char_list.push_front('-');
        w2.char_list.push_front('<');
    }
    else
    {
        w2.char_list.push_front('>');
        w2.char_list.push_front('-');
        w2.char_list.push_front('<');
    }
    list<char>::iterator it = _w.char_list.end();
    _w.char_list.splice(it, w2.char_list);
    //扩展真值表
    _w3.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    vector<char> v;
    v.assign(_w3.variable_set.begin(), _w3.variable_set.end());
    sort(v.begin(), v.end()); //字典序排序
    _w.truth_table = ExpandTruthTable(_w, v);
    _w2.truth_table = ExpandTruthTable(_w2, v);
    //合并字母表集合
    _w.variable_set.insert(w2.variable_set.begin(), w2.variable_set.end());
    //真值表运算
    auto first1 = _w.truth_table.begin();
    auto first2 = _w2.truth_table.begin();
    auto end = _w.truth_table.end();
    while (first1 != end)
    {
        *first1 = (*first1 && *first2) || (!(*first1) && !(*first2));
        ++first1;
        ++first2;
    }
    return _w;
}

//以字符形式输出公式
string ToString(WFF& w)
{
    string output;
    auto first = w.char_list.begin();
    auto end = w.char_list.end();
    while (first != end)
    {
        output += *first;
        ++first;
    }
    return output;
}
