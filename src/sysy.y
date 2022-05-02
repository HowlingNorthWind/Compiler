%code requires {
  #include <memory>
  #include <string>
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

int cnt = 0;
%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  char char_val;
  class BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT CONST
%token <int_val> INT_CONST
%token <ast_val> '+' '-' '*' '/' '%' '!' '='
%token <ast_val> LE GE EQ NE AND OR LT GT

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp Number UnaryExp UnaryOp MulExp AddExp
%type <ast_val> RelExp EqExp LAndExp LOrExp  Decl ConstDecl ConstDef_dup BType
%type <ast_val> ConstDef BlockItem_dup BlockItem LVal ConstExp ConstInitVal
%type <ast_val> VarDecl VarDef_dup VarDef InitVal


%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
  : FuncDef {
    std::cout<<"COMPUNIT START"<<std::endl;
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
    std::cout<<"COMPUNIT END"<<std::endl;
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担

Decl
  : ConstDecl {
    auto ast = new Decl();
    ast->son.push_back($1);
    $$ = ast;
  }
  | VarDecl {
    auto ast = new Decl();
    ast->son.push_back($1);
    $$ = ast;
  }
  ;

ConstDecl
  : CONST BType ConstDef_dup ';'{
    auto ast = new ConstDecl();
    
    ast->son.push_back($2);
    ast->son.push_back($3);
    $$ = ast;
  }
  ;

ConstDef_dup : ConstDef_dup ',' ConstDef {
    $1->son.push_back($3);
    $$ = $1;
  }
  | ConstDef {
    auto ast = new ConstDef_dup();
    ast->son.push_back($1);
    $$ = ast;
  }
  ;

BType
  : INT {
    auto ast = new BType();
    ast->ident = "int";
    $$ = ast;
  }
  ;

ConstDef 
  : IDENT '=' ConstInitVal {
    sym_table[*$1] = $3->val;
    auto ast = new ConstDef();
    ast->ident = *unique_ptr<string>($1);
    ast->son.push_back($2);
    ast->son.push_back($3);
    ast->constinitval = $3->val;
    $$ = ast;
  }
  ;

ConstInitVal
  : ConstExp {
    auto ast = new ConstInitVal();
    ast->val = $1->val;
    ast->son.push_back($1);
    $$ = ast;
  }
  ;

VarDecl 
  : BType VarDef_dup ';' {
    auto ast = new VarDecl();
    ast->son.push_back($1);
    ast->son.push_back($2);
    $$ = ast;
  }
  ;

VarDef_dup
  :  VarDef {
    auto ast = new VarDef_dup();
    ast->son.push_back($1);
    $$ = ast;
  }
  | VarDef_dup ',' VarDef {
    $1->son.push_back($3);
    $$ = $1;
  } 
  ;

VarDef
  : IDENT {
    auto ast = new VarDef();
    ast->ident = *unique_ptr<string>($1);
    sym_table[ast->ident] = "";
    $$ = ast;
  }
  | IDENT '=' InitVal {
    auto ast = new VarDef();
    ast->ident = *unique_ptr<string>($1);
    ast->initval = $3->val;
    cout<<"VarDef"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($3);
    sym_table[ast->ident] = "";
    $$ = ast;
  }
  ;

InitVal
  : Exp {
    auto ast = new InitVal();
    ast->son.push_back($1);
    ast->val = $1->val;
    $$ = ast;
  }
  ;


FuncDef
  : FuncType IDENT '(' ')' Block {
    std::cout<<"FUNCDEF"<<std::endl;
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    auto ast = new FuncTypeAST();
    string str = "int";
    ast->func_type_str = str;
    $$ = ast;
  }
  ;

Block
  : '{' BlockItem_dup '}' {
    std::cout<<"BLOCK"<<std::endl;
    auto ast = new BlockAST();
    ast->son = $2->son;
    $$ = ast;
  }
  ;

BlockItem_dup
  : {
    auto ast = new BlockItem_dup();
    $$ = ast;
  }
  | BlockItem_dup BlockItem {
    $1->son.push_back($2);
    $$ = $1;
  }
  ;

BlockItem
  : Decl {
    auto ast = new BlockItem();
    ast->son.push_back($1);
    $$ = ast;
  }
  | Stmt {
    auto ast = new BlockItem();
    ast->son.push_back($1);
    $$ = ast;
  }
  ;

LVal 
  : IDENT {
    auto ast = new LVal();
    ast->ident = *unique_ptr<string>($1);
    std::variant<int, std::string> variant_tmp = sym_table.at(ast->ident);
    std::cout<<"LVAL"<<std::endl;
    std::cout<<variant_tmp.index()<<std::endl;
    if(variant_tmp.index()==0){
      ast->val = std::get<int>(variant_tmp);
    }
    // ast->val = sym_table.at(*$1);
    $$ = ast;
  }
  ;

ConstExp 
  : Exp {
    auto ast = new ConstExp();
    ast->val = $1->val;
    ast->son.push_back($1);
    $$ = ast;
  }
  ;


Stmt
  : RETURN Exp ';' {
    std::cout<<"STMT1"<<std::endl;
    auto ast = new StmtAST();
    ast->ret = true;
    ast->exp = unique_ptr<BaseAST>($2);
    ast->son.push_back($2);
    std::cout<<"STMT1KKKKKKK"<<std::endl;
    $$ = ast;
  }
  | LVal '=' Exp ';' {
    std::cout<<"STMT2"<<std::endl;
    auto ast = new StmtAST();
    ast->son.push_back($1);
    ast->son.push_back($2);
    ast->son.push_back($3);
    $$ = ast;
  }
  | Block {
    std::cout<<"STMT3"<<std::endl;
    auto ast = new StmtAST();
    ast->son.push_back($1);
    $$ = ast;
  }
  | Exp ';' {
    std::cout<<"STMT4"<<std::endl;
    auto ast = new StmtAST();
    ast->son.push_back($1);
    $$ = ast;
  }
  | ';' {
    std::cout<<"STMT5"<<std::endl;
    auto ast = new StmtAST();
    $$ = ast;
  }
  | RETURN ';' {
    std::cout<<"STMT6"<<std::endl;
    auto ast = new StmtAST();
    ast->ret = true;
    $$ = ast;
  }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->val = $1->val;
    ast->son.push_back($1);
    cout<<"Exp"<<endl;
    cout<<ast->val<<endl;
    $$ = ast;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExp();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->val = $2->val;
    ast->son.push_back($2);
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExp();
   
    ast->val = $1->val;
    ast->son.push_back($1);
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExp();
    ast->number = unique_ptr<BaseAST>($1);
    ast->val = $1->val;
    cout<<"PrimaryExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  ;


Number
  : INT_CONST {
    auto ast = new Number();
    ast->val = $1;
    cout<<"INT_CONST"<<endl;
    cout<<ast->val<<endl;
    ast->isint = true;
    $$ = ast;
  }
  ;

UnaryExp 
  : PrimaryExp{
    auto ast = new UnaryExp();
    ast->val = $1->val;
    cout<<"UnaryExp"<<endl;
    cout<<ast->val<<endl;
    ast->isint = $1->isint;
    ast->son.push_back($1);
    $$ = ast;
  }
  | UnaryOp UnaryExp{
    auto ast = new UnaryExp();
    ast->unaryop = unique_ptr<BaseAST>($1);
    ast->unaryexp = unique_ptr<BaseAST>($2);
    ast->son.push_back($1);
    ast->son.push_back($2);
    if($1->son[0]->op == '-'){
      ast->val = 0 - ($2->val);
    } else if($1->son[0]->op == '!'){
      if($2->val != 0){
        ast->val = 0;
      }else{
        ast->val = 1;
      }
    } else{
      ast->val = $2->val;
    }
    $$ = ast;
  }
  ;

UnaryOp
  : '+'{
    auto ast = new UnaryOp();
    ast->son.push_back($1);
    std::cout<<"ast->type"<<std::endl;
    std::cout<<ast->son[0]->op<<std::endl;
    $$ = ast;  
  }
  | '-'{
    auto ast = new UnaryOp();
    ast->son.push_back($1);
    $$ = ast;  
  }
  | '!'{
    auto ast = new UnaryOp();
    ast->son.push_back($1);
    $$ = ast;
  }
  ;

MulExp 
  : UnaryExp{
    auto ast = new MulExp();
    ast->val = $1->val;
    cout<<"MULExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  | MulExp '*' UnaryExp{
    $1->val = $1->val * $3->val;
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  | MulExp '/' UnaryExp{
    if($3->val != 0){
      $1->val = $1->val / $3->val;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  | MulExp '%' UnaryExp{
    if($3->val != 0){
      $1->val = $1->val % $3->val;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  ;

AddExp
  : MulExp{
    auto ast = new AddExp();
    ast->val = $1->val;
    cout<<"AddExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  | AddExp '+' MulExp{
    // cout<<$1<<endl;
    $1->val = $1->val + $3->val;
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  | AddExp '-' MulExp{
    $1->val = $1->val - $3->val;
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  ;

RelExp
  : AddExp{
    auto ast = new RelExp();
    ast->val = $1->val;
    cout<<"ReLExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  | RelExp LT AddExp{
    if($1->val < $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1; 

  }
  | RelExp GT AddExp{
    if($1->val > $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  | RelExp LE AddExp{
    if($1->val <= $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  | RelExp GE AddExp{
    if($1->val >= $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  ;

EqExp
  : RelExp{
    auto ast = new EqExp();
    ast->val = $1->val;
    cout<<"EqExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  | EqExp EQ RelExp{
    if($1->val == $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  | EqExp NE RelExp{
    if($1->val != $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  ;

LAndExp 
  : EqExp{
    auto ast = new LAndExp();
    ast->val = $1->val;
    cout<<"LAndExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  | LAndExp AND EqExp{
    if($1->val && $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }
  ;

LOrExp
  : LAndExp{
    auto ast = new LOrExp();
    ast->val = $1->val;
    cout<<"LOrExp"<<endl;
    cout<<ast->val<<endl;
    ast->son.push_back($1);
    $$ = ast;
  }
  | LOrExp OR LAndExp{
    if($1->val || $3->val){
      $1->val = 1;
    }else{
      $1->val = 0;
    }
    $1->son.push_back($2);
    $1->son.push_back($3);
    $$ = $1;
  }


%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}