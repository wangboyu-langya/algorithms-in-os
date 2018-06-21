#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;


class token{
public:
    string sequence;
    int line;
    int offset;

    //record the token in usedList used in E, yes 1
    int    usedInE=0;

    token(string s,int l,int o){
        sequence=s;
        line=l;
        offset=o;
    }
};

class symbol{
public:
  string name;
  int    nameLine;
  int    nameOffset;

  int    relAddr;
  int    relAddr0;//the value of relAddr before changing
  int    relAddrLine;
  int    relAddrOffset;

  int    multiDef;//1:multidef
  int    exceedModule;//1:relative address exceed module size
  int    moduleN;
  int    moduleSize;

  int    defNoUse=1;//1 defined but not used

  symbol(string n,int rA){
        name=n;
        relAddr0=rA;
        relAddr=rA;
  }
  symbolPosition(int nL,int nO,int rAL,int rAO){

      nameLine=nL;
      nameOffset=nO;

      relAddrLine=rAL;
      relAddrOffset=rAO;
  }

  int checkSymboMultiDef(vector<symbol> sv){
      for(int i=0;i<sv.size();i++){
            if (name==sv[i].name){
                return i;//multi def
            }
        }
        multiDef=0;
        return -1;//no multi def
      }

   void checkReAddr(int n,int modN){
        //relAddress is bigger than the size of module
        if (relAddr>n) {
                exceedModule=1;
                relAddr=0;
                moduleN=modN;
                moduleSize=n;
        }
        else {exceedModule=0;}
    }
};

class instruction{
public:
   string type;
   int    typeLine;
   int    typeOffset;

   int    op;
   int    opLine;
   int    opOffset;

   instruction(string t,int o){
        type=t;
        op=o;
   }

   instructionPosition(int tL,int tO,int oL,int oO){
      typeLine=tL;
      typeOffset=tO;

      opLine=oL;
      opOffset=oO;
  }
};

class module{
public:
  vector<symbol> defList;
  vector<token> useList;
  vector<instruction> instructionList;

  int moduleN;
  int moduleBase;
};


static map<string,int> table;

void buildTable(vector<symbol> sv, int base){
    string name;
    int reAddr;
    int absAddr;

    if(sv.size()>0){
        for(int i=0;i<sv.size();i++){
            name=sv[i].name;
            reAddr=sv[i].relAddr;
            absAddr=reAddr+base;
            table.insert(std::pair<string,int>(name,absAddr));
        }
    }
}

void printTable(vector<symbol> sv){

    for(int i=0;i<sv.size();i++){

    //relative address exceed module size
        if (sv[i].exceedModule==1){
            cout<<"Warning: Module "<<sv[i].moduleN<<": "<<sv[i].name<<" too big "<<sv[i].relAddr0<<" (max="<<sv[i].moduleSize-1<<") assume zero relative"<<endl;
        }
    }
    cout<<"Symbol Table"<<endl;

    for(int i=0;i<sv.size();i++){
    //Multiple definition
        if(sv[i].multiDef==1){
            cout<<sv[i].name<<"="<<table[sv[i].name];
            cout<<" Error: This variable is multiple times defined; first value used"<<endl;
            }
        else {cout<<sv[i].name<<"="<<table[sv[i].name]<<endl;}
        }
}



void createTokenVector(string b,int l,int o,int j,vector<token> &tV){

    string t=b.substr(o-1,j-o+1);
    token to=token(t,l,o);
    tV.push_back(to);
}

void parseLine(string buffer,int lineN,vector<token> &tV){

    int sOffset=0;
    for (int offset=0;offset<buffer.length();offset++){

        if (buffer.at(offset)==' '||buffer.at(offset)=='\t'){}
        else {
                sOffset=offset+1;
                for (int j=sOffset;j<=buffer.length();j++){
                    if (j==buffer.length()){
                        createTokenVector(buffer,lineN,sOffset,j,tV);
                        break;
                    }
                    if(buffer.at(j)==' '||buffer.at(j)=='\t'){
                        createTokenVector(buffer,lineN,sOffset,j,tV);
                        break;
                    }
                    offset++;
                }
        }
    }

}


