#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define BUFF_SIZE 5
#define NP 1   // Produtor
#define NCP1 5 // CP1
#define NCP2 4 // CP2
#define NCP3 3 // CP3
#define NC 1   // Consumidor
#define FILE_COUNT 2
#define MATRIX_SIZE 2

// Estrutura S
typedef struct
{
    char name[256];
    double A[MATRIX_SIZE][MATRIX_SIZE];
    double B[MATRIX_SIZE][MATRIX_SIZE];
    double C[MATRIX_SIZE][MATRIX_SIZE];
    double V[MATRIX_SIZE];
    double E;
} S;

// Buffer compartilhado
typedef struct
{
    S *buf[BUFF_SIZE];
    int in;
    int out;
    sem_t full;
    sem_t empty;
    sem_t mutex;
} sbuf_t;

sbuf_t shared[4];

// Protótipos das funções
void *Producer(void *arg);
void *ConsumerProducer1(void *arg);
void *ConsumerProducer2(void *arg);
void *ConsumerProducer3(void *arg);
void *Consumer(void *arg);

void *Producer(void *arg)
{
    int i, index;
    index = *((int *)arg);

    FILE *file_list = fopen("entrada_lista.txt", "r");
    if (!file_list)
    {
        perror("Erro ao abrir o arquivo lista de entradas");
        exit(1);
    }

    for (i = 0; i < FILE_COUNT; i++)
    {
        char input_filename[50];
        double A[MATRIX_SIZE][MATRIX_SIZE], B[MATRIX_SIZE][MATRIX_SIZE];

        // Ler o nome do arquivo de entrada
        if (fscanf(file_list, "%s", input_filename) != 1)
        {
            perror("Erro ao ler nome de arquivo");
            exit(1);
        }

        // Abrir o arquivo de entrada
        FILE *input_file = fopen(input_filename, "r");
        if (!input_file)
        {
            perror("Erro ao abrir o arquivo de entrada");
            exit(1);
        }

        // Ler as matrizes A e B do arquivo
        // Ler matriz A
        for (int r = 0; r < MATRIX_SIZE; r++)
        {
            for (int c = 0; c < MATRIX_SIZE; c++)
            {
                fscanf(input_file, "%lf", &A[r][c]);
                printf("valor da matriz %f\n", A[r][c]);
            }
        }

        // Ler matriz B
        for (int r = 0; r < MATRIX_SIZE; r++)
        {
            for (int c = 0; c < MATRIX_SIZE; c++)
            {
                fscanf(input_file, "%lf", &B[r][c]);
            }
        }

        // Criar uma nova estrutura S e preencher com os dados lidos
        S *data = malloc(sizeof(S));
        if (!data)
        {
            perror("Erro ao alocar memória para estrutura S");
            exit(1);
        }

        strcpy(data->name, input_filename);
        memcpy(data->A, A, sizeof(A));
        memcpy(data->B, B, sizeof(B));

        // Colocar o ponteiro para a estrutura S no buffer compartilhado
        sem_wait(&shared[0].empty); // Se não há espaço no buffer, espera
        sem_wait(&shared[0].mutex); // Acesso exclusivo ao buffer
        shared[0].buf[shared[0].in] = data;
        shared[0].in = (shared[0].in + 1) % BUFF_SIZE;
        printf("[P_%d] Produzindo %s...\n", index, input_filename);
        sem_post(&shared[0].mutex); // Libera o acesso ao buffer
        sem_post(&shared[0].full);  // Indica que há um item no buffer

        // Fechar o arquivo de entrada
        fclose(input_file);
    }

    fclose(file_list);
    return NULL;
}

void *ConsumerProducer1(void *arg)
{
    while (1)
    {
        // Pegar um item do buffer compartilhado shared[0]
        sem_wait(&shared[0].full);
        sem_wait(&shared[0].mutex);

        S *data = shared[0].buf[shared[0].out];
        shared[0].out = (shared[0].out + 1) % BUFF_SIZE;

        sem_post(&shared[0].mutex);
        sem_post(&shared[0].empty);

        if (data == NULL)
        {
            // Sinalização para terminar
            break;
        }

        // Realizar a multiplicação de matrizes C = A * B
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            for (int j = 0; j < MATRIX_SIZE; j++)
            {
                data->C[i][j] = 0;
                for (int k = 0; k < MATRIX_SIZE; k++)
                {
                    data->C[i][j] += data->A[i][k] * data->B[k][j];
                }
            }
        }

        printf("[CP1] Processado arquivo %s. Matriz C calculada.\n", data->name);

        // Inserir o item processado no buffer compartilhado shared[1]
        sem_wait(&shared[1].empty);
        sem_wait(&shared[1].mutex);

        shared[1].buf[shared[1].in] = data;
        shared[1].in = (shared[1].in + 1) % BUFF_SIZE;

        sem_post(&shared[1].mutex);
        sem_post(&shared[1].full);
    }

    printf("[CP1] Finalizando thread.\n");
    pthread_exit(NULL);
}

