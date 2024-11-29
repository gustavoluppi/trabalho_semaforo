#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 10
#define NUM_FILES 50

// Função para gerar uma matriz 10x10 de números aleatórios
void generate_matrix(double matrix[MATRIX_SIZE][MATRIX_SIZE])
{
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            matrix[i][j] = (double)(rand() % 100) + ((double)(rand() % 100) / 100.0); // Valores aleatórios com 2 casas decimais
        }
    }
}

// Função para salvar as matrizes em um arquivo
void save_file(const char *filename, double A[MATRIX_SIZE][MATRIX_SIZE], double B[MATRIX_SIZE][MATRIX_SIZE])
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Erro ao criar o arquivo");
        exit(1);
    }

    // Escrever a matriz A
    fprintf(file, "A:\n");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fprintf(file, "%.2f ", A[i][j]);
        }
        fprintf(file, "\n");
    }

    // Escrever a matriz B
    fprintf(file, "B:\n");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fprintf(file, "%.2f ", B[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main()
{
    srand(time(NULL)); // Inicializar o gerador de números aleatórios

    for (int i = 0; i < NUM_FILES; i++)
    {
        double A[MATRIX_SIZE][MATRIX_SIZE];
        double B[MATRIX_SIZE][MATRIX_SIZE];

        // Gerar matrizes A e B com valores aleatórios
        generate_matrix(A);
        generate_matrix(B);

        // Criar o nome do arquivo baseado no índice
        char filename[50];
        sprintf(filename, "entrada%d.in", i + 1);

        // Salvar as matrizes no arquivo
        save_file(filename, A, B);

        printf("Arquivo %s criado.\n", filename);
    }

    return 0;
}