void _parseError(int ecode, int i,vector<token> &tV) {

    int lineN=tV[i].line;
	int offset=tV[i].offset;
	static string errstr[] = {
		"NUM_EXPECTED",		// 0 number expected
		"SYM_EXPECTED",		// 1 symbol expected
		"ADDR_EXPECTED", 	// 2 addressing expected
		"SYM_TOLONG", 		// 3 symbol name is too long
		"TOO_MANY_DEF_IN_MODULE",	// 4 >16
		"TOO_MANY_USE_IN_MODULE",	// 5 >16
		"TOO_MANY_INSTR"		// 6 total num_instr exceeds 512
	};
	cout <<"Parse Error line "<<lineN<<" offset "<<offset<< ": "<<errstr[ecode]<<endl;
}



bool isNumber(string s){
    stringstream sin(s);
     double d;
     char c;
     if(!(sin >> d))
     {
         return false;
     }
     else if (sin >> c)
     {
         return false;
     }
     else { return true;}
}

string readSymbol(int i,vector<token> &tV){
    string buffer;
    buffer=tV[i].sequence;

    if(buffer==""){
        _parseError(1,i,tV);exit(1);
    }

    //do not begin with alpha
    if(!isalpha(buffer.at(0))) { _parseError(1,i,tV);exit(1);}
    //symbol is greater than 16 characters
    if (buffer.length()>16) {_parseError(3,i,tV);exit(1);}

    else return buffer;
}

int readInt(int i,vector<token> &tV){

    string buffer;
    buffer=tV[i].sequence;
    //unexpected token (not read int from next list)
    if(buffer==""){
        _parseError(0,i,tV);exit(1);
    }
    if(isNumber(buffer)){
        int j;
        j=atoi(buffer.c_str());
        return j;
    }
    else {
        _parseError(0,i,tV);
        exit(1);
    }
}

string readInstruction(int i,vector<token> &tV){
    string buffer;
    buffer=tV[i].sequence;

    if(buffer==""){
        _parseError(2,i,tV);
        exit(1);
    }

    //missing token (read int from next list)
    if(buffer!="I"&&buffer!="A"&&buffer!="R"&&buffer!="E") {
            _parseError(2,i,tV);
            exit(1);
    }
    return buffer;
}

int readOp(int i,vector<token> &tV){
    string buffer;
    buffer=tV[i].sequence;
    //unexpected token (not read int from next list)
    if(buffer==""){
        _parseError(2,i,tV);
        exit(1);
    }
    if (isNumber(buffer)){
        int j;
        j=atoi(buffer.c_str());
        return j;
    }
    else {_parseError(2,i,tV);exit(1);}
}


void checkDN(int n,int i,vector<token> &tV){
    if (n>16){
        _parseError(4,i,tV);exit(1);
    }
}

void checkUN(int n,int i,vector<token> &tV){
    if (n>16){
        _parseError(5,i,tV);exit(1);
    }
}

void checkIN(int n,int mB,int i,vector<token> &tV){
    if ((n+mB)>512){
        _parseError(6,i,tV);exit(1);
    }
}


