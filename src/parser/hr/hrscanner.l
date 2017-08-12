/**
 * This file is part of toplogy.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

%{
	#include <iostream>
	#include <cstdlib>
	#include <climits>
	#include <limits.h>
	#include "hrscanner.h"
	#include "tbh.h"
	#include "hrparser.hh"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() ks_hr::HamiltonParser::make_END(ks_hr::location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION driver.increaseLocation(yyleng);
%}

%option nodefault
%option noyywrap
%option c++
%option prefix="Hamilton"
%option yyclass="HamiltonFlexLexer"

Identifier              [a-zA-Z][a-zA-Z_0-9]*
Int                     [-+]?[0-9]+
Float                   [-+]?[0-9]*\.?[0-9]*([eE][-+]?[0-9]+)?
Blank                   [ \t]
UnicodeEscapeSequence   u[0-9A-Fa-f]{4}
EscapeCharacter         ["\\/bfnrt]
EscapeSequence          {EscapeCharacter}|{UnicodeEscapeSequence}
StringCharacter         [^"\\]|\\{EscapeSequence}
String                  \"{StringCharacter}*\"

%%

{Float} {
	errno = 0;
    double n = stod(yytext, NULL);
    /*if (!(DOUBLE_MIN <= n && n <= DOUBLE_MAX && errno != ERANGE))
        driver.error (loc, "double is out of range");*/
    return ks_hr::HamiltonParser::make_FLOAT(n, ks_hr::location());
}

{Int} {
	errno = 0;
    int n = stoi(yytext, NULL);
    /*if (!(INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
        driver.error (loc, "double is out of range");*/
    return ks_hr::HamiltonParser::make_INT(n, ks_hr::location());
}

{Blank}+ {
}

[\n]+ {
	return ks_hr::HamiltonParser::make_EOL(ks_hr::location());
}
            
<<EOF>> {
	return yyterminate();
}


%%


/* This implementation of ExampleFlexLexer::yylex() is required to fill the
 * vtable of the class ExampleFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int HrFlexLexer::yylex()
{
    std::cerr << "in HamiltonFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int HrFlexLexer::yywrap()
{
    return 1;
}
