#include <iostream>
#include <memory>
#include <string>
#include <vector>
extern FILE *yyout;
// 所有 AST 的基类
extern int tmpcnt;

enum TYPE{
  _UnaryExp, _PrimaryExp, _UnaryOp, _Number, _Exp, _OP, _AddExp, _MulExp, _RelExp, 
  _EqExp, _LAndExp, _LOrExp, _LE, _GE, _EQ, _NE, _AND, _OR, _LT, _GT, 
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
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump(std::string& str0) const override {
    // std::cout << "CompUnitAST { ";
    str0 = "";
    func_def->Dump(str0);
    // std::cout << " }";
  }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump(std::string& str0) const override {
    // std::cout << "FuncDefAST { ";
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
    str0 += "(): ";
    // fprintf(yyout, "fun @");
    // fprintf(yyout, ident.c_str());
    // fprintf(yyout, "(): ");
    std::cout << "fun @";
    std::cout << ident << "(): ";
    func_type->Dump(str0);
    str0 += " { \n";
    // fprintf(yyout, " { \n");
    std::cout << " { "<<std::endl;
    block->Dump(str0);
    str0 += "}";
    // fprintf(yyout, "}");
    std::cout << "}";
  }
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type_str;

  void Dump(std::string& str0) const override {
    if(func_type_str == "int")
    {
        str0 += "i32";
        // fprintf(yyout, "i32");
        std::cout << "i32";
    } 
  }
};

// Block 也是 BaseAST
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump(std::string& str0) const override {
    str0 += "%";
    str0 += "entry";
    str0 += ":\n";
    // fprintf(yyout, "%%");
    // fprintf(yyout, "entry");
    // fprintf(yyout, ":\n");
    std::cout << "%";
    std::cout << "entry";
    std::cout << ":"<<std::endl;
    stmt->Dump(str0);
    str0 += "\n";
    // fprintf(yyout, "\n");
    std::cout << std::endl;
  }
};

