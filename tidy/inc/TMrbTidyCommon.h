#ifndef __TMrbTidyCommon_h__
#define __TMrbTidyCommon_h__

#include "Rtypes.h"

#include "TNamed.h"
#include "TString.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

#include "tidyenum.h"

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTidyCommon.h
// Purpose:        Common defs for tidy interface
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbTidyCommon.h,v 1.5 2005-11-17 09:53:48 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

const SMrbNamedXShort kMrbTidyNodeTypes[] =
						{
							{TidyNode_Root, 		"Root"  				},
							{TidyNode_DocType,		"DOCTYPE"				},
							{TidyNode_Comment,		"Comment"				},
							{TidyNode_ProcIns,		"ProcessingInstruction"	},
							{TidyNode_Text, 		"Text"					},
							{TidyNode_Start,		"StartTag"	 			},
							{TidyNode_End,			"EndTag"				},
							{TidyNode_StartEnd, 	"Start/End" 		 	},
							{TidyNode_CDATA,		"UnparsedText"			},
							{TidyNode_Section,		"XMLsection"			},
							{TidyNode_Asp,			"ASPsource" 			},
							{TidyNode_Jste, 		"JSTEsource"			},
							{TidyNode_Php,			"PHPsource" 			},
							{TidyNode_XmlDecl,		"XMLdeclaration"		},
							{0, 					NULL					}
						};

enum					{	TidyTag_MNODE = 200 	};

