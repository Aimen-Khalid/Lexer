#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
using namespace std;
#define rows 81//inputs
#define cols 58//states
void printVector(vector<string> v)
{
	for (int i = 0; i < v.size(); i++)
	{
		cout << v[i] << endl;
	}
}
void print2DArray(int A[rows][cols])
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            cout << A[r][c]<<" ";
        }
        cout << endl;
    }
}
bool isKeyword(string s)
{
    vector<string> keywords{ "False", "none", "True", "as", "break", "class",
        "continue", "def", "elif", "else", "except", "for", "global", "if", "import", "is", "return", "while", "with" };
    return find(keywords.begin(), keywords.end(), s) != keywords.end();
}
int inputToIndex(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'A' && c <= 'Z')
    {
        return c - 55;
    }
    if (c >= 'a' && c <= 'z')
    {
        return c - 61;
    }
    switch (c)
    {
    case '_':
        return 62;
    case '+':
        return 63;
    case '-':
        return 64;
    case '*':
        return 65;
    case '/':
        return 66;
    case '&':
        return 67;
    case '<':
        return 68;
    case '>':
        return 69;
    case '|':
        return 70;
    case '^':
        return 71;
    case '=':
        return 72;
    case '%':
        return 73;
    case '!':
        return 74;
    case '.':
        return 75;
    case ' ':
    case '\t':
    case '(':
    case ')':
    case ':':
    case ',':
    case ']':
    case '[':
        return 76;
    case'#':
        return 77;
    case '\n':
        return 78;
    case '"':
        return 79;
    case '\'':
        return 80;
    default: 
        return -1;
    }

}
void populateTT(ifstream& rdr, int TT[][cols])
{
    string line, word, temp;
    
    getline(rdr, line);//ignoring the heading line that represents states
    int r = 0, c = 0;
    
    while (rdr >> temp) 
    {
        c = 0;
        stringstream s(temp);
        getline(s, word, ',');//ignoring the character input at start of row ie ignoring the first column
        //cout << r << "\t";
        while (getline(s, word, ',')) 
        {
            TT[r][c] = stoi(word);
            //cout << word << " ";
            c++;
        }
        //cout << endl;
        r++;
    }
    
}
bool isOperatorState(int prevState, int currState)
{
    return ((((prevState >= 15 && prevState <= 29) || (prevState == 32 || prevState == 34 || prevState == 40 || prevState == 37))) && currState != 26 && (currState == 13 || currState == 14 || currState == 1 || currState == 0 || currState == 38 || currState == 41 || currState == 42 || currState == 50));
}
bool isIdentifierState(int prevState, int currState)
{
    return prevState >= 1 && prevState <= 12 && (currState == 0 || currState == 41 || currState == 42 || currState == 50 || (currState >= 15 && currState <= 29 && currState != 26))|| (prevState == 30 || prevState == 31 || prevState == 33 || prevState == 35 || prevState == 36 || prevState == 39) && currState == 0;
}
bool isIntState(int prevState, int currState)
{
    return prevState == 13 && (currState == 0 || currState == 41 || (currState >= 15 && currState <= 29 && currState != 26));
}
bool isFloatState(int prevState, int currState)
{
    return prevState == 14 && (currState == 0 || currState == 41 || (currState >= 15 && currState <= 29 && currState != 26));
}
bool isCommentState(int prevState, int currState)
{
    return prevState == 41 && currState == 0;
}
bool isSingleStringState(int prevState, int currState)
{
    return (prevState == 43 && currState != 45) || (prevState == 51 && currState != 53);
}
bool isMultiStringState(int prevState, int currState)
{
    return (prevState == 49 || prevState == 57);
}
void Lexer(ifstream& rdr, int TT[][cols])
{
    int currState = 0, prevState = 0;
    char c;
    string s;
    do
    {
        rdr.get(c);
        if (c == '$')
        {
            cout << "";
        }
        if (rdr.eof())
        {
            if (currState == 46 || currState == 54 || currState == 47 || currState == 55 || currState == 48 || currState == 56)
            {
                cout << "Lexical error. Multi line comment not closed.";
                return;
            }
            c = '\n';
        }
        s.push_back(c);

        prevState = currState;
        if (currState!=44 && currState!=52 && currState != 42 && currState != 50 &&currState != 45 && currState != 46 && currState != 53 && currState != 54 && inputToIndex(c) == -1)
        {
            cout << "Lexical error encountered.";
            return;
        }
        if(!((currState==42 || currState==50 || currState == 44 || currState == 52 || currState == 45 || currState == 46 || currState == 53 || currState == 54) && inputToIndex(c) == -1))//do not change current state if special character encountered within comment
          currState = TT[inputToIndex(c)][currState];
        if (currState == 0 && prevState == 0)//handling multiple consecutive white spaces
            s.clear();

        if (currState == -1)
        {
            cout << "Lexical error encountered.";
            return;
        }
        else if(isOperatorState(prevState,currState))
        {
            s.pop_back();
            streampos oldpos; char* a;//saving the file cursor position to check for 'not in' operator and to track back if its just 'not' and not 'not in'
            if (prevState == 37)
            {
                oldpos = rdr.tellg();
                a = new char[2];
                rdr.read(a, 2);
                if (!(a[0] == 'i' && a[1] == 'n'))
                {
                    cout << "<" << "operator," << " " << "not" << ">" << endl;
                    rdr.seekg(oldpos);
                    s.clear();
                    continue;
                }
                else
                {
                    cout << "<" << "operator," << " " << "not in" << ">" << endl;
                    s.clear();
                    continue;
                }
            }
            cout << "<" << "operator," << " " << s << ">" << endl;
            s.clear();
            if (currState != 0)
                s.push_back(c);
        }
        else if (isIdentifierState(prevState,currState))
        {
            s.pop_back();
            if(isKeyword(s))
                cout << "<" << "keyword," << " " << s << ">" << endl;
            else
                cout << "<" << "identifier," << " " << s << ">" << endl;
            s.clear();          
            if (currState != 0)
                s.push_back(c);
        }
        else if (isIntState(prevState,currState))
        {
            s.pop_back();
            cout << "<" << "int," << " " << s << ">" << endl;
            s.clear();
            if (currState != 0)
                s.push_back(c);
        }
        else if (isFloatState(prevState,currState))
        {
            s.pop_back();
            cout << "<" << "float," << " " << s << ">" << endl;
            s.clear();
            if (currState != 0)
                s.push_back(c);
        }
        else if (isCommentState(prevState,currState))
        {
            s.pop_back();
            cout << "<" << "comment," << " " << s << ">" << endl;
            s.clear();
        }
        else if (isSingleStringState(prevState,currState))
        {
            s.pop_back();
            cout << "<" << "Single line string," << " " << s << ">" << endl;
            s.clear();
            if (currState != 0)
                s.push_back(c);
        }
        else if (isMultiStringState(prevState,currState))
        {
            s.pop_back();
            cout << "<" << "Multi line string," << " " << s << ">" << endl;
            s.clear();
            if (currState != 0)
                s.push_back(c);
        }
        


    }while (!rdr.eof());
   
}
int main1()
{
    ifstream rdr("TT2.csv");
    int TT[rows][cols];
    populateTT(rdr,TT);
    ifstream code("Text.txt");
    Lexer(code, TT);
    return 0;
}