#include <stdio.h>

#define NRW 11 + 2 + 2	// number of reserved words  徐卓+2
#define TXMAX 500		// length of identifier table
#define MAXNUMLEN 14	// maximum number of digits in numbers
#define NSYM 10 + 1 + 2 // maximum number of symbols in array ssym and csym
#define MAXIDLEN 10		// length of identifiers

#define MAXADDRESS 32767 // maximum address
#define MAXLEVEL 32		 // maximum depth of nesting block
#define CXMAX 500		 // size of code array

#define MAXSYM 30	   // maximum number of symbols
#define STACKSIZE 1000 // maximum storage

enum symtype
{
	SYM_NULL,
	SYM_IDENTIFIER,
	SYM_REFERIDENTIFIER, //
	SYM_ARRAYIDENTIFIER,
	SYM_LABEL, //
	SYM_NUMBER,
	SYM_PLUS,
	SYM_MINUS,
	SYM_TIMES,
	SYM_SLASH,
	SYM_ODD,
	SYM_EQU,
	SYM_NEQ,
	SYM_LES,
	SYM_LEQ,
	SYM_GTR,
	SYM_GEQ,
	SYM_LPAREN,
	SYM_RPAREN,
	SYM_LBRAC,
	SYM_RBRAC,
	SYM_COMMA,
	SYM_SEMICOLON,
	SYM_PERIOD,
	SYM_BECOMES,
	SYM_BEGIN,
	SYM_END,
	SYM_IF,
	SYM_THEN,
	SYM_WHILE,
	SYM_DO,
	SYM_CALL,
	SYM_CONST,
	SYM_VAR,
	SYM_PROCEDURE,
	SYM_GOTO, //
	SYM_ELSE, //
	SYM_PRT,  //
	SYM_RND,  //
	SYM_AND,  //
	SYM_OR,	  //
	SYM_NOT,  //

};

enum idtype
{
	ID_CONSTANT,
	ID_VARIABLE,
	ID_PROCEDURE,
	ID_LABEL,
	ID_REFERVARIABLE,
	ID_ARRAY //
};

enum opcode
{
	LIT,
	OPR,
	LOD,
	STO,
	CAL,
	INT,
	JMP,
	JPC,
	LEA,
	LODA,
	STOA,
	PRT,
	RND
}; //

enum oprcode
{
	OPR_RET,
	OPR_NEG,
	OPR_ADD,
	OPR_MIN,
	OPR_MUL,
	OPR_DIV,
	OPR_ODD,
	OPR_EQU,
	OPR_NEQ,
	OPR_LES,
	OPR_LEQ,
	OPR_GTR,
	OPR_GEQ,
	OPR_AND,
	OPR_OR,
	OPR_NOT, //
};

typedef struct
{
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char *err_msg[] =
	{
		/*  0 */ "",
		/*  1 */ "Found ':=' when expecting '='.",
		/*  2 */ "There must be a number to follow '='.",
		/*  3 */ "There must be an '=' to follow the identifier.",
		/*  4 */ "There must be an identifier to follow 'const', 'var', or 'procedure'.",
		/*  5 */ "Missing ',' or ';'.",
		/*  6 */ "Incorrect procedure name.",
		/*  7 */ "Statement expected.",
		/*  8 */ "Follow the statement is an incorrect symbol.",
		/*  9 */ "'.' expected.",
		/* 10 */ "';' expected.",
		/* 11 */ "Undeclared identifier or label.",
		/* 12 */ "Illegal assignment.",
		/* 13 */ "':=' expected.",
		/* 14 */ "There must be an identifier to follow the 'call'.",
		/* 15 */ "A constant or variable or label can not be called.",
		/* 16 */ "'then' expected.",
		/* 17 */ "';' or 'end' expected.",
		/* 18 */ "'do' expected.",
		/* 19 */ "Incorrect symbol.",
		/* 20 */ "Relative operators expected.",
		/* 21 */ "Procedure identifier can not be in an expression.",
		/* 22 */ "Missing ')'.",
		/* 23 */ "The symbol can not be followed by a factor.",
		/* 24 */ "The symbol can not be as the beginning of an expression.",
		/* 25 */ "The number is too great.",
		/* 26 */ "",
		/* 27 */ "",
		/* 28 */ "",
		/* 29 */ "",
		/* 30 */ "",
		/* 31 */ "",
		/* 32 */ "There are too many levels.",
		/* 33 */ "The symbol '*' is expected  ", //注释错误提示
		/* 34 */ "The symbol '/' is expected  ",
		/* 35 */ "There must be an label to follow the goto", //goto的错误提示
		/* 36 */ "A constant or variable or label can not be goto.",
		/* 37 */ "There is a nested procedure",
		/* 38 */ "The procedure name must be followed by ()",
		/* 39 */ "The procedure expects a parameter but other words appear",
		/* 40 */ "The procedure loses a parameter or has an extra ','",
		/* 41 */ "too many parameters",
		/* 42 */ "too few parameters",
		/* 43 */ "unfitted parameter while calling procedure"


};

//////////////////////////////////////////////////////////////////////
char ch;			   // last character read
int sym;			   // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
char id_arr[MAXIDLEN + 1];
int num;			   // last number read
int cc;				   // character count
int ll;				   // line length
int kk;
int err; //err数量
int cx;	 // index of current instruction to be generated.
int level = 0;
int tx = 0;			   // id table ++
int tx_a = 0;		   // arr table ++
int dimen = 0;		   // dimen wei ++
int wei = 0;		   //
int total_wei = 1;	   //
int num_reference = 0; //引用变量数
int in_procedure = 0;  //in a procedure or not
char line[80];
int soa; //数组的大小
int ifelse=0;
int pre_cx=0;
int single_pre_cx=0;
int pre_cx_p=0;
int single_pre_cx_p=0;
instruction code[CXMAX];

char *word[NRW + 1] =
	{
		"", /* place holder */
		"begin", "call", "const", "do", "end", "if",
		"odd", "procedure", "then", "var", "while", "goto", "else", "print", "random" //添加goto else
};

int wsym[NRW + 1] =
	{
		SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
		SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE, SYM_GOTO, SYM_ELSE, SYM_PRT, SYM_RND, //添加
};

int ssym[NSYM + 1] =
	{
		SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
		SYM_LPAREN, SYM_RPAREN, SYM_LBRAC, SYM_RBRAC, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON, SYM_NOT};

char csym[NSYM + 1] =
	{
		' ', '+', '-', '*', '/', '(', ')', '[', ']', '=', ',', '.', ';', '!'};

#define MAXINS 8 + 3 + 2 //xuzzz
char *mnemonic[MAXINS] =
	{
		"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC", "LEA", "LODA", "STOA", "PRT", "RND"};

typedef struct
{
	int n;					  //the num of parameters
	int *kind;				  //store these parameters' kinds
	int dimesnsion;			  //if the parameter is array,storing its dimension
} prodedure_parameters, *ptr; //modified by tll

typedef struct
{
	char name[MAXIDLEN + 1];
	int kind;
	int value;
	ptr procedure_para;
} comtab;

typedef struct
{
	char name[MAXIDLEN + 1];	//数组名
	int dimension[10];			//数组维度信息
} array;

array atable[TXMAX];
comtab table[TXMAX];
typedef struct 
{
	int f; // function code
	int l; // level
	int a; // displacement address
}pre_instruct;
pre_instruct pre[TXMAX];
pre_instruct pre_p[TXMAX];
typedef struct
{
	char name[MAXIDLEN + 1];
	int kind;					
	short level;
	short address;
	ptr procedure_para;
} mask;

FILE *infile;

// EOF PL0.h
