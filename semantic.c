/*----------------------------------------------------------------------------
 Note: the code in this file is not to be shared with anyone or posted online.
 (c) Rida Bazzi, 2015, Adam Doupe, 2015
 ----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "syntax.h"

/* ------------------------------------------------------- */
/* -------------------- LEXER SECTION -------------------- */
/* ------------------------------------------------------- */

#define KEYWORDS  11
#define str(x) #x
typedef enum
{
    END_OF_FILE = -1, VAR = 1, WHILE, INT, REAL, STRING, BOOLEAN,
    TYPE, LONG, DO, CASE, SWITCH,
    PLUS, MINUS, DIV, MULT, EQUAL,
    COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ, DOT,
    ID, NUM, REALNUM,
    ERROR
} token_type;

const char *reserved[] = {"",
    "VAR", "WHILE", "INT", "REAL", "STRING", "BOOLEAN",
    "TYPE", "LONG", "DO", "CASE", "SWITCH",
    "+", "-", "/", "*", "=",
    ":", ",", ";",
    "[", "]", "(", ")", "{", "}",
    "<>", ">", "<", "<=", ">=", ".",
    "ID", "NUM", "REALNUM",
    "ERROR"
};

// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH], global_type[100],relation[100][100],global_copy[100],id_name1[100],type_name1[100]; // token string
token_type t_type,tt_type; // token type
bool activeToken = false;
int tokenLength;
int line_no = 1,count=0;
//expression_type type1, type2;
int i=0,l=0;
int d,h;

struct types
{
    char type_name[100];
    char id_name[100];
    char predefined[100];
}typenames[100];

struct variables
{
   char type_name[100];
   char id_name[100];
   char predefined[100];
   char var_name[100];
   int det;
}varnames[100];

struct linkedlist
{
char data[100];
struct linkedlist *next;
};
struct linkedlist *head;
struct linkedlist *node;
void skipSpace()
{
    char c;

    c = getchar();
    line_no += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line_no += (c == '\n');
    }
    ungetc(c, stdin);
}

int isKeyword(char *s)
{
    int i;

    for (i = 1; i <= KEYWORDS; i++)
    {
        if (strcmp(reserved[i], s) == 0)
        {
            return i;
        }
    }
    return false;
}

/*
 * ungetToken() simply sets a flag so that when getToken() is called
 * the old t_type is returned and the old token is not overwritten.
 * NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
 * AT LEAST ONE CALL TO getToken(). CALLING TWO ungetToken() WILL NOT
 * UNGET TWO TOKENS
 */
void ungetToken()
{
    activeToken = true;
}

token_type scan_number()
{
    char c;

    c = getchar();
    if (isdigit(c))
    {
        // First collect leading digits before dot
        // 0 is a NUM by itself
        if (c == '0')
        {
            token[tokenLength] = c;
            tokenLength++;
            token[tokenLength] = '\0';
        }
        else
        {
            while (isdigit(c))
            {
                token[tokenLength] = c;
                tokenLength++;
                c = getchar();
            }
            ungetc(c, stdin);
            token[tokenLength] = '\0';
        }
        // Check if leading digits are integer part of a REALNUM
        c = getchar();
        if (c == '.')
        {
            c = getchar();
            if (isdigit(c))
            {
                token[tokenLength] = '.';
                tokenLength++;
                while (isdigit(c))
                {
                    token[tokenLength] = c;
                    tokenLength++;
                    c = getchar();
                }
                token[tokenLength] = '\0';
                if (!feof(stdin))
                {
                    ungetc(c, stdin);
                }
                return REALNUM;
            }
            else
            {
                ungetc(c, stdin);
                c = '.';
                ungetc(c, stdin);
                return NUM;
            }
        }
        else
        {
            ungetc(c, stdin);
            return NUM;
        }
    }
    else
    {
        return ERROR;
    }
}

token_type scan_id_or_keyword()
{
    token_type the_type;
    int k;
    char c;

    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c))
        {
            token[tokenLength] = c;
            tokenLength++;
            c = getchar();
        }
        if (!feof(stdin))
        {
            ungetc(c, stdin);
        }
        token[tokenLength] = '\0';
        k = isKeyword(token);
        if (k == 0)
        {
            the_type = ID;
        }
        else
        {
            the_type = (token_type) k;
        }
        return the_type;
    }
    else
    {
        return ERROR;
    }
}

token_type getToken()
{
    char c;

    if (activeToken)
    {
        activeToken = false;
        return t_type;
    }
    skipSpace();
    tokenLength = 0;
    c = getchar();
    switch (c)
    {
        case '.': return DOT;
        case '+': return PLUS;
        case '-': return MINUS;
        case '/': return DIV;
        case '*': return MULT;
        case '=': return EQUAL;
        case ':': return COLON;
        case ',': return COMMA;
        case ';': return SEMICOLON;
        case '[': return LBRAC;
        case ']': return RBRAC;
        case '(': return LPAREN;
        case ')': return RPAREN;
        case '{': return LBRACE;
        case '}': return RBRACE;
        case '<':
            c = getchar();
            if (c == '=')
            {
                return LTEQ;
            }
            else if (c == '>')
            {
                return NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                return LESS;
            }
        case '>':
            c = getchar();
            if (c == '=')
            {
                return GTEQ;
            }
            else
            {
                ungetc(c, stdin);
                return GREATER;
            }
        default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                return scan_number();
            }
            else if (isalpha(c))
            {
                ungetc(c, stdin);
                return scan_id_or_keyword();
            }
            else if (c == EOF)
            {
                return END_OF_FILE;
            }
            else
            {
                return ERROR;
            }
    }
}

/* ----------------------------------------------------------------- */
/* -------------------- SYNTAX ANALYSIS SECTION -------------------- */
/* ----------------------------------------------------------------- */

void syntax_error(const char* msg)
{
    printf("Syntax error while parsing %s line %d\n", msg, line_no);
    exit(1);
}

/* -------------------- PRINTING PARSE TREE -------------------- */
void print_parse_tree(struct programNode* program)
{
    print_decl(program->decl);
    print_body(program->body);
}

void print_decl(struct declNode* dec)
{
    if (dec->type_decl_section != NULL)
    {
        print_type_decl_section(dec->type_decl_section);
    }
    if (dec->var_decl_section != NULL)
    {
        print_var_decl_section(dec->var_decl_section);
    }
}

void print_body(struct bodyNode* body)
{
    printf("{\n");
    print_stmt_list(body->stmt_list);
    printf("}\n");
}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
    printf("VAR\n");
    if (varDeclSection->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclSection->var_decl_list);
    }
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
    print_var_decl(varDeclList->var_decl);
    if (varDeclList->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclList->var_decl_list);
    }
}

