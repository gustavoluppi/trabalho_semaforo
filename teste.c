#include <stdio.h>

int main()
{
    int i, j;
    float matriz[3][3]; // Definindo uma matriz 3x3

    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("matriz.txt", "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Ler os dados do arquivo para a matriz
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            fscanf(arquivo, "%f", &matriz[i][j]); // Lê um número float e armazena na matriz
        }
    }

    // Fechar o arquivo
    fclose(arquivo);

    // Imprimir a matriz
    printf("Matriz lida do arquivo:\n");
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            printf("%.2f ", matriz[i][j]); // Imprime os números com 2 casas decimais
        }
        printf("\n");
    }

    return 0;
}
