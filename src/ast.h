#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <algorithm>
#include <assert.h>

extern FILE *yyout;
// 所有 AST 的基类
extern int tmpcnt;
extern int symcnt;
extern int ifcnt;
extern int whilecnt;
extern int bblockcnt;
extern int landcnt;
extern int lorcnt;
extern int fl_break_continue;
extern std::string cur_end;
extern std::string cur_entry;
extern std::map<std::string, std::variant<int, std::string>> sym_table;
extern std::map<std::string, std::variant<int, std::string>> var_table;
extern std::map<std::string, std::variant<int, std::string>> *curFunvar_table;
extern std::map<std::string, int> value_table;
extern std::map<std::string, std::string> funcTable;

extern std::map<std::string, std::variant<int, std::string>> *cur_table;
extern std::map<std::map<std::string, std::variant<int, std::string>>*, \
std::map<std::string, std::variant<int, std::string>>*> total_table;

extern std::map<std::string, int> *cur_array_dims_table;
extern std::map<std::map<std::string, int>*, std::map<std::string, int>*> total_array_dims_table;

enum TYPE{
  _UnaryExp, _PrimaryExp, _UnaryOp, _Number, _Exp, _OP, _AddExp, _MulExp, _RelExp, 
  _EqExp, _LAndExp, _LOrExp, _LE, _GE, _EQ, _NE, _AND, _OR, _LT, _GT, _Decl, 
  _ConstDecl, _ConstDef_dup, _BType, _ConstDef, _BlockItem_dup, _BlockItem,
  _LVal, _ConstExp, _ConstInitVal, _Stmt, _VarDecl, _VarDef_dup, _VarDef, 
  _InitVal, _Block, _ConstNum, _LValNum, _InitVal_dup, _ConstInitVal_dup, 
  _FuncNum, _FuncArrayNum,
};

class BaseAST {
 public:

  BaseAST() = default;
  BaseAST(TYPE t): type(t){}
  BaseAST(TYPE t, char o): type(t), op(o)
  {

  }
  virtual ~BaseAST() = default;
  virtual void Dump(std::string& str0) const = 0;
  virtual std::string retvaltmp(std::string& str0)
  {
    return "";
  }
  virtual std::string getElemPtrForLVal(std::string& str0, std::string& resident, bool isArrayFunParam, bool isRealArrayParam)
  {
    return "";
  }
  virtual std::vector<std::string> getInitValArray(std::vector<std::string>& resVec, std::vector<int>& dims, std::vector<int>& numsForDims, int allnum, std::string& str0)
  {
    
    return resVec;
  }
  virtual std::string typeForArrayFParam(int& totalDimsNum){
    return "";
  }
  std::vector<BaseAST *> son;
  TYPE type;
  int val;
  char op;
  bool isint = false;
  std::string func_type_all;
  bool isArray;
};


class ConstNumAST: public BaseAST {
  public:
  ConstNumAST(){
    type = _ConstNum;
  };
  std::vector<int> arrayVector;
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    return "";
  }
};



// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;
  
  void Dump(std::string& str0) const override {
    cur_table = &sym_table;

    std::map<std::string, std::variant<int, std::string>> global_var_table;
    curFunvar_table = &global_var_table;

    std::map<std::string, int> global_array_dims_table;
    cur_array_dims_table = &global_array_dims_table;

    str0 = "";
    //lv8-2
    str0 += "decl @getint(): i32\n";
    str0 += "decl @getch(): i32\n";
    str0 += "decl @getarray(*i32): i32\n";
    str0 += "decl @putint(i32)\n";
    str0 += "decl @putch(i32)\n";
    str0 += "decl @putarray(i32, *i32)\n";
    str0 += "decl @starttime()\n";
    str0 += "decl @stoptime()\n";

    funcTable["getint"] = "int";
    funcTable["getch"] = "int";
    funcTable["getarray"] = "int";
    funcTable["putint"] = "void";
    funcTable["putch"] = "void";
    funcTable["putarray"] = "void";
    funcTable["starttime"] = "void";
    funcTable["stoptime"] = "void";

    int sz = son.size();
    symcnt += 1;
    for(int i = 0; i < sz; i++){
      son[i]->Dump(str0);
    }
    symcnt -= 1;
  }
};



// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::string func_type_str;
  void Dump(std::string& str0) const override {
    str0 += "fun @";
    str0 += ident;
    funcTable[ident] = son[0]->func_type_all;
    str0 += "(";
    int sz = son.size();
    if(sz == 3){
      son[1]->Dump(str0);
    }
    str0 += ")";
   
    
    if(son[0]->func_type_all == "int"){
      str0 += ": i32";
    }else if(son[0]->func_type_all == "void"){
      str0 += "";
    }

    str0 += " { \n";

    str0 += "\%entry:\n";

    std::map<std::string, std::variant<int, std::string>> new_table;
    std::map<std::string, std::variant<int, std::string>> *tmp_table = cur_table;
    cur_table = &new_table;
    total_table[cur_table] =  tmp_table;

    
    std::map<std::string, int> new_array_dims_table;
    std::map<std::string, int> *tmp_array_dims_table = cur_array_dims_table;
    cur_array_dims_table = &new_array_dims_table;
    total_array_dims_table[cur_array_dims_table] =  tmp_array_dims_table;


    std::map<std::string, std::variant<int, std::string>> new_var_table;
    std::map<std::string, std::variant<int, std::string>> *tmp_var_table = curFunvar_table;
    curFunvar_table = &new_var_table;

    symcnt += 1;

    if(sz == 3){
      int numOfFParam = son[1]->son.size();
      
      for(int i = 0; i < numOfFParam; i++){
        if(son[1]->son[i]->isArray == false){
          std::string ident = son[1]->son[i]->retvaltmp(str0);
          std::string identIndex = "@"+ident+'_'+std::to_string(symcnt);
          std::string paramName = "@"+ident;
          str0 += " @"+ident+'_'+std::to_string(symcnt);
          str0 += " = alloc i32\n";
          str0 += " store @"+ident+", @"+ident+'_'+std::to_string(symcnt)+'\n';
          (*cur_table)[identIndex] = paramName;
        }else if(son[1]->son[i]->isArray == true){
          std::string ident = son[1]->son[i]->retvaltmp(str0);
          std::string identIndex = "@"+ident+'_'+std::to_string(symcnt);
          std::string paramName = "@"+ident;

          int totalDimsNum = 0;
          std::string typeForArray = son[1]->son[i]->typeForArrayFParam(totalDimsNum);

          str0 += " @"+ident+'_'+std::to_string(symcnt);
          str0 += " = alloc "+typeForArray+"\n";
          str0 += " store @"+ident+", @"+ident+'_'+std::to_string(symcnt)+'\n';
          (*cur_table)[identIndex] = "ArrayParam";
          (*cur_array_dims_table)[identIndex] = totalDimsNum;

        }
        
      }
    }
    
    block->Dump(str0);

    if(son[0]->func_type_all == "int"){
      str0 += " ret 0\n";
    }else{
      str0 += " ret \n";
    }
   
    
    
    str0 += "}\n";
    cur_table = tmp_table;
    curFunvar_table = tmp_var_table;
    cur_array_dims_table = tmp_array_dims_table;
    symcnt -= 1;
  }
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type_str;

  void Dump(std::string& str0) const override {
    if(func_type_str == "int")
    {
        str0 += ": i32";
    } else if(func_type_str == "void"){
      str0 += "";
      
    }
  }
};

class FuncFParamsAST : public BaseAST {
  public:
  void Dump(std::string& str0) const override {
    int sz = son.size();
    for(int i = 0; i < sz; i++){
      son[i]->Dump(str0);
      if(i != sz - 1){
        str0 += ", ";
      }
    }
    
  }

};

