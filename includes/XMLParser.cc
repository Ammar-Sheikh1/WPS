/* XML Parser 
Created by Ammar Sheikh
January 2020 */


/* ASSUME XML 1.0  TODO: Encoding ?? 
*/

#include "XMLParser.h"

#include <algorithm>
#include <stack>
#include <memory>
#include <queue>

#include <iostream>

//TODO: see .h

namespace ammar {

std::string XMLParser::ElementNode::ToString() {
  auto element = "Name: "+ name  + " Text: " + text + " Attributes: ";
  std::for_each(attributeMap.begin(),
                attributeMap.end(), [&](std::pair<const std::string,std::string>& p) {
                element += "{"+ p.first + ":" + p.second + "} "; });
  return element;
}

void XMLParser::build() {
  std::stack<std::shared_ptr<ElementNode>> elements;

  while (!lexer.lexedAllTokens()) {
    lexer.lex();
    if (lexer.getCurrentToken().data == "<") {
      processNewNodeData(elements);
    }

    if (lexer.getCurrentToken().type == Token::TokenType::IDENTIFIER) {
      // add text to newest node, may be multiple tokens
      //processCurrentNodeText(elements);
      elements.top()->text += lexer.getCurrentToken().data;
    }
    if (lexer.getCurrentToken().data == "</") {
      //Close newest node. Move to end of closing tag ">"
      elements.pop();
      while (lexer.lex().getCurrentToken().data != ">") {}
  
    }
  }
}


// Call when current Token is an opening tag. Creates a new element node and  
// initializes it with its name and potential attributes. Note: Moves Lexer 
// state to end of opening tag. ">"
void XMLParser::processNewNodeData(std::stack<std::shared_ptr<ElementNode>>& elements) {
    // Move to Token after opening tag
    lexer.lex();
    if (lexer.getCurrentToken().data == ">")
      return;

    //Create new node and link to tree
    auto addNode = std::make_shared<ElementNode>(lexer.getCurrentToken().data);

    if (!root) {
      root = addNode;
      elements.push(root);
    }
    else {
      addNode->parent = elements.top();
      elements.top()->children.push_back(addNode);
      elements.push(addNode);
    }

    // Add potential attributes to attributeMap
    
    lexer.lex();
    while (lexer.getCurrentToken().data != ">") {
      //get attribute Key
      //std::pair<std::string,std::string> pair;
      std::string Key;
      // Assign token before "=" as the attribute Key
      while (lexer.getCurrentToken().data != "=" ) {
        Key = lexer.getCurrentToken().data;
        lexer.lex();
      }

      // past "="
      lexer.lex();

      //get Value
      if (lexer.getCurrentToken().data == "\"") {
        lexer.lex();
        auto Value = lexer.getCurrentToken().data;
        lexer.lex();

        // Append every token between quotes. Accounts for split tokens
        // via spaces
        while (lexer.getCurrentToken().data != "\"") {
          Value += lexer.getCurrentToken().data;
          lexer.lex();
        }

        // Add to node
        addNode->attributeMap[Key] = Value;
        lexer.lex();
      }
      // Advance until end of starting tag is reached
    }
}


// DFS display of tree
void XMLParser::displayXMLTree() {
  if (!root)
    return;
  std::stack<std::shared_ptr<ElementNode>> s;
  s.push(root);
  while (!s.empty()) {
   auto node = s.top();
    s.pop();
    for (const auto child : node->children)
      s.push(child);

    std::cout << node->ToString() << "\n";
  }
}


//Given a path to an element, generate a list of all Elements TODO: <--- FIX
// Ex "root/child1/child2/child3" -> {child3, child3, child3}
std::vector<std::shared_ptr<XMLParser::ElementNode>> XMLParser::iterate (const std::string& path) {
  return iterate(path, root);
}


std::vector<std::shared_ptr<XMLParser::ElementNode>> 
XMLParser::iterate (const std::string& path, const std::shared_ptr<XMLParser::ElementNode>& rootElement) {
  assert(path.length() > 0);
  assert(rootElement != nullptr);

  // Check Root Node
  std::size_t start = 0;
  std::size_t end = path.find('/');
  if (path.compare(start, end-start, rootElement->name)) {  // If not equivalent return
    return {};
  }

  start = end + 1;
  end = path.find('/', start);

  std::queue<std::shared_ptr<ElementNode>> elementQ;
  elementQ.push(rootElement);

  while (!elementQ.empty()) {
    auto element = elementQ.front();
    elementQ.pop();
    
    //For all children, add if it matches current path 
    for (const auto& children : element->children) {
      if (!path.compare(start, end-start, children->name)){
        elementQ.push(children);
      }
    }

    // Go to next path token, only when we have finished
    // adding all possible elements of that token
    // which occurs, when the next elem.name == currentToken
    if (!elementQ.empty() && !path.compare(start, end-start, elementQ.front()->name)) {
      if (end == std::string::npos)
        break;
      start = end + 1;
      end = path.find('/', start);
    }
  }
  std::vector<std::shared_ptr<ElementNode>> elementList;
  while (!elementQ.empty()) {
    elementList.push_back(elementQ.front());
    elementQ.pop();
  }
  return elementList;
}

} // namespace ammar