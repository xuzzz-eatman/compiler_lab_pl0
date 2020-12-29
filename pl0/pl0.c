// pl0 compiler source code

#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include "PL0.h"
#include "set.c"
//tlllll2333
//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
//获取字符 先读入一行到line[]中 再逐个字符读取
void getch(void)
{
	if (line[cc + 1] == '/' && line[cc + 2] == '/')
	{
		line[cc + 1] = ' ';
		line[cc + 2] = ' ';
		cc = ll;
	} //添加行注释
	while (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ((!feof(infile)) 
			   && ((ch = getc(infile)) != '\n'))
		{
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
		if (line[cc + 1] == '/' && line[cc + 2] == '/')
		{
			line[cc + 1] = ' ';
			line[cc + 2] = ' ';
			cc = ll;
		} //添加行注释
	}

	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym(void)
{	
	if(ifelse==1)
	{
		ifelse=0;
	}
	else
	{
		int i, k;
		char a[MAXIDLEN + 1];

		while (ch == ' ' || ch == '\t')
			getch();
		if (ch == '/' && line[cc + 1] == '*')
		{ //处理块注释 
			getch();
			if (ch == '*')
			{
				getch();
				while (ch != '*' || line[cc + 1] != '/')
				{
					getch();
				}
				getch();
				if (ch == '/')
					getch();
				else
					error(34);
			}
			else
			{
				error(33);
			}
		}
		while (ch == ' ' || ch == '\t')
			getch();
		if (isalpha(ch))
		{ // symbol is a reserved word or an identifier.
			k = 0;
			do
			{
				if (k < MAXIDLEN)
					a[k++] = ch;
				getch();
			} while (isalpha(ch) || isdigit(ch));
			if (ch == ':' && line[cc + 1] != '=')
			{
				getch();
				a[k] = 0;
				strcpy(id, a);
				sym = SYM_LABEL; //对label的识别 GOTO会使用到
			}
			else if (ch == '[')
			{
				a[k] = 0;
				strcpy(id, a);
				sym = SYM_ARRAYIDENTIFIER; //
			}
			else
			{
				a[k] = 0;
				strcpy(id, a);
				word[0] = id;
				i = NRW;
				while (strcmp(id, word[i--]))
					;
				if (++i)
					sym = wsym[i]; // symbol is a reserved word
				else
					sym = SYM_IDENTIFIER; // symbol is an identifier
			}
		}
		else if (isdigit(ch))
		{ // symbol is a number.
			k = num = 0;
			sym = SYM_NUMBER;
			do
			{
				num = num * 10 + ch - '0';
				k++;
				getch();
			} while (isdigit(ch));
			if (k > MAXNUMLEN)
				error(25); // The number is too great.
		}
		else if (ch == '&' && line[cc + 1] != '&')
		{
			getch();
			//exit(0);
			if (isalpha(ch) && !in_procedure)
			{ // symbol is a REFERidentifier.
				k = 0;
				do
				{
					if (k < MAXIDLEN)
						a[k++] = ch;
					getch();
				} while (isalpha(ch) || isdigit(ch));
				if (ch == '='||ch == ' ')
				{
					a[k] = 0;
					strcpy(id, a);
					word[0] = id;
					i = NRW;
					while (strcmp(id, word[i--]))
						;
					if (++i)
						error(11); // symbol is a reserved word  error
					else
						sym = SYM_REFERIDENTIFIER; // symbol is a REFERidentifier
				}
				else
				{
					error(11);
				}
			}
			else if (isalpha(ch) && in_procedure)
			{ //in procedure declaration
				k = 0;
				do
				{
					if (k < MAXIDLEN)
						a[k++] = ch;
					getch();
				} while (isalpha(ch) || isdigit(ch));
				a[k] = 0;
				strcpy(id, a);
				word[0] = id;
				i = NRW;
				while (strcmp(id, word[i--]))
					;
				if (++i)
					error(11); // symbol is a reserved word  error
				else
					sym = SYM_REFERIDENTIFIER; // symbol is a REFERidentifier
			}
		}
		else if (ch == '&' && line[cc + 1] == '&')
		{
			getch();
			sym = SYM_AND; // &&
			getch();
		}
		else if (ch == '|' && line[cc + 1] == '|')
		{
			getch();
			sym = SYM_OR; // ||
			getch();
		}
		else if (ch == ':')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_BECOMES; // :=
				getch();
			}
			else
			{
				sym = SYM_NULL; // illegal?
			}
		}
		else if (ch == '>')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_GEQ; // >=
				getch();
			}
			else
			{
				sym = SYM_GTR; // >
			}
		}
		else if (ch == '<')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_LEQ; // <=
				getch();
			}
			else if (ch == '>')
			{
				sym = SYM_NEQ; // <>
				getch();
			}
			else
			{
				sym = SYM_LES; // <
			}
		}
		else
		{ // other tokens
			i = NSYM;
			csym[0] = ch;
			while (csym[i--] != ch)
				;
			if (++i)
			{
				sym = ssym[i];
				getch();
			}
			else
			{
				printf("Fatal Error: Unknown character.\n");
				exit(1);
			}
		}
	}
	
	
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction.
void gen(int x, int y, int z)
{
	if (cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen

void pre_gen(int x, int y, int z)
{
	if (pre_cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	pre[pre_cx].f = x;
	pre[pre_cx].l = y;
	pre[pre_cx++].a = z;
} // gen
void pre_gen2(int x, int y, int z)
{
	if (pre_cx_p > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	pre_p[pre_cx_p].f = x;
	pre_p[pre_cx_p].l = y;
	pre_p[pre_cx_p++].a = z;
} // gen
//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;

	if (!inset(sym, s1))
	{
		error(n);
		s = uniteset(s1, s2);
		while (!inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
int dx; // data allocation index

// enter object(constant, variable or procedre) into table.
void enter(int kind)
{
	mask *mk;

	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind)
	{
	case ID_CONSTANT:
		if (num > MAXADDRESS)
		{
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		mk = (mask *)&table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	case ID_PROCEDURE:
		mk = (mask *)&table[tx];
		mk->level = level;
		break;
	case ID_LABEL:
		table[tx].value = cx;
		break; // 添加label类型的ID 为goto提供信息
	case ID_REFERVARIABLE:
		mk = (mask *)&table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	case ID_ARRAY:
		mk = (mask *)&table[tx];
		mk->level = level;
		mk->address = dx; //
		dx = dx + total_wei;
	} // switch
} // enter

void enter2()
{
	tx_a++;
	strcpy(atable[tx_a].name, id);
}
//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
int position(char *id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0)
		;
	return i;
} // position   没找到为0

int position2(char *id)
{
	int i;
	strcpy(atable[0].name, id);
	i = tx + 1;
	while (strcmp(atable[--i].name, id) != 0)
		;
	return i;
} // position   没找到为0
//////////////////////////////////////////////////////////////////////
//常值变量声明
void constdeclaration()
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER)
			{
				enter(ID_CONSTANT);
				getsym();
			}
			else
			{
				error(2); // There must be a number to follow '='.
			}
		}
		else
		{
			error(3); // There must be an '=' to follow the identifier.
		}
	}
	else
		error(4);
	// There must be an identifier to follow 'const', 'var', or 'procedure'.
} // constdeclaration

//////////////////////////////////////////////////////////////////////
//普通变量声明
void vardeclaration(void)
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_VARIABLE);
		getsym();
	}
	else if (sym == SYM_REFERIDENTIFIER)	//引用变量
	{
		int i, j, k;
		mask *mk;
		enter(ID_REFERVARIABLE);	//引用变量加入变量表
		getsym();					//获取 =
		getsym();					//获取引用的变量
		if(!in_procedure)
		{
			if (!(i = position(id)))
			{
				error(11); //引用的变量未声明
			}
			else if (sym == SYM_IDENTIFIER)
			{

				mk = (mask *)&table[i];
				if(mk->kind == ID_VARIABLE){	//引用的变量是普通变量
					pre_gen(LEA, level, mk->address);	
				}
					
				else if (mk->kind == ID_REFERVARIABLE){  //引用的变量也是引用变量
					pre_gen(LOD, level, mk->address);
				}
					
				single_pre_cx++;
				mk = (mask *)&table[tx];
				pre_gen(STO, level, mk->address);	//将变量地址存到引用变量中
				single_pre_cx++;
				getsym();
			}
			else if (sym == SYM_ARRAYIDENTIFIER) //引用数组的元素
			{
				j = position2(id);
				mk = (mask *)&table[i];
				pre_gen(LEA, level - mk->level, mk->address);
				single_pre_cx++;
				getsym();
				dimen = 1;
				getsym();

				if (sym == SYM_NUMBER)
				{
						pre_gen(LIT, 0, num);
						single_pre_cx++;			
				}
				else if (sym == SYM_IDENTIFIER)
				{
					k = position(id);
					pre_gen(LIT, 0, table[k].value);
					single_pre_cx++;
				}
				getsym();
				// gen(OPR, 0, OPR_ADD);
				// num_reference++;
				getsym();
				if (sym == SYM_LBRAC)
				{
					pre_gen(LIT, 0, atable[j].dimension[dimen++]);
					single_pre_cx++;
					pre_gen(OPR, 0, OPR_MUL);
					single_pre_cx++;
				}

				while (sym == SYM_LBRAC)
				{
					getsym();

					if (sym == SYM_NUMBER)
					{
						pre_gen(LIT, 0, num);
						single_pre_cx++;
					}
					else if (sym == SYM_IDENTIFIER)
					{
						k = position(id);
						pre_gen(LIT, 0, table[k].value);
						single_pre_cx++;
					}

					pre_gen(OPR, 0, OPR_ADD);
					single_pre_cx++;
					getsym();
					if (sym == SYM_LBRAC)
					{
						pre_gen(LIT, 0, atable[j].dimension[dimen++]);
						single_pre_cx++;
						pre_gen(OPR, 0, OPR_MUL);
						single_pre_cx++;
					}
				}
				pre_gen(OPR, 0, OPR_ADD);
				single_pre_cx++;
				mk = (mask *)&table[tx];
				pre_gen(STO, level, mk->address);
				single_pre_cx++;
			}
		}
		else
		{
			if (!(i = position(id)))
			{
				error(11); //引用的变量未声明
			}
			else if (sym == SYM_IDENTIFIER)
			{

				mk = (mask *)&table[i];
				if(mk->kind == ID_VARIABLE){	//引用的变量是普通变量
					pre_gen2(LEA, level - mk->level, mk->address);	
				}
					
				else if (mk->kind == ID_REFERVARIABLE){  //引用的变量也是引用变量
					pre_gen2(LOD, level - mk->level, mk->address);
				}
					
				single_pre_cx_p++;
				mk = (mask *)&table[tx];
				pre_gen2(STO, level - mk->level, mk->address);	//将变量地址存到引用变量中
				single_pre_cx_p++;
				getsym();
			}
			else if (sym == SYM_ARRAYIDENTIFIER) //引用数组的元素
			{
				j = position2(id);
				mk = (mask *)&table[i];
				pre_gen2(LOD, level - mk->level, mk->address);
				single_pre_cx_p++;
				getsym();
				dimen = 1;
				getsym();

				if (sym == SYM_NUMBER)
				{
						pre_gen2(LIT, 0, num);
						single_pre_cx_p++;			
				}
				else if (sym == SYM_IDENTIFIER)
				{
					
					k = position(id);
					mk = (mask *)&table[k];
					if(mk->kind==ID_CONSTANT)
						pre_gen2(LIT, 0, table[k].value);
					else
					{
						pre_gen2(LOD, level - mk->level, mk->address );
					}
					
					single_pre_cx_p++;
				}
				getsym();
				// gen(OPR, 0, OPR_ADD);
				// num_reference++;
				getsym();
				if (sym == SYM_LBRAC)
				{
					pre_gen2(LIT, 0, atable[j].dimension[dimen++]);
					single_pre_cx_p++;
					pre_gen2(OPR, 0, OPR_MUL);
					single_pre_cx_p++;
				}

				while (sym == SYM_LBRAC)
				{
					getsym();

					if (sym == SYM_NUMBER)
					{
						pre_gen2(LIT, 0, num);
						single_pre_cx_p++;
					}
					else if (sym == SYM_IDENTIFIER)
					{
					
						k = position(id);
						mk = (mask *)&table[k];
						if(mk->kind==ID_CONSTANT)
							pre_gen2(LIT, 0, table[k].value);
						else
						{
							pre_gen2(LOD, level - mk->level, mk->address );
						}
						
						single_pre_cx_p++;
					}

					pre_gen2(OPR, 0, OPR_ADD);
					single_pre_cx_p++;
					getsym();
					if (sym == SYM_LBRAC)
					{
						pre_gen2(LIT, 0, atable[j].dimension[dimen++]);
						single_pre_cx_p++;
						pre_gen2(OPR, 0, OPR_MUL);
						single_pre_cx_p++;
					}
				}
				pre_gen2(OPR, 0, OPR_ADD);
				single_pre_cx_p++;
				mk = (mask *)&table[tx];
				pre_gen2(STO, level - mk->level, mk->address);
				single_pre_cx_p++;
			}
		}
		
		
	}
	else if (sym == SYM_ARRAYIDENTIFIER)
	{
		int posi;
		enter2();
		getsym();
		dimen = 0;
		strcpy(id_arr,id);
		while (sym == SYM_LBRAC)
		{
			getsym();
			if (sym == SYM_NUMBER) //在声明数组时，维度只能是数字或者const
			{
				atable[tx_a].dimension[dimen++] = num;
				total_wei *= num;
			}
			else if (sym == SYM_IDENTIFIER)
			{
				
				if ((posi = position(id)) && (table[posi].kind == ID_CONSTANT))
				{
					atable[tx_a].dimension[dimen++] = table[posi].value;
					total_wei *= table[posi].value;
				}
			}
			getsym();
			getsym();
		}
		strcpy(id,id_arr);
		enter(ID_ARRAY);  //获取完全信息后加入变量表  变量表的地址增加元素个数
		dimen = 0;
		total_wei = 1;
	}
	else
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////

