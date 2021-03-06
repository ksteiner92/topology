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
	#include "swscanner.h"
	#include "selfenergy.h"
	#include "swparser.hh"
	#include "location.hh"

	using namespace std;

	// Original yyterminate() macro returns int. Since we're using Bison 3 variants
	// as tokens, we must redefine it to change type from `int` to `Parser::semantic_type`
	#define yyterminate() ks_sw::SWParser::make_END(location());

	// This will track current scanner location.
	// Action is called when length of the token is known.
	#define YY_USER_ACTION driver.increaseLocation(yyleng);
%}

%option nodefault
%option noyywrap
%option c++
%option prefix="SW"
%option yyclass="SWFlexLexer"

Number                  [-+]?[0-9]*\.?[0-9]*([eE][-+]?[0-9]+)?
Blank                   [ \t]

%%

{Number} {
	//std::cout << "found number " << yytext << std::endl;
	double n = 0.0;
	try {
    	n = stod(yytext, NULL);
    } catch(std::invalid_argument &e) {
    	//std::cout << "warning: " << yytext << " not a number" << std::endl;
    }
    return ks_sw::SWParser::make_NUMBER(n, location());
}

{Blank}+ {
}

[\n]+ {
	return ks_sw::SWParser::make_EOL(location());
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

int _SWFlexLexer::yylex()
{
    std::cerr << "in SWFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int _SWFlexLexer::yywrap()
{
    return 1;
}
