#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>

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
extern std::map<std::string, int> value_table;
extern std::map<std::string, std::string> funcTable;
extern std::map<std::string, std::variant<int, std::string>> *cur_table;
extern std::map<std::map<std::string, std::variant<int, std::string>>*, \
std::map<std::string, std::variant<int, std::string>>*> total_table;

enum TYPE{
  _UnaryExp, _PrimaryExp, _UnaryOp, _Number, _Exp, _OP, _AddExp, _MulExp, _RelExp, 
  _EqExp, _LAndExp, _LOrExp, _LE, _GE, _EQ, _NE, _AND, _OR, _LT, _GT, _Decl, 
  _ConstDecl, _ConstDef_dup, _BType, _ConstDef, _BlockItem_dup, _BlockItem,
  _LVal, _ConstExp, _ConstInitVal, _Stmt, _VarDecl, _VarDef_dup, _VarDef, 
  _InitVal, _Block, 
};

class BaseAST {
 public:

  BaseAST() = default;
  BaseAST(TYPE t): type(t){}
  BaseAST(TYPE t, char o): type(t), op(o)
  {
    std::cout<<"char_______op"<<o<<std::endl;
  }
  virtual ~BaseAST() = default;
  virtual void Dump(std::string& str0) const = 0;
  virtual std::string retvaltmp(std::string& str0)
  {
    return "";
  }
  std::vector<BaseAST *> son;
  TYPE type;
  int val;
  char op;
  bool isint = false;
  std::string func_type_all;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;
  
  void Dump(std::string& str0) const override {
    // std::cout << "CompUnitAST { ";
    cur_table = &sym_table;
    str0 = "";
    str0 += "decl @getint(): i32\n";
    str0 += "decl @getch(): i32\n";
    str0 += "decl @getarray(*i32): i32\n";
    str0 += "decl @putint(i32)\n";
    str0 += "decl @putch(i32)\n";
    str0 += "decl @putarray(i32, *i32)\n";
    str0 += "decl @starttime()\n";
    str0 += "decl @stoptime()\n";

    int sz = son.size();
    std::cout<<"COMP_UNIT_AST_SIZE  "<<sz<<std::endl;
    symcnt += 1;
    for(int i = 0; i < sz; i++){
      std::cout<<"FUNCDEF_INDEX  "<<i<<std::endl;
      son[i]->Dump(str0);
      std::cout<<"FINISH ONE FUNCDEF"<<std::endl;
      std::cout<<str0<<std::endl;
      std::cout<<"COMP_UNIT_AST_SIZE  "<<sz<<std::endl;
    }
    symcnt -= 1;
    // func_def->Dump(str0);
    // std::cout << " }";
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
    std::cout << "FuncDefAST  "<<ident<<" "<<str0<<std::endl;
    // func_type->Dump();
    // std::cout << ", " << ident << ", ";
    // block->Dump();
    // std::cout << " }";
    // std::cout << "fun @";
    // std::cout << ident << "(): ";
    // func_type->Dump();
    // std::cout << " { "<<std::endl;
    // block->Dump();
    // std::cout << " }";
    str0 += "fun @";
    str0 += ident;
    funcTable[ident] = son[0]->func_type_all;
    str0 += "(";
    int sz = son.size();
    if(sz == 3){
      son[1]->Dump(str0);
    }
    str0 += ")";
   

    std::cout<<"ASDFGHGFDS"<<str0<<std::endl;
    
    if(son[0]->func_type_all == "int"){
      str0 += ": i32";
    }else if(son[0]->func_type_all == "void"){
      str0 += "";
    }
    // func_type->Dump(str0);
    std::cout<<"func_TYPE"<<str0<<std::endl;
    str0 += " { \n";
    // fprintf(yyout, " { \n");
    // std::cout << " { "<<std::endl;

    str0 += "\%entry:\n";

    std::cout<<str0<<std::endl;

    std::map<std::string, std::variant<int, std::string>> new_table;
    std::map<std::string, std::variant<int, std::string>> *tmp_table = cur_table;
    cur_table = &new_table;
    total_table[cur_table] =  tmp_table;
    std::map<std::string, std::variant<int, std::string>>::iterator iter;
    std::cout<<"ITERATOR CUR"<<std::endl;
    for(iter = (*cur_table).begin(); iter != (*cur_table).end(); ++iter){
      std::cout<<iter->first<<std::endl;
    }
    std::cout<<"ITERATOR TMP"<<std::endl;
    for(iter = (*tmp_table).begin(); iter != (*tmp_table).end(); ++iter){
      std::cout<<iter->first<<std::endl;
    }
    symcnt += 1;

    if(sz == 3){
      int numOfFParam = son[1]->son.size();
      
      for(int i = 0; i < numOfFParam; i++){
        std::string ident = son[1]->son[i]->retvaltmp(str0);
        std::string identIndex = "@"+ident+'_'+std::to_string(symcnt);
        std::string paramName = "@"+ident;
        str0 += " @"+ident+'_'+std::to_string(symcnt);
        str0 += " = alloc i32\n";
        str0 += " store @"+ident+", @"+ident+'_'+std::to_string(symcnt)+'\n';
        (*cur_table)[identIndex] = paramName;
      }
    }
    
    block->Dump(str0);

    if(son[0]->func_type_all == "int"){
       char c = str0.back();
        while(c!= '\%'){
          str0.pop_back();
          c = str0.back();
        }
        str0.pop_back();
    }else{
      str0 += " ret \n";
    }
   
    
    
    str0 += "}\n";
    cur_table = tmp_table;
    symcnt -= 1;
    std::cout<<str0<<std::endl;
    // fprintf(yyout, "}");
    // std::cout << "}";
  }
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type_str;

