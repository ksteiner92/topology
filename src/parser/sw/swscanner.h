#ifndef SWSCANNER_H
#define SWSCANNER_H

//#define yyFlexLexer KPathFlexLexer
//#include <FlexLexer.h>
//#undef yyFlexLexer

/**
 * Generated Flex class name is yyFlexLexer by default. If we want to use more flex-generated
 * classes we should name them differently. See scanner.l prefix option.
 * 
 * Unfortunately the implementation relies on this trick with redefining class name
 * with a preprocessor macro. See GNU Flex manual, "Generating C++ Scanners" section
 */
//#ifndef __FLEX_LEXER_H
#undef yyFlexLexer
#define yyFlexLexer _SWFlexLexer

#include <FlexLexer.h>
//#endif
// Scanner method signature is defined by this macro. Original yylex() returns int.
// Sinice Bison 3 uses symbol_type, we must change returned type. We also rename it
// to something sane, since you cannot overload return type.
#undef YY_DECL
#define YY_DECL ks_sw::SWParser::symbol_type ks_sw::SWScanner::getNextToken()

#include "swparser.hh" // this is needed for symbol_type

namespace ks_sw {

// Forward declare interpreter to avoid include. Header is added inimplementation file.
class SelfEnergy;

class SWScanner : public _SWFlexLexer {
public:
   SWScanner(SelfEnergy &driver) :
           driver(driver) {
   }

   virtual ~SWScanner() {
   }

   virtual ks_sw::SWParser::symbol_type getNextToken();

private:
   SelfEnergy &driver;
};

}

#endif
