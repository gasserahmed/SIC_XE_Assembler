#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
using namespace std;

//Main Functions//
void LoadMap();
void WriteSYMTAB();
void ReadInput();
void ProccessData();
void WriteOutput();

//Helper Functions//
void formatter();
void format_2();
void format_3();
void resw_resb();
void byte_word();
void check_start();
void check_label();
void check_rsub();
void the_end();

string capitalizer(string input);
int to_int(string input);
int to_hexa(string input);


typedef struct Data
{
    int address;
    bool error;
    string label;
    string opcode;
    string operand;
} Data;

Data data_line;

typedef struct Values
{
    int format;
    int opcode;
} Values;


map <string,int> SYMTAB;
map <string,Values> OPTAB;
map <string,Values>::iterator it=OPTAB.begin();

string last_line;
string error_statement;
int start_flag=0;
int end_flag=0;
int loccounter=0;
int length=0;
int before_start=0;
int flag_format4=0;
int total_error=0;

int main()
{
    LoadMap();
    ReadInput();
    the_end();
    ofstream of("LISFILE.txt",ios::app);
    cout<<"--- There is/are "<<total_error<<" line(s) having an error ---\n\t---------------------------------"<<endl;
    of<<"\n--- There is/are "<<total_error<<" line(s) having an error ---\n\t---------------------------------"<<endl;
    if(total_error==0)
    {

        cout<<"\n--- S u c c e s s f u l  A s s e m b l y ---\n\t---------------------------------"<<endl;
        of<<"\n---  S u c c e s s f u l  A s s e m b l y  ---\n\t---------------------------------"<<endl;
    }
    else
    {
        cout<<"\n--- U n s u c c e s s f u l  A s s e m b l y ---\n\t---------------------------------"<<endl;
        of<<"\n--- U n S u c c e s s f u l  A s s e m b l y ---\n\t---------------------------------"<<endl;
    }
    return 0;

}

void LoadMap()
{
    OPTAB["START"]= {0,-1};
    OPTAB["END"]= {0,-1};
    OPTAB["RESW"]= {3,-1};
    OPTAB["RESB"]= {1,-1};
    OPTAB["BYTE"]= {1,-1};
    OPTAB["WORD"]= {3,-1};

    OPTAB["TIXR"]= {2,184};
    OPTAB["ADDR"]= {2,144};
    OPTAB["COMPR"]= {2,160};
    OPTAB["DIVR"]= {2,156};
    OPTAB["MULR"]= {2,96};
    OPTAB["RMO"]= {2,172};
    OPTAB["SUBR"]= {2,148};

    OPTAB["ADD"]= {3,24};
    OPTAB["COMP"]= {3,40};
    OPTAB["DIV"]= {3,36};
    OPTAB["J"]= {3,60};
    OPTAB["JEQ"]= {3,48};
    OPTAB["JGT"]= {3,52};
    OPTAB["JLT"]= {3,56};
    OPTAB["JSUB"]= {3,72};
    OPTAB["LDA"]= {3,0};
    OPTAB["LDB"]= {3,104};
    OPTAB["LDCH"]= {3,80};
    OPTAB["LDF"]= {3,112};
    OPTAB["LDL"]= {3,8};
    OPTAB["LDS"]= {3,108};
    OPTAB["LDT"]= {3,116};
    OPTAB["LDX"]= {3,4};
    OPTAB["MUL"]= {3,32};
    OPTAB["RSUB"]= {3,76};
    OPTAB["STA"]= {3,12};
    OPTAB["STB"]= {3,120};
    OPTAB["STCH"]= {3,54};
    OPTAB["STF"]= {3,128};
    OPTAB["STL"]= {3,20};
    OPTAB["STS"]= {3,124};
    OPTAB["STT"]= {3,132};
    OPTAB["STX"]= {3,16};
    OPTAB["SUB"]= {3,28};
    OPTAB["TIX"]= {3,44};

    return;
}

void the_end()
{
    if(capitalizer(last_line)=="END")
    {
        if(end_flag>1)
        {
            total_error++;
            cout<<"*** There are more than one END statement before the last END"<<endl;
            return;
        }

        if(end_flag==1)
            return;
    }

    else
    {
        total_error++;
        if(end_flag>0)
        {
            cout<<"*** There are more than one END statement before the last END"<<endl;
            cout<<"*** There must be an END statement at the END of file"<<endl;
            return;
        }

        if(end_flag==0)
        {
            cout<<"*** There must be an END statement at the END of file"<<endl;
            return;
        }
    }
}



