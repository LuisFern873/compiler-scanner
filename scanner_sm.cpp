#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <unordered_map>
using namespace std;

/*
  Autores:
  Luis Méndez (202110216)
  Jean Sotomayor (202110660)
*/

// Incluir estructura de datos que permita la busqueda de las palabras reservadas

class Token {
public:
  enum Type { 
    ID, LABEL, NUM, EOL, ERR, END, PUSH, JMPEQ, 
    JMPGT, JMPGE, JMPLT, JMPLE, SKIP, POP, DUP, 
    SWAP, ADD, SUB, MULT, DIV, STORE, LOAD, GOTO
  };
  static const char* token_names[23]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string source);
};

const char* Token::token_names[23] = {
  "ID", "LABEL", "NUM", "EOL", "ERR", "END", "PUSH", "JMPEQ", 
  "JMPGT", "JMPGE", "JMPLT", "JMPLE", "SKIP", "POP", "DUP", 
  "SWAP", "ADD", "SUB", "MULT", "DIV", "STORE", "LOAD", "GOTO"
};

Token::Token(Type type):type(type) { lexema = ""; }

Token::Token(Type type, char c):type(type) { lexema = c; }

Token::Token(Type type, const string source): type(type) { lexema = source; }

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
  if (tok.lexema.empty())
    return outs << Token::token_names[tok.type];
  else
    return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
  return outs << *tok;
}



class Scanner {
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();  
private:
  string input;
  int first, current;
  int state;
  char nextChar();
  void rollBack();
  void startLexema();
  void incrStartLexema();
  string getLexema();
  unordered_map<string,Token::Type> keywords = {
    {"push", Token::PUSH},
    {"jmpeq", Token::JMPEQ},
    {"jmpgt", Token::JMPGT},
    {"jmpge", Token::JMPGE},
    {"jmplt", Token::JMPLT},
    {"jmple", Token::JMPLE},
    {"skip", Token::SKIP},
    {"pop", Token::POP},
    {"dup", Token::DUP},
    {"swap", Token::SWAP},
    {"add", Token::ADD},
    {"sub", Token::SUB},
    {"mult", Token::MULT},
    {"div", Token::DIV},
    {"store", Token::STORE},
    {"load", Token::LOAD},
    {"goto", Token::GOTO}
  };
};

Scanner::Scanner(const char* s):input(s),first(0),current(0) { }

Token* Scanner::nextToken() {

  char c;
  state = 0;
  startLexema();

  while (1) {
    switch (state) { 
      case 0: c = nextChar();
        if (c == ' ') { incrStartLexema(); state = 0; }
        else if (isalpha(c)) { state = 1; }
        else if (isdigit(c)) { state = 4; }
        else if (c == '\n') { state = 6; }
        else if (c == '\0') return new Token(Token::END);
        else return new Token(Token::ERR, c);
        break;
      case 1: c = nextChar();
        if (isalpha(c) || isdigit(c)) state = 1;
        else if (c == ':') state = 3;
        else state = 2;
        break;
      case 2: rollBack();
        if (keywords.find(getLexema()) != keywords.end()) {
            auto tokenName = keywords[getLexema()];
            return new Token(tokenName);
        } else {
          return new Token(Token::ID, getLexema());
        }
      case 3: 
        return new Token(Token::LABEL, getLexema().substr(0, getLexema().length() - 1));
      case 4: c = nextChar();
        if (isdigit(c)) state = 4;
        else state = 5;
        break;
      case 5: rollBack();
        return new Token(Token::NUM, getLexema());
      case 6: c = nextChar();
        if (c == '\n') state = 6;
        else state = 7;
        break;
      case 7:
        return new Token(Token::EOL);
      default: return NULL;
    }
  }
}

Scanner::~Scanner() { }

char Scanner::nextChar() {
  int c = input[current];
  if (c != '\0') current++;
  return c;
}

void Scanner::rollBack() { // retract
  if (input[current] != '\0')
    current--;
}

void Scanner::startLexema() {
  first = current;  
  return;
}

void Scanner::incrStartLexema() {
  first++;
}


string Scanner::getLexema() {
  return input.substr(first,current-first);
}





// ---------------------------------------------------

int main(int argc, const char* argv[]) {
  
  if (argc != 2) {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  // Cambiar:
  // input es ahora el nombre de archivo con las instruccion de la SM
  

  Scanner scanner(argv[1]);
  Token* tk = scanner.nextToken();
  while (tk->type != Token::END) {
    cout << "next token " << tk << endl;
    delete tk;
    tk =  scanner.nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;

}


