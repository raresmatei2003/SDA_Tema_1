/* MATEI Rares-Andrei - 315CC*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SMAX 20

// interschimba continutul a 2 variabile de tip T
#define SWAP(a, b, T) \
    {                 \
        T aux = a;    \
        a = b;        \
        b = aux;      \
    }

// elibereaza memoria unei liste de tip T
#define FREELISTA(p, T) \
    while (p)           \
    {                   \
        T aux = p;      \
        p = p->next;    \
        free(aux);      \
        aux = NULL;     \
    }

// structura pentru lista dubla
typedef struct doublelist
{
    char car;
    struct doublelist *next, *prev;
} *DoubleList, DoubleListCell;

// structura pentru banda ce contine o lista dubla si un deget
// start retine adresa santinelei
typedef struct band
{
    DoubleList start, deget;
} *Band, BandCell;

// structura pentru un nod dintr-o coada
typedef struct queuecell
{
    int code;
    char car;
    struct queuecell *next;
} *QueueCell, SQueueCell;

// structura pentru coada ce retine inceputul si finalul acesteia
typedef struct queue
{
    QueueCell start, end;
} *Queue, SQueue;

// structura pentru stiva
typedef struct stack
{
    DoubleList adresa;
    struct stack *next;
} *Stack, StackCell;

// functie ce initializeaza o banda cu santinela si caracterul '#' spre
// care arata degetul
Band init_band()
{
    Band banda = malloc(sizeof(BandCell));
    DoubleList santinela = malloc(sizeof(DoubleListCell));
    DoubleList first = malloc(sizeof(DoubleListCell));
    first->next = NULL;
    first->prev = santinela;
    first->car = '#';
    santinela->next = first;
    santinela->prev = NULL;
    banda->start = santinela;
    banda->deget = first;
    return banda;
}

Stack init_stack()
{
    Stack p = NULL;
    return p;
}

Queue init_queue()
{
    Queue p = malloc(sizeof(SQueue));
    p->start = p->end = NULL;
    return p;
}

// functie care adauga o adresa in stiva
void push(Stack *p, DoubleList adr)
{
    Stack aux = malloc(sizeof(StackCell));
    aux->adresa = adr;
    aux->next = *p;
    *p = aux;
}

// functie care schimba adresa degetului cu cea a varfului lui stack1,
// introduce la inceputul lui stack2 noul varf al lui stack1 si
// avanseaza cu varful lui stack1 pe urmatoarea pozitie din stiva
void stack_top_move(Band banda, Stack *stack1, Stack *stack2)
{
    // schimb pozitia degetului cu prima din stack1
    SWAP(banda->deget, (*stack1)->adresa, DoubleList);

    Stack aux = *stack1;

    *stack1 = (*stack1)->next;

    // repozitionez fostul varf al lui stack1 ca varful stivei stack2
    aux->next = *stack2;
    *stack2 = aux;
}

// adaug o operatie la sfarsitul cozii
void enqueue(Queue p, int code, char car)
{
    QueueCell aux = malloc(sizeof(SQueueCell));
    aux->code = code;
    aux->car = car;
    aux->next = NULL;
    if (!(p->start))
    {
        p->start = p->end = aux;
    }
    else
    {
        p->end->next = aux;
        p->end = aux;
    }
}

// scot prima operatie din coada
void dequeue(Queue p)
{
    QueueCell aux = p->start;
    if (p->start == p->end)
        p->start = p->end = NULL;
    else
        p->start = p->start->next;
    free(aux);
}

// functie care codifica operatiile (cod >=0 inseamna operatie update,
// -1 pentru restul operatilor posibile)
int get_code(char s[SMAX])
{
    int code = -1;
    if (strstr(s, "WRITE"))
    {
        code = 6;
    }
    else
    {
        if (strstr(s, "LEFT"))
        {
            code += 1;
        }
        else if (strstr(s, "RIGHT"))
        {
            code += 2;
        }
        if (strstr(s, "CHAR"))
        {
            code += 2;
        }
        else if (strstr(s, "INSERT"))
        {
            code += 4;
        }
    }
    return code;
}

void write(Band banda, char caracter, FILE *out)
{
    banda->deget->car = caracter;
}

void insert_left(Band banda, char caracter, FILE *out)
{
    // daca nu exista un element cu 2 pozitii in stanga inseamna ca degetul
    // arata la primul element din lista, fiind astfel imposibil sa fie
    // introdus un caracter
    if (banda->deget->prev != banda->start)
    {
        DoubleList aux = malloc(sizeof(DoubleListCell));
        aux->car = caracter;
        aux->next = banda->deget;
        aux->prev = banda->deget->prev;
        banda->deget->prev->next = aux;
        banda->deget->prev = aux;
        banda->deget = aux;
    }
    else
    {
        fprintf(out, "ERROR\n");
    }
}

void insert_right(Band banda, char caracter, FILE *out)
{
    DoubleList aux = malloc(sizeof(DoubleListCell));
    aux->car = caracter;
    aux->next = banda->deget->next;
    aux->prev = banda->deget;
    // verific daca mai exista o celula dupa deget
    if (banda->deget->next)
    {
        banda->deget->next->prev = aux;
    }
    banda->deget->next = aux;
    banda->deget = aux;
}

void move_left(Band banda, char caracter, FILE *out)
{
    if (banda->deget->prev->prev)
    {
        banda->deget = banda->deget->prev;
    }
}

void move_right(Band banda, char caracter, FILE *out)
{
    // verific daca urmatoarea adresa este NULL
    if (!banda->deget->next)
    {
        insert_right(banda, '#', out);
    }
    else
    {
        banda->deget = banda->deget->next;
    }
}

// caut caracterul folosind o variabila auxiliara ca in cazul in care nu
// este gasit degetul ramane cu pozitia neschimbata
void move_left_char(Band banda, char caracter, FILE *out)
{
    DoubleList aux = banda->deget;
    while (aux->prev && aux->car != caracter)
    {
        aux = aux->prev;
    }
    if (!(aux->prev))
    {
        fprintf(out, "ERROR\n");
    }
    else
    {
        banda->deget = aux;
    }
}

// merg cu degetul la dreapta cat timp mai am elemente in lista si nu am gasit
// caracterul dorit. Daca dupa parcurgere degetul nu arata catre caracterul
// dorit, inseamna ca am ajuns la final si introduc la dreapta '#'
void move_right_char(Band banda, char caracter, FILE *out)
{
    while (banda->deget->next && banda->deget->car != caracter)
    {
        banda->deget = banda->deget->next;
    }
    if (banda->deget->car != caracter)
    {
        insert_right(banda, '#', out);
    }
}

void show_current(Band banda, FILE *out)
{
    fprintf(out, "%c\n", banda->deget->car);
}

void show(Band banda, FILE *out)
{
    DoubleList p = banda->start->next;
    while (p)
    {
        if (p == banda->deget)
        {
            fprintf(out, "|%c|", p->car);
        }
        else
        {
            fprintf(out, "%c", p->car);
        }
        p = p->next;
    }
    fprintf(out, "\n");
}

int main()
{
    int n, i;

    FILE *in = fopen("tema1.in", "r");
    FILE *out = fopen("tema1.out", "w");

    // vector cu pointeri la operatiile de tip update
    void (*operatie_update[7])(Band, char, FILE *) = {move_left,
                                                      move_right,
                                                      move_left_char,
                                                      move_right_char,
                                                      insert_left,
                                                      insert_right,
                                                      write};

    Band banda = init_band();

    Queue update = init_queue();

    Stack undo_start = init_stack();
    Stack redo_start = init_stack();

    fscanf(in, "%d", &n);
    fgetc(in);
    for (i = 0; i < n; i++)
    {
        char operatie[SMAX];
        fgets(operatie, SMAX, in);
        // elimin endline-ul din sir citit daca exista
        int poz_last_char = strlen(operatie) - 1;
        if (operatie[poz_last_char] == '\n')
        {
            operatie[poz_last_char] = '\0';
            poz_last_char--;
        }

        // codificarea operatiilor de tip update
        int code = get_code(operatie);

        // daca am obtinut un cod nenegativ atunci inseamna ca a fost citita
        // o operatie de tip update, astfel o introduc la finalul cozii
        if (code >= 0)
        {
            enqueue(update, code, operatie[poz_last_char]);
        }
        else
        {
            if (!strcmp(operatie, "SHOW_CURRENT"))
            {
                show_current(banda, out);
            }
            else if (!strcmp(operatie, "SHOW"))
            {
                show(banda, out);
            }
            else if (!strcmp(operatie, "EXECUTE"))
            {
                // retin o copie a degetului
                DoubleList aux = banda->deget;

                // apelez operatia corespunzatoare codului
                operatie_update[update->start->code](banda,
                                                     update->start->car, out);

                // daca a fost executata operatia MOVE_LEFT sau MOVE_RIGHT
                // si degetul a fost mutat, introduc in stiva undo fosta
                // pozitie a degetului
                if ((update->start->code == 0 || update->start->code == 1) &&
                    aux != banda->deget)
                {
                    push(&undo_start, aux);
                }
                else if (update->start->code == 6)
                {
                    // daca a fost executata operatia WRITE atunci pot
                    // elibera stivele undo si redo
                    FREELISTA(undo_start, Stack);
                    undo_start = NULL;
                    FREELISTA(redo_start, Stack);
                    redo_start = NULL;
                }

                // elimin prima operatie din coada uptade
                dequeue(update);
            }
            else if (!strcmp(operatie, "UNDO"))
            {
                stack_top_move(banda, &undo_start, &redo_start);
            }
            else
            {
                stack_top_move(banda, &redo_start, &undo_start);
            }
        }
    }

    FREELISTA(banda->start, DoubleList);
    FREELISTA(update->start, QueueCell);
    FREELISTA(undo_start, Stack);
    FREELISTA(redo_start, Stack);
    free(banda);
    free(update);
    fclose(in);
    fclose(out);
    return 0;
}