void check_rsub()
{
    if(capitalizer(data_line.label)=="RSUB")
    {
        data_line.opcode=data_line.label;
        data_line.label.clear();
        loccounter+=length;
        length=3;

        return;
    }



    if(capitalizer(data_line.operand)=="RSUB")
    {
        data_line.label=data_line.opcode;
        data_line.opcode=data_line.operand;
        data_line.operand.clear();
        loccounter+=length;
        length=3;
        return;
    }


}




void check_label()
{

    map <string,int>::iterator lab=SYMTAB.begin();
    lab=SYMTAB.find(capitalizer(data_line.label));

    if(!(data_line.label.empty()))
    {
        if(lab!=SYMTAB.end())
        {
            data_line.error=true;
            error_statement+="*** Duplicated label, only the first occurrence will be considered";
            return;
        }
        else
        {

            SYMTAB[capitalizer(data_line.label)]=loccounter;

        }
    }
}

void check_start()
{
    if(start_flag==0 && before_start==0)
    {
        int x=to_hexa(data_line.operand);
        if(x==-1)
        {
            data_line.error=true;
            error_statement+=" *** Invalid operand";
            start_flag=1;
            loccounter=0;
            length=0;
            return;
        }
        else
        {
            loccounter=x;
            start_flag=1;
            data_line.error=false;
            return;
        }
    }

    else
    {
        data_line.error=true;
        error_statement+=" *** Duplicated or Misplaced START statement";
        return;
    }
}

void byte_word()
{

    if(capitalizer(data_line.opcode)=="WORD")
    {

        if(data_line.operand.empty())
        {
            data_line.error=true;
            error_statement+="*** Missing or Misplaced operand";
            loccounter=loccounter+length;
            length=0;
            return;
        }

        if(to_int(data_line.operand)!=-1)
        {
            loccounter=loccounter+length;
            length=3;
            return;
        }

        else
        {
            string r1;

            stringstream stream_str(data_line.operand);
            int length2=length;
            length=0;
            while(getline(stream_str,r1,','))
            {

                if(to_int(r1)!=-1)
                    length=length+3;
                else
                {
                    data_line.error=true;
                    loccounter=loccounter+length2;
                    length=0;
                    error_statement+="*** Missing or Misplaced operand";
                    return;
                }
            }
        }
    }
    if(capitalizer(data_line.opcode)=="BYTE")
    {
        if(data_line.operand.empty())
        {
            data_line.error=true;
            error_statement+="*** Missing or Misplaced operand";
            loccounter=loccounter+length;
            length=0;
            return;
        }

        if(capitalizer(data_line.operand)=="C" || capitalizer(data_line.operand)=="C''" || capitalizer(data_line.operand)=="X" || capitalizer(data_line.operand)=="x''")
        {
            loccounter=loccounter+length;
            length=0;
            return;

        }

        string temp=capitalizer(data_line.operand);
        string temp2="'";

        if(temp[0]=='C' && temp[1]==temp2[0] && temp[data_line.operand.length()-1]!=temp2[0] )
        {
            data_line.error=true;
            error_statement+=" *** Character string is too long";
            loccounter=loccounter+length;
            length=0;
            return;
        }

        if(temp[0]=='C' && temp[1]==temp2[0] && temp[data_line.operand.length()-1]==temp2[0] )
        {
            loccounter=loccounter+length;
            length=data_line.operand.length()-3;
            return;
        }

        if(temp[0]=='x' && temp[1]==temp2[0] && temp[data_line.operand.length()-1]!=temp2[0] )
        {
            data_line.error=true;
            error_statement+=" *** Character string is too long";
            loccounter=loccounter+length;
            length=0;
            return;
        }

        if(temp[0]=='X' && temp[1]==temp2[0] && temp[data_line.operand.length()-1]==temp2[0] )
        {
            int l=temp.length()-3;

            string hexa=temp.substr (2,l);

            int flag=0;
            if(to_hexa(hexa)==-1)
            {
                data_line.error=true;
                error_statement+=" *** Not a hexadecimal string" ;
                loccounter=loccounter+length;
                length=0;
                flag=1;
            }

            if((l%2)!=0)
            {
                data_line.error=true;
                error_statement+=" *** Hex string must be of even length" ;
                loccounter=loccounter+length;
                length=0;
                return;
            }

            if(flag==1)
                return;

            loccounter=loccounter+length;
            length=data_line.operand.length()-3;
            length=length/2;
            return;
        }

        data_line.error=true;
        error_statement+="*** Invalid operand";
        loccounter=loccounter+length;
        length=0;
        return;
    }
}