// Stmt 也是 BaseAST
class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;

  void Dump(std::string& str0) const override {
    
    std::string tmp = exp->retvaltmp(str0);
    // str0 += std::to_string(number).c_str();
    // fprintf(yyout, "  ret ");
    // fprintf(yyout, std::to_string(number).c_str());
    std::cout<<tmp<<std::endl;
    std::cout<<"asdfghjkasdfghjkasdfghj"<<std::endl;
    str0 += " ret ";
    std::cout<<str0<<std::endl;
    str0 += tmp.c_str();
    std::cout<<str0<<std::endl;
    // std::cout <<"  "<< "ret ";
    // std::cout << number;
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
    std::string tmp = son[0]->retvaltmp(str0);
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
  
  UnaryExp()
  {
    type = _UnaryExp;
  }
  
  void Dump(std::string& str0) const override {
  }


  std::string retvaltmp(std::string& str0) override {
    std::cout<<"11111111"<<std::endl;
    std::string tmp1;
    std::string tmp2;
    if(son[0]->type == _PrimaryExp)
    {
      std::cout<<"a11111111"<<std::endl;
      BaseAST* ptr = son[0]; 
      if(ptr->son[0]->type == _Number)
      {
        std::cout<<"aa11111111"<<std::endl;
        tmp1 = std::to_string(son[0]->son[0]->val);
      }else if(ptr->son[0]->type == _Exp)
      {
        std::cout<<"ab11111111"<<std::endl;
        
        tmp1 = ptr->son[0]->retvaltmp(str0);
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
        
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        str0 += "sub ";
        str0 += "0, ";
        str0 += tmp2.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
      }else if (son[0]->son[0]->op == '!')
      {
        std::cout<<"d11111111"<<std::endl;
        tmp2 = son[1]->retvaltmp(str0);
        tmp1 = "%" + std::to_string(tmpcnt);
        tmpcnt++;
        
        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        str0 += "eq ";
        str0 += "0, ";
        str0 += tmp2.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
      }else if (son[0]->son[0]->op == '+')
      {
        std::cout<<"e11111111"<<std::endl;
        tmp1 = son[1]->retvaltmp(str0);
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
      return son[0]->retvaltmp(str0);
    }
    std::string tmp1, tmp2, tmp3;
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
        if(son[i]->op == '+')
        {
          str0 += "add";
        }else if(son[i]->op == '-')
        {
          str0 += "sub";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->op == '+')
        {
          str0 += "add";
        }else if(son[i]->op == '-')
        {
          str0 += "sub";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
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
      return son[0]->retvaltmp(str0);
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
        if(son[i]->op == '*')
        {
          str0 += "mul";
        }else if(son[i]->op == '/')
        {
          str0 += "div";
        }else if(son[i]->op == '%')
        {
          str0 += "mod";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->op == '*')
        {
          str0 += "mul";
        }else if(son[i]->op == '/')
        {
          str0 += "div";
        }else if(son[i]->op == '%')
        {
          str0 += "mod";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
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
      return son[0]->retvaltmp(str0);
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
          str0 += "lt";
        }else if(son[i]->type == _GT)
        {
          str0 += "gt";
        }else if(son[i]->type == _LE)
        {
          str0 += "le";
        }else if(son[i]->type == _GE)
        {
          str0 += "ge";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->type == _LT)
        {
          str0 += "lt";
        }else if(son[i]->type == _GT)
        {
          str0 += "gt";
        }else if(son[i]->type == _LE)
        {
          str0 += "le";
        }else if(son[i]->type == _GE)
        {
          str0 += "ge";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
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
      return son[0]->retvaltmp(str0);
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
        }
        else if(son[i]->type == _NE)
        {
          str0 += "ne";
        }
        str0 += ' ';
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
      }else
      {
        tmp2 = tmp1;
        tmp3 = son[i+1]->retvaltmp(str0);

        str0 += " ";
        str0 += tmp1.c_str();
        str0 += " = ";
        if(son[i]->type == _EQ)
        {
          str0 += "eq";
        }
        else if(son[i]->type == _NE)
        {
          str0 += "ne";
        }
        str0 += " ";
        str0 += tmp2.c_str();
        str0 += ", ";
        str0 += tmp3.c_str();
        str0 += "\n";
        std::cout<<str0<<std::endl;
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
    // if(son.size() == 1)
    // {
    //   return son[0]->retvaltmp(str0);
    // }
    // std::string tmp1,tmp2,tmp3;
    // tmp1 = "%" + std::to_string(tmpcnt);
    // tmpcnt++;
    // for(int i = 1; i < son.size(); i += 2)
    // {
     
    //   if(i == 1)
    //   {
    //     tmp2 = son[0]->retvaltmp(str0);
    //     tmp3 = son[i+1]->retvaltmp(str0);

    //     str0 += " ";
    //     str0 += tmp1.c_str();
    //     str0 += " = ";
    //     if(son[i]->type == _AND)
    //     {
    //       str0 += "and";
    //     }
    //     str0 += ' ';
    //     str0 += tmp2.c_str();
    //     str0 += ", ";
    //     str0 += tmp3.c_str();
    //     str0 += "\n";
    //     std::cout<<str0<<std::endl;
    //   }else
    //   {
    //     tmp2 = tmp1;
    //     tmp3 = son[i+1]->retvaltmp(str0);

    //     str0 += " ";
    //     str0 += tmp1.c_str();
    //     str0 += " = ";
    //     if(son[i]->type == _AND)
    //     {
    //       str0 += "and";
    //     }
    //     str0 += " ";
    //     str0 += tmp2.c_str();
    //     str0 += ", ";
    //     str0 += tmp3.c_str();
    //     str0 += "\n";
    //     std::cout<<str0<<std::endl;
    //   } 
      
    // }
    // return tmp1;
    if(son.size() == 1)
    {
      return son[0]->retvaltmp(str0);
    }
    std::string tmp1, tmp2, tmp3;
    for(int i = 0; i < son.size(); i += 2)
    {
      son[i]->retvaltmp(str0);
      std::cout<<"son[i].val"<<std::endl;
      std::cout<<son[i]->val<<std::endl;
      if(son[i]->val == 0)
      {
        return "0";
      }
    }
    return "1";
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
    // if(son.size() == 1)
    // {
    //   return son[0]->retvaltmp(str0);
    // }
    // std::string tmp1,tmp2,tmp3;
    // tmp1 = "%" + std::to_string(tmpcnt);
    // tmpcnt++;
    // for(int i = 1; i < son.size(); i += 2)
    // {
     
    //   if(i == 1)
    //   {
    //     tmp2 = son[0]->retvaltmp(str0);
    //     tmp3 = son[i+1]->retvaltmp(str0);

    //     str0 += " ";
    //     str0 += tmp1.c_str();
    //     str0 += " = ";
    //     if(son[i]->type == _OR)
    //     {
    //       str0 += "or";
    //     }
    //     str0 += ' ';
    //     str0 += tmp2.c_str();
    //     str0 += ", ";
    //     str0 += tmp3.c_str();
    //     str0 += "\n";
    //     std::cout<<str0<<std::endl;
    //   }else
    //   {
    //     tmp2 = tmp1;
    //     tmp3 = son[i+1]->retvaltmp(str0);

    //     str0 += " ";
    //     str0 += tmp1.c_str();
    //     str0 += " = ";
    //     if(son[i]->type == _OR)
    //     {
    //       str0 += "or";
    //     }
    //     str0 += " ";
    //     str0 += tmp2.c_str();
    //     str0 += ", ";
    //     str0 += tmp3.c_str();
    //     str0 += "\n";
    //     std::cout<<str0<<std::endl;
    //   } 
      
    // }
    // return tmp1;
    if(son.size() == 1)
    {
      return son[0]->retvaltmp(str0);
    }
    std::string tmp1, tmp2, tmp3;
    for(int i = 0; i < son.size(); i += 2)
    {
      son[i]->retvaltmp(str0);
      if(son[i]->val >= 1)
      {
        return "1";
      }
    }
    return "0";
  }
};