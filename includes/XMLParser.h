/* XML Parser 
Created by Ammar Sheikh
January 2020 */


/* ASSUME XML 1.0  TODO: Encoding ?? 
*/

#ifndef AMMARLIB_XMLPARSER_H
#define AMMARLIB_XMLPARSER_H


#include "Lexer.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
//#define NDEBUG
#include <cassert>
#include <cctype>
#include <stack>
#include <algorithm>
#include <memory>

//TODO: clean up long lines
// ignore optional xml declaration tag
// tree display fuction
// add comments
// add that one constructor below. 

// XMLParser class. Upon construction it opens the provided file. Once "build()"
// is called, an XML DOM tree is built and the internal iterator can be used to 
// iterate over data. 


namespace ammar {

// XMLParser builds the elementTree, and provides  a method for iterating
// over data

class ElementNode;

class XMLParser {

  //Predefined special characters for XML docs.Currently these are specifically
  // chosen to meet the demands of a netxml document.
  static constexpr std::initializer_list<char> identifers {'a', 'b', 'c', 'd',
  'e', 'f', 'g', 'h', 'i', 'j' ,'k', 'l', 'm', 'n', 'o', 'p' ,'q', 'r','s', 
  't', 'u', 'v', 'w', 'x', 'y' ,'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
  'I', 'J', 'K', 'L','M', 'N', 'O', 'P','Q', 'R', 'S', 'T', 'U', 'V', 'W', 
  'X', 'Y', 'Z', '.', '_', '-', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
  '0', ':'};
  static constexpr std::initializer_list<char> operators {'<', '>','=', '/'};
  static constexpr std::initializer_list<char> literals {'\"'};
  static constexpr std::initializer_list<char> differs {'>', '<'};


  // An ElementNode contains information about XML elements.
  struct ElementNode {

    ElementNode(const std::string& element_name, ElementNode* prnt = nullptr) :
              name(element_name), parent(prnt)  {};

    ElementNode(const ElementNode&) = delete;          
   

    std::string ToString();

    // Element Name
    std::string name;

    // Element Key-Value attributes specified in starting tag
    std::unordered_map<std::string, std::string> attributeMap;
  
    // Element Text between start/end tags
    std::string text;

    // Children of Element
    std::vector<std::shared_ptr<ElementNode>> children;

    // Parent of Element
    std::shared_ptr<ElementNode> parent;

  };

  // Holds root to XMLTree
  std::shared_ptr<ElementNode> root; 

  // Handles Tokenization of XML file
  Lexer lexer;

  // Internals functions used during parsing of tokens.
  // Consider making sep static
  void processNewNodeData(std::stack<std::shared_ptr<ElementNode>>& elements);

 public:

  XMLParser(const char* file_name, std::size_t bufferSize = 256) :
  lexer(file_name, XMLParser::operators, XMLParser::identifers, 
  XMLParser::literals, {}, XMLParser::differs, bufferSize), root(nullptr) {}

  //TODO: FINISH
  //XMLParser(const char* file_name, const std::initializer_list<class _E>)
  

  // Builds entire XML DOM Tree. Iteration can be done after calling build().
  void build();
  void displayXMLTree();

  std::vector<std::shared_ptr<ElementNode>> iterate(const std::string& str);
  std::vector<std::shared_ptr<ElementNode>> iterate(const std::string& std,const std::shared_ptr<ElementNode>& rootNode);

};

} // namespace ammar

#endif // AMMARLIB_XMLPARSER_H