void resw_resb()
{
    if(data_line.operand.empty() || to_int(data_line.operand)==-1 )
    {
        data_line.error=true;
        if(data_line.operand.empty())
            error_statement+="*** Missing or Misplaced operand field";
        else
            error_statement+="*** Invalid operand";
        loccounter=loccounter+length;
        length=0;
        return;
    }

    loccounter=loccounter+length;

    length=to_int(data_line.operand);

    if(capitalizer(data_line.opcode)=="RESW")
        length=length*3;
    return;
}


void format_2()
{
    if(flag_format4==1)
    {
        data_line.error=true;
        loccounter=loccounter+length;
        length=0;
        error_statement+=" *** Can not be a format 4 instruction";
    }
    else
    {
        loccounter=loccounter+length;
        length=it->second.format;
    }

    string r1,r2;
    stringstream stream_str(data_line.operand);
    getline(stream_str,r1,',');
    getline(stream_str,r2,',');
    r1=capitalizer(r1);
    r2=capitalizer(r2);

    if(r1.empty())
    {
        data_line.error=true;
        error_statement+=" *** Inavlid address for a register (missing registers here)\n";
        return;
    }

    if(r1!="A" && r1!="S" && r1!="T"  && r1!="W" && r1!="X" )
    {

        data_line.error=true;
        error_statement+=" *** Invalid address for register r1 (no register called): ";
        error_statement=error_statement+r1;
        error_statement+="\n";

    }

    if(r2.empty())
    {
        data_line.error=true;
        error_statement+=" *** Missing comma in operand field (r2 is missing)";
        return;
    }

    if(r2!="A" && r2!="S" && r2!="T"  && r2!="W" && r2!="X" )
    {
        data_line.error=true;
        error_statement+=" *** Invalid address for register r2 (no register called): ";
        error_statement=error_statement+r2;
        error_statement+="\n";
        return;
    }
}

void format_3()
{
    if(flag_format4==1)
    {
        string plus="+";
        plus=plus+data_line.opcode;
        data_line.opcode=plus;
        data_line.error=false;
        loccounter=loccounter+length;
        length=it->second.format;
        length++;
        return;
    }

    else
    {
        data_line.error=false;
        loccounter=loccounter+length;
        length=it->second.format;
        return;
    }
}


void formatter()
{
    int x=it->second.format;

    if(end_flag>0)
    {
        data_line.error=true;
        before_start++;
        error_statement+=" *** Nothing should follow the END statement";
        loccounter=loccounter+length;
        length=0;
    }




    if(capitalizer(data_line.opcode)=="END")
    {
        end_flag++;
        loccounter+=length;
        length=0;
        return;
    }

    if(capitalizer(data_line.opcode)=="START")
    {
        check_start();
        return;
    }

    before_start++;


    if(capitalizer(data_line.label)=="RSUB" || capitalizer(data_line.operand)=="RSUB")
    {
        check_rsub();
        return;
    }

    if(capitalizer(data_line.opcode)=="RESW"  || capitalizer(data_line.opcode)=="RESB")
    {
        resw_resb();
        return;
    }

    if(capitalizer(data_line.opcode)=="WORD"  || capitalizer(data_line.opcode)=="BYTE")
    {
        byte_word();
        return;
    }

    if(x==2)
    {
        format_2();
        return;
    }

    if(x==3)
    {
        format_3();
        return;
    }
}



void ProccessData()
{
    //Starter//

    flag_format4=0;
    data_line.error=false;
    error_statement.clear();
    last_line=data_line.opcode;

    if(data_line.opcode[0]=='+')
    {
        flag_format4=1;
        data_line.opcode.erase(0,1);
    }

    it=OPTAB.find(capitalizer(data_line.opcode));

    //Comment//
    if(data_line.label[0]=='.')
    {
        WriteOutput();
        return;
    }

    //Valid//
    if(it!=OPTAB.end() || capitalizer(capitalizer(data_line.label))=="RSUB" || capitalizer(capitalizer(data_line.operand))=="RSUB")
    {
        formatter();
        check_label();
        WriteOutput();
        return;
    }

    else
    {
        data_line.error=true;
        before_start++;
        error_statement+=" *** Invalid operation code";
        loccounter=loccounter+length;
        length=0;
        check_label();
        WriteOutput();
        return;
    }
}