void listcode(int from, int to)
{
	int i;

	printf("\n");
	for (i = from; i < to; i++)
	{
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
	printf("\n");
} // listcode

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)
{
	void expression(symset fsys);
	int i, j;
	symset set;

	// test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	if (inset(sym, facbegsys))
	{
		if (sym == SYM_IDENTIFIER)
		{
			if ((i = position(id)) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				switch (table[i].kind)
				{
					mask *mk;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					break;
				case ID_VARIABLE:
					mk = (mask *)&table[i];
					gen(LOD, level - mk->level, mk->address);
					break;
				case ID_REFERVARIABLE:
					mk = (mask *)&table[i];
					gen(LOD, level - mk->level, mk->address);
					gen(LODA, 0, 0);
					break;
				case ID_PROCEDURE:
					error(21); // Procedure identifier can not be in an expression.
					break;
				} // switch
			}
			getsym();
		}
		else if (sym == SYM_ARRAYIDENTIFIER)
		{
			mask *mk;
			if (!(i = position(id)))
			{
				error(11); // Undeclared identifier.
			}
			else if (table[i].kind != ID_ARRAY)
			{
				error(12);
				i = 0;
			}
			j = position2(id);
			mk = (mask *)&table[i];
			if (in_procedure)
			{
				gen(LOD, level - mk->level, mk->address);
			}
			else
				gen(LEA, level - mk->level, mk->address);
			getsym();
			dimen = 1;
			getsym();
			expression(fsys);
			getsym();
			if (sym == SYM_LBRAC)
			{
				gen(LIT, 0, atable[j].dimension[dimen++]);
				gen(OPR, 0, OPR_MUL);
			}

			while (sym == SYM_LBRAC)
			{
				getsym();
				expression(fsys);
				gen(OPR, 0, OPR_ADD);
				getsym();
				if (sym == SYM_LBRAC)
				{
					gen(LIT, 0, atable[j].dimension[dimen++]);
					gen(OPR, 0, OPR_MUL);
				}
			}
			gen(OPR, 0, OPR_ADD);
			gen(LODA, 0, 0);
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			gen(LIT, 0, num);
			getsym();
		}
		else if (sym == SYM_RND)
		{
			getsym();
			if (sym != SYM_LPAREN)
				error(38);
			else
			{
				getsym();
				if (sym == SYM_RPAREN)
				{
					gen(RND, 0, 0);
					getsym();
				}
				else
				{
					set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
					expression(set);
					destroyset(set);
					gen(RND, 0, 1);
					if (sym == SYM_RPAREN)
						getsym();
					else
					{
						error(22);
					}
				}
			}
		}
		else if (sym == SYM_LPAREN)
		{
			getsym();
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
			destroyset(set);
			if (sym == SYM_RPAREN)
			{
				getsym();
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		else if (sym == SYM_MINUS) // UMINUS,  Expr -> '-' Expr
		{
			getsym();
			factor(fsys);
			gen(OPR, 0, OPR_NEG);
		}
		else if (sym == SYM_NOT)
		{
			getsym();
			factor(fsys);
			gen(OPR, 0, OPR_NOT);
		}
		// test(fsys, createset(SYM_LPAREN,SYM_RBRAC, SYM_NULL), 23);
	} // if
} // factor

//////////////////////////////////////////////////////////////////////
void term(symset fsys)
{
	int mulop;
	symset set;

	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH)
	{
		mulop = sym;
		getsym();
		factor(set);
		if (mulop == SYM_TIMES)
		{
			gen(OPR, 0, OPR_MUL);
		}
		else
		{
			gen(OPR, 0, OPR_DIV);
		}
	} // while
	destroyset(set);
} // term

//////////////////////////////////////////////////////////////////////
void expression(symset fsys)
{
	int addop;
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));

	term(set);
	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_PLUS)
		{
			gen(OPR, 0, OPR_ADD);
		}
		else
		{
			gen(OPR, 0, OPR_MIN);
		}
	} // while

	destroyset(set);
} // expression