void print_var_decl(struct var_declNode* varDecl)
{
    print_id_list(varDecl->id_list);
    printf(": ");
    print_type_name(varDecl->type_name);
    printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
    printf("TYPE\n");
    if (typeDeclSection->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclSection->type_decl_list);
    }
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList)
{
    print_type_decl(typeDeclList->type_decl);
    if (typeDeclList->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclList->type_decl_list);
    }
}

void print_type_decl(struct type_declNode* typeDecl)
{
    print_id_list(typeDecl->id_list);
    printf(": ");
    print_type_name(typeDecl->type_name);
    printf(";\n");
}

void print_type_name(struct type_nameNode* typeName)
{
    if (typeName->type != ID)
    {
        printf("%s ", reserved[typeName->type]);
    }
    else
    {
        printf("%s ", typeName->id);
    }
}

void print_id_list(struct id_listNode* idList)
{
    printf("%s ", idList->id);
    if (idList->id_list != NULL)
    {
        printf(", ");
        print_id_list(idList->id_list);
    }
}

void print_stmt_list(struct stmt_listNode* stmt_list)
{
    print_stmt(stmt_list->stmt);
    if (stmt_list->stmt_list != NULL)
    {
        print_stmt_list(stmt_list->stmt_list);
    }

}

void print_assign_stmt(struct assign_stmtNode* assign_stmt)
{
    printf("%s ", assign_stmt->id);
    printf("= ");
    print_expression_prefix(assign_stmt->expr);
    printf("; \n");
}

void print_stmt(struct stmtNode* stmt)
{
    switch (stmt->stmtType)
    {
        case ASSIGN:
            print_assign_stmt(stmt->assign_stmt);
            break;
        case WHILE:
            print_while_stmt(stmt->while_stmt);
            break;
        case DO:
            print_do_stmt(stmt->while_stmt);
            break;
        case SWITCH:
            print_switch_stmt(stmt->switch_stmt);
            break;
    }
}

void print_expression_prefix(struct exprNode* expr)
{
    if (expr->tag == EXPR)
    {
        printf("%s ", reserved[expr->op]);
        print_expression_prefix(expr->leftOperand);
        print_expression_prefix(expr->rightOperand);
    }
    else if (expr->tag == PRIMARY)
    {
        if (expr->primary->tag == ID)
        {
            printf("%s ", expr->primary->id);
        }
        else if (expr->primary->tag == NUM)
        {
            printf("%d ", expr->primary->ival);
        }
        else if (expr->primary->tag == REALNUM)
        {
            printf("%.4f ", expr->primary->fval);
        }
    }
}

void print_while_stmt(struct while_stmtNode* while_stmt)
{
    // TODO: implement this for your own debugging purposes
    printf("WHILE ");
    print_condition(while_stmt->condition);
    print_body(while_stmt->body);
}

void print_do_stmt(struct while_stmtNode* do_stmt)
{
    // TODO: implement this for your own debugging purposes
    printf("DO");
    print_body(do_stmt->body);
    printf("WHILE ");
    print_condition(do_stmt->condition);
    printf(";");
    
}

void print_condition(struct conditionNode* condition)
{
    // TODO: implement this for your own debugging purposes
      if(condition->left_operand->tag==NUM)
	printf("%d ",condition->left_operand->ival);
      else if(condition->left_operand->tag==REALNUM)
        printf("%.4f ",condition->left_operand->fval);
      else if(condition->left_operand->tag==ID)
        printf("%s ", condition->left_operand->id);
	
      printf("%s ",reserved[condition->relop]);

      if(condition->right_operand->tag==NUM)
        printf("%d ",condition->right_operand->ival);
      else if(condition->right_operand->tag==REALNUM)
        printf("%.4f ",condition->right_operand->fval);
      else if(condition->right_operand->tag==ID)
        printf("%s ", condition->right_operand->id);


}

void print_case(struct caseNode* cas)
{
    // TODO: implement this for your own debugging purposes
    // CASE NUM COLON body
    printf("CASE ");
    printf("%d ",cas->num);
    printf(": ");
    print_body(cas->body);
}

void print_case_list(struct case_listNode* case_list)
{
    // TODO: implement this for your own debugging purposes
        print_case(case_list->cas);
    if (case_list->case_list != NULL)
    {
        print_case_list(case_list->case_list);
    }

}

void print_switch_stmt(struct switch_stmtNode* switc)
{
    // TODO: implement this for your own debugging purposes
    // SWITCH ID LBRACE case_list RBRACE
    printf("SWITCH ");
    printf("%s ",switc->id);
    printf("{\n");
    print_case_list(switc->case_list);
    printf("\n}");

}

/* -------------------- PARSING AND BUILDING PARSE TREE -------------------- */

// Note that the following function is not
// called case because case is a keyword in C/C++
void typecopy(int j,int k)
{	int a;
	if(strcmp(typenames[j].predefined,"")!=0)
	{
		strcpy(typenames[k].predefined,typenames[j].predefined);
	}
	else if(strcmp(typenames[j].id_name,"")!=0)
	{
		for(a=0;a<i;a++)
		{
			if(strcmp(typenames[a].type_name,typenames[j].id_name)==0)
			{
				if(strcmp(typenames[a].predefined,"")!=0)
				{strcpy(typenames[j].predefined,typenames[a].predefined);break;}
				else
				{
					typecopy(a,j);
					strcpy(typenames[j].predefined,typenames[a].predefined);
					strcpy(typenames[k].predefined,typenames[j].predefined);
				}
			}
		}
	}
}
struct caseNode* cas()
{
    // TODO: implement this for extra credit
    struct caseNode*ca;
    ca=ALLOC(struct caseNode);

    t_type=getToken();
    if(t_type==CASE)
    {
    	t_type=getToken();
	if(t_type==NUM)
	{
        
	    ca->num=atoi(token);
	
	    t_type=getToken();
	    if(t_type==COLON)
	    {
	    //t_type=getToken();
	    //ca->body=ALLOC(struct bodyNode);
	    ca->body=body();
	    return ca;
	    }
	    else
	    syntax_error("case. COLON expected");
	}
	 else
            syntax_error("case. NUM expected");
    }
    else
    syntax_error("case. CASE expected");

    return NULL;
}

struct case_listNode* case_list()
{
    // TODO: implement this for extra credit
    struct case_listNode* caseList;

