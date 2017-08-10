%{
	#include <iostream>
	#include <cstdlib>
	#include <climits>
	#include <complex>
	#include <limits.h>
	#include "confscanner.h"
	#include "config.h"
	#include "confparser.hh"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() ks_conf::ConfigParser::make_END(location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION driver.increaseLocation(yyleng);
%}

%option nodefault
%option noyywrap
%option c++
%option prefix="Config"
%option yyclass="ConfigFlexLexer"

Identifier              [a-zA-Z][a-zA-Z_0-9]*
RealNumber              [-+]?[0-9]*\.?[0-9]*([eE][-+]?[0-9]+)?
ImagNumber				[-+]?[0-9]*\.?[0-9]*([eE][-+]?[0-9]+)?i
Blank                   [ \t]
UnicodeEscapeSequence   u[0-9A-Fa-f]{4}
EscapeCharacter         ["\\/bfnrt]
EscapeSequence          {EscapeCharacter}|{UnicodeEscapeSequence}
StringCharacter         [^"\\]|\\{EscapeSequence}
String                  \"{StringCharacter}*\"
MultilineComment		[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]
LineComment				"//".*

%%

{Identifier} {
	return ks_conf::ConfigParser::make_ID(yytext, location());
};

{String} {
	return ks_conf::ConfigParser::make_STRING(yytext, location());
};

"=" return ks_conf::ConfigParser::make_ASSIGN(location());

";" return ks_conf::ConfigParser::make_SEMICOL(location());

"{" return ks_conf::ConfigParser::make_OBJ_START(location());

"}" return ks_conf::ConfigParser::make_OBJ_END(location());

"+" return ks_conf::ConfigParser::make_PLUS(location());
"-"	return ks_conf::ConfigParser::make_MINUS(location());
"*"	return ks_conf::ConfigParser::make_MULTIPLY(location());
"/"	return ks_conf::ConfigParser::make_DIVIDE(location());
"("	return ks_conf::ConfigParser::make_LEFT_BRACE(location());
")"	return ks_conf::ConfigParser::make_RIGHT_BRACE(location());

{MultilineComment} {}

{LineComment} {}

{RealNumber} {
    std::complex<double> n(stod(yytext, NULL), 0.0);
    return ks_conf::ConfigParser::make_NUMBER(n, location());
};

{ImagNumber} {
    std::complex<double> n(0.0, stod(yytext, NULL));
    return ks_conf::ConfigParser::make_NUMBER(n, location());
};

{Blank}+ {
};

[\n]+ {
};
            
<<EOF>> {
	return yyterminate();
};

%%

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
 * vtable of the class ExampleFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int ConfFlexLexer::yylex()
{
    std::cerr << "in ConfigFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int ConfFlexLexer::yywrap()
{
    return 1;
}
