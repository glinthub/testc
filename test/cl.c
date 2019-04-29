#include <stdlib.h>
#include <stdio.h>
typedef struct{
	float value;
	int   flag;
}fi;

typedef struct{
	fi *base;
	fi *top;
}stack;

#define OP_ADD		0
#define OP_SUB		1
#define OP_MUL		2
#define OP_DIV		3
#define OP_LB		4
#define OP_RB		5
#define OP_SEP		6
#define OP_NUM		7

/*
char fu[7]=				{'+','-','*','/','(',')','#'};
char biao[7][7]={		+	{'>','>','<','<','<','>','>'},
				-	{'>','>','<','<','<','>','>'},
				*	{'>','>','>','>','<','>','>'},
				/	{'>','>','>','>','<','>','>'},
				(	{'<','<','<','<','<','=','E'},
				)	{'>','>','>','>','E','>','>'},
				#	{'<','<','<','<','<','E','='} };
*/

char fu[7]=			{'+','-','*','/','(',')','#'};
char op_table[7][7]={
	{'>','>','<','<','E','>', '>'},
	{'>','>','<','<','E','>', '>'},
	{'>','>','>','>','E','>', '>'},
	{'>','>','>','>','E','>', '>'},
	{'<','<','<','<','E','=', '>'},
	{'E','E','E','E','E','E', '>'},
	{'<','<','<','<','E','E', '='},
};

char op2c(int i)
{
	switch(i)
	{
		case OP_ADD:	    return '+';
	    case OP_SUB:	    return '-';
	    case OP_MUL:	    return '*';
	    case OP_DIV:	    return '/';
	    case OP_LB:			return '(';
	    case OP_RB:   		return ')';
	    case OP_SEP:	    return '#';
	    default:	    	return '?';
	}
}

unsigned char c2op(char s)
{
	switch(s)
	{
		case '+': return 0;
	    case '-': return 1;
	    case '*': return 2;
	    case '/': return 3;
	    case '(': return 4;
	    case ')': return 5;
	    case '#': return 6;
	    default: return s;
	}
}

int check_op(unsigned int op_left, unsigned int op_right)
{
	if (op_left >= OP_NUM || op_right >= OP_NUM)
		return -1;
	if (op_table[op_left][op_right] == 'E')
		return -1;

	return 0;
}

stack initstack(stack **stint)
{
	*stint = (stack *)malloc(sizeof(stack));
	(*stint)->base=(fi *)malloc(sizeof(fi)*100);
	(*stint)->top=(*stint)->base;
}

void dumpstack(stack *stk)
{
	fi *elmnt;

	printf("stack dump: ");
	for (elmnt = stk->base; elmnt != stk->top; elmnt += 1)
	{
		if (elmnt->flag)
			printf("%c", op2c(elmnt->value));
		else
			printf("%d", elmnt->value);
	}
	printf("\n");
}

push(stack **p,float a1,int a2)
	{(*p)->top->value=a1;
	(*p)->top->flag=a2;
	(*p)->top+=1;
}

float pop(stack **p)
{
	float temp;
	(*p)->top-=1;
	temp=(*p)->top->value;
	return(temp);
}

show(stack **p)
{
	int i;
	for(i=0;(*p)->base+i!=(*p)->top;i++)
	  printf("  %f\n",((*p)->base+i)->value);
}

float top1(stack **p)
{
	return((((*p)->top)-1)->value);
}

float top2(stack **p)
{
	return((((*p)->top)-2)->value);
}

double add(double a,double b)
    {   return(a+b);}

double sub(double a,double b)
    {   return(a-b);}

double mul(double a,double b)
    {   return(a*b);}

double idiv(double a,double b)
    {  return(a/b);       }

float huan(float a,float b,int c)
{
	float i;
	if(c==0)
	  i=add(a,b);
	else if(c==1)
	  i=sub(a,b);
	else if(c==2)
	  i=mul(a,b);
	else if(c==3)
	  i=idiv(a,b);
	return(i);
}

form(char *s,stack **one)
{
	int i,j=0;
	float ok;
	char temp[80];
	push(one,6,1);
	for(i=0;*(s+i)!='\0';i++)
	{
		if(c2op(*(s+i))==*(s+i))
		{
			*(temp+j)=c2op(*(s+i));
			j++;
		}
		else if((c2op(*(s+i))!=*(s+i))&&(j==0))
		{
			push(one,c2op(*(s+i)),1);
		}
		else if((c2op(*(s+i))!=*(s+i))&&(j!=0))
		{
			*(temp+j)='\0';
			ok=atof(temp);
			push(one,ok,0);
			j=0;
			i--;
		}
	}
	if(j!=0)
	{
		*(temp+j)='\0';
		ok=atof(temp);
		push(one,ok,0);
	}
	push(one,6,1);
}

main(int argc, char *argv[])
{
	char s[80];
	int i,c,sign;
	float temp;
	float a,b,result;
	stack *stk_all,*stk_op,*stk_num;
	initstack(&stk_all);
	initstack(&stk_op);
	initstack(&stk_num);
	int op_left, op_right;
	/*
	clrscr();
	gets(s);
	form(s,&one);
	*/
	if (argc < 2)
		return -1;
	form(argv[1],&stk_all);
	//show(&one);
	while(stk_all->base!=stk_all->top)
	{
		temp=pop(&stk_all);
		sign=stk_all->top->flag;
		/*printf("\ntemp=%f",temp);
		printf("\nsign=%d",sign);*/
		if(sign==1)
			push(&stk_op,temp,1);
		else if(sign==0)
		{
			push(&stk_num,temp,0);
			op_left = (int)top1(&stk_all);
			op_right = (int)top1(&stk_op);
			if (check_op(op_left, op_right))
			{
				printf("expression error!\n");
				return -1;
			}
		}

		while((stk_num->top) > (stk_num->base)+1)
		{
			temp=pop(&stk_all);
			push(&stk_op,temp,1);
			op_left = (int)top1(&stk_op);
			op_right = (int)top2(&stk_op);
			printf("%c %d %c\n", op2c(op_left),(int)top1(&stk_num),op2c(op_right));
			if(op_table[op_left][op_right]=='<')
			{
				/*printf("\n\n\nfuhao=%c    ",biao[(int)top1(&fuhao)][(int)top2(&fuhao)]);*/
				op_left=pop(&stk_op);
				push(&stk_all,op_left,1);
				temp=pop(&stk_num);a=temp;
				temp=pop(&stk_num);b=temp;
				op_right=pop(&stk_op);c=(int)op_right;
				/*printf("\na=%f,b=%f,c=%d",a,b,c); */
				result=huan(a,b,c);
				/*printf("\njieguo=%f",jieguo);*/
				push(&stk_num,result,0);
				//dumpstack(stk_all);
			}
			else if(op_table[op_left][op_right]=='=')
			{
				temp=pop(&stk_op);
				temp=pop(&stk_op);
				dumpstack(stk_num);
				//push(&stk_all, result, 0);
			}
			else if(op_table[op_left][op_right]=='>')
			{
				break;
			}
			else
			{
				printf("expression error!\n");
				return -1;
			}
		}
	}
	printf("result=%f",result);
}