    t_type = getToken();
    if (t_type == CASE)
    {
        ungetToken();
        caseList = ALLOC(struct case_listNode);
        caseList->cas = cas();
        t_type = getToken();
        if (t_type == CASE)
        {
            ungetToken();
            caseList->case_list = case_list();
            return caseList;
        }
        else // If the next token is not in FOLLOW(stmt_list),
            // let the caller handle it.
        {
            ungetToken();
            caseList->case_list = NULL;
            return caseList;
        }
     }
     else
    {
        syntax_error("stmt_list. ID, WHILE, DO or SWITCH expected");
    }
    assert(false);

        return NULL;
}

struct switch_stmtNode* switch_stmt()
{
    // TODO: implement this for extra credit
    struct switch_stmtNode* switchstmt;
    t_type = getToken();
    if(t_type==SWITCH)
    {
         int j;
         t_type=getToken();
	 if(t_type==ID)
	 {
	     switchstmt = ALLOC(struct switch_stmtNode);
	     switchstmt->id = strdup(token);
	      for(j=0;j<i;j++)
        if(strcmp(typenames[j].type_name,switchstmt->id)==0)
        {printf("ERROR CODE 1.4 %s\n",switchstmt->id);exit(0);}
        else if(strcmp(typenames[j].id_name,switchstmt->id)==0)
        {printf("ERROR CODE 1.4 %s\n",switchstmt->id);exit(0);}
	 else if(strcmp(typenames[j].predefined,switchstmt->id)==0)
        {printf("ERROR CODE 1.4 %s\n",switchstmt->id);exit(0);}
	     for(j=0;j<l;j++)
        if(strcmp(varnames[j].var_name,switchstmt->id)==0)
        {
                if(strcmp(varnames[j].predefined,"")!=0)
                {
                        if(strcmp(varnames[j].predefined,reserved[INT])!=0 && strcmp(varnames[j].predefined,reserved[NUM])!=0)
                        {
                                printf("TYPE MISMATCH %d C5\n");exit(0);
                        }
                        else
                        break;
                }
                else
                {
                        strcpy(varnames[j].predefined,reserved[INT]);break;
                }
        }
        if(j==l)
        {
                strcpy(varnames[j].var_name,switchstmt->id);
                strcpy(varnames[j].predefined,reserved[INT]);
                l++;
        }
	
	     t_type=getToken();
	     if(t_type==LBRACE)
	     {
	         switchstmt->case_list = case_list();
		 t_type=getToken();
		 if(t_type==RBRACE)
		    return switchstmt;
		 else
		    syntax_error("switch_stmt. RBRACE expected");
	     }
	     else
		syntax_error("switch_stmt. LBRACE expected");
	 }
	 else
	    syntax_error("switch_stmt. ID expected");
    }
    else
    syntax_error("switch_stmt. SWITCH expected");
    return NULL;
}

struct while_stmtNode* do_stmt()
{
    // TODO: implement this
    struct while_stmtNode* dostmt;
    t_type=getToken();
    if(t_type==DO)
    {
	dostmt=ALLOC(struct while_stmtNode);
	//dostmt->body=ALLOC(struct bodyNode);
	dostmt->body=body();
	t_type=getToken();
	if(t_type==WHILE)
	{
	  //  dostmt->condition=ALLOC(struct conditionNode);
	    dostmt->condition=condition();
	    t_type=getToken();
	    if(t_type==SEMICOLON)
		return dostmt;
	    else
		syntax_error("do_stmt. SEMICOLON expected");
	}
	else
	    syntax_error("asign_stmt. WHILE expected");
    }
    else
	syntax_error("asign_stmt. DO expected");
    assert(false);
    return NULL;
}

struct primaryNode* primary()
{
   /* // TODO: implement this*/
    struct primaryNode* primar;
	int j;
    primar = ALLOC(struct primaryNode);
    t_type = getToken();
    if (t_type == NUM) 
    {
       primar->tag = NUM;
       primar->ival = atoi(token);
       return primar;
    }
    else if (t_type == REALNUM) 
    {
       primar->tag = REALNUM;
       primar->fval = atof(token);
       return primar;
    }
    if (t_type == ID) 
    {
       primar->tag = ID;
       primar->id = strdup(token);
	for(j=0;j<i;j++)
	if(strcmp(typenames[j].type_name,primar->id)==0)
	{printf("ERROR CODE 1.4 %s\n",primar->id);exit(0);}
	else if(strcmp(typenames[j].id_name,primar->id)==0)
	{printf("ERROR CODE 1.4 %s\n",primar->id);exit(0);}


       return primar;
    }
    else
    {
        syntax_error("primary. NUM, REALNUM, or ID, expected");
    }
    
    return NULL;
}

struct conditionNode* condition()
{
    // TODO: implement this
    struct conditionNode* cond;
    int j,k;
    char type1[100],type2[100],typeID1[100],typeID2[100];
    strcpy(type1,"");
    strcpy(type2,"");
    strcpy(typeID1,"");
    strcpy(typeID2,"");

