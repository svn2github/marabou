#ifndef __LEXER_H__
#define __LEXER_H__

/* lexer.h -- Lexer for html parser
  
   (c) 1998-2004 (W3C) MIT, ERCIM, Keio University
   See tidy.h for the copyright notice.
  
   CVS Info:
    $Author: marabou $ 
    $Date: 2004-11-16 15:24:27 $ 
    $Revision: 1.1 $ 

*/

/*
  Given an input source, it returns a sequence of tokens.

     GetToken(source) gets the next token
     UngetToken(source) provides one level undo

  The tags include an attribute list:

    - linked list of attribute/value nodes
    - each node has 2 NULL-terminated strings.
    - entities are replaced in attribute values

  white space is compacted if not in preformatted mode
  If not in preformatted mode then leading white space
  is discarded and subsequent white space sequences
  compacted to single space characters.

  If XmlTags is no then Tag names are folded to upper
  case and attribute names to lower case.

 Not yet done:
    -   Doctype subset and marked sections
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "forward.h"

/* lexer character types
*/
#define digit       1
#define letter      2
#define namechar    4
#define white       8
#define newline     16
#define lowercase   32
#define uppercase   64


/* node->type is one of these values
*/
#define RootNode        0
#define DocTypeTag      1
#define CommentTag      2
#define ProcInsTag      3
#define TextNode        4
#define StartTag        5
#define EndTag          6
#define StartEndTag     7
#define CDATATag        8
#define SectionTag      9
#define AspTag          10
#define JsteTag         11
#define PhpTag          12
#define XmlDecl         13



/* lexer GetToken states
*/
#define LEX_CONTENT     0
#define LEX_GT          1
#define LEX_ENDTAG      2
#define LEX_STARTTAG    3
#define LEX_COMMENT     4
#define LEX_DOCTYPE     5
#define LEX_PROCINSTR   6
#define LEX_ENDCOMMENT  7
#define LEX_CDATA       8
#define LEX_SECTION     9
#define LEX_ASP         10
#define LEX_JSTE        11
#define LEX_PHP         12
#define LEX_XMLDECL     13

/* ParseDocTypeDecl state constants */
#define DT_INTERMEDIATE 0
#define DT_DOCTYPENAME  1
#define DT_PUBLICSYSTEM 2
#define DT_QUOTEDSTRING 3
#define DT_INTSUBSET    4

/* content model shortcut encoding
*/
#define CM_UNKNOWN      0
#define CM_EMPTY        (1 << 0)
#define CM_HTML         (1 << 1)
#define CM_HEAD         (1 << 2)
#define CM_BLOCK        (1 << 3)
#define CM_INLINE       (1 << 4)
#define CM_LIST         (1 << 5)
#define CM_DEFLIST      (1 << 6)
#define CM_TABLE        (1 << 7)
#define CM_ROWGRP       (1 << 8)
#define CM_ROW          (1 << 9)
#define CM_FIELD        (1 << 10)
#define CM_OBJECT       (1 << 11)
#define CM_PARAM        (1 << 12)
#define CM_FRAMES       (1 << 13)
#define CM_HEADING      (1 << 14)
#define CM_OPT          (1 << 15)
#define CM_IMG          (1 << 16)
#define CM_MIXED        (1 << 17)
#define CM_NO_INDENT    (1 << 18)
#define CM_OBSOLETE     (1 << 19)
#define CM_NEW          (1 << 20)
#define CM_OMITST       (1 << 21)

/* If the document uses just HTML 2.0 tags and attributes described
** it as HTML 2.0 Similarly for HTML 3.2 and the 3 flavors of HTML 4.0.
** If there are proprietary tags and attributes then describe it as
** HTML Proprietary. If it includes the xml-lang or xmlns attributes
** but is otherwise HTML 2.0, 3.2 or 4.0 then describe it as one of the
** flavors of Voyager (strict, loose or frameset).
*/

/* unknown */
#define xxxx                   0u

/* W3C defined HTML/XHTML family document types */
#define HT20                   1u
#define HT32                   2u
#define H40S                   4u
#define H40T                   8u
#define H40F                  16u
#define H41S                  32u
#define H41T                  64u
#define H41F                 128u
#define X10S                 256u
#define X10T                 512u
#define X10F                1024u
#define XH11                2048u
#define XB10                4096u

