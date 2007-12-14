#include <iostream>
#include <fstream>
#include <Riostream.h>
#include <TList.h>
#include <TSAXParser.h>
#include <TXMLAttr.h>

ofstream out;

class SaxHandler {
public:
   SaxHandler() { }

   void     OnStartDocument() { }
   void     OnEndDocument();
   void     OnStartElement(const char*, const TList*);
   void     OnEndElement(const char*);
   void     OnCharacters(const char*);
   void     OnComment(const char*);
   void     OnWarning(const char*);
   void     OnError(const char*);
   void     OnFatalError(const char*);
   void     OnCdataBlock(const char*, Int_t);
};

void SaxHandler::OnEndDocument()
{
   out << endl;
}

void SaxHandler::OnStartElement(const char *name, const TList *attributes)
{
   out << "<" << name;

   TXMLAttr *attr;

   TIter next(attributes);
   while ((attr = (TXMLAttr*) next())) {
      out << " " << attr->GetName() << "=\"" << attr->GetValue() << "\"";
   }

   out  << ">";
}

void SaxHandler::OnEndElement(const char *name)
{
   out << "</" << name << ">";
}

void SaxHandler::OnCharacters(const char *characters)
{
   out << characters;
}

void SaxHandler::OnComment(const char *text)
{
   out << "<!--" << text << "-->";
}

void SaxHandler::OnWarning(const char *text)
{
   cerr << "Warning: " << text << endl;
}

void SaxHandler::OnError(const char *text)
{
   cerr << "Error: " << text << endl ;
}

void SaxHandler::OnFatalError(const char *text)
{
   cerr << "FatalError: " << text << endl ;
}

void SaxHandler::OnCdataBlock(const char *text, Int_t len)
{
   out << "OnCdataBlock() " << text;
}



void sax(const Char_t * XmlFile)
{
   TSAXParser *saxParser = new TSAXParser();
   SaxHandler *saxHandler = new SaxHandler();

   saxParser->ConnectToHandler("SaxHandler", saxHandler);
   out.open("sax.tree", ios::out);
   saxParser->ParseFile(XmlFile);
   out.close();
}
