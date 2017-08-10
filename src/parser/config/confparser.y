%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define parser_class_name { ConfigParser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { ks_conf }
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <map>
    #include <stdint.h>
    #include "Eigen/Eigenvalues"
	#include "Eigen/Dense"

    using namespace std;

    namespace ks_conf {
        class ConfigScanner;
        class Config;
        class NumberObject;
        class ConfObject;
        class StringObject;
        class Object;
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
    #include <complex>
    #include "confscanner.h"
    #include "confparser.hh"
    #include "config.h"
    #include "location.hh"
    #include "confobj.h"
    #include "Eigen/Eigenvalues"
	#include "Eigen/Dense"
    
    // yylex() arguments are defined in parser.y
    static ks_conf::ConfigParser::symbol_type yylex(ks_conf::ConfigScanner &scanner, ks_conf::Config &driver) {
        return scanner.getNextToken();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace ks_conf;
}

%lex-param { ks_conf::ConfigScanner &scanner }
%lex-param { ks_conf::Config &driver }
%parse-param { ks_conf::ConfigScanner &scanner }
%parse-param { ks_conf::Config &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token <std::string> STRING  "string";
%token <std::complex<double> > NUMBER "number";
%token <std::string> ID "id";
%token ASSIGN "="
%token SEMICOL ";"
%token OBJ_START "{"
%token OBJ_END "}"
%token PLUS "+"
%token MINUS "-"
%token MULTIPLY "*"
%token DIVIDE "/"
%token LEFT_BRACE "("
%token RIGHT_BRACE ")"
%type <std::map<std::string, ks_conf::ConfObject* > > obj "obj";
%type <std::pair<std::string, ks_conf::ConfObject* > > obj1 "obj1";
%type <std::pair<std::string, ks_conf::ConfObject* > > assignment "assignment";
%type <std::pair<std::string, ks_conf::ConfObject* > > str "str";
%type <std::pair<std::string, ks_conf::ConfObject* > > num "num";
%type <std::complex<double> > mixed_expression "mixed_expression"
%type <std::complex<double> > expression "expression"

%start assignments

%%

assignments:
    | assignments assignment {
    	driver.insert($2);
    };
    
assignment: str {
    		$$ = $1;
    	  };
		  | num {
    		$$ = $1;
    	  };
		  | obj1 {
    		$$ = $1;
    	  };
	
str:  ID ASSIGN STRING SEMICOL {
		std::string str = $3;
		str = str.substr(1, str.length() - 2);
 		$$ = std::pair<std::string, ks_conf::ConfObject* >($1, (new ks_conf::StringObject())->setValue(str));
 	};
 	
obj1: ID ASSIGN OBJ_START obj OBJ_END {
 		$$ = std::pair<std::string, ks_conf::ConfObject* >($1, (new ks_conf::Object())->setValue($4));
 	};
 	
num: ID ASSIGN mixed_expression SEMICOL {
 		$$ = std::pair<std::string, ks_conf::ConfObject* >($1, (new ks_conf::NumberObject())->setValue($3));
 	};
 	| ID ASSIGN expression SEMICOL {
 		$$ = std::pair<std::string, ks_conf::ConfObject* >($1, (new ks_conf::NumberObject())->setValue($3));
 	};

mixed_expression: NUMBER                 		 { $$ = $1; }
	  | mixed_expression PLUS mixed_expression	 { $$ = $1 + $3; }
	  | mixed_expression MINUS mixed_expression	 { $$ = $1 - $3; }
	  | mixed_expression MULTIPLY mixed_expression { $$ = $1 * $3; }
	  | mixed_expression DIVIDE mixed_expression	 { $$ = $1 / $3; }
	  | LEFT_BRACE mixed_expression RIGHT_BRACE		 { $$ = $2; }
	  | expression PLUS mixed_expression	 	 { $$ = $1 + $3; }
	  | expression MINUS mixed_expression	 	 { $$ = $1 - $3; }
	  | expression MULTIPLY mixed_expression 	 { $$ = $1 * $3; }
	  | expression DIVIDE mixed_expression	 { $$ = $1 / $3; }
	  | mixed_expression PLUS expression	 	 { $$ = $1 + $3; }
	  | mixed_expression MINUS expression	 	 { $$ = $1 - $3; }
	  | mixed_expression MULTIPLY expression 	 { $$ = $1 * $3; }
	  | mixed_expression DIVIDE expression	 { $$ = $1 / $3; }
	  | expression DIVIDE expression		 { $$ = $1 / (std::complex<double>)$3; }
;

expression: NUMBER				{ $$ = $1; }
	  | expression PLUS expression	{ $$ = $1 + $3; }
	  | expression MINUS expression	{ $$ = $1 - $3; }
	  | expression MULTIPLY expression	{ $$ = $1 * $3; }
	  | LEFT_BRACE expression RIGHT_BRACE		{ $$ = $2; }
 	
obj: obj assignment {
        $1.insert($2);
        $$ = $1;
	};
   | {
   	 std::map<std::string, ks_conf::ConfObject* > o;
   	 $$ = o;
   };
    
%%

// Bison expects us to provide implementation - otherwise linker complains
void ks_conf::ConfigParser::error(const location &loc , const std::string &message) {
	cout << "Error: " << message << endl << "Error location: " << driver.getLocation() << endl;
}