void pass1(char* filetest){
//void pass1(){
    //load the file
    ifstream Text;
    Text.open(filetest);
  //   Text.open("C:\\Users\\Administrator\\Desktop\\lab 1\\lab1samples\\lab1samples\\input-1");
  //   Text.open("C:\\Users\\Administrator\\Desktop\\lab 1\\test2.txt");

    //read text line by line and make the text into an tokenVector
    vector<token> tokenVector;
    string buffer;
    int lineN=1;
    int line1, line2;
    string lastLine;
    int finalC=0;

    while(!Text.eof()){
        getline(Text,buffer);
        if(buffer==""){
     //           line1=lineN;
                finalC++;
                lineN++;
        }
        else{
            line2=lineN;
            lastLine=buffer;
            parseLine(buffer,lineN,tokenVector);
            lineN++;
            finalC=0;
        }
    }
    //if the last line is \n, add an token
    if(finalC==1){
            token t("",line2,lastLine.size()+1);
            tokenVector.push_back(t);
        }
    if(finalC>1){
            token t("",line2+finalC-1,1);
            tokenVector.push_back(t);
    }

    if((line1>line2)&&(finalC==0)){
        token t("",line2,lastLine.size()+1);
        tokenVector.push_back(t);
    }



    //read file module by module
    int mBase=0;
    int moduleN=0;
    vector<symbol> defSymV;
    //get the token one by one
    for(int i=0;i<tokenVector.size();i++){
        moduleN++;
        if(i==(tokenVector.size()-1)){break;}
    //get defSymbol
        int defN=readInt(i,tokenVector);
        checkDN(defN,i,tokenVector);
        for (int j=0;j<defN;j++){
            i++;
            string n=readSymbol(i,tokenVector);

            i++;
            int value=readInt(i,tokenVector);
            symbol s(n,value);
        //check for multiple definitions
            int position=s.checkSymboMultiDef(defSymV);
            if (position<0){
                    defSymV.push_back(s);
            }
            else {
                    defSymV[position].multiDef=1;//multi def
            }
        }

        if(i==(tokenVector.size()-2)){break;}

    //passUseList
        i++;
        int N=readInt(i,tokenVector);
        checkUN(N,i,tokenVector);
        for (int j=0;j<N;j++){
            i++;
            string n=readSymbol(i,tokenVector);
        }

         if(i==(tokenVector.size()-2)){break;}
    //get the number of instructions (mBase) and pass the instructionList
        i++;
        N=readInt(i,tokenVector);
        checkIN(N,mBase,i,tokenVector);
        for (int j=0;j<N;j++){
            i++;
            string n=readInstruction(i,tokenVector);
            i++;
            int value=readOp(i,tokenVector);
        }

    //if there is new definition,check if the relative address exceed the size of the module
        if(defN>0){
            defSymV[defSymV.size()-1].checkReAddr(N,moduleN);
        }
    //insert new symbol into table
        buildTable(defSymV,mBase);
    //update base
        mBase=mBase+N;
    }
    printTable(defSymV);
    Text.close();
}


//warning:defined and in uselist but not used
void warningR7(vector<module> mo, int i){
            for (int j=0;j<mo[i].useList.size();j++){
                if (mo[i].useList[j].usedInE!=1){//&&table.count(mo[i].useList[j].sequence)!=0){
                cout<<"Warning: Module "<<mo[i].moduleN<<": "<<mo[i].useList[j].sequence<<" appeared in the uselist but was not actually used"<<endl;
                }
            }
}

//defined symbol used
void warningR4(vector<module> &mo){
    cout<<endl;
    for (int i=0;i<mo.size();i++){
        for (int j=0;j<mo[i].defList.size();j++){
            for (int m=0;m<mo.size();m++){
                for(int u=0;u<mo[m].useList.size();u++){
                    if(mo[i].defList[j].name==mo[m].useList[u].sequence&&mo[m].useList[u].usedInE==1){
                        mo[i].defList[j].defNoUse=0;
                    }
                }
            }
        }
    }
    for (int i=0;i<mo.size();i++){
        for (int j=0;j<mo[i].defList.size();j++){
            if(mo[i].defList[j].defNoUse==1){
            cout<<"Warning: Module "<<mo[i].moduleN<<": "<<mo[i].defList[j].name<<" was defined but never used"<<endl;
            }


        }
    }
   cout<<endl;
}