class FuncFParamAST : public BaseAST {
  public:
  std::unique_ptr<ConstNumAST> constNumAST;
  std::string ident;
  void Dump(std::string& str0) const override {
    str0 += "@"+ident;
    str0 += ": ";
    if(isArray == false){
      son[0]->Dump(str0);
    }else if(isArray == true){

      int numOfArrayVector = constNumAST->arrayVector.size();
      if(numOfArrayVector == 0){
        str0 += "*i32";
      }else if(numOfArrayVector > 0){
        std::string tmpvecstr = "[i32, "+ std::to_string(constNumAST->arrayVector[numOfArrayVector - 1]) + "]";
        for(int i = numOfArrayVector - 2; i >= 0; i--)
        {
          tmpvecstr = "[" + tmpvecstr +", "+std::to_string(constNumAST->arrayVector[i])+"]";
        }
        str0 += "*";
        str0 += tmpvecstr;
      }
    }

    
  }
  std::string retvaltmp(std::string& str0) override{
    return ident;
  }

  std::string typeForArrayFParam(int& totalDimsNum) override{
    std::string resStr;
    int numOfArrayVector = constNumAST->arrayVector.size();
    totalDimsNum += numOfArrayVector + 1;
    if(numOfArrayVector == 0){
      resStr += "*i32";
    }else if(numOfArrayVector > 0){
      std::string tmpvecstr = "[i32, "+ std::to_string(constNumAST->arrayVector[numOfArrayVector - 1]) + "]";
      for(int i = numOfArrayVector - 2; i >= 0; i--)
      {
        tmpvecstr = "[" + tmpvecstr +", "+std::to_string(constNumAST->arrayVector[i])+"]";
      }
      resStr += "*";
      resStr += tmpvecstr;
    }
    return resStr;
  }

};

class FuncRParamsAST : public BaseAST {
  public:
  void Dump(std::string& str0) const override {
    for(int i = 0; i < son.size(); i++){
      std::string regForFuncRParam = son[i]->retvaltmp(str0);
      if(i!=0){
        str0+=", ";
      }
      str0 += regForFuncRParam;
      
    }
  }
  

};


// Block 也是 BaseAST
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
  BlockAST(){
    type = _Block;
  }

  void Dump(std::string& str0) const override {
    std::map<std::string, std::variant<int, std::string>> new_table;
    std::map<std::string, std::variant<int, std::string>> *tmp_table = cur_table;
    cur_table = &new_table;
    total_table[cur_table] =  tmp_table;

    std::map<std::string, int> new_array_dims_table;
    std::map<std::string, int> *tmp_array_dims_table = cur_array_dims_table;
    cur_array_dims_table = &new_array_dims_table;
    total_array_dims_table[cur_array_dims_table] =  tmp_array_dims_table;


    symcnt += 1;
    for(int i = 0; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
    str0 += "\n";

    cur_table = tmp_table;
    cur_array_dims_table = tmp_array_dims_table;
    symcnt -= 1;
  }
};

// Stmt 也是 BaseAST
class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;
  StmtAST(){
    type = _Stmt;
  }
  bool ret = false;
  bool fl_if = false; 
  bool fl_while = false;
  bool fl_break = false;
  bool fl_continue = false;
  void Dump(std::string& str0) const override {
    
    if(ret == true){
      if(son.size() > 0){
        std::string tmp = exp->retvaltmp(str0);
        str0 += " ret ";
        str0 += tmp.c_str();
        str0 += '\n';
        str0 += "\%"+std::to_string(bblockcnt)+':'+'\n';
        bblockcnt += 1;
      }else{
        str0 += " ret ";
        str0 += '\n';
        str0 += "\%"+std::to_string(bblockcnt)+':'+'\n';
        bblockcnt += 1;
      }
    }else if(fl_if == true){
      std::string tmpexp = son[0]->retvaltmp(str0);
      int tmp_ifcnt = ifcnt;
      ifcnt += 1;
      if(son.size()==3){
        str0 += " br "+tmpexp+", "+"\%then"+std::to_string(tmp_ifcnt)+", "+"\%else"+std::to_string(tmp_ifcnt)+'\n';
      }
      
      if(son.size()==2){
        str0 += " br "+tmpexp+", "+"\%then"+std::to_string(tmp_ifcnt)+", "+"\%end"+std::to_string(tmp_ifcnt)+'\n';
      }


      str0 += "\%then"+std::to_string(tmp_ifcnt)+":"+'\n';
      son[1]->Dump(str0);
      str0 += " jump \%end" + std::to_string(tmp_ifcnt) + '\n';
      if(son.size()==3){
        str0 += "\%else"+std::to_string(tmp_ifcnt)+":"+'\n';
        son[2]->Dump(str0);
        str0 += " jump \%end" + std::to_string(tmp_ifcnt) + '\n';
        
      }

      str0 += "\%end" + std::to_string(tmp_ifcnt) + ":" + '\n';

    }else if(fl_while){
      int tmp_whilecnt = whilecnt;
      whilecnt += 1;
      std::string tmp_end = cur_end;
      std::string tmp_entry = cur_entry;

      cur_end = "\%while_end"+std::to_string(tmp_whilecnt);
      cur_entry = "\%while_entry"+std::to_string(tmp_whilecnt);
      str0 += " jump \%while_entry"+std::to_string(tmp_whilecnt)+'\n';

      str0 += "\%while_entry"+std::to_string(tmp_whilecnt)+':'+'\n';
      std::string tmpexp = son[0]->retvaltmp(str0);
      str0 += " br "+tmpexp+", \%while_body"+std::to_string(tmp_whilecnt)+", \%while_end"+std::to_string(tmp_whilecnt)+'\n';


      str0 += "\%while_body"+std::to_string(tmp_whilecnt)+':'+'\n';
      son[1]->Dump(str0);
      str0 += " jump \%while_entry"+std::to_string(tmp_whilecnt)+'\n';

      str0 += "\%while_end"+std::to_string(tmp_whilecnt)+':'+'\n';

      cur_end = tmp_end;
      cur_entry = tmp_entry;



    }else if(fl_break){
      str0 += " jump "+cur_end+'\n';
      str0 += "\%fl_break_continue"+std::to_string(fl_break_continue)+':'+'\n';
      fl_break_continue += 1;
    }else if(fl_continue){
      str0 += " jump "+cur_entry+'\n';
      str0 += "\%fl_break_continue"+std::to_string(fl_break_continue)+':'+'\n';
      fl_break_continue += 1;
    }else if(son.size() == 0){
      return;
    }else if(son[0]->type == _LVal){
      if(son[0]->isArray == false){
        std::string tmpexp, tmpident;
        std::string resident;
        tmpexp = son[2]->retvaltmp(str0);
        tmpident = '@'+son[0]->retvaltmp(str0);
        int tmpsymcnt = symcnt;
        std::map<std::string, std::variant<int, std::string>> *search_table = cur_table;
        while(tmpsymcnt > 0){
          std::string tmptmpident = tmpident + '_' + std::to_string(tmpsymcnt);
          if((*search_table).find(tmptmpident) != (*search_table).end()){
            resident = tmptmpident;
            break;
          }
          search_table = total_table[search_table];
          tmpsymcnt -= 1;
        }
        if(tmpsymcnt == 0){
          std::cout<<"WRONG AT FIND IDENT"<<std::endl;
          assert(false);
        }

        str0 += " store " + tmpexp + ", " + resident+'\n';
        str0 += '\n';
      }else if(son[0]->isArray == true){
        bool isArrayFunParam = false;
        std::string tmpexp, tmpident;
        std::string resident;
        tmpexp = son[2]->retvaltmp(str0);
        tmpident = '@'+son[0]->retvaltmp(str0);
        int tmpsymcnt = symcnt;
        std::map<std::string, std::variant<int, std::string>> *search_table = cur_table;
        std::map<std::string, int> *search_array_dims_table = cur_array_dims_table;
        while(tmpsymcnt > 0){
          std::string tmptmpident = tmpident + '_' + std::to_string(tmpsymcnt);
          if((*search_table).find(tmptmpident) != (*search_table).end()){
            resident = tmptmpident;
            std::variant<int, std::string> variant_tmp = (*search_table).at(resident);
            if(variant_tmp.index() == 0){
              assert(false);
            }
            std::string arrayType = std::get<std::string>(variant_tmp);
            if(arrayType == "ArrayParam"){
              isArrayFunParam = true;
            }
            break;
          }
          search_table = total_table[search_table];
          search_array_dims_table = total_array_dims_table[search_array_dims_table];
          tmpsymcnt -= 1;
        }
        if(tmpsymcnt == 0){
          std::cout<<"WRONG AT FIND IDENT"<<std::endl;
          assert(false);
        }
        std::string ptrForLVal = son[0]->getElemPtrForLVal(str0, resident, isArrayFunParam, false);

        str0 += " store " + tmpexp + ", " + ptrForLVal+'\n';
        str0 += '\n';
      }
      
    }else if(son[0]->type == _Exp){
      son[0]->retvaltmp(str0);
    }else if(son[0]->type == _Block){
      son[0]->Dump(str0);
    }else{

    }
   
  }
};