/* proprietary stuff */
#define VERS_SUN            8192u
#define VERS_NETSCAPE      16384u
#define VERS_MICROSOFT     32768u

/* special flag */
#define VERS_XML           65536u

/* compatibility symbols */
#define VERS_UNKNOWN       (xxxx)
#define VERS_HTML20        (HT20)
#define VERS_HTML32        (HT32)
#define VERS_HTML40_STRICT (H40S|H41S|X10S)
#define VERS_HTML40_LOOSE  (H40T|H41T|X10T)
#define VERS_FRAMESET      (H40F|H41F|X10F)
#define VERS_XHTML11       (XH11)
#define VERS_BASIC         (XB10)

/* meta symbols */
#define VERS_HTML40        (VERS_HTML40_STRICT|VERS_HTML40_LOOSE|VERS_FRAMESET)
#define VERS_IFRAME        (VERS_HTML40_LOOSE|VERS_FRAMESET)
#define VERS_LOOSE         (VERS_HTML20|VERS_HTML32|VERS_IFRAME)
#define VERS_EVENTS        (VERS_HTML40|VERS_XHTML11)
#define VERS_FROM32        (VERS_HTML32|VERS_HTML40)
#define VERS_FROM40        (VERS_HTML40|VERS_XHTML11|VERS_BASIC)
#define VERS_XHTML         (X10S|X10T|X10F|XH11|XB10)

/* all W3C defined document types */
#define VERS_ALL           (VERS_HTML20|VERS_HTML32|VERS_FROM40)

/* all proprietary types */
#define VERS_PROPRIETARY   (VERS_NETSCAPE|VERS_MICROSOFT|VERS_SUN)

/* Linked list of class names and styles
*/
struct _Style;
typedef struct _Style TagStyle;

struct _Style
{
    tmbstr tag;
    tmbstr tag_class;
    tmbstr properties;
    TagStyle *next;
};


/* Linked list of style properties
*/
struct _StyleProp;
typedef struct _StyleProp StyleProp;

struct _StyleProp
{
    tmbstr name;
    tmbstr value;
    StyleProp *next;
};




/* Attribute/Value linked list node
*/

struct _AttVal
{
    AttVal*           next;
    const Attribute*  dict;
    Node*             asp;
    Node*             php;
    int               delim;
    tmbstr            attribute;
    tmbstr            value;
};



/*
  Mosaic handles inlines via a separate stack from other elements
  We duplicate this to recover from inline markup errors such as:

     <i>italic text
     <p>more italic text</b> normal text

  which for compatibility with Mosaic is mapped to:

     <i>italic text</i>
     <p><i>more italic text</i> normal text

  Note that any inline end tag pop's the effect of the current
  inline start tag, so that </b> pop's <i> in the above example.
*/
struct _IStack
{
    IStack*     next;
    const Dict* tag;        /* tag's dictionary definition */
    tmbstr      element;    /* name (NULL for text nodes) */
    AttVal*     attributes;
};


/* HTML/XHTML/XML Element, Comment, PI, DOCTYPE, XML Decl,
** etc. etc.
*/

struct _Node
{
    Node*       parent;         /* tree structure */
    Node*       prev;
    Node*       next;
    Node*       content;
    Node*       last;

    AttVal*     attributes;
    const Dict* was;            /* old tag when it was changed */
    const Dict* tag;            /* tag's dictionary definition */

    tmbstr      element;        /* name (NULL for text nodes) */

    uint        start;          /* start of span onto text array */
    uint        end;            /* end of span onto text array */
    uint        type;           /* TextNode, StartTag, EndTag etc. */

    uint        line;           /* current line of document */
    uint        column;         /* current column of document */

    Bool        closed;         /* true if closed by explicit end tag */
    Bool        implicit;       /* true if inferred */
    Bool        linebreak;      /* true if followed by a line break */

#ifdef TIDY_STORE_ORIGINAL_TEXT
    tmbstr      otext;
#endif
};


/*
  The following are private to the lexer
  Use NewLexer() to create a lexer, and
  FreeLexer() to free it.
*/

struct _Lexer
{
#if 0  /* Move to TidyDocImpl */
    StreamIn* in;           /* document content input */
    StreamOut* errout;      /* error output stream */

