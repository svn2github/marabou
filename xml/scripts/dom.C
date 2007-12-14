#include <iostream>
#include <fstream>

#include <Riostream.h>
#include <TList.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLAttr.h>

ofstream out;

void ParseContext(TXMLNode *node)
{
   for ( ; node; node = node->GetNextNode()) {
      if (node->GetNodeType() == TXMLNode::kXMLElementNode) { // Element Node
         out << node->GetNodeName() << ": ";
         if (node->HasAttributes()) {
            TList* attrList = node->GetAttributes();
            TIter next(attrList);
            TXMLAttr *attr;
            while ((attr =(TXMLAttr*)next())) {
               out << attr->GetName() << ":" << attr->GetValue();
            }
         }
     }
     if (node->GetNodeType() == TXMLNode::kXMLTextNode) { // Text node
        out << node->GetContent();
     }
     if (node->GetNodeType() == TXMLNode::kXMLCommentNode) { //Comment node
        out << "Comment: " << node->GetContent();
     }

     ParseContext(node->GetChildren());
   }
}


void dom(const Char_t * XmlFile)
{
  TDOMParser *domParser = new TDOMParser();

  domParser->SetValidate(true);
  domParser->ParseFile(XmlFile);

  TXMLNode *node = domParser->GetXMLDocument()->GetRootNode();

  out.open("dom.tree", ios::out);
  ParseContext(node);
  out.close();
}