class ExpAST : public BaseAST {
 public:
  ExpAST()
  {
    type = _Exp;
  }


  
  void Dump(std::string& str0) const override {
  }

  std::string retvaltmp(std::string& str0) override
  {
    
    std::string tmp = son[0]->retvaltmp(str0);
    val = son[0]->val;
    return tmp;
  }
};


class PrimaryExp : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> number;

 
  PrimaryExp()
  {
    type = _PrimaryExp;
  }
  void Dump(std::string& str0) const override {
  }
};


class Number : public BaseAST {
 public:

  Number()
  {
    type = _Number;
  }


  void Dump(std::string& str0) const override {
  }
};

class UnaryExp : public BaseAST {
 public:
  std::unique_ptr<BaseAST> primaryexp;
  std::unique_ptr<BaseAST> unaryop;
  std::unique_ptr<BaseAST> unaryexp;

  int cnt1;
  int cnt2;
  bool is_ident;
  std::string ident;
  UnaryExp()
  {
    type = _UnaryExp;
  }
  
  void Dump(std::string& str0) const override {
  }


  std::string retvaltmp(std::string& str0) override {
    std::string tmp1;
    std::string tmp2;
    if(is_ident == true){
      std::string regForFun;
      if(funcTable[ident] == "int"){
        regForFun = '%'+std::to_string(tmpcnt);
        tmpcnt++;
        if(son.size()>0){
          int numForRParams = son[0]->son.size();
          std::vector<std::string> allRegsForFuncRParams;
          for(int i = 0; i < numForRParams; i++){
            std::string regForFuncRParam = son[0]->son[i]->retvaltmp(str0);
            allRegsForFuncRParams.push_back(regForFuncRParam);
          }

          str0 += " "+regForFun+" = call @"+ident+"(";
          for(int i = 0; i < allRegsForFuncRParams.size(); i++){
            if(i!=0){
              str0+=", ";
            }
            str0 += allRegsForFuncRParams[i];
          }
          str0 += ")\n";
        }else if(son.size() == 0){
          str0 += " "+regForFun+" = call @"+ident+"()\n";
        }
        
      }else if(funcTable[ident] == "void"){
        regForFun = "";
        
        if(son.size()>0){
          int numForRParams = son[0]->son.size();
          std::vector<std::string> allRegsForFuncRParams;
          for(int i = 0; i < numForRParams; i++){
            std::string regForFuncRParam = son[0]->son[i]->retvaltmp(str0);
            allRegsForFuncRParams.push_back(regForFuncRParam);
          }


          str0 += " call @"+ident+'(';
          for(int i = 0; i < allRegsForFuncRParams.size(); i++){
            if(i!=0){
              str0+=", ";
            }
            str0 += allRegsForFuncRParams[i];
          }
          str0 += ")\n";
        }else if(son.size() == 0){
          str0 += " call @"+ident+"()\n";
        }

        
      }
      return regForFun;

    }else if(son[0]->type == _PrimaryExp)
    {
      BaseAST* ptr = son[0]; 
      if(ptr->son[0]->type == _Number)
      {
        tmp1 = std::to_string(son[0]->son[0]->val);
        val = son[0]->son[0]->val;
      }else if(ptr->son[0]->type == _Exp)
      {
        tmp1 = ptr->son[0]->retvaltmp(str0);
        val = ptr->son[0]->val;
      }else if(ptr->son[0]->type == _LVal)
      {
        if(ptr->son[0]->isArray == false){
          tmp1 = ptr->son[0]->retvaltmp(str0);

          bool isArrayWithoutSymbol = false;
          bool isArrayFunParam = false;

          std::string tmpident = '@' + tmp1;
          std::string resident;
          int tmpsymcnt = symcnt;
          std::map<std::string, std::variant<int, std::string>> *search_table = cur_table;
          std::map<std::string, int> *search_array_dims_table = cur_array_dims_table;

          int fullArrayDimsNum = 0;
          
          while(tmpsymcnt > 0){
            std::string tmptmpident = tmpident + '_' + std::to_string(tmpsymcnt);
            if((*search_table).find(tmptmpident) != (*search_table).end()){
              resident = tmptmpident;
              std::variant<int, std::string> variant_tmp = (*search_table).at(resident);
              if(variant_tmp.index() == 1){
                std::string arrayType = std::get<std::string>(variant_tmp);
                if(arrayType == "ArrayParam"){
                  isArrayFunParam = true;
                  isArrayWithoutSymbol = true;
                }
                if(arrayType == "initArray"){
                  isArrayWithoutSymbol = true;
                }
                if(arrayType == "initGlobalArray"){
                  isArrayWithoutSymbol = true;
                }
              }
              if(isArrayWithoutSymbol == true){

                fullArrayDimsNum = (*search_array_dims_table)[resident];
                
              
              }
              
              break;
            }
            search_table = total_table[search_table];
            
            search_array_dims_table = total_array_dims_table[search_array_dims_table];
 
            tmpsymcnt -= 1;          
          }

          if(tmpsymcnt == 0){
            std::cout<<"WRONG AT FIND IDENT"<<std::endl;
          }

          if(isArrayWithoutSymbol == true){
            int curArrayDims = 0;

            if(curArrayDims >= fullArrayDimsNum){
              assert(false);
            }

            std::string ptrIdent = ptr->son[0]->getElemPtrForLVal(str0, resident, isArrayFunParam, false);


            if(isArrayFunParam == false){
              std::string ptrnum = "0";
              std::string regForPtr = '%'+std::to_string(tmpcnt);
              tmpcnt++;
              str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+ptrnum+"\n";

              ptrIdent = regForPtr;
            }else if(isArrayFunParam == true){
              std::string regToLoadArrayParam = '%'+std::to_string(tmpcnt);
              tmpcnt++;
              str0 += " "+regToLoadArrayParam+" = load "+resident+"\n";
              std::string regToGetPtrArrayParam = '%'+std::to_string(tmpcnt);
              tmpcnt++;
              std::string ptrnum0 = "0";
              str0 += " "+regToGetPtrArrayParam+" = getptr "+regToLoadArrayParam+", "+ptrnum0+"\n";
          
              ptrIdent = regToGetPtrArrayParam;
            }
            
            


            return ptrIdent;
          }else{
            val = value_table[resident];
            std::variant<int, std::string> variant_tmp = (*search_table).at(resident);
            if(variant_tmp.index() == 1){
              
              std::string tmptmp;
              tmptmp = "%" + std::to_string(tmpcnt);
              tmpcnt++;
              tmp1 = '@' + tmp1;
              str0 += " "+tmptmp+" = load "+resident+'\n';
              return tmptmp;
            }else if(variant_tmp.index() == 0){
              int tmpval = std::get<int>(variant_tmp);
              std::string tmp = std::to_string(tmpval);
              return tmp;
            }
          }
          
          
          
        }else if(ptr->son[0]->isArray == true){
          bool isArrayFunParam = false;
          tmp1 = ptr->son[0]->retvaltmp(str0);

          std::string tmpident = '@' + tmp1;
          std::string resident;
          int tmpsymcnt = symcnt;
          int fullArrayDimsNum = 0;
          std::map<std::string, std::variant<int, std::string>> *search_table = cur_table;
          
          std::map<std::string, int> *search_array_dims_table = cur_array_dims_table;

          while(tmpsymcnt > 0){
            std::string tmptmpident = tmpident + '_' + std::to_string(tmpsymcnt);
            
            if((*search_table).find(tmptmpident) != (*search_table).end()){
              resident = tmptmpident;

              std::variant<int, std::string> variant_tmp = (*search_table).at(resident);
              if(variant_tmp.index() == 0){
                assert(false);
              }
              std::string arrayType = std::get<std::string>(variant_tmp);

              if(arrayType == "ArrayParam"){
                isArrayFunParam = true;
              }

              fullArrayDimsNum = (*search_array_dims_table)[resident];

              break;
            }
            search_table = total_table[search_table];
            search_array_dims_table = total_array_dims_table[search_array_dims_table];
            tmpsymcnt -= 1;          
          }

          if(tmpsymcnt == 0){
            std::cout<<"WRONG AT FIND IDENT"<<std::endl;
            assert(false);
          }

         

          if(ptr->son[0]->type != _LVal){
            assert(false);
          }
          BaseAST* myLValNum = ptr->son[0]->son[0];
          if(myLValNum->type != _LValNum){
            assert(false);
          }
          int curLValDimsNum = myLValNum->son.size();



          std::string ptrIdent = ptr->son[0]->getElemPtrForLVal(str0, resident, isArrayFunParam, false);
          
          if(curLValDimsNum == fullArrayDimsNum){
            std::string valueForLValArray = '%'+std::to_string(tmpcnt);
            tmpcnt++;
            str0 += " "+valueForLValArray+" = load "+ptrIdent+"\n";

            return valueForLValArray;
          }else if(curLValDimsNum < fullArrayDimsNum){
            std::string ptrnum = "0";
            std::string regForPtr = '%'+std::to_string(tmpcnt);
            tmpcnt++;
            str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+ptrnum+"\n";

            ptrIdent = regForPtr;
            return ptrIdent;
          }else{
            assert(false);
          }
          

          
        }
        
      }
    }else if (son[0]->type == _UnaryOp)
    {

      if(son[0]->son[0]->op == '-')
      {

        tmp2 = son[1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        val = 0 - son[1]->val;
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        str0 += "sub ";
        str0 += "0, ";
        str0 += tmp2.c_str();
        str0 += "\n";

      }else if (son[0]->son[0]->op == '!')
      {

        tmp2 = son[1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        val = !(son[1]->val);
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        str0 += "eq ";
        str0 += "0, ";
        str0 += tmp2.c_str();
        str0 += "\n";

      }else if (son[0]->son[0]->op == '+')
      {

        tmp1 = son[1]->retvaltmp(str0);
        val = son[1]->val;
      }else
      {
        std::cout<<"wrong_op_type"<<std::endl;
        assert(false);
      }
    }
    return tmp1;
  }
};

class UnaryOp : public BaseAST {
  public:

  UnaryOp()
  {
    type = _UnaryOp;
  }
  void Dump(std::string& str0) const override {
    
  }
};


class Op : public BaseAST {
  public:
  Op(TYPE t, char o):BaseAST(t,o){}

  void Dump(std::string& str0) const override {
   
  }
};


class LE : public BaseAST {
  public:
  LE()
  {
    type = _LE;
  }

  void Dump(std::string& str0) const override {
   
  }
};


class GE : public BaseAST {
  public:
  GE()
  {
    type = _GE;
  }

  void Dump(std::string& str0) const override {
   
  }
};


class EQ : public BaseAST {
  public:
  EQ()
  {
    type = _EQ;
  }

  void Dump(std::string& str0) const override {
   
  }
};



class NE : public BaseAST {
  public:
  NE()
  {
    type = _NE;
  }

  void Dump(std::string& str0) const override {
   
  }
};


class AND : public BaseAST {
  public:
  AND()
  {
    type = _AND;
  }

  void Dump(std::string& str0) const override {
   
  }
};



class OR : public BaseAST {
  public:
  OR()
  {
    type = _OR;
  }

  void Dump(std::string& str0) const override {
   
  }
};


class LT : public BaseAST {
  public:
  LT()
  {
    type = _LT;
  }

  void Dump(std::string& str0) const override {
   
  }
};


class GT : public BaseAST {
  public:
  GT()
  {
    type = _GT;
  }

  void Dump(std::string& str0) const override {
   
  }
};


class AddExp : public BaseAST {
  public:

  AddExp()
  {
    type = _AddExp;
  }
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override {
    if(son.size() == 1)
    {
      std::string ktmp = son[0]->retvaltmp(str0);
      val = son[0]->val;
      return ktmp;
    }
    std::string tmp1, tmp2, tmp3;
    
    for(int i = 1; i < son.size(); i += 2)
    {
      if(i == 1)
      {
        tmp2 = son[0]->retvaltmp(str0);
        tmp3 = son[i+1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->op == '+')
        {
          val = son[0]->val + son[i+1]->val;
          str0 += "add";
        }else if(son[i]->op == '-')
        {
          val = son[0]->val - son[i+1]->val;
          str0 += "sub";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->op == '+')
        {
          val = val + son[i+1]->val;
          str0 += "add";
        }else if(son[i]->op == '-')
        {
          val = val - son[i+1]->val;
          str0 += "sub";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      }  
    }
    return tmp1;
  }
  
};

class MulExp : public BaseAST {
  public:

  MulExp()
  {
    type = _MulExp;
  }
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    if(son.size() == 1)
    {
      std::string ktmp = son[0]->retvaltmp(str0);
      val = son[0]->val;
      return ktmp;
    }
    std::string tmp1,tmp2,tmp3;
    
    for(int i = 1; i < son.size(); i += 2)
    {
     
      if(i == 1)
      {
        tmp2 = son[0]->retvaltmp(str0);
        tmp3 = son[i+1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->op == '*')
        {
          val = son[0]->val * son[i+1]->val;
          str0 += "mul";
        }else if(son[i]->op == '/')
        {
          if(son[i+1]->val != 0){
            val = son[0]->val / son[i+1]->val;
          }
          
          str0 += "div";
        }else if(son[i]->op == '%')
        {
          if(son[i+1]->val != 0){
            val = son[0]->val % son[i+1]->val;
          }
          str0 += "mod";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->op == '*')
        {
          val = val * son[i+1]->val;
          str0 += "mul";
        }else if(son[i]->op == '/')
        {
          if(son[i+1]->val != 0){
            val = son[0]->val / son[i+1]->val;
          }
          str0 += "div";
        }else if(son[i]->op == '%')
        {
          if(son[i+1]->val != 0){
            val = son[0]->val % son[i+1]->val;
          }
          str0 += "mod";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      } 
      
    }
    return tmp1;
  }



};


class RelExp : public BaseAST {
  public:

  RelExp()
  {
    type = _RelExp;
  }
  void Dump(std::string& str0) const override {
   
  }
   std::string retvaltmp(std::string& str0) override  {
    if(son.size() == 1)
    {
      std::string ktmp = son[0]->retvaltmp(str0);
      val = son[0]->val;
      return ktmp;
    }
    std::string tmp1,tmp2,tmp3;
    tmp1 = "%" + std::to_string(tmpcnt);
    tmpcnt++;
    for(int i = 1; i < son.size(); i += 2)
    {
     
      if(i == 1)
      {
        tmp2 = son[0]->retvaltmp(str0);
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->type == _LT)
        {
          val = (son[0]->val < son[i+1]->val);
          str0 += "lt";
        }else if(son[i]->type == _GT)
        {
          val = (son[0]->val > son[i+1]->val);
          str0 += "gt";
        }else if(son[i]->type == _LE)
        {
          val = (son[0]->val <= son[i+1]->val);
          str0 += "le";
        }else if(son[i]->type == _GE)
        {
          val = (son[0]->val >= son[i+1]->val);
          str0 += "ge";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->type == _LT)
        {
          val = (val < son[i+1]->val);
          str0 += "lt";
        }else if(son[i]->type == _GT)
        {
          val = (val > son[i+1]->val);
          str0 += "gt";
        }else if(son[i]->type == _LE)
        {
          val = (val <= son[i+1]->val);
          str0 += "le";
        }else if(son[i]->type == _GE)
        {
          val = (val >= son[i+1]->val);
          str0 += "ge";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      } 
      
    }
    return tmp1;
  }
};


class EqExp : public BaseAST {
  public:

  EqExp()
  {
    type = _EqExp;
  }
  void Dump(std::string& str0) const override {
   
  }
   std::string retvaltmp(std::string& str0) override  {
    if(son.size() == 1)
    {
      std::string ktmp = son[0]->retvaltmp(str0);
      val = son[0]->val;
      return ktmp;
    }
    std::string tmp1,tmp2,tmp3;
    tmp1 = "%" + std::to_string(tmpcnt);
    tmpcnt++;
    for(int i = 1; i < son.size(); i += 2)
    {
     
      if(i == 1)
      {
        tmp2 = son[0]->retvaltmp(str0);
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->type == _EQ)
        {
          str0 += "eq";
          val = (son[0]->val == son[i+1]->val);
        }
        else if(son[i]->type == _NE)
        {
          str0 += "ne";
          val = (son[0]->val != son[i+1]->val);
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->type == _EQ)
        {
          val = (val == son[i+1]->val);
          str0 += "eq";
        }
        else if(son[i]->type == _NE)
        {
          val = (val != son[i+1]->val);
          str0 += "ne";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";

      } 
      
    }
    return tmp1;
  }
};


class LAndExp : public BaseAST {
  public:

  LAndExp()
  {
    type = _LAndExp;
  }
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    if(son.size() == 1)
    {
      std::string ktmp = son[0]->retvaltmp(str0);
      val = son[0]->val;

      return ktmp;
    }
    std::string tmp1, tmp2, tmp3;
    int tmplandcnt= landcnt;
    landcnt += 1;
    str0 += "@land_res_"+std::to_string(tmplandcnt)+" = alloc i32"+'\n';
    for(int i = 0; i < son.size(); i += 2)
    {
      std::string tmptmp = son[i]->retvaltmp(str0);

      str0 += " br "+tmptmp+", \%land_"+std::to_string(tmplandcnt);
      str0 += +"_"+std::to_string(i)+", \%end_land_zero"+std::to_string(tmplandcnt)+'\n';
      str0 += "\%land_"+std::to_string(tmplandcnt)+"_"+std::to_string(i)+":"+'\n';

    }
    str0 += " \%land_res_1_"+std::to_string(tmplandcnt)+" = add 0, 1"+'\n';
    str0 += " store \%land_res_1_"+std::to_string(tmplandcnt)+", @land_res_"+std::to_string(tmplandcnt)+'\n';
    str0 += " jump \%land_end_"+std::to_string(tmplandcnt)+'\n';

    str0 += "\%end_land_zero"+std::to_string(tmplandcnt)+":"+'\n';
    str0 += " \%land_res_2_"+std::to_string(tmplandcnt)+" = add 0, 0"+'\n';
    str0 += " store \%land_res_2_"+std::to_string(tmplandcnt)+", @land_res_"+std::to_string(tmplandcnt)+'\n';
    str0 += " jump \%land_end_"+std::to_string(tmplandcnt)+'\n';
    str0 += "\%land_end_"+std::to_string(tmplandcnt)+':'+'\n';
    std::string tmpres = "@land_res_"+std::to_string(tmplandcnt);

    std::string tmptmpans = "\%ans_land_"+std::to_string(tmplandcnt);
    str0 += " "+tmptmpans+" = load"+tmpres+'\n';
    return tmptmpans;
  }
};


class LOrExp : public BaseAST {
  public:

  LOrExp()
  {
    type = _LOrExp;
  }
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    if(son.size() == 1)
    {
      std::string ktmp = son[0]->retvaltmp(str0);
      val = son[0]->val;

      return ktmp;
    }
    std::string tmp1, tmp2, tmp3;
    int tmplorcnt= lorcnt;
    lorcnt += 1;
    str0 += "@lor_res_"+std::to_string(tmplorcnt)+" = alloc i32"+'\n';
    for(int i = 0; i < son.size(); i += 2)
    {

      std::string tmptmp = son[i]->retvaltmp(str0);
      str0 += " br "+tmptmp+", \%end_lor_one"+std::to_string(tmplorcnt)+", \%lor_"+std::to_string(tmplorcnt);
      str0 += +"_"+std::to_string(i)+'\n';
      str0 += "\%lor_"+std::to_string(tmplorcnt)+"_"+std::to_string(i)+":"+'\n';

    }
    str0 += " \%lor_res_1_"+std::to_string(tmplorcnt)+" = add 0, 0"+'\n';
    str0 += " store \%lor_res_1_"+std::to_string(tmplorcnt)+", @lor_res_"+std::to_string(tmplorcnt)+'\n';
    str0 += " jump \%lor_end_"+std::to_string(tmplorcnt)+'\n';

    str0 += "\%end_lor_one"+std::to_string(tmplorcnt)+":"+'\n';
    str0 += " \%lor_res_2_"+std::to_string(tmplorcnt)+" = add 0, 1"+'\n';
    str0 += " store \%lor_res_2_"+std::to_string(tmplorcnt)+", @lor_res_"+std::to_string(tmplorcnt)+'\n';
    str0 += " jump \%lor_end_"+std::to_string(tmplorcnt)+'\n';
    str0 += "\%lor_end_"+std::to_string(tmplorcnt)+':'+'\n';
    std::string tmpres = "@lor_res_"+std::to_string(tmplorcnt);

    std::string tmptmpans = "\%ans_lor_"+std::to_string(tmplorcnt);
    str0 += " "+tmptmpans+" = load"+tmpres+'\n';
    return tmptmpans;

  }
};

class Decl: public BaseAST {
  public:
  Decl(){
    type = _Decl;
  }
  