string capitalizer(string input)
{
    for (int i=0; i<input.length(); i++)
        input[i]=toupper(input[i]);

    return input;
}

int to_int(string input)
{
    for(int counter=0; counter<input.length(); counter++)

        if(isdigit(input[counter])==0)
            return -1;



    int x;

    stringstream ss;
    ss<<input;
    ss>>x;

    return x;
}

int to_hexa(string input)
{
    for(int counter=0; counter<input.length(); counter++)

        if(isxdigit(input[counter])==0)
            return -1;

    int x;

    stringstream ss;
    ss<<hex<<input;
    ss>>x;

    return x;
}

void ReadInput()
{
    ifstream FILE("srcfile.txt");
    ofstream nof("LISFILE.txt");
    char c;
    string label_str,opcode_str,operand_str,str;
    ofstream of("LISFILE.txt",ios::app);
    of<<"\t----------------------------------\n\t\t  Program List\n\t\t  ------------\nAddress      Label    OpCode  Operand\n-------      -----    ------  -------"<<endl;


    while(!(FILE.eof()))
    {
        c=FILE.get();

        while(c!='\n')
        {
            if(c==' ')
            {
                c=FILE.get();
                continue;
            }

            if(c=='.')
            {
                FILE.unget();
                getline(FILE,data_line.label,'\n');
                data_line.opcode.clear();
                data_line.operand.clear();
            }

            else
            {
                FILE.unget();

                label_str.clear();
                opcode_str.clear();
                operand_str.clear();

                getline(FILE,str,'\n');
                stringstream stream_str(str);
                stream_str>>label_str;
                stream_str>>opcode_str;
                stream_str>>operand_str;
                stream_str.clear();

                if(operand_str.empty())
                {
                    data_line.label.clear();
                    data_line.opcode = label_str;
                    data_line.operand = opcode_str;
                }
                else if(opcode_str.empty() && operand_str.empty())
                {
                    data_line.label.clear();
                    data_line.opcode = label_str;
                    data_line.operand.clear();
                }
                else
                {
                    data_line.label = label_str;
                    data_line.opcode = opcode_str;
                    data_line.operand = operand_str;
                }

            }
            ProccessData();
            break;
        }

    }
    of<<"\n\t----------------------------------\n\t\t  Symbol Table\n\t\t  ------------\n\t\tLabel\t   Address\n\t\t-----\t   -------"<<endl;
    WriteSYMTAB();
    of<<"\t----------------------------------";
    return;
}

void WriteOutput()
{
    if(data_line.error==true)
        total_error++;

    ofstream of("LISFILE.txt",ios::app);

    if(data_line.label[0]=='.')
    {
        cout<<"\t    "<<data_line.label<<endl;
        of<<"\t    "<<data_line.label<<endl;
        return;
    }

    if(data_line.error==true)
    {
        cout<<error_statement<<endl;
        of<<error_statement<<endl;
    }

    cout<<setfill('0')<<setw(5)<<right<<hex<<uppercase<<loccounter<<"        ";
    cout<<setfill(' ')<<setw(9)<<left<<data_line.label<<setfill(' ')<<setw(8)<<data_line.opcode<<data_line.operand<<endl;
    of<<setfill('0')<<setw(5)<<right<<hex<<uppercase<<loccounter<<"        ";
    of<<setfill(' ')<<setw(9)<<left<<data_line.label<<setfill(' ')<<setw(8)<<data_line.opcode<<data_line.operand<<endl;
    return ;
}

void WriteSYMTAB()
{
    ofstream of("LISFILE.txt",ios::app);
    typedef map<string,int>::iterator MapIterator;
    for (MapIterator iter =SYMTAB.begin(); iter != SYMTAB.end(); iter++)
    {
        cout<<"\t\t"<<setfill(' ')<<setw(11)<<left<<iter->first<<setfill('0')<<setw(5)<<right<<iter->second<<endl;
        of<<"\t\t"<<setfill(' ')<<setw(11)<<left<<iter->first<<setfill('0')<<setw(5)<<right<<hex<<uppercase<<iter->second<<endl;

    }

}
