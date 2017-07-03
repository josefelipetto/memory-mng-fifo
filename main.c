/*
 * main.c
 *
 *  Author: José Henrique Medeiros Felipetto
 *  FIFO Memory Management Algorithm
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/* The structure below defines virtual pages*/
typedef struct virtualPages{
    int valor;
    struct virtualPages *prox;
} stVirtualPage;

/* Struct used to measure time */
static struct timeval start_time;
typedef long long int64;

stVirtualPage *queueHead, *frameHead;

/* PROTOTYPES */
stVirtualPage *readFile(const char *arq);
void execute(stVirtualPage *entrada,int frames,int arq);
int searchOnFrame(stVirtualPage *frame, int value, int frames);
void replaceOnFrame(stVirtualPage *frame, int from, int to, int frames);
void insertNode(stVirtualPage *queue, int value);
int removeNode();
void addOnFrame(stVirtualPage *frame, int value);
void showStruct(stVirtualPage *list);
int test();
/* END PROTOTYPES */


void init_time() {
    gettimeofday(&start_time, NULL);
}

int64 get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (int64) (t.tv_sec - start_time.tv_sec) * 1000000
    + (t.tv_usec - start_time.tv_usec);
}


/*
	main function. This is where you call your customized function test, acording to your needs.
*/
int main(){
    test();
    return 0;
}

/*
	My test function to test the algorithm. 
*/
int test(){
    stVirtualPage *entradasHead;
    int64 inicio, fim;
    
    // File 1
    stVirtualPage *entradas = readFile((const char *)"arquivo1.txt");

    inicio = get_time();
    execute(entradas,4,1);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    // File 2
    entradas = readFile((const char *)"arquivo2.txt");
    inicio = get_time();
    execute(entradas,4,2);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    // File 3
    entradas = readFile((const char *)"arquivo3.txt");
    
    entradasHead = entradas;
    inicio = get_time();
    execute(entradas,64,3);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));
    
    entradas = entradasHead;
    inicio = get_time();
    execute(entradas,256,3);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));
    
    entradas = entradasHead;
    inicio = get_time();
    execute(entradas,1024,3);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));
    
    entradas = entradasHead;
    inicio = get_time();
    execute(entradas,2048,3);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    // File 4
    entradas = readFile((const char *)"arquivo4.txt");

    entradasHead = entradas;
    inicio = get_time();
    execute(entradas,64,4);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    entradas = entradasHead;
    inicio = get_time();
    execute(entradas,256,4);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    entradas = entradasHead;
    inicio = get_time();
    execute(entradas,1024,4);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    entradas = entradasHead;
    inicio = get_time();
    execute(entradas,2048,4);
    fim = get_time();
    printf("Execution time:: %.2f us \n\n ", (double)(fim-inicio));

    return 0;
}


/*
	Open a test file and return a linked list of it
*/
stVirtualPage *readFile(const char *arq)
{


    stVirtualPage *inicio, *pagina, *pag_ant;
	FILE *arquivo;
	int valor;
	char req[10];

	arquivo = fopen (arq, "r");
    pagina = malloc(sizeof(struct virtualPages));
    inicio = pagina;
    pag_ant = pagina;
    inicio->valor=valor; 
    fgets(req,9,arquivo);       			/* Le o arquivo */
	valor = atoi(req);						/* Converte o string lido para inteiro */
    inicio->valor=valor;
    inicio->prox = NULL;

	while(!feof(arquivo) )
	{
		fgets(req,9,arquivo);				/* Le o arquivo */
		if (!feof(arquivo))					/* Verifica se Ã© fim de arquivo */
		{
			valor = atoi(req);
            pagina = malloc(sizeof(stVirtualPage)); 		/* Aloca nova pÃ¡gina na memÃ³ria  */
            pagina->valor=valor;						

            pag_ant->prox = pagina;						/* Inclui a pÃ¡gina na estrutura */
            pag_ant = pagina;
            pagina->prox = NULL;
		}
	}
    return inicio;
}

/*
	Run the algorithm to the specified entry file, number of frames and number of the file
*/
void execute(stVirtualPage *entrada,int frames, int arq){
    int page_faults = 0, a, req = 1,ret = -1, frame_count = 1;
    stVirtualPage *frame = malloc(sizeof(stVirtualPage)) ,*queue = malloc(sizeof(stVirtualPage));
    frame->valor = entrada->valor; frame->prox = NULL; queue->valor = entrada->valor; queue->prox = NULL;
    queueHead = queue; frameHead = frame;
    entrada = entrada->prox;
    while(entrada != NULL){

        ret = searchOnFrame(frame,entrada->valor,frames);
        if(!ret){
            if(frame_count >= frames)
                page_faults++;
            if(frame_count >= frames){
                a = removeNode();
            }
            insertNode(queueHead,entrada->valor);

            if(frame_count >= frames)
                replaceOnFrame(frameHead,a,entrada->valor,frames);
            else
                addOnFrame(frameHead,entrada->valor);
            frame_count++;
        }
        req++;
        entrada = entrada->prox;
    }
    printf ("File %d, %5d pages, %7d requisitions: FIFO: %5d Page Faults ||| ",arq, frames, req, page_faults) ;
}

/*
	Checks if the value it's already on the frame
*/
int searchOnFrame(stVirtualPage *frame, int value, int frames){
    int count = 0;
    stVirtualPage *aux = frame;
    
    while(aux!=NULL && count < frames){
        if(aux->valor == value)
            return 1;
        aux = aux->prox;
        count++;
    }
    return 0;
}

/*
	Replace some value 'from' with value 'to' inside the frame 'frame'. 
*/
void replaceOnFrame(stVirtualPage *frame, int from, int to, int frames){
    int count = 0;
    while(frame !=  NULL){
        if(frame->valor == from){
            frame->valor = to;
        }
        frame = frame->prox;
    }
}

/*
	Add a value to the frame 'frame'
*/
void addOnFrame(stVirtualPage *frame, int value){
    while(frame->prox != NULL){
        frame = frame->prox;
    }

    stVirtualPage *newNode = malloc(sizeof(stVirtualPage));
    newNode->valor = value;
    newNode->prox  = NULL;
    frame->prox = newNode;
    return;
}

/*
	Insert a node with the value 'value' inside the queue 'queue'
*/
void insertNode(stVirtualPage *queue, int value){
    while(queue->prox != NULL){
        queue = queue->prox;
    }

    stVirtualPage *newNode = malloc(sizeof(stVirtualPage));
    newNode->valor = value;
    newNode->prox  = NULL;
    queue->prox = newNode;
    return;
}

/*
	Remove the first node of the queue
*/
int removeNode(){
    int ret = queueHead->valor;
    stVirtualPage *temp = queueHead->prox;
    free((void *)queueHead);
    queueHead = temp;
    return ret;
}

/*
	Debug function to show a struct 'list'. This is just for dev purposes.
*/
void showStruct(stVirtualPage *list){
    printf(" =========================================\n");
    printf(" Show struct \n");
    stVirtualPage *aux = list;
    while(aux != NULL){
        printf("Valor: %d\n",aux->valor);
        aux = aux->prox;
    }
    printf("End Show Struct \n");
    printf(" =========================================\n");
}