const SMrbNamedXShort kMrbTidyTagIds[] =
						{
							{TidyTag_UNKNOWN,		"UNKNOWN"		},
							{TidyTag_A, 			"A" 	},
							{TidyTag_ABBR,			"ABBR"		},
							{TidyTag_ACRONYM,		"ACRONYM"		},
							{TidyTag_ADDRESS,		"ADDRESS"		},
							{TidyTag_ALIGN, 		"ALIGN" 	},
							{TidyTag_APPLET,		"APPLET"		},
							{TidyTag_AREA,			"AREA"		},
							{TidyTag_B, 			"B" 	},
							{TidyTag_BASE,			"BASE"		},
							{TidyTag_BASEFONT,		"BASEFONT"		},
							{TidyTag_BDO,			"BDO"		},
							{TidyTag_BGSOUND,		"BGSOUND"		},
							{TidyTag_BIG,			"BIG"		},
							{TidyTag_BLINK, 		"BLINK" 	},
							{TidyTag_BLOCKQUOTE,	"BLOCKQUOTE"		},
							{TidyTag_BODY,			"BODY"		},
							{TidyTag_BR,			"BR"		},
							{TidyTag_BUTTON,		"BUTTON"		},
							{TidyTag_CAPTION,		"CAPTION"		},
							{TidyTag_CENTER,		"CENTER"		},
							{TidyTag_CITE,			"CITE"		},
							{TidyTag_CODE,			"CODE"		},
							{TidyTag_COL,			"COL"		},
							{TidyTag_COLGROUP,		"COLGROUP"		},
							{TidyTag_COMMENT,		"COMMENT"		},
							{TidyTag_DD,			"DD"		},
							{TidyTag_DEL,			"DEL"		},
							{TidyTag_DFN,			"DFN"		},
							{TidyTag_DIR,			"DIR"		},
							{TidyTag_DIV,			"DIV"		},
							{TidyTag_DL,			"DL"		},
							{TidyTag_DT,			"DT"		},
							{TidyTag_EM,			"EM"		},
							{TidyTag_EMBED, 		"EMBED" 	},
							{TidyTag_FIELDSET,		"FIELDSET"		},
							{TidyTag_FONT,			"FONT"		},
							{TidyTag_FORM,			"FORM"		},
							{TidyTag_FRAME, 		"FRAME" 	},
							{TidyTag_FRAMESET,		"FRAMESET"		},
							{TidyTag_H1,			"H1"		},
							{TidyTag_H2,			"H2"		},
							{TidyTag_H3,			"H3"		},
							{TidyTag_H4,			"H4"		},
							{TidyTag_H5,			"H5"		},
							{TidyTag_H6,			"H6"		},
							{TidyTag_HEAD,			"HEAD"		},
							{TidyTag_HR,			"HR"		},
							{TidyTag_HTML,			"HTML"		},
							{TidyTag_I, 			"I" 	},
							{TidyTag_IFRAME,		"IFRAME"		},
							{TidyTag_ILAYER,		"ILAYER"		},
							{TidyTag_IMG,			"IMG"		},
							{TidyTag_INPUT, 		"INPUT" 	},
							{TidyTag_INS,			"INS"		},
							{TidyTag_ISINDEX,		"ISINDEX"		},
							{TidyTag_KBD,			"KBD"		},
							{TidyTag_KEYGEN,		"KEYGEN"		},
							{TidyTag_LABEL, 		"LABEL" 	},
							{TidyTag_LAYER, 		"LAYER" 	},
							{TidyTag_LEGEND,		"LEGEND"		},
							{TidyTag_LI,			"LI"		},
							{TidyTag_LINK,			"LINK"		},
							{TidyTag_LISTING,		"LISTING"		},
							{TidyTag_MAP,			"MAP"		},
							{TidyTag_MARQUEE,		"MARQUEE"		},
							{TidyTag_MENU,			"MENU"		},
							{TidyTag_META,			"META"		},
							{TidyTag_MULTICOL,		"MULTICOL"		},
							{TidyTag_NOBR,			"NOBR"		},
							{TidyTag_NOEMBED,		"NOEMBED"		},
							{TidyTag_NOFRAMES,		"NOFRAMES"		},
							{TidyTag_NOLAYER,		"NOLAYER"		},
							{TidyTag_NOSAVE,		"NOSAVE"		},
							{TidyTag_NOSCRIPT,		"NOSCRIPT"		},
							{TidyTag_OBJECT,		"OBJECT"		},
							{TidyTag_OL,			"OL"		},
							{TidyTag_OPTGROUP,		"OPTGROUP"		},
							{TidyTag_OPTION,		"OPTION"		},
							{TidyTag_P, 			"P" 	},
							{TidyTag_PARAM, 		"PARAM" 	},
							{TidyTag_PLAINTEXT, 	"PLAINTEXT" 	},
							{TidyTag_PRE,			"PRE"		},
							{TidyTag_Q, 			"Q" 	},
							{TidyTag_RB,			"RB"		},
							{TidyTag_RBC,			"RBC"		},
							{TidyTag_RP,			"RP"		},
							{TidyTag_RT,			"RT"		},
							{TidyTag_RTC,			"RTC"		},
							{TidyTag_RUBY,			"RUBY"		},
							{TidyTag_S, 			"S" 	},
							{TidyTag_SAMP,			"SAMP"		},
							{TidyTag_SCRIPT,		"SCRIPT"		},
							{TidyTag_SELECT,		"SELECT"		},
							{TidyTag_SERVER,		"SERVER"		},
							{TidyTag_SERVLET,		"SERVLET"		},
							{TidyTag_SMALL, 		"SMALL" 	},
							{TidyTag_SPACER,		"SPACER"		},
							{TidyTag_SPAN,			"SPAN"		},
							{TidyTag_STRIKE,		"STRIKE"		},
							{TidyTag_STRONG,		"STRONG"		},
							{TidyTag_STYLE, 		"STYLE" 	},
							{TidyTag_SUB,			"SUB"		},
							{TidyTag_SUP,			"SUP"		},
							{TidyTag_TABLE, 		"TABLE" 	},
							{TidyTag_TBODY, 		"TBODY" 	},
							{TidyTag_TD,			"TD"		},
							{TidyTag_TEXTAREA,		"TEXTAREA"	},
							{TidyTag_TFOOT, 		"TFOOT" 	},
							{TidyTag_TH,			"TH"		},
							{TidyTag_THEAD, 		"THEAD" 	},
							{TidyTag_TITLE, 		"TITLE" 	},
							{TidyTag_TR,			"TR"		},
							{TidyTag_TT,			"TT"		},
							{TidyTag_U, 			"U" 		},
							{TidyTag_UL,			"UL"		},
							{TidyTag_VAR,			"VAR"		},
							{TidyTag_WBR,			"WBR"		},
							{TidyTag_XMP,			"XMP"		},
							{TidyTag_MNODE, 		"MNODE"		},
							{0, 					NULL		}
						};

enum					{	TidyTag_MNODE_MH  = TidyTag_MNODE,
							TidyTag_MNODE_MB,
							TidyTag_MNODE_MX,
							TidyTag_MNODE_MC,
							TidyTag_MNODE_MI,
							TidyTag_MNODE_MS
						};

const SMrbNamedX kMrbTidyMnodeIds[] =
						{
							{TidyTag_MNODE_MH,		"mh",		"header"		},
							{TidyTag_MNODE_MB,		"mb",		"body"			},
							{TidyTag_MNODE_MX,		"mx",		"expand"		},
							{TidyTag_MNODE_MC,		"mc",		"code"			},
							{TidyTag_MNODE_MI,		"mi",		"include"		},
							{TidyTag_MNODE_MS,		"ms",		"switch"		},
							{0, 					NULL,		NULL			}
						};

enum					{	TidyLink_Marabou	=	1,
							TidyLink_Root,
							TidyLink_Others
						};

const SMrbNamedX kMrbTidyLinkTypes[] =
						{
							{TidyLink_Marabou, 		"mlinks",	"http://www.bl.physik.uni-muenchen.de/marabou/html/"	},
							{TidyLink_Root, 		"rlinks",	"http://root.cern.ch/root/htmldoc/"	},
							{TidyLink_Others,		"olinks"	"http://other.link.somewhere/"	},
							{0, 					NULL,		NULL							}
						};

#endif
