#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#define READ 0
#define WRITE 1
#define INGAME 0
#define ENDGAME 1

int checkLetter(char word[30], char letterSelected[30], char strangeWord[30]);

int main()
{
    pid_t processA, processB, processC, processD;
    int T_H1_N2[2], T_H2_N1[2], T_H1_N1[2], T_H2_N2[2], fileDescriptor1, fileDescriptor2;
    char letter[30];
    char selectedWord1[30] = "ropas";
    char selectedWord2[30] = "telas";
    char unknowWord1[30] = "-----";
    char unknowWord2[30] = "-----";
    int turn_1 = 0;
    int turn_2 = 0;
    int status = INGAME;

    if (pipe(T_H1_N2) < 0) exit(1);
    else if (pipe (T_H2_N1) < 0) exit(2);
    else if(pipe(T_H1_N1) < 0) exit(3);
    else if(pipe(T_H2_N2) < 0) exit(4);
    else  printf("Pipes Created Succes\n\n");

    processA = fork();

    if (processA == 0) //hijo
        processB = fork();
    else//padre
    {
        processC = fork();
        if (processC == 0)
            processD = fork();
    }

    if(processA == 0)
    {      
        if(processB > 0) // HIJO 1
        {
            close(T_H1_N2[READ]);

            close(T_H2_N1[READ]);
            close(T_H2_N1[WRITE]);

            close(T_H1_N1[READ]);

            close(T_H2_N2[READ]);
            close(T_H2_N2[WRITE]);

            write(T_H1_N1[WRITE], selectedWord1, sizeof(selectedWord1));
            printf("* Palabra enviada Hijo 1 (%d): %s\n", getpid(), selectedWord1);

            while (status != ENDGAME)
            {
                if(turn_1 == 5 || turn_2 == 5 || status == ENDGAME) break;

                sleep(1);
                fgets(letter, 30, stdin);
                printf("-- Letra enviada Hijo 1 (%d) : %s",getpid(), letter);
                write(T_H1_N2[WRITE], &letter, sizeof(letter));
                sleep(1);
            }
            close(T_H1_N2[WRITE]);
            close(T_H1_N1[WRITE]);
        }   
        else// NIETO 1
        {
            close(T_H1_N2[READ]); 
            close(T_H1_N2[WRITE]);

            close(T_H2_N1[WRITE]);

            close(T_H1_N1[WRITE]);

            close(T_H2_N2[READ]);
            close(T_H2_N2[WRITE]);

            fileDescriptor1 = read(T_H1_N1[READ], &selectedWord1, sizeof(selectedWord1));
            printf("Palabra leida Nieto 1 (%d) es: %s\n",getpid(), selectedWord1);

            while (status != ENDGAME)
            {
                if(turn_1 == 5 || turn_2 == 5 || status == ENDGAME) break;

                fileDescriptor1 = read(T_H2_N1[READ], &letter, sizeof(letter));
                printf("Letra recibida Nieto 1 (%d): %c\n\n",getpid(), letter[0]);
                status = checkLetter(selectedWord1, letter, unknowWord1);
                sleep(1);
                turn_2++;
            }
            close(T_H2_N1[READ]);
        }
    }
    else
    {
        if (processC == 0)
        {
            if(processD > 0) // HIJO 2
            {
                sleep(1);
                close(T_H2_N1[READ]);

                close(T_H1_N2[WRITE]);
                close(T_H1_N2[READ]);

                close(T_H1_N1[READ]);
                close(T_H1_N1[WRITE]);

                close(T_H2_N2[READ]);

                write(T_H2_N2[WRITE], selectedWord2, sizeof(selectedWord2));
                printf("* Palabra enviada Hijo 2 (%d): %s\n", getpid(), selectedWord2);

                while (status != ENDGAME)
                {
                    if(turn_1 == 5 || turn_2 == 5 || status == ENDGAME) break;

                    sleep(1);
                    fgets(letter, 30, stdin);
                    printf("-- Letra enviada Hijo 2 (%d): %s", getpid(), letter);
                    write(T_H2_N1[WRITE], &letter, sizeof(letter));
                    sleep(1);
                }
                close(T_H2_N1[WRITE]);
                close(T_H2_N2[WRITE]);
            }
            else // NIETO 2
            {
                sleep(1);
                close(T_H2_N1[WRITE]);
                close(T_H2_N1[READ]);

                close(T_H1_N2[WRITE]);

                close(T_H1_N1[READ]);
                close(T_H1_N1[WRITE]);

                close(T_H2_N2[WRITE]);

                fileDescriptor2 = read(T_H2_N2[READ], &selectedWord2, sizeof(selectedWord2));
                printf("Palabra leida Nieto 2 (%d) es: %s\n\n", getpid(), selectedWord2);

                while (status != ENDGAME)
                {
                    if(turn_1 == 5 || turn_2 == 5 || status == ENDGAME) break;

                    fileDescriptor2 = read(T_H1_N2[READ], &letter, sizeof(letter));
                    printf("Letra recibida Nieto 2 (%d): %c\n\n", getpid(), letter[0]);
                    status = checkLetter(selectedWord2, letter, unknowWord2);
                    sleep(1);
                    turn_1++;
                }
                close(T_H1_N2[READ]);
            }
        }
        else
        {
            close(T_H1_N2[READ]);
            close(T_H1_N2[WRITE]);
            close(T_H2_N1[READ]);
            close(T_H2_N1[WRITE]); 
            while (wait(NULL) != -1)
            {
                system("clear");
                printf("Game Over \n");
            }
        }
    }
}

int checkLetter(char word[30], char letterSelected[30], char strangeWord[30])
{
    int i;
    int counter = 0;
    char gamemode[30];

    for(i = 0; i < 10 ; i++)
    {
        if(word[i] == letterSelected[0])
        {
            strangeWord[i] = letterSelected[0];
            counter++;
        }
    }
    if(counter != 0) 
    {
        printf("ACERTASTE!!! la palabra desconocida es: %s\n\n",strangeWord);
        if(word[0] == strangeWord[0] && 
        word[1] == strangeWord[1] && 
        word[2] == strangeWord[2] && 
        word[3] == strangeWord[3] &&
        word[4] == strangeWord[4]) 
        {
            printf("GANASTE!!!\n");
            fgets(gamemode, 30, stdin);
            printf("(%s) \n", gamemode);

            if(strncmp(gamemode,"exit",4)==0)
            {
                printf("PRESIONASTE SALIR \n");
                return ENDGAME;
            }
            else if(strncmp(gamemode,"nuev",4)==0)
            {
                printf("PRESIONASTE REPETIR");
                return 2;
            }

        } 
    }
    else printf("ERROR!!! la letra no corresponde: %s\n\n", strangeWord);

    return INGAME;

    counter = 0;
}