void *ConsumerProducer2(void *arg)
{
    while (1)
    {
        // Pegar um item do buffer compartilhado shared[1]
        sem_wait(&shared[1].full);
        sem_wait(&shared[1].mutex);

        S *data = shared[1].buf[shared[1].out];
        shared[1].out = (shared[1].out + 1) % BUFF_SIZE;

        sem_post(&shared[1].mutex);
        sem_post(&shared[1].empty);

        if (data == NULL)
        {
            // Sinalização para terminar
            break;
        }

        // Calcular o vetor V como a soma das colunas da matriz C
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            data->V[j] = 0;
            for (int i = 0; i < MATRIX_SIZE; i++)
            {
                data->V[j] += data->C[i][j];
            }
        }

        printf("[CP2] Processado arquivo %s. Vetor V calculado.\n", data->name);

        // Inserir o item processado no buffer compartilhado shared[2]
        sem_wait(&shared[2].empty);
        sem_wait(&shared[2].mutex);

        shared[2].buf[shared[2].in] = data;
        shared[2].in = (shared[2].in + 1) % BUFF_SIZE;

        sem_post(&shared[2].mutex);
        sem_post(&shared[2].full);
    }

    printf("[CP2] Finalizando thread.\n");
    pthread_exit(NULL);
}

void *ConsumerProducer3(void *arg)
{
    while (1)
    {
        // Pegar um item do buffer compartilhado shared[2]
        sem_wait(&shared[2].full);
        sem_wait(&shared[2].mutex);

        S *data = shared[2].buf[shared[2].out];
        shared[2].out = (shared[2].out + 1) % BUFF_SIZE;

        sem_post(&shared[2].mutex);
        sem_post(&shared[2].empty);

        if (data == NULL)
        {
            // Sinalização para terminar
            break;
        }

        // Calcular o valor E como a soma dos elementos do vetor V
        data->E = 0;
        for (int i = 0; i < MATRIX_SIZE; i++)
        {
            data->E += data->V[i];
        }

        printf("[CP3] Processado arquivo %s. Valor E calculado: %f\n", data->name, data->E);

        // Inserir o item processado no buffer compartilhado shared[3]
        sem_wait(&shared[3].empty);
        sem_wait(&shared[3].mutex);

        shared[3].buf[shared[3].in] = data;
        shared[3].in = (shared[3].in + 1) % BUFF_SIZE;

        sem_post(&shared[3].mutex);
        sem_post(&shared[3].full);
    }

    printf("[CP3] Finalizando thread.\n");
    pthread_exit(NULL);
}

void *Consumer(void *arg)
{
    FILE *output = fopen("saida.out", "w");
    if (!output)
    {
        perror("Erro ao abrir o arquivo de saída");
        pthread_exit(NULL);
    }

    int processed_count = 0;

    while (1)
    {
        // Pegar um item do buffer compartilhado shared[3]
        sem_wait(&shared[3].full);
        sem_wait(&shared[3].mutex);

        S *data = shared[3].buf[shared[3].out];
        shared[3].out = (shared[3].out + 1) % BUFF_SIZE;

        sem_post(&shared[3].mutex);
        sem_post(&shared[3].empty);

        if (data == NULL)
        {
            // Sinalização para terminar
            break;
        }

        // Escrever o resultado no arquivo de saída
        fprintf(output, "Resultado do arquivo %s:\n", data->name);
        fprintf(output, "Valor E: %f\n", data->E);

        free(data);
        processed_count++;

        if (processed_count >= FILE_COUNT)
        {
            break;
        }
    }

    fclose(output);
    printf("[Consumer] Finalizando thread.\n");
    pthread_exit(NULL);
}

int main()
{
    // Inicializar semáforos e buffers compartilhados
    for (int i = 0; i < 4; i++)
    {
        shared[i].in = shared[i].out = 0;
        sem_init(&shared[i].full, 0, 0);
        sem_init(&shared[i].empty, 0, BUFF_SIZE);
        sem_init(&shared[i].mutex, 0, 1);
    }

    pthread_t producer, cp1, cp2, cp3, consumer;
    int producer_id = 0;

    // Criar threads
    pthread_create(&producer, NULL, Producer, &producer_id);
    pthread_create(&cp1, NULL, ConsumerProducer1, NULL);
    pthread_create(&cp2, NULL, ConsumerProducer2, NULL);
    pthread_create(&cp3, NULL, ConsumerProducer3, NULL);
    pthread_create(&consumer, NULL, Consumer, NULL);

    // Esperar pelas threads
    pthread_join(producer, NULL);
    pthread_join(cp1, NULL);
    pthread_join(cp2, NULL);
    pthread_join(cp3, NULL);
    pthread_join(consumer, NULL);

    // Finalizar semáforos
    for (int i = 0; i < 4; i++)
    {
        sem_destroy(&shared[i].full);
        sem_destroy(&shared[i].empty);
        sem_destroy(&shared[i].mutex);
    }

    return 0;
}