    t_type=getToken();
    if(t_type==REAL||t_type==ID||t_type==NUM)
    {
	ungetToken();
	cond=ALLOC(struct conditionNode);
	cond->left_operand=primary();
	 strcpy(type1,reserved[cond->left_operand->tag]);
        if(strcmp(type1,reserved[ID])==0)
	{
		strcpy(typeID1,cond->left_operand->id);
		for(j=0;j<l;j++)
		{
			if(strcmp(typeID1,varnames[j].var_name)==0)
			{
				if(strcmp(varnames[j].predefined,"")!=0)
				{
					strcpy(typeID1,varnames[j].predefined);
					k=j;
					break;
				}
				else
				{	 if(strcmp(varnames[j].id_name,"")!=0)
                                        {strcpy(global_copy,varnames[j].id_name);}
                                        else if(strcmp(varnames[j].type_name,"")!=0)
                                        {strcpy(global_copy,varnames[j].type_name);}

					strcpy(typeID1,"");k=j;break;
				}
			}
		}
		if(j==l)
		{
			strcpy(varnames[j].var_name,strdup(token));
			k=j;l++;
			strcpy(typeID1,"");
		}
	}
	t_type=getToken();
	if(t_type==GREATER || t_type==GTEQ || t_type==LESS || t_type==LTEQ || t_type==NOTEQUAL)
	{
		cond->relop=t_type;
 		cond->right_operand=primary();
		 strcpy(type2,reserved[cond->right_operand->tag]);
                if(strcmp(type2,reserved[ID])==0)
                {
                        strcpy(typeID2,cond->right_operand->id);
                        for(j=0;j<l;j++)
                        {
				 if(strcmp(typeID2,varnames[j].var_name)==0)
                                {
					if(strcmp(varnames[j].predefined,"")!=0)
					{
						strcpy(typeID2,varnames[j].predefined);
						if(strcmp(type1,reserved[ID])!=0)
						{
							if(strcmp(typeID2,type1)!=0)
							{printf("TYPE MISMATCH %d C3");exit(0);}
							else
							return cond;
						}
						else
						{
							if(strcmp(typeID1,"")!=0)
							{
							 if(strcmp(typeID2,typeID1)!=0)
                                                        {printf("TYPE MISMATCH %d C3");exit(0);}
                                                        else
                                                        return cond;
							}
							else
							{

							strcpy(varnames[k].predefined,typeID2);
							return cond;
							}
						}
					}
					else
					{	
						if(strcmp(type1,reserved[ID])!=0)
						{
							strcpy(varnames[j].predefined,type1);
							return cond;
						}
						else if(strcmp(typeID1,"")!=0)
						{
							strcpy(varnames[j].predefined,typeID1);
							return cond;
						}
						else
						{ 
							if(strcmp(varnames[j].id_name,"")!=0)
				                          {
                                                        strcpy(varnames[k].id_name,varnames[j].id_name);
                                                          }
							else if(strcmp(varnames[j].type_name,"")!=0)
							{
							strcpy(varnames[k].type_name,varnames[j].type_name);
							}
                                                        else if(strcmp(varnames[k].id_name,"")!=0)        					                  {
                                            
							           strcpy(varnames[j].id_name,varnames[k].id_name);   
				                      }
							else if(strcmp(varnames[k].type_name,"")!=0)                                                                {
                                                        
                                                                   strcpy(varnames[j].type_name,varnames[k].type_name);                     
                                                      } 

							return cond;
						}
					
					}
				}
			}
			if(j==l)
			{
				if(strcmp(type1,reserved[ID])!=0)
				{
					 strcpy(varnames[j].predefined,type1);l++;
                                         return cond;
				}
				else if(strcmp(typeID1,"")!=0)
				{
					strcpy(varnames[j].predefined,typeID1);
					strcpy(typeID2,typeID1);
					l++;return cond;
				}
				else
                                {
                                        return cond;
				}
			}
		}
		else
		{
			if(strcmp(type1,reserved[ID])!=0)
			{
				if(strcmp(type1,type2)!=0)
				{printf("TYPE MISMATCH %d C3");exit(0);}
				else
				return cond;
			}
			else
			{
				if(strcmp(typeID1,"")!=0)
				{
					if(strcmp(typeID1,type2)!=0)
					{printf("TYPE MISMATCH %d C3",line_no);exit(0);}
					else
					return cond;
				}
				else
				{
					strcpy(varnames[k].predefined,type2);return cond;
				}
			}
		}

		return cond;
	}
	else if(t_type==LBRACE)
	{
		ungetToken();
		if(strcmp(type1,reserved[ID])==0)
	        {	//printf("\n %s ",varnames[k].var_name);
        		for(j=0;j<l;j++)
        		if(strcmp(varnames[k].var_name,varnames[j].var_name)==0)
        		{
		                if(strcmp(varnames[k].predefined,"")==0)
                		{strcpy(varnames[k].predefined,reserved[BOOLEAN]);return cond;}
                		else if(strcmp(reserved[BOOLEAN],varnames[k].predefined)!=0)
                		{printf("TYPE MISMATCH %d C4",line_no);exit(0);}
                		else
                		return cond;
        		}
		 /*if(j==l)
        	 {strcpy(varnames[j].var_name,strdup(token));varnames[j].predefined=BOOLEAN;l++;return cond;}*/
		 }
		else
		syntax_error("condition. relational operator expected");		
	}
	else
	 if(t_type==SEMICOLON)
        {
                ungetToken();
                if(strcmp(type1,reserved[ID])==0)
                {       //printf("\n %s ",varnames[k].var_name);
                        for(j=0;j<l;j++)
                        if(strcmp(varnames[k].var_name,varnames[j].var_name)==0)
                        {
                                if(strcmp(varnames[k].predefined,"")==0)
                                {strcpy(varnames[k].predefined,reserved[BOOLEAN]);return cond;}
                                else if(strcmp(reserved[BOOLEAN],varnames[k].predefined)!=0)
                                {printf("TYPE MISMATCH %d C4",line_no);exit(0);}
                                else
                                return cond;
                        }
                 /*if(j==l)
 *                  {strcpy(varnames[j].var_name,strdup(token));varnames[j].predefined=BOOLEAN;l++;return cond;}*/
                 }
                else
                syntax_error("condition. relationaloperator expected");
        }

     }
     else
	 syntax_error("condition. ID, NUM or REALNUM expected");
    return NULL;
}

struct while_stmtNode* while_stmt()
{
    // TODO: implement this
    struct while_stmtNode* whilestmt;
    t_type=getToken();
    
        if(t_type==WHILE)
        {   
	   whilestmt=ALLOC(struct while_stmtNode);
          //  whilestmt->condition=ALLOC(struct conditionNode);
            whilestmt->condition=condition();
                whilestmt->body=body();
	        return whilestmt;
         }
         else
	    syntax_error("while_stmt. WHILE expected");

    assert(false);

    return NULL;
}

