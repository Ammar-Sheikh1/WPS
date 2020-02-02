/* Base Lexer Class for Parsing 

  Ammar Sheikh 

  TODO: ADD FULL UTF-8 ENCODING SUPPORT
  Currently only supports data between U+000 to U+007F (ASCII)
  
  This class defines the Lexer interface. 

  */

#ifndef AMMARLIB_UTILS_LEXER_H
#define AMMARLIB_UTILS_LEXER_H

#include <fstream>
#include <unordered_set>
#include <initializer_list>
#include <cassert>
#include <string>
#include <cstddef>

//Todo: Wrap up internal structs- Token access, LexCache
//      finish lex function
//      Write tests
//      Keywords should be std::string right ?

namespace ammar {
  
  // MAYBE CHANGE ACCESS OF TOKEN
struct Token {
  enum class TokenType{
    UNSPECIFIED,
    OPERATOR,
    IDENTIFIER,
    LITERAL,
    KEYWORD,
  };

  std::string data;
  TokenType type;
  bool consumed = false;  
};  

class Lexer {
 public:
  using BufferPtr = std::string::const_iterator;
 private: 

  // LexCache enforces ordering of character searches to 
  // minimize possible unordered_set O(n) searches
  struct LexCache {
    bool first(char c) { return firstSet->count(c); }       // if (Cache.first())
    bool second(char c) { return secondSet->count(c); };    // else if (Cache.second())
    bool third(char c) { return thirdSet->count(c); };      // else if (Cache.third())
    bool fourth(char c) { return fourthSet->count(c); };    // else if (Cache.fourth())

    //LexCache constructor should take in some sort of ordering property
    // Ideas: last used 

    LexCache(const std::unordered_set<char>* one, 
            const std::unordered_set<char>* two, 
            const std::unordered_set<char>* three, 
            const std::unordered_set<char>* four) :
            firstSet(one), secondSet(two), thirdSet(three), fourthSet(four) {}

    LexCache(const LexCache&) = delete;        
   private: 
    const std::unordered_set<char>* firstSet;
    const std::unordered_set<char>* secondSet;
    const std::unordered_set<char>* thirdSet;
    const std::unordered_set<char>* fourthSet;
  };

  // operators, identifiers, literals, keywords, must be initialized
  const std::unordered_set<char> Operators;
  const std::unordered_set<char> Identifiers;
  const std::unordered_set<char> Literals;
  const std::unordered_set<char> Keywords;

  // remedial DS to cope with edge cases such as <iden></iden>
  const std::unordered_set<char> Differentiators;


  std::ifstream Source;
  std::string Buffer;
  BufferPtr currentPtr;
  BufferPtr endPtr;
  Token CurrentToken;
  LexCache Cache;


  bool bufferIsEmpty() const { return currentPtr >= endPtr; }
  void fillBuffer();
  std::size_t captureToken(BufferPtr start, 
                          const std::unordered_set<char>& set, 
                          Token::TokenType t);

 public:

  //const & initializer lists ???
  Lexer(const char* file_name, std::initializer_list<char> opers,
        std::initializer_list<char> idents, 
        std::initializer_list<char> lits,
        std::initializer_list<char> keys, 
        std::initializer_list<char> differs = {}, 
        std::size_t bufferSize = 256) : 
        Source(file_name), Buffer(bufferSize, '\0'), currentPtr(nullptr),
        endPtr(nullptr), Operators(opers), Identifiers(idents), 
        Literals(lits), Keywords(keys), Differentiators(differs),
        Cache(&Operators, &Identifiers, &Literals, &Keywords) { 
          assert(Source.is_open() && "File could not be opened");
        }

  Lexer(const Lexer&) = delete;      

  // gathers next token into CurrentToken
  Lexer& lex();

  // Returns the current token,
  Token getCurrentToken() noexcept { 
    CurrentToken.consumed = true; 
    return CurrentToken; }

  Token::TokenType getCurrentTokenType() const noexcept { 
    return CurrentToken.type; 
  }

  bool lexedAllTokens() const noexcept {return Source.eof() && bufferIsEmpty();}
};

} // namespace ammar

#endif