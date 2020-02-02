/* Base Lexer Class for Parsing 

  Ammar Sheikh 

  TODO: ADD FULL UTF-8 ENCODING SUPPORT
  Currently only supports data between U+000 to U+007F
  
  This class defines the Lexer implementation. 

  */
#include "Lexer.h"
#include <string>
#include <cstddef>

namespace ammar {

void Lexer::fillBuffer() {
  Buffer.replace(Buffer.begin(), Buffer.end(), Buffer.capacity(), '\0');
  Source.getline(&Buffer[0], Buffer.capacity());
  currentPtr = Buffer.begin();
  endPtr = currentPtr + Buffer.find('\0');
}  

Lexer& Lexer::lex() {
  if(Source.eof() && bufferIsEmpty()) {
    CurrentToken.data = "";
    return *this;
  }

  if(bufferIsEmpty())
    fillBuffer();

  while (!bufferIsEmpty()) {
    if (Operators.count(*currentPtr)) {
      std::size_t increment = captureToken(currentPtr, Operators, 
                                          Token::TokenType::OPERATOR);
      currentPtr += increment;
      break;
    }
    else if (Identifiers.count(*currentPtr)) {
      std::size_t increment = captureToken(currentPtr, Identifiers, 
                                          Token::TokenType::IDENTIFIER);
      currentPtr += increment;
      break;
    }
    else if (Literals.count(*currentPtr)) {
      std::size_t increment = captureToken(currentPtr, Literals, 
                                          Token::TokenType::LITERAL);
      currentPtr += increment;
      break;
    }
    // else if (Keywords.count()) {
    // }
    ++currentPtr;
  }
  return *this;
}

std::size_t Lexer::captureToken(Lexer::BufferPtr startPtr, 
                  const std::unordered_set<char>& set, Token::TokenType t) {
  std::string data;
  CurrentToken.type = t;
  CurrentToken.consumed = false;
  std::size_t jump = 0;
  auto dataContainsDiffers = [&]() {
    for (char c : data) {
      if (Differentiators.count(c)) {
        return true;
      }
    }
    return false;
  };

  while (startPtr < endPtr && set.count(*startPtr)) {
    if (Differentiators.count(*startPtr) && dataContainsDiffers())
        break;
    data += *startPtr;
    ++jump;
    ++startPtr;
  }

  CurrentToken.data = std::move(data);
  return jump;
}



} // namespace ammar

