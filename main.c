#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "slp.h"
#include "prog1.h"
#include "print_slp.h"

int cnt = 0;
typedef struct node
{
    string id;
    int k;
    struct node *nxt;
} node;

typedef struct ret
{
    int k;
    bool isnum;
} ret;
node *head = NULL;

void table_insert(string id)
{
    node *new = checked_malloc(sizeof(node));
    new->id = id;
    new->k = ++cnt;
    new->nxt = head;
    head = new;
}
int table_search(string id)
{
    node *temp = head;
    while (temp)
    {
        if (temp->id == id)
            return temp->k;
        temp = temp->nxt;
    }
    assert(0);
}
void generate_stm(FILE *stream, A_stm stm);
ret generate_exp(FILE *stream, A_exp exp, string id);
void generate_expList(FILE *stream, A_expList expList);

ret generate_exp(FILE *stream, A_exp exp, string id)
{
    ret ans = {0, 0};
    ret ans_left,ans_right;
    switch (exp->kind)
    {
    case A_numExp:
        if (id != NULL)
        {
            table_insert(id);
            fprintf(stream, "    %%%d = add i32 %d, 0\n", cnt, exp->u.num);
        }
        ans.isnum = 1;
        ans.k = exp->u.num;
        break;
    case A_idExp:
        if (id != NULL)
        {
            int temp = table_search(exp->u.id);
            table_insert(id);
            fprintf(stream, "    %%%d = add i32 %%%d , 0\n", cnt, temp);
            ans.k = temp;
            ans.isnum = 0;
        }
        else
        {
            ans.k = table_search(exp->u.id);
            ans.isnum = 0;
        }
        break;
    case A_opExp:
        ans_left = generate_exp(stream, exp->u.op.left, NULL);
        ans_right = generate_exp(stream, exp->u.op.right, NULL);
        if (id != NULL)
        {
            table_insert(id);
            ans.k = cnt;
        }
        else
            ans.k = ++cnt;
        fprintf(stream, "    %%%d = ", ans.k);
        switch (exp->u.op.oper)
        {
        case A_plus:
            fprintf(stream, "add i32 ");
            break;
        case A_minus:
            fprintf(stream, "sub i32 ");
            break;
        case A_times:
            fprintf(stream, "mul i32 ");
            break;
        case A_div:
            fprintf(stream, "sdiv i32 ");
            break;
        }
        if (!ans_left.isnum)
            fprintf(stream, "%%");
        fprintf(stream, "%d , ", ans_left.k);
        if (!ans_right.isnum)
            fprintf(stream, "%%");
        fprintf(stream, "%d\n", ans_right.k);
        break;
    case A_eseqExp:
        generate_stm(stream, exp->u.eseq.stm);
        ans = generate_exp(stream, exp->u.eseq.exp, id);
        break;
    }
    return ans;
}
void generate_expList(FILE *stream, A_expList expList)
{
    ret ans;
    switch (expList->kind)
    {
    case A_pairExpList:
        ans = generate_exp(stream, expList->u.pair.head, NULL);
        fprintf(stream, "    call void @putint(i32  ");
        if (!ans.isnum)
            fprintf(stream, "%%");
        fprintf(stream,"%d)\n", ans.k);
        fprintf(stream, "    call void @putch(i32  10)\n");
        generate_expList(stream, expList->u.pair.tail);
        break;
    case A_lastExpList:
        ans = generate_exp(stream, expList->u.last, NULL);
        fprintf(stream, "    call void @putint(i32  ");
        if (!ans.isnum)
            fprintf(stream, "%%");
        fprintf(stream,"%d)\n", ans.k);
        fprintf(stream, "    call void @putch(i32  10)\n");
        break;
    }
    return;
}
void generate_stm(FILE *stream, A_stm stm)
{
    switch (stm->kind)
    {
    case A_compoundStm:
        generate_stm(stream, stm->u.compound.stm1);
        generate_stm(stream, stm->u.compound.stm2);
        break;
    case A_assignStm:
        generate_exp(stream, stm->u.assign.exp, stm->u.assign.id);
        break;
    case A_printStm:
        generate_expList(stream, stm->u.print.exps);
        break;
    }
}
void generate_main_ll(FILE *stream, A_stm stm)
{

    // declare extern func + function head
    fprintf(stream, "declare void @putint(i32 ) #1\n");
    fprintf(stream, "declare void @putch(i32 ) #1\n");
    fprintf(stream, "define dso_local i32 @main() #0 {\n");

    // actual code emitter:
    // Here is a "hardcoded" one. You need to redo this so it works for all legitimate prog1.c

    // fprintf(stream, "    %%1 = add i32 5, 3\n");
    // fprintf(stream, "    call void @putint(i32  %%1)\n");
    // fprintf(stream, "    call void @putch(i32  10)\n");
    // fprintf(stream, "    %%2 = sub i32 %%1, 1\n");
    // fprintf(stream, "    call void @putint(i32  %%2)\n");
    // fprintf(stream, "    call void @putch(i32  10)\n");
    // fprintf(stream, "    %%3 = mul i32 10, %%1\n");
    // fprintf(stream, "    call void @putint(i32  %%3)\n");
    // fprintf(stream, "    call void @putch(i32  10)\n");
    generate_stm(stream,stm);
    // fprintf(stream, "    call void @putch(i32  10)\n");
    // End of the function
    fprintf(stream, "    ret i32 0;\n }\n");

    return;
}

int main(int argc, const char *argv[])
{

    FILE *stream;

    //  print_slpis testing purpose:
    print_slp_stmt(stdout, prog1());
    fprintf(stdout, ";\n");

    stream = fopen("main.ll", "w");

    generate_main_ll(stream, prog1()); // generate main.ll for prog1()

    return 0;
}
