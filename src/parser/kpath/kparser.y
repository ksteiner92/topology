%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define parser_class_name { KPathParser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { ks_k }
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include "Eigen/Eigenvalues"
	#include "Eigen/Dense"

    using namespace std;

    namespace ks_k {
        class KPathScanner;
        class KPath;
    }
}

// Bison calls yylex() function that must be provided by us to suck tokens
// from the scanner. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%code top
{
    #include <iostream>
    #include "kscanner.h"
    #include "kparser.hh"
    #include "kpath.h"
    #include "location.hh"
    #include "Eigen/Eigenvalues"
	#include "Eigen/Dense"
    
    // yylex() arguments are defined in parser.y
    static ks_k::KPathParser::symbol_type yylex(ks_k::KPathScanner &scanner, ks_k::KPath &driver) {
        return scanner.getNextToken();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace ks_k;
}

%lex-param { ks_k::KPathScanner &scanner }
%lex-param { ks_k::KPath &driver }
%parse-param { ks_k::KPathScanner &scanner }
%parse-param { ks_k::KPath &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token <std::string> STRING  "string";
%token <double> NUMBER "number";
%token <Eigen::Vector3d> POINT "point"
%token EOL "end of line"

%type<Eigen::Vector3d> point;

%start start

%%

start:
     | start line
     | start EOL
     | END
;

line: exp neighbors
;

neighbors:
	  | neighbors NUMBER {
	  driver.addNeighbor(driver.getNumKPoints() - 1, (size_t) $2);
	  }
	  ;
	  | neighbors STRING

exp: symm_point
    | off_symm_point
;

symm_point : STRING point {
	driver.addSymmPoint($1, $2);
}
;

off_symm_point: point {
	driver.addPoint($1);
}

point: NUMBER NUMBER NUMBER NUMBER {
	$$ = Eigen::Vector3d($1 / $4, $2 / $4, $3 / $4);
}  
%%

// Bison expects us to provide implementation - otherwise linker complains
void ks_k::KPathParser::error(const location &loc , const std::string &message) {
	cout << "Error: " << message << endl << "Error location: " << driver.getLocation() << endl;
}