//////////////////////////////////////////////////////////////////////
void and_condition(symset fsys)
{
	void condition(symset fsys);
	int relop;
	symset set;

	if (sym == SYM_ODD)
	{
		getsym();
		expression(fsys);
		gen(OPR, 0, 6);
	}
	else if (sym == SYM_LPAREN)
	{
		getsym();
		set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
		condition(set);
		destroyset(set);
		if (sym == SYM_RPAREN)
		{
			getsym();
		}
		else
		{
			error(22); // Missing ')'.
		}
	}
	else if (sym == SYM_NOT)
	{
		getsym();
		condition(fsys);
		gen(OPR, 0, OPR_NOT);
	}
	else
	{
		set = uniteset(relset, fsys);
		expression(set);
		destroyset(set);
		if (!inset(sym, relset))
		{
			error(20);
			printf("%d\n", sym);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
			switch (relop)
			{
			case SYM_EQU:
				gen(OPR, 0, OPR_EQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
			} // switch
		}	  // else
	}		  // else
} // condition

void or_condition(symset fsys)
{
	symset set;

	set = uniteset(fsys, createset(SYM_AND, SYM_NULL));
	and_condition(set);
	while (sym == SYM_AND)
	{
		getsym();
		and_condition(set);
		gen(OPR, 0, OPR_AND);
	} // while
	destroyset(set);
} // term

void condition(symset fsys)
{
	symset set;

	set = uniteset(fsys, createset(SYM_OR, SYM_NULL));

	or_condition(set);
	while (sym == SYM_OR)
	{
		getsym();
		or_condition(set);
		gen(OPR, 0, OPR_OR);
	} // while

	destroyset(set);
}

void call_procedure(int i)
{
	//mask *mk;
	//mk = (mask *)&table[i];
	//gen(CAL, level - mk->level, mk->address);
	//printf("hhhhhhhh: %s\n", id);
	ptr para = table[i].procedure_para;
	getsym();
	if (sym != SYM_LPAREN)
		error(38);
	else
	{
		int num_p = para->n; //number of parameters
		int pos;
		getsym();
		while (sym != SYM_RPAREN && num_p--)
		{
			if (sym == SYM_IDENTIFIER)
			{
				if (!(pos = position(id)))
				{
					error(11); //undefined parameters
					break;
				}
				else
				{
					if (para->kind[num_p] == ID_VARIABLE)
					{
						mask *mk;
						mk = (mask *)&table[pos];
						if (mk->kind == ID_VARIABLE){
							gen(LOD, level - mk->level, mk->address);
							getsym();
						}
							
						else if (mk->kind == ID_ARRAY)
						{
							int j = position2(id);
							gen(LEA, level - mk->level, mk->address);
							getsym();
							dimen = 1;
							getsym();
							symset s1 = createset(SYM_PERIOD, SYM_NULL);
							symset s2 = uniteset(declbegsys, statbegsys);
							symset set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL, SYM_COMMA);
							symset set2 = uniteset(s1, s2);
							symset set3 = uniteset(set1, set2);
							symset fsys = uniteset(set3, facbegsys);
							expression(fsys);
							getsym();
							if (sym == SYM_LBRAC)
							{
								gen(LIT, 0, atable[j].dimension[dimen++]);
								gen(OPR, 0, OPR_MUL);
							}

							while (sym == SYM_LBRAC)
							{
								getsym();
								expression(fsys);
								gen(OPR, 0, OPR_ADD);
								getsym();
								if (sym == SYM_LBRAC)
								{
									gen(LIT, 0, atable[j].dimension[dimen++]);
									gen(OPR, 0, OPR_MUL);
								}
							}
							gen(OPR, 0, OPR_ADD);
							gen(LODA, 0, 0);
							destroyset(set1);
							destroyset(set2);
							destroyset(s1);
							destroyset(s2);
							destroyset(fsys);
						}
					}
					else if (para->kind[num_p] == ID_REFERVARIABLE)
					{
						mask *mk;
						mk = (mask *)&table[pos];
						if (mk->kind == ID_VARIABLE){
							gen(LEA, level, mk->address);
							getsym();
						}

						else if (mk->kind == ID_REFERVARIABLE){
							gen(LOD, level, mk->address);
							getsym();
						}

						else if (mk->kind == ID_ARRAY)
						{
							int j = position2(id);
							gen(LEA, level - mk->level, mk->address);
							getsym();
							dimen = 1;
							getsym();
							symset fsys = createset(SYM_ARRAYIDENTIFIER, SYM_IDENTIFIER, SYM_RND, SYM_NOT, SYM_NUMBER, SYM_LPAREN, SYM_RBRAC, SYM_MINUS, SYM_NULL);
							expression(fsys);
							getsym();
							if (sym == SYM_LBRAC)
							{
								gen(LIT, 0, atable[j].dimension[dimen++]);
								gen(OPR, 0, OPR_MUL);
							}

							while (sym == SYM_LBRAC)
							{
								getsym();
								expression(fsys);
								gen(OPR, 0, OPR_ADD);
								getsym();
								if (sym == SYM_LBRAC)
								{
									gen(LIT, 0, atable[j].dimension[dimen++]);
									gen(OPR, 0, OPR_MUL);
								}
							}
							gen(OPR, 0, OPR_ADD);
							destroyset(fsys);
						}
					}
					else if (para->kind[num_p] == ID_ARRAY)
					{
						mask *mk;
						mk = (mask *)&table[pos];
						if (mk->kind == ID_ARRAY)
							gen(LEA, level, mk->address);
						getsym();
					}
				}
			}
			else if (sym == SYM_ARRAYIDENTIFIER) //read an array
			{
				if (!(pos = position(id)))
				{
					error(11); //undefined parameters
					break;
				}
				else
				{
					if (para->kind[num_p] == ID_VARIABLE)
					{
						mask *mk;
						mk = (mask *)&table[pos];
						if (mk->kind == ID_ARRAY)
						{
							int j = position2(id);
							gen(LEA, level - mk->level, mk->address);
							getsym();
							dimen = 1;
							getsym();
							symset s1 = createset(SYM_PERIOD, SYM_NULL);
							symset s2 = uniteset(declbegsys, statbegsys);
							symset set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL, SYM_COMMA);
							symset set2 = uniteset(s1, s2);
							symset set3 = uniteset(set1, set2);
							symset fsys = uniteset(set3, facbegsys);
							expression(fsys);
							getsym();
							if (sym == SYM_LBRAC)
							{
								gen(LIT, 0, atable[j].dimension[dimen++]);
								gen(OPR, 0, OPR_MUL);
							}

							while (sym == SYM_LBRAC)
							{
								getsym();
								expression(fsys);
								gen(OPR, 0, OPR_ADD);
								getsym();
								if (sym == SYM_LBRAC)
								{
									gen(LIT, 0, atable[j].dimension[dimen++]);
									gen(OPR, 0, OPR_MUL);
								}
							}
							gen(OPR, 0, OPR_ADD);
							gen(LODA, 0, 0);
							destroyset(set1);
							destroyset(set2);
							destroyset(s1);
							destroyset(s2);
							destroyset(fsys);
						}
					}
					else if (para->kind[num_p] == ID_REFERVARIABLE)
					{
						mask *mk;
						mk = (mask *)&table[pos];
						if (mk->kind == ID_VARIABLE)
							gen(LEA, level, mk->address);
						else if (mk->kind == ID_REFERVARIABLE)
							gen(LOD, level, mk->address);
						else if (mk->kind == ID_ARRAY)
						{
							int j = position2(id);
							gen(LEA, level - mk->level, mk->address);
							getsym();
							dimen = 1;
							getsym();
							symset fsys = createset(SYM_ARRAYIDENTIFIER, SYM_IDENTIFIER, SYM_RND, SYM_NOT, SYM_NUMBER, SYM_LPAREN, SYM_RBRAC, SYM_MINUS, SYM_NULL);
							expression(fsys);
							getsym();
							if (sym == SYM_LBRAC)
							{
								gen(LIT, 0, atable[j].dimension[dimen++]);
								gen(OPR, 0, OPR_MUL);
							}

							while (sym == SYM_LBRAC)
							{
								getsym();
								expression(fsys);
								gen(OPR, 0, OPR_ADD);
								getsym();
								if (sym == SYM_LBRAC)
								{
									gen(LIT, 0, atable[j].dimension[dimen++]);
									gen(OPR, 0, OPR_MUL);
								}
							}
							gen(OPR, 0, OPR_ADD);
							destroyset(fsys);
						}
					}
					else if (para->kind[num_p] == ID_ARRAY)
					{
						mask *mk;
						mk = (mask *)&table[pos];
						if (mk->kind == ID_ARRAY)
							gen(LEA, level, mk->address);
						getsym();
					}
				}
			}
			else if(sym==SYM_NUMBER){
				gen(LIT,0,num);
				getsym();
			}
			if (sym == SYM_COMMA)
			{
				getsym();
			}
		}
		if (sym == SYM_RPAREN && num_p == 0)
		{
			mask *mk;
			mk = (mask *)&table[i];
			gen(CAL, level - mk->level, mk->address);
		}
		else if (num_p == 0)
		{
			error(41); //too many parameters
		}
		else if (sym == SYM_RPAREN)
		{
			error(42); //too few parameters
		}
	}
}
//////////////////////////////////////////////////////////////////////
void statement(symset fsys)
{
	int i, j, cx1, cx2;
	symset set1, set;
	if (sym == SYM_LABEL)
	{
		if (!(i = position(id)))
		{
			enter(ID_LABEL);
		}
		else
		{
			if (code[table[i].value].f == JMP && code[table[i].value].a == -1)
			{
				code[table[i].value].a = cx;
				table[i].value = cx; //修改之前的jmp的地址  GOTO 
			}
			else
			{
				error(36); //重复定义label 
			}
		}

		getsym();
		statement(fsys);
	} //识别label 

	else if (sym == SYM_IDENTIFIER)
	{ // variable assignment
		mask *mk;
		if (!(i = position(id)))
		{
			error(11); // Undeclared identifier.
		}
		else if (table[i].kind != ID_VARIABLE && table[i].kind != ID_REFERVARIABLE)
		{
			error(12); // Illegal assignment.
			i = 0;
		}
		getsym();
		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		else
		{
			error(13); // ':=' expected.
		}
		mk = (mask *)&table[i];
		if (table[i].kind == ID_REFERVARIABLE)
			gen(LOD, level - mk->level, mk->address);
		expression(fsys);

		mk = (mask *)&table[i];

		if (i && table[i].kind == ID_VARIABLE)
		{
			gen(STO, level - mk->level, mk->address);
		}
		else if (i && table[i].kind == ID_REFERVARIABLE)
		{
			gen(STOA, 0, 0);
		}
	}
	else if (sym == SYM_ARRAYIDENTIFIER)
	{
		mask *mk;
		if (!(i = position(id)))
		{
			error(11); // Undeclared identifier.
		}
		else if (table[i].kind != ID_ARRAY)
		{
			error(12);
			i = 0;
		}
		j = position2(id);
		mk = (mask *)&table[i];
		if (in_procedure)
		{
			gen(LOD, level - mk->level, mk->address);
		}
		else
			gen(LEA, level - mk->level, mk->address);

		getsym();
		dimen = 1;
		getsym();
		expression(fsys);
		getsym();
		if (sym == SYM_LBRAC)
		{
			gen(LIT, 0, atable[j].dimension[dimen++]);
			gen(OPR, 0, OPR_MUL);
		}

		while (sym == SYM_LBRAC)
		{
			getsym();
			expression(fsys);
			gen(OPR, 0, OPR_ADD);
			getsym();
			if (sym == SYM_LBRAC)
			{
				gen(LIT, 0, atable[j].dimension[dimen++]);
				gen(OPR, 0, OPR_MUL);
			}
		}
		gen(OPR, 0, OPR_ADD);
		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		else
		{
			error(13); // ':=' expected.
		}
		expression(fsys);
		gen(STOA, 0, 0);
	}
	else if (sym == SYM_CALL)
	{ // procedure call
		getsym();
		if (sym != SYM_IDENTIFIER)
		{
			error(14); // There must be an identifier to follow the 'call'.
		}
		else
		{
			if (!(i = position(id)))
			{
				error(11); // Undeclared identifier.
			}
			else if (table[i].kind == ID_PROCEDURE)
				call_procedure(i);
			else
			{
				error(15); // A constant or variable can not be called.
			}
			getsym();
		}
	}
	else if (sym == SYM_IF)
	{ // if statement
		getsym();
		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN)
		{
			getsym();
		}
		else
		{
			error(16); // 'then' expected.
		}
		cx1 = cx;
		gen(JPC, 0, 0);
		statement(fsys);
		getsym();
		//填入JPC的地址
		if (sym == SYM_ELSE) //else 添加 
		{
			
			cx2 = cx;
			gen(JMP, 0, 0);
			code[cx1].a = cx;
			getsym();
			statement(fsys);
			code[cx2].a = cx; //回填JMP地址
		}
		else
		{
			ifelse = 1;
			code[cx1].a = cx;
		} // 不存在else  即单独一个if then
	}
	else if (sym == SYM_BEGIN)
	{ // block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(10);
			}
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
		{
			getsym();
		}
		else
		{
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE)
	{ // while statement
		cx1 = cx;
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		cx2 = cx;
		gen(JPC, 0, 0);
		if (sym == SYM_DO)
		{
			getsym();
		}
		else
		{
			error(18); // 'do' expected.
		}
		statement(fsys);
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
	}
	else if (sym == SYM_GOTO) //goto的实现 by徐卓
	{
		getsym();
		if (sym != SYM_IDENTIFIER)
		{
			error(35); //There must be an label to follow the goto
		}
		else
		{
			if (!(i = position(id)))
			{
				enter(ID_LABEL);
				gen(JMP, 0, -1); //先出现goto 后面才出现label暂时填-1,表示label未出现  by徐卓
			}
			else if (table[i].kind == ID_LABEL)
			{
				if (code[table[i].value].f == JMP && code[table[i].value].a == -1)
				{
					code[table[i].value].a = cx;
					table[i].value = cx;
					gen(JMP, 0, -1); //依然未出现label 但之前出现了GOTO,使上一个GOTO到下一个GOTO by徐卓
				}
				else
					gen(JMP, 0, table[i].value);
			}
			else
			{
				error(36); // A constant or variable or label can not be goto
			}
			getsym();
		}
	}
	else if (sym == SYM_PRT)
	{
		getsym();
		if (sym != SYM_LPAREN)
		{
			error(38); //The procedure name must be followed by ()
		}
		else
		{
			getsym();
			if (sym == SYM_RPAREN)
			{
				gen(PRT, 0, 0);
				getsym();
			}

			else
			{
				set = uniteset(createset(SYM_RPAREN, SYM_COMMA, SYM_NULL), fsys);
				expression(set);
				destroyset(set);
				gen(PRT, 0, 1);
				while (sym == SYM_COMMA)
				{
					getsym();
					set = uniteset(createset(SYM_RPAREN, SYM_COMMA, SYM_NULL), fsys);
					expression(set);
					destroyset(set);
					gen(PRT, 0, 1);
				}
				if (sym != SYM_RPAREN)
				{
					error(22); //missing ')'
				}
				else
				{
					getsym();
				}
			}
		}
	}

	test(fsys, phi, 19);
} // statement