    uint badAccess;         /* for accessibility errors */
    uint badLayout;         /* for bad style errors */
    uint badChars;          /* for bad character encodings */
    uint badForm;           /* for mismatched/mispositioned form tags */
    uint warnings;          /* count of warnings in this document */
    uint errors;            /* count of errors */
#endif

    uint lines;             /* lines seen */
    uint columns;           /* at start of current token */
    Bool waswhite;          /* used to collapse contiguous white space */
    Bool pushed;            /* true after token has been pushed back */
    Bool insertspace;       /* when space is moved after end tag */
    Bool excludeBlocks;     /* Netscape compatibility */
    Bool exiled;            /* true if moved out of table */
    Bool isvoyager;         /* true if xmlns attribute on html element */
    uint versions;          /* bit vector of HTML versions */
    int  doctype;           /* version as given by doctype (if any) */
    Bool bad_doctype;       /* e.g. if html or PUBLIC is missing */
    uint txtstart;          /* start of current node */
    uint txtend;            /* end of current node */
    uint state;             /* state of lexer's finite state machine */

    Node* token;            /* current parse point */
    Node* root;             /* remember root node of the document */
    Node* parent;           /* remember parent node for CDATA elements */
    
    Bool seenEndBody;       /* true if a </body> tag has been encountered */
    Bool seenEndHtml;       /* true if a </html> tag has been encountered */

    /*
      Lexer character buffer

      Parse tree nodes span onto this buffer
      which contains the concatenated text
      contents of all of the elements.

      lexsize must be reset for each file.
    */
    tmbstr lexbuf;          /* MB character buffer */
    uint lexlength;         /* allocated */
    uint lexsize;           /* used */

    /* Inline stack for compatibility with Mosaic */
    Node* inode;            /* for deferring text node */
    IStack* insert;         /* for inferring inline tags */
    IStack* istack;
    uint istacklength;      /* allocated */
    uint istacksize;        /* used */
    uint istackbase;        /* start of frame */

    TagStyle *styles;          /* used for cleaning up presentation markup */

#if 0
    TidyDocImpl* doc;       /* Pointer back to doc for error reporting */
#endif 
};


/* Lexer Functions
*/
Node *CommentToken( Lexer *lexer );

/* choose what version to use for new doctype */
int HTMLVersion( TidyDocImpl* doc );

ctmbstr GetFPIFromVers(uint vers);

/* everything is allowed in proprietary version of HTML */
/* this is handled here rather than in the tag/attr dicts */

void ConstrainVersion( TidyDocImpl* doc, uint vers );

Bool IsWhite(uint c);
Bool IsDigit(uint c);
Bool IsLetter(uint c);
Bool IsNewline(uint c);
Bool IsNamechar(uint c);
Bool IsXMLLetter(uint c);
Bool IsXMLNamechar(uint c);

Bool IsLower(uint c);
Bool IsUpper(uint c);
uint ToLower(uint c);
uint ToUpper(uint c);

char FoldCase( TidyDocImpl* doc, tmbchar c, Bool tocaps );


Lexer* NewLexer( TidyDocImpl* doc );
Bool EndOfInput( TidyDocImpl* doc );
void FreeLexer( TidyDocImpl* doc );

/* store character c as UTF-8 encoded byte stream */
void AddCharToLexer( Lexer *lexer, uint c );

/*
  Used for elements and text nodes
  element name is NULL for text nodes
  start and end are offsets into lexbuf
  which contains the textual content of
  all elements in the parse tree.

  parent and content allow traversal
  of the parse tree in any direction.
  attributes are represented as a linked
  list of AttVal nodes which hold the
  strings for attribute/value pairs.
*/
Node* NewNode( Lexer* lexer );


/* used to clone heading nodes when split by an <HR> */
Node *CloneNode( TidyDocImpl* doc, Node *element );

/* free node's attributes */
void FreeAttrs( TidyDocImpl* doc, Node *node );

/* doesn't repair attribute list linkage */
void FreeAttribute( TidyDocImpl* doc, AttVal *av );

/* remove attribute from node then free it
*/
void RemoveAttribute( TidyDocImpl* doc, Node *node, AttVal *attr );

/*
  Free document nodes by iterating through peers and recursing
  through children. Set next to NULL before calling FreeNode()
  to avoid freeing peer nodes. Doesn't patch up prev/next links.
 */
void FreeNode( TidyDocImpl* doc, Node *node );

