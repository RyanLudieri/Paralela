#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    const int MAX_NUMBERS = 2000;
    int numbers[MAX_NUMBERS];
    int result_sum = 0;
    int random_amount;
    int number_random;

    if (world_rank == 0)
    {
        srand(time(NULL));
        for (int j = 1; j < world_size; j++)
        {

            // Gera um número aleatório entre 1000 e 2000
            int min = 1000;
            int max = 2000;
            random_amount = min + rand() % (max - min + 1);

            for (int i = 0; i < random_amount; i++)
            {
                // Gera numero aleatorio de 0 a 99 para cada posicao de "numbers"
                int min = 0;
                int max = 99;
                number_random = min + rand() % (max - min + 1);

                numbers[i] = number_random;
            }
            MPI_Send(&random_amount, 1, MPI_INT, j, 0, MPI_COMM_WORLD);

            MPI_Send(numbers, 2000, MPI_INT, j, 0, MPI_COMM_WORLD);
        }
        for (int i = 1; i < world_size; i++)
        {
            int value;
            MPI_Recv(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result_sum += value;
            printf("Received %d from %d process\n", value, i);
        }
        printf("Soma total dos escravos: %d\n", result_sum);
    }
    else
    {
        // MPI_Status status;
        MPI_Recv(&random_amount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(numbers, 2000, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // MPI_Get_count(&status, MPI_INT, &result_amount);

        for (int i = 0; i < random_amount; i++)
        {
            result_sum += numbers[i];
        }
        /* A ideia aqui é pegar o somatório de cada escravo e depois o mestre irá fazer o somatório de cada escravo gerando o resultado final
            Só que no caso, o escravo não ira enviar um vetor e sim apenas o resultado do seu somatório*/
        MPI_Send(&result_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        printf("Processo %d envio o número %d para o Mestre\n", world_rank, result_sum);
    }

    MPI_Finalize();
}