  void Dump(std::string& str0) const override {
    son[0]->Dump(str0);
  }
  std::string retvaltmp(std::string& str0) override  {
    son[0]->retvaltmp(str0);
    return "";
  }
};



class ConstDecl: public BaseAST {
  public:
  ConstDecl(){
    type = _ConstDecl;
  }
  
  void Dump(std::string& str0) const override {

    for(int i = 1; i < son.size(); i++){
      son[i]->Dump(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {

    for(int i = 1; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
    return "";
  }
};


class ConstDef_dup: public BaseAST {
  public:
  ConstDef_dup(){
    type = _ConstDef_dup;
  }
  
  void Dump(std::string& str0) const override {

    for(int i = 0; i < son.size(); i++){
      son[i]->Dump(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {

    for(int i = 0; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
    return "";
  }
};


class BType: public BaseAST {
  public:
  std::string ident;
  BType(){
    type = _BType;
  }
  
  void Dump(std::string& str0) const override {
    if(ident == "int"){
      str0 += "i32";
    }
   
  }
  std::string retvaltmp(std::string& str0) override  {
    return "";
  }
};


class ConstDef: public BaseAST {
  public:
  std::string ident;
  ConstDef(){
    type = _ConstDef;
  }
  std::unique_ptr<BaseAST> constInitValAST;
  std::unique_ptr<ConstNumAST> constNumAST;
  int constinitval;
  void Dump(std::string& str0) const override {
    if(isArray == false){
      std::string tmpident = '@'+ident + '_' + std::to_string(symcnt);
      (*cur_table)[tmpident] = constinitval;
    }else if(isArray == true){

      std::string tmp;

      tmp = '@' + ident + '_' + std::to_string(symcnt);
      if((*curFunvar_table).find(tmp) ==  (*curFunvar_table).end()){
        str0 += "global " + tmp + " = alloc ";
        int numOfArrayVector = constNumAST->arrayVector.size();
        std::string tmpvecstr = "[i32, "+ std::to_string(constNumAST->arrayVector[numOfArrayVector - 1]) + "]";
        for(int i = numOfArrayVector - 2; i >= 0; i--)
        {
          tmpvecstr = "[" + tmpvecstr +", "+std::to_string(constNumAST->arrayVector[i])+"]";
        }
        str0 += tmpvecstr;
        (*curFunvar_table)[tmp] = "used";
      }

      
      std::vector<int> numForEachDim;
      numForEachDim.push_back(1);
      int mulRes = 1;
      for(int i = constNumAST->arrayVector.size() - 1; i > 0; i--){
        mulRes *= constNumAST->arrayVector[i];
        numForEachDim.push_back(mulRes);
      }
      mulRes *= constNumAST->arrayVector[0];
      reverse(numForEachDim.begin(), numForEachDim.end());

      
       if(son.size() > 0){
        str0 += ", ";

        std::vector<std::string> initValArray;
        if(son[2]->son.size()>0){
          if(son[2]->isArray == false){
            assert(false);
          }
          son[2]->son[0]->getInitValArray(initValArray, constNumAST->arrayVector, numForEachDim, mulRes,str0);
        }else{
          for(int i = 0; i < mulRes; i++){
            initValArray.push_back("0");
          }
        }

        globalInitSearch(constNumAST->arrayVector, numForEachDim, 0, str0, initValArray, 0);
        str0 += "\n";

        (*cur_table)[tmp] = "initGlobalArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;
      }
      else{
        str0 += ", zeroinit\n";
        (*cur_table)[tmp] = "initGlobalArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;
      }
      (*curFunvar_table)[tmp] = "used";
    }
  }
  std::string retvaltmp(std::string& str0) override  {
    if(isArray == false){
      std::string tmpident = '@'+ident + '_' + std::to_string(symcnt);
      if(isArray == false){
        (*cur_table)[tmpident] = constinitval;
      }
      return "";
    }else if(isArray == true){
      std::string tmp;
      std::string tmp1;


      tmp = '@' + ident + '_' + std::to_string(symcnt);
      if((*curFunvar_table).find(tmp) ==  (*curFunvar_table).end()){
        str0 += " " + tmp + " = alloc ";
        int numOfArrayVector = constNumAST->arrayVector.size();
        std::string tmpvecstr = "[i32, "+ std::to_string(constNumAST->arrayVector[numOfArrayVector - 1]) + "]";
        for(int i = numOfArrayVector - 2; i >= 0; i--)
        {
          tmpvecstr = "[" + tmpvecstr +", "+std::to_string(constNumAST->arrayVector[i])+"]";
        }
        str0 += tmpvecstr + '\n';
        (*curFunvar_table)[tmp] = "used";
      }
    
      std::vector<int> numForEachDim;
      numForEachDim.push_back(1);
      int mulRes = 1;
      for(int i = constNumAST->arrayVector.size() - 1; i > 0; i--){
        mulRes *= constNumAST->arrayVector[i];
        numForEachDim.push_back(mulRes);
      }
      mulRes *= constNumAST->arrayVector[0];
      reverse(numForEachDim.begin(), numForEachDim.end());



      if(son.size() > 0){
        std::vector<std::string> initValArray;
        if(son[2]->son.size()>0){
          if(son[2]->isArray == false){
            assert(false);
          }
          son[2]->son[0]->getInitValArray(initValArray, constNumAST->arrayVector, numForEachDim, mulRes,str0);
        }else{
          for(int i = 0; i < mulRes; i++){
            initValArray.push_back("0");
          }
        }

        getElemPtr(tmp, constNumAST->arrayVector, numForEachDim, 0, str0, initValArray, 0);


        (*cur_table)[tmp] = "initArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;

      }
      else{

        std::vector<std::string> initValArray;
        for(int i = 0; i < mulRes; i++){
          initValArray.push_back("0");
        }
        getElemPtr(tmp, constNumAST->arrayVector, numForEachDim, 0, str0, initValArray, 0);

        (*cur_table)[tmp] = "initArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;

      }
      
    }
    return "";
  }

  void getElemPtr(std::string ptrIdent,  std::vector<int>& dims, std::vector<int>& numsForDims, int curPtrIndex, std::string& str0, std::vector<std::string>& initArray, int initValIndex)
  {
    for(int i = 0; i < dims[curPtrIndex]; i++){
      std::string regForPtr = '%'+std::to_string(tmpcnt);
      tmpcnt++;
      str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+std::to_string(i)+"\n";
      int tmpInitValIndex = initValIndex + numsForDims[curPtrIndex]*i;
      if(curPtrIndex+1 == dims.size()){
        str0 += " store "+initArray[tmpInitValIndex]+", "+regForPtr+"\n";
      }else{
        getElemPtr(regForPtr, dims, numsForDims, curPtrIndex+1, str0, initArray, tmpInitValIndex);
      }
    }
  }
  void globalInitSearch(std::vector<int>& dims, std::vector<int>& numsForDims, int curDimIndex, std::string& str0, std::vector<std::string>& initArray, int initValIndex) const
  {
    str0 += "{";
    for(int i = 0; i < dims[curDimIndex]; i++){
      if(i != 0){
        str0 += ",";
      }
      int tmpInitValIndex = initValIndex + numsForDims[curDimIndex]*i;
      if(curDimIndex + 1 == dims.size()){
        str0 += initArray[tmpInitValIndex];
      }else{
        globalInitSearch(dims, numsForDims, curDimIndex+1, str0, initArray, tmpInitValIndex);
      }

    }
    str0 += "}";
  }
};




class BlockItem_dup: public BaseAST {
  public:
  BlockItem_dup(){
    type = _BlockItem_dup;
  }
  
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    for(int i = 0; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
    return "";
  }
};

class BlockItem: public BaseAST {
  public:
  BlockItem(){
    type = _BlockItem;
  }
  
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {

    if(son[0]->type == _Stmt){
      son[0]->Dump(str0);
    }else if(son[0]->type == _Decl){
      son[0]->retvaltmp(str0);
    }
    return "";
  }
};

class LVal: public BaseAST {
  public:
  LVal(){
    type = _LVal;
  }
  std::string ident;
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {

    std::string tmp = '@'+ident;
    return ident;
  }
  std::string getElemPtrForLVal(std::string& str0, std::string& resident, bool isArrayFunParam, bool isRealArrayParam) override
  {
    if(isRealArrayParam == true){
      std::string ptrIdent = resident;
    
      if(isArrayFunParam == false){
        
        std::string ptrnum = "0";
        std::string regForPtr = '%'+std::to_string(tmpcnt);
        tmpcnt++;
        str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+ptrnum+"\n";
        ptrIdent = regForPtr;
        
      }else if(isArrayFunParam == true){
        std::string regToLoadArrayParam = '%'+std::to_string(tmpcnt);
        tmpcnt++;
        str0 += " "+regToLoadArrayParam+" = load "+resident+"\n";
        std::string regToGetPtrArrayParam = '%'+std::to_string(tmpcnt);
        tmpcnt++;
        std::string ptrnum0 = "0";
        str0 += " "+regToGetPtrArrayParam+" = getptr "+regToLoadArrayParam+", "+ptrnum0+"\n";
    
        ptrIdent = regToGetPtrArrayParam;
      }
      

      return ptrIdent;
    }else if(isRealArrayParam == false){
      std::string ptrIdent = resident;
    
      if(isArrayFunParam == false){
        if(son.size()>0){
          for(int i = 0; i < son[0]->son.size(); i++){
            std::string ptrnum = son[0]->son[i]->retvaltmp(str0);
            std::string regForPtr = '%'+std::to_string(tmpcnt);
            tmpcnt++;
            str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+ptrnum+"\n";
            ptrIdent = regForPtr;
          }
        }
        
      }else if(isArrayFunParam == true){
        if(son.size()>0){
          std::string regToLoadArrayParam = '%'+std::to_string(tmpcnt);
          tmpcnt++;
          str0 += " "+regToLoadArrayParam+" = load "+resident+"\n";
          std::string regToGetPtrArrayParam = '%'+std::to_string(tmpcnt);
          tmpcnt++;
          std::string ptrnum0 = son[0]->son[0]->retvaltmp(str0);
          str0 += " "+regToGetPtrArrayParam+" = getptr "+regToLoadArrayParam+", "+ptrnum0+"\n";
      
          ptrIdent = regToGetPtrArrayParam;
          for(int i = 1; i < son[0]->son.size(); i++){
            std::string ptrnum = son[0]->son[i]->retvaltmp(str0);
            std::string regForPtr = '%'+std::to_string(tmpcnt);
            tmpcnt++;
            str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+ptrnum+"\n";
            ptrIdent = regForPtr;
          }
        }
      }
      return ptrIdent;
    }

    assert(false);
  
  }
};



class LValNum: public BaseAST {
  public:
  LValNum(){
    type = _LValNum;
  }
  std::string ident;
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    return "";
  }
};



class ConstExp: public BaseAST {
  public:
  ConstExp(){
    type = _ConstExp;
  }
  
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    return std::to_string(val);
  }
};

class ConstInitVal: public BaseAST {
  public:
  ConstInitVal(){
    type = _ConstInitVal;
  }
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    return "";
  }
  
  std::vector<std::string> getInitValArray(std::vector<std::string>& resVec, std::vector<int>& dims, std::vector<int>& numsForDims, int allnum, std::string& str0) override
  {
    if(isArray == false){
      std::string tmp = son[0]->retvaltmp(str0);
      resVec.push_back(tmp);
    }
    if(son.size() == 0){
      int curIndex = resVec.size();
      std::vector<std::string> sonArrayVec;
      int indexForSonArray = numsForDims.size();
      int numsForZero = 0;
      for(int i = 0; i < numsForDims.size(); i++){
        if(curIndex%numsForDims[i]==0){
          indexForSonArray = i;
          numsForZero = numsForDims[i];
          break;
        }
      }
      if(indexForSonArray == numsForDims.size()){
        assert(false);
      }
      for(int i = 0; i < numsForZero; i++){
        resVec.push_back("0");
      }

    }else if(son.size() > 0){
      int curIndex = resVec.size();
      std::vector<std::string> sonArrayVec;
      int indexForSonArray = numsForDims.size();
      for(int i = 0; i < numsForDims.size(); i++){
        if(curIndex%numsForDims[i]==0){
          indexForSonArray = i;
          break;
        }
      }
      if(indexForSonArray == numsForDims.size()){
        assert(false);
      }
      std::vector<int> sonDims;
      std::vector<int> sonNumsForDims;
      int allsonnum = 1;
      for(int i = 0; i < dims.size(); i++){
        if(i <= indexForSonArray){
          continue;
        }
        sonDims.push_back(dims[i]);
        sonNumsForDims.push_back(numsForDims[i]);
        allsonnum *= dims[i];
      }
      son[0]->getInitValArray(sonArrayVec, sonDims, sonNumsForDims, allsonnum, str0);
      resVec.insert(resVec.end(), sonArrayVec.begin(), sonArrayVec.end());
    }
    return resVec;
  }
};


class ConstInitVal_dup: public BaseAST {
  public:
  ConstInitVal_dup(){
    type = _ConstInitVal_dup;
  }
  std::string ident;
  void Dump(std::string& str0) const override {
   
  }
  
  std::string retvaltmp(std::string& str0) override  {
    return "";
  }
  
  std::vector<std::string> getInitValArray(std::vector<std::string>& resVec, std::vector<int>& dims, std::vector<int>& numsForDims, int allnum, std::string& str0) override
  {
    for(int i = 0; i < son.size(); i++){
      son[i]->getInitValArray(resVec, dims, numsForDims, allnum, str0);
    }
    int curIndex = resVec.size();
    while(curIndex < allnum){
      resVec.push_back("0");
      curIndex++;
    }
    return resVec;
  }
};


class VarDecl: public BaseAST {
  public:
  VarDecl(){
    type = _VarDecl;
  }
  
  void Dump(std::string& str0) const override {
    if(son[1]->type == _VarDef_dup){
      son[1]->Dump(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {
    if(son[1]->type == _VarDef_dup){
      son[1]->retvaltmp(str0);
    }
    return "";
  }
};




class VarDef_dup: public BaseAST {
  public:
  VarDef_dup(){
    type = _VarDef_dup;
  }
  
  void Dump(std::string& str0) const override {
    for(int i = 0; i < son.size(); i++){
      son[i]->Dump(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {
    for(int i = 0; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
    return "";
  }
};




class VarDef: public BaseAST {
  public:
  VarDef(){
    type = _VarDef;
  }
  std::string ident;
  int initval;
  std::unique_ptr<ConstNumAST> constNumAST;
  void Dump(std::string& str0) const override {
    if(isArray == false){
      std::string tmp;

      tmp = '@' + ident + '_' + std::to_string(symcnt);

      str0 += "global "+tmp+" = alloc i32, ";

      if(son.size() > 0){
        std::string globalVarExp = son[0]->retvaltmp(str0);
        str0 += globalVarExp+'\n';
        (*cur_table)[tmp] = globalVarExp;
        value_table[tmp] = son[0]->val;
      }
      else{
        str0 += "zeroinit\n";
        (*cur_table)[tmp] = std::to_string(initval);
        value_table[tmp] = 0;
      }
      (*curFunvar_table)[tmp] = "used";
   
    }else if(isArray == true){
      std::string tmp;

      tmp = '@' + ident + '_' + std::to_string(symcnt);
      if((*curFunvar_table).find(tmp) ==  (*curFunvar_table).end()){
        str0 += "global " + tmp + " = alloc ";
        int numOfArrayVector = constNumAST->arrayVector.size();
        std::string tmpvecstr = "[i32, "+ std::to_string(constNumAST->arrayVector[numOfArrayVector - 1]) + "]";
        for(int i = numOfArrayVector - 2; i >= 0; i--)
        {
          tmpvecstr = "[" + tmpvecstr +", "+std::to_string(constNumAST->arrayVector[i])+"]";
        }
        str0 += tmpvecstr ;
        (*curFunvar_table)[tmp] = "used";
      }

          
      std::vector<int> numForEachDim;
      numForEachDim.push_back(1);
      int mulRes = 1;
      for(int i = constNumAST->arrayVector.size() - 1; i > 0; i--){
        mulRes *= constNumAST->arrayVector[i];
        numForEachDim.push_back(mulRes);
      }
      mulRes *= constNumAST->arrayVector[0];
      reverse(numForEachDim.begin(), numForEachDim.end());


      if(son.size() > 0){
        str0 += ", ";

        std::vector<std::string> initValArray;
        if(son[0]->son.size()>0){
          if(son[0]->isArray == false){
            assert(false);
          }
          son[0]->son[0]->getInitValArray(initValArray, constNumAST->arrayVector, numForEachDim, mulRes,str0);
        }else{
          for(int i = 0; i < mulRes; i++){
            initValArray.push_back("0");
          }
        }

        globalInitSearch(constNumAST->arrayVector, numForEachDim, 0, str0, initValArray, 0);
        str0 += "\n";


        (*cur_table)[tmp] = "initGlobalArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;
      }
      else{
        str0 += ", zeroinit\n";
        (*cur_table)[tmp] = "initGlobalArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;
      }
      (*curFunvar_table)[tmp] = "used";
    }
   
  }
  std::string retvaltmp(std::string& str0) override  {
    if(isArray == false){
      std::string tmp;
      std::string tmp1;


      tmp = '@' + ident + '_' + std::to_string(symcnt);
      if((*curFunvar_table).find(tmp) ==  (*curFunvar_table).end()){
        str0 += " " + tmp + " = alloc i32"+"\n";
        (*curFunvar_table)[tmp] = "used";
      }
    
      if(son.size() > 0){
        tmp1 = son[0]->retvaltmp(str0);
        str0 += " store " + tmp1 +", " + tmp+'\n';
        str0 += "\n";
        (*cur_table)[tmp] = tmp1;
        value_table[tmp] = son[0]->val;

      }
      else{
        str0 += " store " + std::to_string(initval) +", " + tmp+'\n';
        str0 += "\n";
        (*cur_table)[tmp] = std::to_string(initval);
        value_table[tmp] = 0;

      }
    

      return "";
    }else if(isArray == true){

      std::string tmp;
      std::string tmp1;


      tmp = '@' + ident + '_' + std::to_string(symcnt);
      if((*curFunvar_table).find(tmp) ==  (*curFunvar_table).end()){
        str0 += " " + tmp + " = alloc ";
        int numOfArrayVector = constNumAST->arrayVector.size();
        std::string tmpvecstr = "[i32, "+ std::to_string(constNumAST->arrayVector[numOfArrayVector - 1]) + "]";
        for(int i = numOfArrayVector - 2; i >= 0; i--)
        {
          tmpvecstr = "[" + tmpvecstr +", "+std::to_string(constNumAST->arrayVector[i])+"]";
        }
        str0 += tmpvecstr + '\n';
        (*curFunvar_table)[tmp] = "used";
      }
    
      std::vector<int> numForEachDim;
      numForEachDim.push_back(1);
      int mulRes = 1;
      for(int i = constNumAST->arrayVector.size() - 1; i > 0; i--){
        mulRes *= constNumAST->arrayVector[i];
        numForEachDim.push_back(mulRes);
      }
      mulRes *= constNumAST->arrayVector[0];
      reverse(numForEachDim.begin(), numForEachDim.end());



      if(son.size() > 0){
        std::vector<std::string> initValArray;
        if(son[0]->son.size()>0){
          if(son[0]->isArray == false){
            assert(false);
          }
          son[0]->son[0]->getInitValArray(initValArray, constNumAST->arrayVector, numForEachDim, mulRes,str0);
        }else{
          for(int i = 0; i < mulRes; i++){
            initValArray.push_back("0");
          }
        }


        getElemPtr(tmp, constNumAST->arrayVector, numForEachDim, 0, str0, initValArray, 0);


        (*cur_table)[tmp] = "initArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;


      }
      else{

        std::vector<std::string> initValArray;
        for(int i = 0; i < mulRes; i++){
          initValArray.push_back("0");
        }
        getElemPtr(tmp, constNumAST->arrayVector, numForEachDim, 0, str0, initValArray, 0);



        (*cur_table)[tmp] = "initArray";
        (*cur_array_dims_table)[tmp] = constNumAST->arrayVector.size();
        value_table[tmp] = 0;
        

      }
    }
    return "";
  }

  void getElemPtr(std::string ptrIdent,  std::vector<int>& dims, std::vector<int>& numsForDims, int curPtrIndex, std::string& str0, std::vector<std::string>& initArray, int initValIndex)
  {
    for(int i = 0; i < dims[curPtrIndex]; i++){
      std::string regForPtr = '%'+std::to_string(tmpcnt);
      tmpcnt++;
      str0 += " "+regForPtr+" = getelemptr "+ptrIdent+", "+std::to_string(i)+"\n";
      int tmpInitValIndex = initValIndex + numsForDims[curPtrIndex]*i;
      if(curPtrIndex+1 == dims.size()){
        str0 += " store "+initArray[tmpInitValIndex]+", "+regForPtr+"\n";
      }else{
        getElemPtr(regForPtr, dims, numsForDims, curPtrIndex+1, str0, initArray, tmpInitValIndex);
      }
    }
  }
  void globalInitSearch(std::vector<int>& dims, std::vector<int>& numsForDims, int curDimIndex, std::string& str0, std::vector<std::string>& initArray, int initValIndex) const
  {
    str0 += "{";
    for(int i = 0; i < dims[curDimIndex]; i++){
      if(i != 0){
        str0 += ",";
      }
      int tmpInitValIndex = initValIndex + numsForDims[curDimIndex]*i;
      if(curDimIndex + 1 == dims.size()){
        str0 += initArray[tmpInitValIndex];
      }else{
        globalInitSearch(dims, numsForDims, curDimIndex+1, str0, initArray, tmpInitValIndex);
      }

    }
    str0 += "}";
  }
};



class InitVal: public BaseAST {
  public:
  InitVal(){
    type = _InitVal;
  }
  
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    std::string tmp = son[0]->retvaltmp(str0);
    val = son[0]->val;
    return tmp;
  }
  std::vector<std::string> getInitValArray(std::vector<std::string>& resVec, std::vector<int>& dims, std::vector<int>& numsForDims, int allnum, std::string& str0) override
  {
    if(isArray == false){
      std::string tmp = son[0]->retvaltmp(str0);
      resVec.push_back(tmp);
    }
    if(son.size() == 0){
      int curIndex = resVec.size();
      std::vector<std::string> sonArrayVec;
      int indexForSonArray = numsForDims.size();
      int numsForZero = 0;
      for(int i = 0; i < numsForDims.size(); i++){
        if(curIndex%numsForDims[i]==0){
          indexForSonArray = i;
          numsForZero = numsForDims[i];
          break;
        }
      }
      if(indexForSonArray == numsForDims.size()){
        assert(false);
      }
      for(int i = 0; i < numsForZero; i++){
        resVec.push_back("0");
      }

    }else if(son.size() > 0){
      int curIndex = resVec.size();
      std::vector<std::string> sonArrayVec;
      int indexForSonArray = numsForDims.size();
      for(int i = 0; i < numsForDims.size(); i++){
        if(curIndex%numsForDims[i]==0){
          indexForSonArray = i;
          break;
        }
      }
      if(indexForSonArray == numsForDims.size()){
        assert(false);
      }
      std::vector<int> sonDims;
      std::vector<int> sonNumsForDims;
      int allsonnum = 1;
      for(int i = 0; i < dims.size(); i++){
        if(i <= indexForSonArray){
          continue;
        }
        sonDims.push_back(dims[i]);
        sonNumsForDims.push_back(numsForDims[i]);
        allsonnum *= dims[i];
      }
      son[0]->getInitValArray(sonArrayVec, sonDims, sonNumsForDims, allsonnum, str0);
      resVec.insert(resVec.end(), sonArrayVec.begin(), sonArrayVec.end());
    }
    return resVec;
  }
};

class InitVal_dup: public BaseAST {
  public:
  InitVal_dup(){
    type = _InitVal_dup;
  }
  std::string ident;
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    return "";
  }

  std::vector<std::string> getInitValArray(std::vector<std::string>& resVec, std::vector<int>& dims, std::vector<int>& numsForDims, int allnum, std::string& str0) override
  {
    for(int i = 0; i < son.size(); i++){
      son[i]->getInitValArray(resVec, dims, numsForDims, allnum, str0);
    }
    int curIndex = resVec.size();
    while(curIndex < allnum){
      resVec.push_back("0");
      curIndex++;
    }
    return resVec;
  }
};