struct exprNode* factor()
{
    struct exprNode* facto;
	int q,w,x;
	struct linkedlist *node2;
    t_type = getToken();
    if (t_type == LPAREN)
    {
        facto = expr();
        t_type = getToken();
        if (t_type == RPAREN)
        {
            return facto;
        }
        else
        {
            syntax_error("factor. RPAREN expected");
        }
    }
    else if (t_type == NUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = NUM;
        facto->primary->ival = atoi(token);
	if(strcmp(global_type,"")==0)
	{
		strcpy(global_type,"NUM");count++;
	}
	else 
	{
		if((strcmp(reserved[NUM],global_type)!=0)&&(strcmp(reserved[INT],global_type)!=0))
		{
			if(count==0)
			{printf("TYPE MISMATCH %d C1\n",line_no);exit(0);}
			else
			{printf("TYPE MISMATCH %d C2\n",line_no);exit(0);}
		}

	}
        return facto;
    }
    else if (t_type == REALNUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = REALNUM;
        facto->primary->fval = atof(token);
	if(strcmp(global_type,"")==0)
        {
                strcpy(global_type,"REALNUM");count++;
        }
        else
        {
                if(strcmp(reserved[REALNUM],global_type)!=0 && strcmp(reserved[REAL],global_type)!=0)
                {
                        if(count==0)
                        {printf("TYPE MISMATCH %d C1\n",line_no);exit(0);}
                        else
                        {printf("TYPE MISMATCH %d C2\n",line_no);exit(0);}
                }
        }        


        return facto;
    }
    else if (t_type == ID)
    {	int j;
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = ID;
        facto->primary->id = strdup(token);
	 for(j=0;j<i;j++)
        if(strcmp(typenames[j].type_name,facto->primary->id)==0)
        {printf("ERROR CODE 1.4 %s\n",typenames[j].type_name);exit(0);}
        else if(strcmp(typenames[j].id_name,facto->primary->id)==0)
        {printf("ERROR CODE 1.4 %s\n",typenames[j].id_name);exit(0);}
	 else if(strcmp(typenames[j].predefined,facto->primary->id)==0)
        {printf("ERROR CODE 1.4 %s\n",typenames[j].id_name);exit(0);}
	
	//node=(struct linkedlist*)malloc(sizeof(struct linkedlist));
	/*node=head;
	while(node->next!=NULL)
	node=node->next;
	node2=(struct linkedlist*)malloc(sizeof(struct linkedlist));
        //node2->data=(char*)malloc(sizeof(char));
        strcpy(node2->data,strdup(token));
        node2->next=NULL;
        node->next=node2;
        */
	strcpy(relation[d],strdup(token));
	d++;
	if(strcmp(global_type,"")!=0)
	{
		/*check for the variable*/
		for(q=0;q<l;q++)
		{
			if(strcmp(varnames[q].var_name,facto->primary->id)==0)
			{	
				
				if(strcmp(varnames[q].predefined,"")!=0)
				{if(strcmp(varnames[q].predefined,global_type)!=0)
				{
					if(count==0)
		                        {printf("TYPE MISMATCH %d C1\n",line_no);exit(0);}
                		        else
                        		{printf("TYPE MISMATCH %d C2\n",line_no);exit(0);}
				}
				else
				break;
				
				}
			/*	else if(strcmp(varnames[q].id_name,"")!=0)
				{if(strcmp(varnames[q].id_name,global_type)!=0)
                                {
                                        if(count==0)
                                        {printf("TYPE MISMATCH %d C1 904",line_no);exit(0);}
                                        else
                                        {printf("TYPE MISMATCH %d C2 906",line_no);exit(0);}
                                }
				else
				break;
			
				}*/
				else  if( strcmp(varnames[q].predefined,"")==0) 
				{
					strcpy(varnames[q].predefined,global_type);	count++;break;
				}
				
				/*else if(strcmp(global_type,"")==0 )
				{
					strcpy(global_type,""); count++;break;	
				}*/
			}
		}
		if(q==l)
		{
			w=q;l++;
			strcpy(varnames[w].var_name,facto->primary->id);
			if(strcmp(global_type,"")!=0)
			{
				for(x=0;x<35;x++)
				if(strcmp(global_type,reserved[x])==0)
				{
					strcpy(varnames[w].predefined,global_type);count++;break;
				}
				/*if(x==35)
				{
					strcpy(varnames[w].id_name,global_type);count++;
				}*/
			}
							
		}
	}
	else
	{
		for(q=0;q<l;q++)
                {
                        if(strcmp(varnames[q].var_name,facto->primary->id)==0)
                        {
                                if(strcmp(varnames[q].predefined,"")!=0)
                                {
                                    strcpy(global_type,varnames[q].predefined);break;
                                }
				              
                                /*else if(strcmp(varnames[q].id_name,"")!=0)
                                {
					strcpy(global_type,varnames[q].id_name);break;
                                }*/
				else
				{
					strcpy(global_type,"");break;
				}
                         }
		}
		 if(q==l)
                {
                        w=q;l++;
                        strcpy(varnames[w].var_name,facto->primary->id);
                }

	}
	/*node2=(struct linkedlist*)malloc(sizeof(struct linkedlist));
	//node2->data=(char*)malloc(sizeof(char));
	strcpy(node2->data,strdup(token));
	node2->next=NULL;
	node->next=node2;*/
        return facto;
    }
    else
    {
        syntax_error("factor. NUM, REALNUM, or ID, expected");
    }
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* term()
{
    struct exprNode* ter;
    struct exprNode* f;

    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        f = factor();
	//type1 = f->primary->tag;
	
        t_type = getToken();
        if (t_type == MULT || t_type == DIV)
        {
            ter = ALLOC(struct exprNode);
            ter->op = t_type;
            ter->leftOperand = f;
	  //  type1 = f->primary->tag;
	    count++;
            ter->rightOperand = term();
	    
            ter->tag = EXPR;
            ter->primary = NULL;
	/*	if(type1==type2)
		{type2=type1;*/
		return ter;//}
		/*else
		{syntax_error("type mismatch");
            }*/
        }
        else if (t_type == SEMICOLON || t_type == PLUS ||
                 t_type == MINUS || t_type == RPAREN)
        {
	    
	//	type2=type1;    
            ungetToken();
            return f;
        }
        else
        {
            syntax_error("term. MULT or DIV expected");
        }
    }
    else
    {
        syntax_error("term. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* expr()
{
    struct exprNode* exp;
    struct exprNode* t;

    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        t = term();
	
        t_type = getToken();
        if (t_type == PLUS || t_type == MINUS)
        {
            exp = ALLOC(struct exprNode);
            exp->op = t_type;
            exp->leftOperand = t;
		count++;
            exp->rightOperand = expr();
            exp->tag = EXPR;
            exp->primary = NULL;
            return exp;
        }
        else if (t_type == SEMICOLON || t_type == MULT ||
                 t_type == DIV || t_type == RPAREN)
        {
            ungetToken();
            return t;
        }
        else
        {
            syntax_error("expr. PLUS, MINUS, or SEMICOLON expected");
        }
    }
    else
    {
        syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct assign_stmtNode* assign_stmt()
{
    struct assign_stmtNode* assignStmt;
	int j,k,m,y,z,b;
	//*head = NULL;
	//struct linkedlist *node;
	struct linkedlist *node2;
    t_type = getToken();
    if (t_type == ID)
    {
        assignStmt = ALLOC(struct assign_stmtNode);
        assignStmt->id = strdup(token);
	  for(j=0;j<i;j++)
        if(strcmp(typenames[j].type_name,assignStmt->id)==0)
        {printf("ERROR CODE 1.4 %s\n",typenames[j].type_name);exit(0);}
        else if(strcmp(typenames[j].id_name,assignStmt->id)==0)
        {printf("ERROR CODE 1.4 %s\n",typenames[j].id_name);exit(0);}
	else if(strcmp(typenames[j].predefined,assignStmt->id)==0)
	{printf("ERROR CODE 1.4 %s\n",typenames[j].id_name);exit(0);}
	/*if(head==NULL)
	{	
		node=(struct linkedlist*)malloc(sizeof(struct linkedlist));
		//node->data=(char*)malloc(sizeof(char));
		strcpy(node->data,strdup(assignStmt->id));
		node->next=NULL;
		head=node;
	}
	else
		{printf("assignment\n");	
		//node=(struct linkedlist*)malloc(sizeof(struct linkedlist));
		node=head;
		printf("1069");
		while(node->next!=NULL)
		node=node->next;
		node2=(struct linkedlist*)malloc(sizeof(struct linkedlist));
		//node2->data=(char*)malloc(sizeof(char));
		strcpy(node2->data,strdup(assignStmt->id));
		node2->next=NULL;
		node->next=node2;
		}
		printf("after\n");*/
	/*check if the token is already in varlist hence set g_type*/
	for(k=0;k<l;k++)
	{
		if(strcmp(varnames[k].var_name,assignStmt->id)==0)
		{
			if(strcmp(varnames[k].predefined,"")!=0)
			{strcpy(global_type,varnames[k].predefined);break;}
			/*else if(strcmp(varnames[k].id_name,"")!=0)
                        {strcpy(global_type,varnames[k].id_name);break;}*/
			else
			{strcpy(global_type,"");break;}
		}
	}
	if(k==l)
	{
		m=k;
		strcpy(varnames[m].var_name,assignStmt->id);
		l++;
	}
	strcpy(relation[d],strdup(token));
	d++;
        t_type = getToken();
        if (t_type == EQUAL)
        {
            assignStmt->expr = expr();
	    //if(strcpy(global_type,"")!=0)
	//	{	
			if(strcmp(global_type,"")!=0)
                 	{
                            /* to be changed */ // for(z=0;z<i;z++)
                               // if(strcmp(global_type,typenames[z].id_name)==0)
                           /*     {
                                        
                                        z=999999;break;
                                }*/
                         }
			/*node=(struct linkedlist*)malloc(sizeof(struct linkedlist));
			node=head;
			while(node!=NULL)
			{	
				for(y=0;y<l;y++)
				if(strcmp(node->data,varnames[y].var_name)==0)
				{
					if(z>=35)
					strcpy(varnames[y].id_name,global_type);
					else
					strcpy(varnames[y].predefined,global_type);
				}
				node=node->next;
			}*/
		/*printf("\n %d \n",z);
		for(h=0;h<d;h++)
		{
		printf("%s\t",relation[h]);
		}*/
		if(strcmp(global_type,"")!=0)
		for(h=0;h<d;h++)
		{
			for(j=0;j<l;j++)
			if(strcmp(relation[h],varnames[j].var_name)==0)
			{	
				strcpy(varnames[j].predefined,global_type);
			}
			
		}
	 	//printf("\n GT%s \n",global_type); 
		
		strcpy(global_copy,global_type);
		  if(strcmp(global_type,"")==0)
			{
				 for(h=0;h<d;h++)
                {
                        for(j=0;j<l;j++)
                        if(strcmp(relation[h],varnames[j].var_name)==0)
                        {
                                if(strcmp(varnames[j].type_name,"")!=0)
				if(strcmp(global_type,"")==0)
				{strcpy(global_type,varnames[j].type_name);}
				                                
                        }

                }
		 for(h=0;h<d;h++)
                {
                        for(j=0;j<l;j++)
                        if(strcmp(relation[h],varnames[j].var_name)==0)
                        {
				strcpy(varnames[j].type_name,global_type);
	
			}
		}}
		 for(h=0;h<d;h++)
                {
                        for(j=0;j<l;j++)
                        if(strcmp(relation[h],varnames[j].var_name)==0)
                        {
				// if(strcmp(varnames[j].id_name,"")!=0)
				 {strcpy(global_type,varnames[j].id_name);}

                         } 
		}
		 for(h=0;h<d;h++)
                {
                        for(j=0;j<l;j++)
                        if(strcmp(relation[h],varnames[j].var_name)==0)
                        {
                                strcpy(varnames[j].id_name,global_type);

                        }
                }
		if(strcmp(global_copy,"")!=0)
		for(h=0;h<d;h++)
		{
			for(j=0;j<l;j++)
			if(strcmp(relation[h],varnames[j].var_name)==0)
			{
				if(strcmp(varnames[j].id_name,"")!=0)
				{
					strcpy(id_name1,varnames[j].id_name);
					for(b=0;b<l;b++)
					if(strcmp(id_name1,varnames[b].id_name)==0)
					{
						strcpy(varnames[b].predefined,global_copy);
					}break;
				}
				else if(strcmp(varnames[j].type_name,"")!=0)
                                {
                                        strcpy(type_name1,varnames[j].type_name);
                                        for(b=0;b<l;b++)
                                        if(strcmp(type_name1,varnames[b].type_name)==0)
                                        {
                                                strcpy(varnames[b].predefined,global_copy);
                                        }break;
                                }

			}
		}
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
		/*node=head;
		while(node!=NULL)
		{printf("\n %s",node->data);node=node->next;}*/
                return assignStmt;
            }
            else
            {
                syntax_error("asign_stmt. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("assign_stmt. EQUAL expected");
        }
    }
    else
    {
        syntax_error("assign_stmt. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmtNode* stmt()
{
    struct stmtNode* stm;

    stm = ALLOC(struct stmtNode);
    t_type = getToken();
	count=0;
	strcpy(global_type,"");
	//head=(struct linkedlist*)malloc(sizeof(struct linkedlist));
	head=NULL;
	d=0;h=0;
	if (t_type == ID) // assign_stmt
    {
        ungetToken();
        stm->assign_stmt = assign_stmt();
        stm->stmtType = ASSIGN;
    }
    else if (t_type == WHILE) // while_stmt
    {
        ungetToken();
        stm->while_stmt = while_stmt();
        stm->stmtType = WHILE;
    }
    else if (t_type == DO)  // do_stmt
    {
        ungetToken();
        stm->while_stmt = do_stmt();
        stm->stmtType = DO;
    }
    else if (t_type == SWITCH) // switch_stmt
    {
        ungetToken();
        stm->switch_stmt = switch_stmt();
        stm->stmtType = SWITCH;
    }
    else
    {
        syntax_error("stmt. ID, WHILE, DO or SWITCH expected");
    }
	free(head);
    return stm;
}

struct stmt_listNode* stmt_list()
{
    struct stmt_listNode* stmtList;

    t_type = getToken();
    if (t_type == ID || t_type == WHILE ||
        t_type == DO || t_type == SWITCH)
    {
        ungetToken();
        stmtList = ALLOC(struct stmt_listNode);
        stmtList->stmt = stmt();
        t_type = getToken();
        if (t_type == ID || t_type == WHILE ||
            t_type == DO || t_type == SWITCH)
        {
            ungetToken();
            stmtList->stmt_list = stmt_list();
            return stmtList;
        }
        else // If the next token is not in FOLLOW(stmt_list),
            // let the caller handle it.
        {
            ungetToken();
            stmtList->stmt_list = NULL;
            return stmtList;
        }
    }
    else
    {
        syntax_error("stmt_list. ID, WHILE, DO or SWITCH expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct bodyNode* body()
{
    struct bodyNode* bod;

    t_type = getToken();
    if (t_type == LBRACE)
    {
        bod = ALLOC(struct bodyNode);
        bod->stmt_list = stmt_list();
        t_type = getToken();
        if (t_type == RBRACE)
        {
            return bod;
        }
        else
        {
            syntax_error("body. RBRACE expected");
        }
    }
    else
    {
        syntax_error("body. LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_nameNode* type_name()
{
    struct type_nameNode* tName;

    tName = ALLOC(struct type_nameNode);
    t_type = getToken();
    if (t_type == ID || t_type == INT || t_type == REAL ||
        t_type == STRING || t_type == BOOLEAN || t_type == LONG)
    {
        tName->type = t_type;

	
        if (t_type == ID)
        {
            tName->id = strdup(token);
	    
        }
        else
        {
            tName->id = NULL;
        }
        return tName;
    }
    else
    {
        syntax_error("type_name. type name expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct id_listNode* id_list()
{
    struct id_listNode* idList;

    idList = ALLOC(struct id_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        idList->id = strdup(token);
	
        t_type = getToken();
        if (t_type == COMMA)
        {
            idList->id_list = id_list();
            return idList;
        }
        else if (t_type == COLON)
        {
            ungetToken();
            idList->id_list = NULL;
            return idList;
        }
        else
        {
            syntax_error("id_list. COMMA or COLON expected");
        }
    }
    else
    {
        syntax_error("id_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_declNode* type_decl()
{
    struct type_declNode* typeDecl;

    typeDecl = ALLOC(struct type_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDecl->id_list = id_list();
	
        t_type = getToken();
        if (t_type == COLON)
        {
            typeDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                return typeDecl;
            }
            else
            {
                syntax_error("type_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("type_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("type_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_declNode* var_decl()
{
    struct var_declNode* varDecl;

    varDecl = ALLOC(struct var_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            varDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                return varDecl;
            }
            else
            {
                syntax_error("var_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("var_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("var_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_listNode* var_decl_list()
{
    struct var_decl_listNode* varDeclList;

    varDeclList = ALLOC(struct var_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDeclList->var_decl = var_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            varDeclList->var_decl_list = var_decl_list();
            return varDeclList;
        }
        else
        {
            ungetToken();
            varDeclList->var_decl_list = NULL;
            return varDeclList;
        }
    }
    else
    {
        syntax_error("var_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_listNode* type_decl_list()
{
    struct type_decl_listNode* typeDeclList;

    typeDeclList = ALLOC(struct type_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDeclList->type_decl = type_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            typeDeclList->type_decl_list = type_decl_list();
            return typeDeclList;
        }
        else
        {
            ungetToken();
            typeDeclList->type_decl_list = NULL;
            return typeDeclList;
        }
    }
    else
    {
        syntax_error("type_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_sectionNode* var_decl_section()
{
    struct var_decl_sectionNode *varDeclSection;

    varDeclSection = ALLOC(struct var_decl_sectionNode);
    t_type = getToken();
    if (t_type == VAR)
    {
        // no need to ungetToken()
        varDeclSection->var_decl_list = var_decl_list();
        return varDeclSection;
    }
    else
    {
        syntax_error("var_decl_section. VAR expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_sectionNode* type_decl_section()
{
    struct type_decl_sectionNode *typeDeclSection;

    typeDeclSection = ALLOC(struct type_decl_sectionNode);
    t_type = getToken();
    if (t_type == TYPE)
    {
        typeDeclSection->type_decl_list = type_decl_list();
        return typeDeclSection;
    }
    else
    {
        syntax_error("type_decl_section. TYPE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct declNode* decl()
{
    struct declNode* dec;
    struct type_decl_sectionNode* tree;
    struct type_decl_listNode* free;

    struct type_declNode *mode;
    struct id_listNode* node;
    dec = ALLOC(struct declNode);
    dec->type_decl_section = NULL;
    dec->var_decl_section = NULL;
    
    t_type = getToken();
    if (t_type == TYPE)
    {
        ungetToken();
        dec->type_decl_section = type_decl_section();
	tree=dec->type_decl_section;
	/*while(tree!=NULL)
	{*/
	free=tree->type_decl_list;
	while(free!=NULL)
	{mode=free->type_decl;
	node=mode->id_list;
	while(node!=NULL)
	{
	strcpy(typenames[i].type_name,node->id);
	if(mode->type_name->type!=ID)
	strcpy(typenames[i].predefined,reserved[mode->type_name->type]);
	else
	{
	strcpy(typenames[i].id_name,mode->type_name->id);}
	/* printf("%s : ",typenames[i].type_name);
        printf("%d\n",typenames[i].predefined);*/
	i++;
	node=node->id_list;
	}
        free=free->type_decl_list;
	}
	t_type = getToken();
        if (t_type == VAR)
        {   struct var_decl_sectionNode* tree1;
	    struct var_decl_listNode* free1;
	    struct var_declNode* mode1;
	    struct id_listNode* node1;
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
	    tree1=dec->var_decl_section;
        /*while(tree!=NULL)
 *         {*/
        free1=tree1->var_decl_list;
        while(free1!=NULL)
        {mode1=free1->var_decl;
        node1=mode1->id_list;
        while(node1!=NULL)
        {
        strcpy(varnames[l].var_name,node1->id);
        if(mode1->type_name->type!=ID)
        {strcpy(varnames[l].predefined,reserved[mode1->type_name->type]);}
        else
        {
        strcpy(varnames[l].type_name,mode1->type_name->id);}
        /* printf("%s : ",typenames[i].type_name);
 *         printf("%d\n",typenames[i].predefined);*/
        l++;
        node1=node1->id_list;
        }
        free1=free1->var_decl_list;

        }}
        else
        {
            ungetToken();
            dec->var_decl_section = NULL;
        }
        return dec;
    }
    else
    {
        dec->type_decl_section = NULL;
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
	     struct var_decl_sectionNode* tree1;
            struct var_decl_listNode* free1;
            struct var_declNode* mode1;
            struct id_listNode* node1;
	     tree1=dec->var_decl_section;
        /*while(tree!=NULL)
 *  *         {*/
        free1=tree1->var_decl_list;
        while(free1!=NULL)
        {mode1=free1->var_decl;
        node1=mode1->id_list;
        while(node1!=NULL)
        {
        strcpy(varnames[l].var_name,node1->id);
        if(mode1->type_name->type!=ID)
        {strcpy(varnames[l].predefined,reserved[mode1->type_name->type]);}
        else
        {
        strcpy(varnames[l].type_name,mode1->type_name->id);}
        /* printf("%s : ",typenames[i].type_name);
 *  *         printf("%d\n",typenames[i].predefined);*/
        l++;
        node1=node1->id_list;
        }
        free1=free1->var_decl_list;

        }
            return dec;
        }
        else
        {
            if (t_type == LBRACE)
            {
                ungetToken();
                dec->var_decl_section = NULL;
                return dec;
            }
            else
            {
                syntax_error("decl. LBRACE expected");
            }
        }
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct programNode* program()
{
    struct programNode* prog;
    struct declNode* decl1;
    int j,k;
    prog = ALLOC(struct programNode);
    t_type = getToken();
    if (t_type == TYPE || t_type == VAR || t_type == LBRACE)
    {
        ungetToken();
        prog->decl = decl();
	        for(j=0;j<i;j++)
        for(k=j+1;k<i;k++)
        if(strcmp(typenames[j].type_name,typenames[k].type_name)==0)
        {printf("ERROR CODE 1.1 %s\n",typenames[j].type_name);exit(0);}

        for(j=0;j<i;j++)
        for(k=0;k<i;k++)
        if(strcmp(typenames[j].type_name,typenames[k].id_name)==0)
	if(j>=k)
        {printf("ERROR CODE 1.2 %s\n",typenames[j].type_name);exit(0);}

        for(j=0;j<i;j++)
        for(k=0;k<l;k++)
        if(strcmp(typenames[j].type_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 1.3 %s\n",typenames[j].type_name);exit(0);}
        else if(strcmp(typenames[j].id_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 1.3 %s\n",typenames[j].id_name);exit(0);}

        for(j=0;j<l;j++)
        for(k=j+1;k<l;k++)
        if(strcmp(varnames[j].var_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 2.1 %s\n",varnames[j].var_name);exit(0);}

        for(j=0;j<l;j++)
        for(k=0;k<l;k++)
        if(strcmp(varnames[j].type_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 2.2 %s\n",varnames[j].type_name);exit(0);}
	
	for(j=0;j<l;j++)
	for(k=0;k<i;k++)
	if(strcmp(typenames[j].type_name,typenames[k].id_name)==0)
	{
		if(strcmp(typenames[j].predefined,"")!=0)
		strcpy(typenames[k].predefined,typenames[j].predefined);
		else
		typecopy(j,k);
	}

	/*   for(j=0;j<i;j++)
    {
        printf("%s : ",typenames[j].type_name);
        printf("predefined%s\n",typenames[j].predefined);
        printf("idname%s\n",typenames[j].id_name);
    }*/
         
        for(j=0;j<l;j++)
        for(k=0;k<i;k++)
        if(strcmp(varnames[j].type_name,typenames[k].type_name)==0)
        {strcpy(varnames[j].predefined,typenames[k].predefined);
        strcpy(varnames[j].id_name,typenames[k].id_name);}
	
/*	           for(j=0;j<l;j++)
    {
        printf("%s : ",varnames[j].var_name);
	        printf("predefined - %s\n",varnames[j].predefined);
        printf("given typename - %s\n",varnames[j].type_name);
        printf("id_name - %s\n",varnames[j].id_name);
    }*/
		

        prog->body = body();
        return prog;
    }
    else
    {
        syntax_error("program. TYPE or VAR or LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

int main()
{
    struct programNode* parseTree;
	int j,k;    
    
    parseTree = program();
    for(j=0;j<i;j++)
    {
	printf("%s : ",typenames[j].type_name);
	printf("predefined%s\n",typenames[j].predefined);
	printf("idname%s\n",typenames[j].id_name);
    }
	    for(j=0;j<l;j++)
    {
        printf("%s : ",varnames[j].var_name);
        //if(varnames[j].predefined)
        printf("predefined - %s\n",varnames[j].predefined);
        printf("given typename - %s\n",varnames[j].type_name);
	printf("id_name - %s\n",varnames[j].id_name);
    }  
	
/*	for(j=0;j<i;j++)
	for(k=j+1;k<i;k++)
	if(strcmp(typenames[j].type_name,typenames[k].type_name)==0)
	{printf("ERROR CODE 1.1 %s\n",typenames[j].type_name);return 1;}

	for(j=0;j<i;j++)
        for(k=0;k<i;k++)
        if(strcmp(typenames[j].type_name,typenames[k].id_name)==0)
        {printf("ERROR CODE 1.2 %s\n",typenames[j].type_name);return 1;}
	
        for(j=0;j<i;j++)
        for(k=0;k<l;k++)
        if(strcmp(typenames[j].type_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 1.3 %s\n",typenames[j].type_name);return 1;}
	else if(strcmp(typenames[j].id_name,varnames[k].var_name)==0)
	{printf("ERROR CODE 1.3 %s\n",typenames[j].id_name);return 1;}
	
        for(j=0;j<l;j++)
        for(k=j+1;k<l;k++)
        if(strcmp(varnames[j].var_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 2.1 %s\n",varnames[j].var_name);return 1;}
        
        for(j=0;j<l;j++)
        for(k=0;k<l;k++)
        if(strcmp(varnames[j].type_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 2.2 %s\n",varnames[j].type_name);return 1;}*/
        /*else if(strcmp(varnames[j].id_name,varnames[k].var_name)==0)
        {printf("ERROR CODE 2.2 %s\n",varnames[j].id_name);return 1;}*/

/*	for(j=0;j<l;j++)
	for(k=0;k<i;k++)
	if(strcmp(varnames[j].type_name,typenames[k].type_name)==0)
	{varnames[j].predefined=typenames[k].predefined;
	strcpy(varnames[j].id_name,typenames[k].id_name);}*/
	
/*            for(j=0;j<l;j++)
    {
        printf("%s : ",varnames[j].var_name);
        //if(varnames[j].predefined)
        printf("pre-user id_name -%s\n",varnames[j].id_name);
                printf("predefined - %s\n",reserved[varnames[j].predefined]);
                        printf("given typename - %s\n",varnames[j].type_name);
                            }*/
        

	
	
    // TODO: remove the next line after you complete the parser
    //print_parse_tree(parseTree); // This is just for debugging purposes
    // TODO: do type checking & print output according to project specification
    return 0;
}