  void Dump(std::string& str0) const override {
    std::cout<<"FuncTypeASTTT"<<std::endl;
    if(func_type_str == "int")
    {
        str0 += ": i32";
        // fprintf(yyout, "i32");
        std::cout << "i32";
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
  std::string ident;
  void Dump(std::string& str0) const override {
    str0 += "@"+ident;
    str0 += ": ";
    son[0]->Dump(str0);
    
  }
  std::string retvaltmp(std::string& str0) override{
    return ident;
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
    std::map<std::string, std::variant<int, std::string>>::iterator iter;
    std::cout<<"ITERATOR CUR"<<std::endl;
    for(iter = (*cur_table).begin(); iter != (*cur_table).end(); ++iter){
      std::cout<<iter->first<<std::endl;
    }
    std::cout<<"ITERATOR TMP"<<std::endl;
    for(iter = (*tmp_table).begin(); iter != (*tmp_table).end(); ++iter){
      std::cout<<iter->first<<std::endl;
    }
    symcnt += 1;
    std::cout<<"SYMCNT "<<symcnt<<std::endl;
    // stmt->Dump(str0);
    // son[0]->retvaltmp(str0);
    for(int i = 0; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
    str0 += "\n";

    cur_table = tmp_table;
    symcnt -= 1;
    // fprintf(yyout, "\n");
    // std::cout << std::endl;
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
    std::cout<<"Stmt"<<std::endl;
    
    if(ret == true){
      if(son.size() > 0){
      std::cout<<"STMT1"<<std::endl;
      std::string tmp = exp->retvaltmp(str0);
      // str0 += std::to_string(number).c_str();
      // fprintf(yyout, "  ret ");
      // fprintf(yyout, std::to_string(number).c_str());
      std::cout<<tmp<<std::endl;
      std::cout<<"asdfghjkasdfghjkasdfghj"<<std::endl;
      str0 += " ret ";
      // std::cout<<str0<<std::endl;
      str0 += tmp.c_str();
      str0 += '\n';
      str0 += "\%"+std::to_string(bblockcnt)+':'+'\n';
      bblockcnt += 1;
      std::cout<<str0<<std::endl;
      // std::cout <<"  "<< "ret ";
      // std::cout << number;
      }else{
        std::cout<<"STMT RETURN NULL"<<std::endl;
        str0 += " ret ";
        str0 += "0";
        str0 += '\n';
        str0 += "\%"+std::to_string(bblockcnt)+':'+'\n';
        bblockcnt += 1;
        std::cout<<str0<<std::endl;
      }
    }else if(fl_if == true){
      std::cout<<"STMT__IF"<<std::endl;
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
      std::cout<<str0<<std::endl;

    }else if(fl_while){
      std::cout<<"STMT_WHILE"<<std::endl;
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
      // if(fl_break_continue != 1){
      //     str0 += " jump \%while_entry"+std::to_string(tmp_whilecnt)+'\n';
      //   }else if(fl_break_continue == 1){
      //     fl_break_continue = 0;
      //   }
      

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
      std::cout<<"STMT2"<<std::endl;
      std::string tmpexp, tmpident;
      std::string resident;
      tmpexp = son[2]->retvaltmp(str0);
      tmpident = '@'+son[0]->retvaltmp(str0);
      int tmpsymcnt = symcnt;
      std::map<std::string, std::variant<int, std::string>> *search_table = cur_table;
      // std::map<std::string, std::variant<int, std::string>> tmp_table = *cur_table;
      while(tmpsymcnt > 0){
        std::string tmptmpident = tmpident + '_' + std::to_string(tmpsymcnt);
        if((*search_table).find(tmptmpident) != (*search_table).end()){
          resident = tmptmpident;
          std::cout<<"resident  "<<resident<<std::endl;
          break;
        }
        search_table = total_table[search_table];
        tmpsymcnt -= 1;
      }
      // *cur_table = tmp_table;
      if(tmpsymcnt == 0){
        std::cout<<"WRONG AT FIND IDENT"<<std::endl;
      }

      str0 += " store " + tmpexp + ", " + resident+'\n';
      str0 += '\n';
      std::cout<<str0<<std::endl;
    }else if(son[0]->type == _Exp){
      std::cout<<"STMT EXP"<<std::endl;
      son[0]->retvaltmp(str0);
    }else if(son[0]->type == _Block){
      std::cout<<"STMT BLOCK"<<std::endl;
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
    std::cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"<<std::endl;
    std::cout<<"Exp"<<std::endl;
    std::string tmp = son[0]->retvaltmp(str0);
    val = son[0]->val;
    std::cout<<"EXPAAAAAAAAAAA"<<std::endl;
    std::cout<<tmp<<std::endl;
    std::cout<<str0<<std::endl;
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
    std::cout<<"UnaryExp"<<std::endl;
    std::cout<<"11111111"<<std::endl;
    std::string tmp1;
    std::string tmp2;
    if(is_ident == true){
      std::string regForFun;
      if(funcTable[ident] == "int"){
        std::cout<<"UNARY FUNC INT"<<std::endl;
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
      std::cout<<"a11111111"<<std::endl;
      BaseAST* ptr = son[0]; 
      if(ptr->son[0]->type == _Number)
      {
        std::cout<<"aa11111111"<<std::endl;
        tmp1 = std::to_string(son[0]->son[0]->val);
        val = son[0]->son[0]->val;
      }else if(ptr->son[0]->type == _Exp)
      {
        std::cout<<"ab11111111"<<std::endl;
        tmp1 = ptr->son[0]->retvaltmp(str0);
        val = ptr->son[0]->val;
      }else if(ptr->son[0]->type == _LVal)
      {
        tmp1 = ptr->son[0]->retvaltmp(str0);

        std::string tmpident = '@' + tmp1;
        std::string resident;
        int tmpsymcnt = symcnt;
        std::map<std::string, std::variant<int, std::string>> *search_table = cur_table;
        // std::map<std::string, std::variant<int, std::string>> tmp_table = *cur_table;
        std::cout<<"START FIND IDENT"<<std::endl;
        std::cout<<"TMPSYMCNT "<<tmpsymcnt<<std::endl;
        std::cout<<"SYMCNT "<<symcnt<<std::endl;
        while(tmpsymcnt > 0){
          std::string tmptmpident = tmpident + '_' + std::to_string(tmpsymcnt);
          std::cout<<"TMPSYMCNT "<<tmpsymcnt<<std::endl;
          std::cout<<"TMPTMPIDENT "<<tmptmpident<<std::endl;

          std::map<std::string, std::variant<int, std::string>>::iterator iter;
          std::cout<<"ITERATOR"<<std::endl;
          for(iter = (*search_table).begin(); iter != (*search_table).end(); ++iter){
            std::cout<<iter->first<<std::endl;
          }
          if((*search_table).find(tmptmpident) != (*search_table).end()){
            resident = tmptmpident;
            std::cout<<"resident  "<<resident<<std::endl;
            break;
          }
          search_table = total_table[search_table];
          tmpsymcnt -= 1;          
        }
        // *cur_table = tmp_table;
        if(tmpsymcnt == 0){
          std::cout<<"WRONG AT FIND IDENT"<<std::endl;
        }

        
        val = value_table[resident];
        std::cout<<"RESIIIIDENT    "<<resident<<std::endl;
        std::cout<<val<<std::endl;
        std::variant<int, std::string> variant_tmp = (*search_table).at(resident);
        std::cout<<variant_tmp.index()<<std::endl;
        if(variant_tmp.index() == 1){
          
          std::string tmptmp;
          tmptmp = "%" + std::to_string(tmpcnt);
          tmpcnt++;
          tmp1 = '@' + tmp1;
          str0 += " "+tmptmp+" = load "+resident+'\n';
          std::cout<<str0<<std::endl;
          // val = ptr->son[0]->val;
          return tmptmp;
        }else if(variant_tmp.index() == 0){
          int tmpval = std::get<int>(variant_tmp);
          std::string tmp = std::to_string(tmpval);
          // val = tmpval;
          return tmp;
        }
        
      }
    }else if (son[0]->type == _UnaryOp)
    {
      std::cout<<"b11111111"<<std::endl;
      std::cout<<son[0]->op<<std::endl;
      std::cout<<son[0]->son[0]->op<<std::endl;
      if(son[0]->son[0]->op == '-')
      {
        std::cout<<"c11111111"<<std::endl;
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
        // std::cout<<str0<<std::endl;
      }else if (son[0]->son[0]->op == '!')
      {
        std::cout<<"d11111111"<<std::endl;
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
        // std::cout<<str0<<std::endl;
      }else if (son[0]->son[0]->op == '+')
      {
        std::cout<<"e11111111"<<std::endl;
        tmp1 = son[1]->retvaltmp(str0);
        val = son[1]->val;
      }else
      {
        std::cout<<"wrong_op_type"<<std::endl;
      }
    }
    return tmp1;
  }
};

class UnaryOp : public BaseAST {
  public:
  // char op;
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
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};


class LE : public BaseAST {
  public:
  LE()
  {
    type = _LE;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};


class GE : public BaseAST {
  public:
  GE()
  {
    type = _GE;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};


class EQ : public BaseAST {
  public:
  EQ()
  {
    type = _EQ;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};



class NE : public BaseAST {
  public:
  NE()
  {
    type = _NE;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};


class AND : public BaseAST {
  public:
  AND()
  {
    type = _AND;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};



class OR : public BaseAST {
  public:
  OR()
  {
    type = _OR;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};


class LT : public BaseAST {
  public:
  LT()
  {
    type = _LT;
  }
  // char op;
  void Dump(std::string& str0) const override {
   
  }
};


class GT : public BaseAST {
  public:
  GT()
  {
    type = _GT;
  }
  // char op;
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
        // std::cout<<str0<<std::endl;
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
        // std::cout<<str0<<std::endl;
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
          val = son[0]->val % son[i+1]->val;
          str0 += "mod";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        // std::cout<<str0<<std::endl;
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
          val = val / son[i+1]->val;
          str0 += "div";
        }else if(son[i]->op == '%')
        {
          val = val % son[i+1]->val;
          str0 += "mod";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        // std::cout<<str0<<std::endl;
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
        // std::cout<<str0<<std::endl;
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
        // std::cout<<str0<<std::endl;
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
        // std::cout<<str0<<std::endl;
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
        // std::cout<<str0<<std::endl;
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
      std::cout<<"LANDVAL "<<val<<std::endl;
      return ktmp;
    }
    std::string tmp1, tmp2, tmp3;
    int tmplandcnt= landcnt;
    landcnt += 1;
    str0 += "@land_res_"+std::to_string(tmplandcnt)+" = alloc i32"+'\n';
    for(int i = 0; i < son.size(); i += 2)
    {
      std::string tmptmp = son[i]->retvaltmp(str0);
      std::cout<<"son[i].val"<<std::endl;
      std::cout<<son[i]->val<<std::endl;
      str0 += " br "+tmptmp+", \%land_"+std::to_string(tmplandcnt);
      str0 += +"_"+std::to_string(i)+", \%end_land_zero"+std::to_string(tmplandcnt)+'\n';
      str0 += "\%land_"+std::to_string(tmplandcnt)+"_"+std::to_string(i)+":"+'\n';

      // if(son[i]->val == 0)
      // {
      //   val = 0;
      //   return "0";
      // }
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
      std::cout<<"LORVAL "<<val<<std::endl;
      return ktmp;
    }
    std::string tmp1, tmp2, tmp3;
    int tmplorcnt= lorcnt;
    lorcnt += 1;
    str0 += "@lor_res_"+std::to_string(tmplorcnt)+" = alloc i32"+'\n';
    for(int i = 0; i < son.size(); i += 2)
    {

      std::string tmptmp = son[i]->retvaltmp(str0);
      std::cout<<"son[i].val"<<std::endl;
      std::cout<<son[i]->val<<std::endl;
      str0 += " br "+tmptmp+", \%end_lor_one"+std::to_string(tmplorcnt)+", \%lor_"+std::to_string(tmplorcnt);
      str0 += +"_"+std::to_string(i)+'\n';
      str0 += "\%lor_"+std::to_string(tmplorcnt)+"_"+std::to_string(i)+":"+'\n';

      // son[i]->retvaltmp(str0);
      // if(son[i]->val >= 1)
      // {
      //   val = 1;
      //   return "1";
      // }
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
    // val = 0;
    // return "0";
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
    std::cout<<"ConstDecl"<<std::endl;
    for(int i = 1; i < son.size(); i++){
      son[i]->retvaltmp(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {
    std::cout<<"ConstDecl"<<std::endl;
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
   
  }
  std::string retvaltmp(std::string& str0) override  {
    std::cout<<"ConstDef_dup"<<std::endl;
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
  int constinitval;
  void Dump(std::string& str0) const override {
   
  }
  std::string retvaltmp(std::string& str0) override  {
    std::cout<<"ConstDef"<<std::endl;
    std::cout<<ident<<std::endl;
    std::cout<<constinitval<<std::endl;
    std::string tmpident = '@'+ident + '_' + std::to_string(symcnt);
    std::cout<<"CONST_DEF_TMPIDENT"<<tmpident<<std::endl;
    (*cur_table)[tmpident] = constinitval;
    return "";
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
    std::cout<<"BlockItem_dup"<<std::endl;
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
    std::cout<<"BlockItem"<<std::endl;
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
    std::cout<<"LVal"<<std::endl;
    // std::variant<int, std::string> variant_tmp = sym_table.at(ident);
    // std::cout<<variant_tmp.index()<<std::endl;
    // val = std::get<int>(variant_tmp);
    // std::string tmp = std::to_string(val);
    std::string tmp = '@'+ident;
    return ident;
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
    return "";
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
};





class VarDecl: public BaseAST {
  public:
  VarDecl(){
    type = _VarDecl;
  }
  
  void Dump(std::string& str0) const override {
    std::cout<<"GLOBAL VarDecl"<<std::endl;
    if(son[1]->type == _VarDef_dup){
      son[1]->Dump(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {
    std::cout<<"VarDecl"<<std::endl;
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
    std::cout<<"VarDef_dup"<<std::endl;
    for(int i = 0; i < son.size(); i++){
      son[i]->Dump(str0);
    }
  }
  std::string retvaltmp(std::string& str0) override  {
    std::cout<<"VarDef_dup"<<std::endl;
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
  void Dump(std::string& str0) const override {
    std::cout<<"GLOBAL VarDef"<<std::endl;
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
    var_table[tmp] = "used";
   
  }
  std::string retvaltmp(std::string& str0) override  {
    std::cout<<"VarDef"<<std::endl;
    std::string tmp;
    std::string tmp1;
    // if(son.size()>0){
    //   son[0]->retvaltmp(str0);
    // }

    tmp = '@' + ident + '_' + std::to_string(symcnt);
    if(var_table.find(tmp) ==  var_table.end()){
      str0 += " " + tmp + " = alloc i32"+"\n";
      var_table[tmp] = "used";
    }
   
    std::cout<<"VARDEF  "<<tmp<<std::endl;
    std::cout<<"SYMCNT  "<<symcnt<<std::endl;
    if(son.size() > 0){
      tmp1 = son[0]->retvaltmp(str0);
      str0 += " store " + tmp1 +", " + tmp+'\n';
      str0 += "\n";
      (*cur_table)[tmp] = tmp1;
      value_table[tmp] = son[0]->val;
      std::cout<<"INITVAL VARDEF ONE"<<tmp1<<std::endl;
      // std::cout<<str0<<std::endl;
    }
    else{
      str0 += " store " + std::to_string(initval) +", " + tmp+'\n';
      str0 += "\n";
      (*cur_table)[tmp] = std::to_string(initval);
      value_table[tmp] = 0;
      std::cout<<"INITVAL VARDEF TWO"<<std::to_string(initval)<<std::endl;
      // std::cout<<str0<<std::endl;
    }
   

    return "";
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
};

