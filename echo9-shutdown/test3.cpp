#include "leetcodeutil.h"
using namespace std;

class Solution {
public:
    vector<string> generateParents(int count) {
        string str;
        int num = 0;
        parenthesisArrayCount(0, 0, count, str, num);
        cout << num << endl;

        return ans;
    }
private:
    void parenthesisArrayCount(int l,int r,int n,string s,int& num) {  
        if(r==n) {  
            num++;  
            // cout<<s<<endl;  
            ans.push_back(s);
            return;  
        }  
        if(r==l) {  
            s.append("(");  
            l++;  
            parenthesisArrayCount(l,r,n,s,num);  
        }  
        else {//r<l  
            if(l==n) {  
                s.append(")");  
                r++;  
                parenthesisArrayCount(l,r,n,s,num);  
            }  
            else {  
                s.append("(");  
                l++;  
                parenthesisArrayCount(l,r,n,s,num);  
                s.pop_back();  
                l--;  
                s.append(")");  
                r++;  
                parenthesisArrayCount(l,r,n,s,num);  
            }  
        }

        return;  
    } 
vector<string> ans; 
};

int main(int argc, char const *argv[])
{
    cout << Solution().generateParents(2) << endl;

    return 0;
}