Node* TextToken( Lexer *lexer );

/* used for creating preformatted text from Word2000 */
Node *NewLineNode( Lexer *lexer );

/* used for adding a &nbsp; for Word2000 */
Node *NewLiteralTextNode(Lexer *lexer, ctmbstr txt );

Node* CommentToken(Lexer *lexer);
Node* GetCDATA( TidyDocImpl* doc, Node *container );

void AddByte( Lexer *lexer, tmbchar c );
void AddStringLiteral( Lexer* lexer, ctmbstr str );
void AddStringLiteralLen( Lexer* lexer, ctmbstr str, int len );

/* find element */
Node* FindDocType( TidyDocImpl* doc );
Node* FindHTML( TidyDocImpl* doc );
Node* FindHEAD( TidyDocImpl* doc );
Node* FindTITLE(TidyDocImpl* doc);
Node* FindBody( TidyDocImpl* doc );
Node* FindXmlDecl(TidyDocImpl* doc);

/* Returns containing block element, if any */
Node* FindContainer( Node* node );

/* add meta element for Tidy */
Bool AddGenerator( TidyDocImpl* doc );

/* examine <!DOCTYPE> to identify version */
int FindGivenVersion( TidyDocImpl* doc, Node* doctype );
int ApparentVersion( TidyDocImpl* doc );


Bool CheckDocTypeKeyWords(Lexer *lexer, Node *doctype);

ctmbstr HTMLVersionName( TidyDocImpl* doc );
ctmbstr HTMLVersionNameFromCode( uint vers, Bool isXhtml );

Bool SetXHTMLDocType( TidyDocImpl* doc );


/* fixup doctype if missing */
Bool FixDocType( TidyDocImpl* doc );

/* ensure XML document starts with <?xml version="1.0"?> */
/* add encoding attribute if not using ASCII or UTF-8 output */
Bool FixXmlDecl( TidyDocImpl* doc );

Node* InferredTag(TidyDocImpl* doc, TidyTagId id);

Bool ExpectsContent(Node *node);


void UngetToken( TidyDocImpl* doc );


/*
  modes for GetToken()

  MixedContent   -- for elements which don't accept PCDATA
  Preformatted   -- white space preserved as is
  IgnoreMarkup   -- for CDATA elements such as script, style
*/
#define IgnoreWhitespace    0
#define MixedContent        1
#define Preformatted        2
#define IgnoreMarkup        3
#define CdataContent        4

Node* GetToken( TidyDocImpl* doc, uint mode );

void InitMap(void);

Bool IsValidAttrName( ctmbstr attr );


/* create a new attribute */
AttVal *NewAttribute(void);

/* create a new attribute with given name and value */
AttVal *NewAttributeEx(ctmbstr name, ctmbstr value);

/*************************************
  In-line Stack functions
*************************************/


/* duplicate attributes */
AttVal* DupAttrs( TidyDocImpl* doc, AttVal* attrs );

/*
  push a copy of an inline node onto stack
  but don't push if implicit or OBJECT or APPLET
  (implicit tags are ones generated from the istack)

  One issue arises with pushing inlines when
  the tag is already pushed. For instance:

      <p><em>text
      <p><em>more text

  Shouldn't be mapped to

      <p><em>text</em></p>
      <p><em><em>more text</em></em>
*/
void PushInline( TidyDocImpl* doc, Node* node );

/* pop inline stack */
void PopInline( TidyDocImpl* doc, Node* node );

Bool IsPushed( TidyDocImpl* doc, Node* node );

/*
  This has the effect of inserting "missing" inline
  elements around the contents of blocklevel elements
  such as P, TD, TH, DIV, PRE etc. This procedure is
  called at the start of ParseBlock. when the inline
  stack is not empty, as will be the case in:

    <i><h1>italic heading</h1></i>

  which is then treated as equivalent to

    <h1><i>italic heading</i></h1>

  This is implemented by setting the lexer into a mode
  where it gets tokens from the inline stack rather than
  from the input stream.
*/
int InlineDup( TidyDocImpl* doc, Node *node );

/*
 defer duplicates when entering a table or other
 element where the inlines shouldn't be duplicated
*/
void DeferDup( TidyDocImpl* doc );
Node *InsertedToken( TidyDocImpl* doc );

#ifdef __cplusplus
}
#endif


#endif /* __LEXER_H__ */
