#include <string>  
#include <iostream>  
using namespace std;  
  
void parenthesisArrayCount(int l,int r,int n,string s,int& num)  
{  
    if(r==n)  
    {  
        num++;  
        cout<<s<<endl;  
        return;  
    }  
    if(r==l)  
    {  
        s.append("(");  
        l++;  
        parenthesisArrayCount(l,r,n,s,num);  
    }  
    else//r<l  
    {  
        if(l==n)  
        {  
            s.append(")");  
            r++;  
            parenthesisArrayCount(l,r,n,s,num);  
        }  
        else  
        {  
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
int main()  
{  
    int num=0;  
    string s;  
    parenthesisArrayCount(0,0,2,s,num);  
    cout<<"共"<<num<<"种";  
    return 1;  
}  