void buildMap(vector<module> &mo){

    int mapLine=0;
    string instructionT;

    cout<<endl;
    cout<<"Memory Map"<<endl;


    //get mapVaule for each instruction module by module
    for(int i=0;i<mo.size();i++){

            for (int j=0;j<mo[i].instructionList.size();j++){
                instructionT=mo[i].instructionList[j].type;

                if(instructionT=="I"){
                    //op unchanged
                    int op=mo[i].instructionList[j].op;

                    //rule 10;if I value>10000
                    if(op>10000){
                        cout.fill('0');//填充字符0
                        cout<<setw(3)<<mapLine;
                        cout<<": 9999 Error: Illegal immediate value; treated as 9999"<<endl;
                    }
                    else{
                        int opc=op/1000;
                        int opr=op%1000;

                        cout.fill('0');//填充字符0
                        cout<<setw(3)<<mapLine;
                        cout<<": "<<opc;
                        cout.fill('0');
                        cout<<setw(3)<<opr<<endl;
                    }
                }

                if(instructionT=="A"){
                    int op=mo[i].instructionList[j].op;
                    int opc=op/1000;
                    int opr=op%1000;

                    //op unchanged
                    //rule8: if absolute address exceed the size of machine,use zero,report error
                    if (opr>512){
                        opr=0;
                        cout.fill('0');//填充字符0
                        cout<<setw(3)<<mapLine;
                        cout<<": "<<opc;
                        cout.fill('0');
                        cout<<setw(3)<<opr;
                        cout<<" Error: Absolute address exceeds machine size; zero used"<<endl;
                    }

                    else{
                        cout.fill('0');//填充字符0
                        cout<<setw(3)<<mapLine;
                        cout<<": "<<opc;
                        cout.fill('0');
                        cout<<setw(3)<<opr<<endl;
                    }
                }
                if(instructionT=="R"){

                    //op + module base
                    int op=mo[i].instructionList[j].op;

                    if(op>10000){
                        cout.fill('0');//填充字符0
                        cout<<setw(3)<<mapLine;
                        cout<<": 9999 Error: Illegal opcode; treated as 9999"<<endl;
                    }
                    else{
                        int opc=op/1000;
                        int opr=op%1000;

                        if (opr>mo[i].instructionList.size()){
                            opr=0;
                            opr=opr+mo[i].moduleBase;
                            cout.fill('0');//填充字符0
                            cout<<setw(3)<<mapLine;
                            cout<<": "<<opc;
                            cout.fill('0');
                            cout<<setw(3)<<opr<<" Error: Relative address exceeds module size; zero used"<<endl;
                    }
                    else {
                            opr=opr+mo[i].moduleBase;
                            cout.fill('0');//填充字符0
                            cout<<setw(3)<<mapLine;
                            cout<<": "<<opc;
                            cout.fill('0');
                            cout<<setw(3)<<opr<<endl;
                            }
                    }
                }
                if(instructionT=="E"){
                    //get operand to get referenced symbol
                    int op=mo[i].instructionList[j].op;
                    int opc=op/1000;
                    int opr=op%1000;

                    if (opr>=mo[i].useList.size()){

                            cout.fill('0');//填充字符0
                            cout<<setw(3)<<mapLine;
                            cout<<": "<<op;
                            cout<<" Error: External address exceeds length of uselist; treated as immediate"<<endl;
                    }

                    else{

                        string refSymbol=mo[i].useList[opr].sequence;

                        //symbol appeared but not defined
                        if(table.count(refSymbol)==0){
                            opr=0;
                            mo[i].useList[opr].usedInE=1;
                            cout.fill('0');//填充字符0
                            cout<<setw(3)<<mapLine<<": ";
                            cout<<opc;
                            cout.fill('0');//填充字符0
                            cout<< setw(3)<<opr;
                            cout<<" Error: "<<refSymbol<<" is not defined; zero used"<<endl;
                        }

                        else{//defined

                            //defined and used, record the symbol in useList referenced by E, usedInE=1, the symbol is used
                            for(int p=0;p<mo[i].useList.size();p++){
                                if (p==opr) {
                                    mo[i].useList[p].usedInE=1;
                                }
                            }
                            //opr = opr+table value of referenced symbol; opcode unchanged;
                            opr=table[refSymbol];

                            cout.fill('0');//填充字符0
                            cout<<setw(3)<<mapLine;
                            cout<<": "<<opc;
                            cout.fill('0');//填充字符0
                            cout << setw(3) << opr << endl;
                        }
                    }

                }
                mapLine++;
            }
         //final warning reported module by module rule 7, in useList not used
        warningR7(mo,i);
    }

}