//////////////////////////////////////////////////////////////////////
void block(symset fsys, int paranum) //程序体
{									 //paranum 为当前程序体的参数个数
	int cx0;						 // initial code index
	mask *mk;
	int block_dx;
	int savedTx, savedDx;
	symset set1, set;

	dx = 3;
	block_dx = dx;
	mk = (mask *)&table[tx - paranum]; //the true procedure place
	mk->address = cx;
	gen(JMP, 0, 0);
	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	do
	{
		if (sym == SYM_CONST)
		{ // constant declarations
			getsym();
			do
			{
				constdeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			} while (sym == SYM_IDENTIFIER);
		} // if

		if (sym == SYM_VAR)
		{ // variable declarations
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			} while (sym == SYM_IDENTIFIER);
		}			   // if
		block_dx = dx; //save dx before handling procedure call!
		while (sym == SYM_PROCEDURE)
		{					  // procedure declarations
			int para_num = 0; //the number of parameters
			if (in_procedure) //a nested procedure
			{
				error(37);
				getsym();
				//skip this nested procedure
				while (sym != SYM_SEMICOLON)
					getsym();
				getsym();
				break;
			}
			getsym();

			if (sym == SYM_IDENTIFIER)
			{
				enter(ID_PROCEDURE);
				getsym();
			}
			else
			{
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}

			level++;
			savedTx = tx;
			int savedTx_a = tx_a;
			ptr para;
			in_procedure = 1;	   //set mark=1 ,in procedure
			if (sym == SYM_LPAREN) //get parameters
			{
				savedDx = dx;
				getsym();
				while (sym != SYM_RPAREN) //look for ')'
				{
					enter2();
					if (sym == SYM_VAR)
					{
						getsym();
						para_num++;
						//printf("%d,%s\n", para_num, id);
						if (sym == SYM_IDENTIFIER)
						{
							enter(ID_VARIABLE);
							getsym();
						}
						else if (sym == SYM_REFERIDENTIFIER)
						{
							enter(ID_REFERVARIABLE);
							getsym();
						}
						else if (sym == SYM_ARRAYIDENTIFIER)
						{
							enter2();
							getsym();
							dimen = 0;
							strcpy(id_arr,id);
							while (sym == SYM_LBRAC)
							{
								getsym();
								if (sym == SYM_NUMBER)
								{
									atable[tx_a].dimension[dimen++] = num;
									total_wei *= num;
								}
								else if (sym == SYM_IDENTIFIER)
								{
									int posi;
									if ((posi = position(id)) && table[posi].kind == ID_CONSTANT)
									{
										atable[tx_a].dimension[dimen++] = table[posi].value;
										total_wei *= table[posi].value;
									}
								}
								getsym();
								getsym();
							}
							strcpy(id,id_arr);
							enter(ID_ARRAY);
							dimen = 0;
							total_wei = 1;
						}
					}
					else
					{
						error(39); //other reserved words
						getsym();
					}
					if (sym == SYM_COMMA)
					{
						getsym();
						if (sym == SYM_RPAREN)
						{
							error(40);
						}
					}
				}
				if (para_num) //have parameters
				{
					mask *mk_p = (mask *)&table[tx - para_num];
					para = (ptr)malloc(sizeof(prodedure_parameters));
					para->n = para_num;
					para->kind = (int *)malloc(para_num * sizeof(int));
					mk_p->procedure_para = para;
				}
				for (int i = 0; i < para_num; i++)
				{
					mask *mk_p = (mask *)&table[tx - i];
					mk_p->address = -1 - i;
					para->kind[i] = mk_p->kind;
				}
				getsym();
			}
			else
			{
				error(38);
			}
			dx = savedDx; //恢复dx的值

			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}

			savedTx = tx; //record the last parameters' place
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set, para_num);
			destroyset(set1);
			destroyset(set);
			tx = savedTx - para_num; //recover the tx to the definition of procedure
			//printf("procedure %s %d\n", table[tx].name, para_num);
			tx_a = savedTx_a; //recvoer the array table
			level--;

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				// test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}
		}			   // while
		dx = block_dx; //restore dx after handling procedure call!
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		// test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	} while (inset(sym, declbegsys));

	code[mk->address].a = cx; //需要修改 cx



	mk->address = cx;
	//printf("procedure %s %d\n", mk->name, mk->address);
	cx0 = cx;
	gen(INT, 0, block_dx);

	if(!in_procedure){
		for(int q=pre_cx-single_pre_cx;q<pre_cx;q++){
			gen(pre[q].f,pre[q].l,pre[q].a);
		}
		pre_cx-=single_pre_cx;
		single_pre_cx=0;
	}
	else
	{
		
		for(int q=pre_cx_p-single_pre_cx_p;q<pre_cx_p;q++){
			gen(pre_p[q].f,pre_p[q].l,pre_p[q].a);
		}
		pre_cx_p-=single_pre_cx_p;
		single_pre_cx_p=0;
	}
	

	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	gen(OPR, 0, OPR_RET); // return
	test(fsys, phi, 8);	  // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
	in_procedure = 0; //out of procedure
} // block

