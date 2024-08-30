#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    // Inicializa o ambiente MPI
    MPI_Init(NULL, NULL);

    int world_size;  // Armazena o número total de processos
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;  // Armazena o identificador do processo corrente
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const int MAX_NUMBERS = 2000;  // Define o número máximo de elementos que podem ser gerados
    float numbers[MAX_NUMBERS];    // Array para armazenar os números gerados
    float result_sum = 0.0;        // Variável para armazenar a soma dos números processados
    int random_amount;             // Armazena a quantidade de números gerados aleatoriamente

    int M = 5 * (world_size - 1);  // Calcula o número total de mensagens a serem enviadas, multiplicando por 5 o número de processos escravos

    if (world_rank == 0)  // Processo mestre
    {
        srand(time(NULL));  // Inicializa o gerador de números aleatórios

        for (int m = 0; m < M; m++)
        {
            // Gera um número aleatório de números entre 1000 e 2000
            int min = 1000;
            int max = 2000;
            random_amount = min + rand() % (max - min + 1);

            // Preenche o array 'numbers' com valores aleatórios entre 0.0 e 99.0
            for (int i = 0; i < random_amount; i++)
            {
                numbers[i] = ((float)rand() / RAND_MAX) * 99.0;
            }

            // Recebe a identificação de um processo escravo pronto para receber dados
            int slave_rank;
            MPI_Recv(&slave_rank, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Envia a quantidade de números gerados para o processo escravo selecionado
            MPI_Send(&random_amount, 1, MPI_INT, slave_rank, 0, MPI_COMM_WORLD);
            // Envia os números gerados para o processo escravo selecionado
            MPI_Send(numbers, random_amount, MPI_FLOAT, slave_rank, 0, MPI_COMM_WORLD);
        }

        // Recebe a soma calculada de cada processo escravo
        for (int i = 1; i < world_size; i++)
        {
            float value;
            MPI_Recv(&value, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result_sum += value;  // Acumula a soma dos valores recebidos
            printf("Received %.2f from process %d\n", value, i);
        }

        // Exibe a soma total calculada por todos os processos escravos
        printf("Total sum from all slaves: %.2f\n", result_sum);
    }
    else  // Processos escravos
    {
        for (int m = 0; m < M / (world_size - 1); m++)
        {
            // Notifica o processo mestre de que o escravo está pronto para receber dados
            MPI_Send(&world_rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

            // Recebe a quantidade de números a serem processados
            MPI_Recv(&random_amount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Recebe os números que serão somados
            MPI_Recv(numbers, random_amount, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Calcula a soma dos números recebidos
            result_sum = 0.0;
            for (int i = 0; i < random_amount; i++)
            {
                result_sum += numbers[i];
            }

            // Envia a soma calculada de volta ao processo mestre
            MPI_Send(&result_sum, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

            // Exibe a soma que foi enviada ao mestre
            printf("Process %d sent sum %.2f to Master\n", world_rank, result_sum);
        }
    }
}