//void pass2(){
void pass2(char* filetest){
    //load the file
    ifstream Text;
    Text.open(filetest);
 //   Text.open("C:\\Users\\Administrator\\Desktop\\lab 1\\lab1samples\\lab1samples\\input-1");
//    Text.open("C:\\Users\\Administrator\\Desktop\\lab 1\\test2.txt");

    //read text line by line and make the text into an tokenVector
    vector<token> tokenVector;
    string buffer;
    int lineN=1;
    int line1, line2;
    string lastLine;
    int finalC=0;

    while(!Text.eof()){
        getline(Text,buffer);
        if(buffer==""){
     //           line1=lineN;
                finalC++;
                lineN++;
        }
        else{
            line2=lineN;
            lastLine=buffer;
            parseLine(buffer,lineN,tokenVector);
            lineN++;
            finalC=0;
        }
    }
    //if the last line is \n, add an token
    if(finalC==1){
            token t("",line2,lastLine.size()+1);
            tokenVector.push_back(t);
        }
    if(finalC>1){
            token t("",line2+finalC-1,1);
            tokenVector.push_back(t);
    }

    if((line1>line2)&&(finalC==0)){
        token t("",line2,lastLine.size()+1);
        tokenVector.push_back(t);
    }

    //read file module by module
    vector<module> moVector;
    int moduleN=0;//module number
    int mBase=0;//module base

    //get the token one by one
    for(int i=0;i<tokenVector.size();i++){

        if(i==(tokenVector.size()-1)){break;}
        //create module
        module mo;
        moduleN++;
        mo.moduleN=moduleN;
        mo.moduleBase=mBase;


        //Set definition List
        int defN=readInt(i,tokenVector);
        for (int j=0;j<defN;j++){
            i++;
            string name=readSymbol(i,tokenVector);
            int    nL=tokenVector[i].line;
            int    nO=tokenVector[i].offset;
            i++;
            int    relA=readInt(i,tokenVector);

            int    relAL=tokenVector[i].line;
            int    relAO=tokenVector[i].offset;

            symbol s(name,relA);

            //if not exist, put into mo.defList
            int position=s.checkSymboMultiDef(mo.defList);
            if (position<0){
                s.symbolPosition(nL,nO,relAL,relAO);
                mo.defList.push_back(s);
            }

        }

        if(i==(tokenVector.size()-2)){break;}
        //set Use List
        i++;
        int N=readInt(i,tokenVector);
        for (int j=0;j<N;j++){
            i++;
            string name=readSymbol(i,tokenVector);
            int    nL=tokenVector[i].line;
            int    nO=tokenVector[i].offset;

            token t(name,nL,nO);
            mo.useList.push_back(t);
        }


        if(i==(tokenVector.size()-2)){break;}
        //set Instruction List
        i++;
        N=readInt(i,tokenVector);
        for (int j=0;j<N;j++){
            i++;
            string type=readInstruction(i,tokenVector);
            int    tL=tokenVector[i].line;
            int    tO=tokenVector[i].offset;

            i++;
            int    o=readOp(i,tokenVector);
            int    oL=tokenVector[i].line;
            int    oO=tokenVector[i].offset;

            //create instruction and put into mo.useList
            instruction ins(type,o);
            ins.instructionPosition(tL,tO,oL,oO);
            mo.instructionList.push_back(ins);
        }

        //updatate module base
        mBase=mBase+N;
        //if there is new definition,check if the relative address exceed the size of the module
        if(defN>0){
            mo.defList[mo.defList.size()-1].checkReAddr(N,moduleN);
        }

        moVector.push_back(mo);

    }

     buildMap(moVector);
     warningR4(moVector);
     Text.close();
}

 int main(int argc, char* argv[]){
     char* f=argv[1];
      pass1(f);
      pass2(f);

}