//////////////////////////////////////////////////////////////////////
//沿静态链查找
int base(int stack[], int currentLevel, int levelDiff)
{
	int b = currentLevel;

	while (levelDiff--)
		b = stack[b];
	return b;
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
void interpret()
{
	int pc; // program counter
	int stack[STACKSIZE];
	int top;	   // top of stack
	int b;		   // program, base, and top-stack register
	instruction i; // instruction register

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;
	do
	{
		i = code[pc++];
		switch (i.f)
		{
		case LIT:
			stack[++top] = i.a;
			break;
		case OPR:
			switch (i.a) // operator
			{
			case OPR_RET:
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
				break;
			case OPR_NEG:
				stack[top] = -stack[top];
				break;
			case OPR_ADD:
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV:
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_ODD:
				stack[top] %= 2;
				break;
			case OPR_EQU:
				top--;
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ:
				top--;
				stack[top] = stack[top] != stack[top + 1];
				break;
			case OPR_LES:
				top--;
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ:
				top--;
				stack[top] = stack[top] >= stack[top + 1];
				break;
			case OPR_GTR:
				top--;
				stack[top] = stack[top] > stack[top + 1];
				break;
			case OPR_LEQ:
				top--;
				stack[top] = stack[top] <= stack[top + 1];
				break;
			case OPR_AND: //
				top--;
				stack[top] = stack[top] && stack[top + 1];
				break;
			case OPR_OR: //
				top--;
				stack[top] = stack[top] || stack[top + 1];
				break;
			case OPR_NOT: //
				stack[top] = !stack[top];
				break;
			} // switch
			break;
		case LOD:
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO:
			stack[base(stack, b, i.l) + i.a] = stack[top];
			//printf("%d\n", stack[top]);
			top--;
			break;
		case LEA:
			stack[++top] = base(stack, b, i.l) + i.a;
			break;
		case LODA:
			stack[top] = stack[stack[top]];
			break;
		case STOA:
			stack[stack[top - 1]] = stack[top];
			// printf("%d\n", stack[top]);
			top = top - 2;
			break;
		case CAL:
			stack[top + 1] = base(stack, b, i.l);
			// generate new block mark
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
			break;
		case INT:
			top += i.a;
			break;
		case JMP:
			pc = i.a;
			break;
		case JPC:
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		case PRT: //
			if (i.a == 0)
				printf("\n");
			else if (i.a == 1)
			{
				printf("%d ", stack[top--]);
			}
			break;
		case RND: //
			if (i.a == 0)
				stack[++top] = rand();
			else
			{
				stack[top] = rand() % stack[top];
			}
			break;
		} // switch
	} while (pc);

	printf("\nEnd executing PL/0 program.\n");
} // interpret

//////////////////////////////////////////////////////////////////////
int main()
{
	FILE *hbin;
	char s[80];
	int i;
	symset set, set1, set2;
	srand((unsigned)time(NULL));
	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	phi = createset(SYM_NULL);
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);

	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_GOTO, SYM_PRT, SYM_NULL); //添加了GOTO by 徐卓
	facbegsys = createset(SYM_ARRAYIDENTIFIER, SYM_IDENTIFIER, SYM_RND, SYM_NOT, SYM_NUMBER, SYM_LPAREN, SYM_RBRAC, SYM_MINUS, SYM_NULL);

	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set, 0);
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	if (err == 0)
	{
		hbin = fopen("hbin.txt", "w");
		for (i = 0; i < cx; i++)
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		fclose(hbin);
	}

	if (err == 0)
		interpret();
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
	listcode(0, cx);		
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
