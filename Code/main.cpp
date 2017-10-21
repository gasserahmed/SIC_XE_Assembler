#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

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

void LoadMap();
void ReadInput();
void ProccessData();
void WriteSYMTAB();
int check_first();
//void CheckLabel();
void print();
string capitalizer(string input);
int to_hexa(string input);
void WriteOutput();
map <string,int> SYMTAB;
map <string,Values> OPTAB;


int start_flag=0;
int end_flag=0;
int loccounter=0;
int length=0;
string error_statement;
int before_start=0;


int main()
{
    SYMTAB["label"]=0;
    SYMTAB["temp"]=0006;
//    map <string,int>::iterator sit=SYMTAB.begin();
//    sit=SYMTAB.find("label");
//    cout<<sit->first;
    LoadMap();
    ReadInput();
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
    OPTAB["ADD"]= {3,24};
    OPTAB["ADDR"]= {2,144};
    OPTAB["COMP"]= {3,40};
    OPTAB["COMPR"]= {2,160};
    OPTAB["DIV"]= {3,36};
    OPTAB["DIVR"]= {2,156};
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
    OPTAB["MULR"]= {2,96};
    OPTAB["RMO"]= {2,172};
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
    OPTAB["SUBR"]= {2,148};
    OPTAB["TIX"]= {3,44};
    OPTAB["TIXR"]= {2,184};

    return;
}




void ProccessData()
{
    map <string,Values>::iterator it=OPTAB.begin();
    int flag_format4=0;
    if(data_line.opcode[0]=='+')
    {
        flag_format4=1;
        data_line.opcode.erase(0,1);

    }
    it=OPTAB.find(capitalizer(data_line.opcode));

    if(data_line.label[0]=='.')
    {
        data_line.error=false;
        print();
        WriteOutput();
        return;
    }
    //////////////////comment//////////////
    /////////////////////////////////////

    if(it!=OPTAB.end())
    {

        if(capitalizer(data_line.opcode)=="START")
        {
            if(start_flag==0 && before_start==0)
            {


                int x=to_hexa(data_line.operand);
                if(x==-1)
                {
                    data_line.error=true;
                    error_statement=" ****** illegal operand";
                    start_flag=1;
                    loccounter=0;
                    print();
                    WriteOutput();
                    return;
                }
                else
                {
                    loccounter=x;
                    start_flag=1;
                    data_line.error=false;
                    print();
                    WriteOutput();
                    return;
                }
            }
            else
            {
                data_line.error=true;
                error_statement=" ****** duplicate or misplaced START statement ";
                print();
                WriteOutput();
                return;
            }



        }

        ///////////////start/////////////////
        /////////////////////////////////////

        else
        {
            if(flag_format4==0)
            {
                data_line.error=false;
                before_start++;
                loccounter=loccounter+length;
                length=it->second.format;
                print();
                WriteOutput();
                return;
            }


            if(flag_format4==1 && it->second.format==3 )
            {
                string plus="+";
                plus=plus+data_line.opcode;
                data_line.opcode=plus;

                data_line.error=false;
                before_start++;
                loccounter=loccounter+length;
                length=it->second.format;
                length++;
                print();
                WriteOutput();
                return;
            }


            if(flag_format4==1 && it->second.format<3)
            {
                string plus="+";
                plus=plus+data_line.opcode;
                data_line.opcode=plus;
                data_line.error=true;
                before_start++;
                loccounter=loccounter+length;
                length=0;
                error_statement=" ****** can not be format 4 instrcction ";
                print();
                WriteOutput();
                return;
            }





        }



    }
    ////////////////////////////////legal operation/////////
    ////////////////////////////////////////////////////////

    else
    {
        data_line.error=true;
        before_start++;
        error_statement=" ****** unrecognized operation code";
        loccounter=loccounter+length;
        length=0;
        print();
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



/*
int check_first()
{
    for(int counter=0;counter<20;counter++)
    {
        if(table[counter].label[0]=='.')
        continue;

        else
        {
            if(capitalizer(table[counter].opcode)=="START")
            return to_hexa(table[counter].operand);
            else
            return -1;
        }
    }
    return -1;
}

*/

int to_hexa(string input)
{
    for(int counter=0; counter<input.length(); counter++)
        if(isxdigit(input[counter])==0)
            return -1;

    int x;
    std::stringstream ss;
    ss << std::hex << input;
    ss >> x;

    return x;
}




void print()
{
    if(data_line.label[0]=='.')
    {
        cout<<"\t    "<<data_line.label<<endl;
        return;
    }

    if(data_line.error==true)
        cout<<error_statement<<endl;
    cout<<setfill('0')<<setw(5)<<right<<std::hex<<loccounter<<"        ";
    cout<<setfill(' ')<<setw(9)<<left<<data_line.label<<setfill(' ')<<setw(8)<<data_line.opcode<<data_line.operand<<endl;
    return ;
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
    ofstream of("LISFILE.txt",ios::app);
    if(data_line.label[0]=='.')
    {
        of<<"\t    "<<data_line.label<<endl;
        return;
    }
    //CheckLabel();
    if(data_line.error==true)
        of<<error_statement<<endl;
    of<<setfill('0')<<setw(5)<<right<<std::hex<<loccounter<<"        ";
    of<<setfill(' ')<<setw(9)<<left<<data_line.label<<setfill(' ')<<setw(8)<<data_line.opcode<<data_line.operand<<endl;
    return ;
}
void WriteSYMTAB()
{
    ofstream of("LISFILE.txt",ios::app);
    typedef map<string,int>::iterator MapIterator;
    for (MapIterator iter =SYMTAB.begin(); iter != SYMTAB.end(); iter++)
    {
        of<<"\t\t"<<setfill(' ')<<setw(11)<<left<<iter->first<<setfill('0')<<setw(5)<<right<<iter->second<<endl;

    }

}
//void CheckLabel()
//{
//    map <string,int>::iterator sit=SYMTAB.begin();
//
//    sit=SYMTAB.find(capitalizer(data_line.label));
//
//    if(data_line.label==sit->first)
//    {
//        data_line.error=true;
//        error_statement=" ****** Repeated Label ";
//        data_line.address=sit->second;
//    }
//    if(data_line.label[0]==' ')
//    {
//        data_line.error==false;
//    }
//    return;
//}

