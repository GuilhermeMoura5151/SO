/* Problema do jantar dos filósofos de Dijkstra.
   Adaptação da solução documentada em  "Modern Operating Systems" de A. S.
   Tanenbaum a uma sincronização baseada em passagem de mensagens.
   Implementação com primitivas SVIPC.

   Processo gerador dos processos filósofos
 */


#define   FILOSOFO     "msgFilos"          /* nome dos processos filósofos */

/* alusão a módulos ou bibliotecas externas */

#include  <stdio.h>
#include  <stdlib.h>
#include  <stdbool.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <string.h>

#include  "message.h"            /* módulo de gestão de filas de mensagens */


/* caracterização do estado interno */

#define  N          5                /* n.º de filósofos (menor do que 10) */
#define  MEDITANDO  0                         /* o filósofo está a meditar */
#define  COM_FOME   1                          /* o filósofo está com fome */
#define  COMENDO    2                           /* o filósofo está a comer */
#define  NMAXITER  20                           /* n.º máximo de iterações */


/* estrutura das mensagens trocadas e variáveis associadas */

/* mensagem que reporta o estado actual dos filósofos */

typedef struct { 
    unsigned int estado[N];                 /* estado actual dos filósofos */
} CORPO;

typedef struct { 
    long destid;                          /* identificação do destinatário */
    CORPO et;                                         /* corpo da mensagem */
} MENSAGEM_EAF;

static MENSAGEM_EAF meaf = { N+1 };    /* caracterização do estado interno */


int  main (void)
{
    char n_fic[21];                  /* nome do ficheiro de armazenamento do
                        estado interno */
    char n_fic_err[] = "err ";           /* base do nome do ficheiro de erro */
    FILE *fic;                                      /* descritor do ficheiro */
    int msgid;                         /* identificador da fila de mensagens */
    int t;                                               /* teste de leitura */
    char opt;                                         /* resposta a pergunta */
    unsigned int niter;                               /* número de iterações */
    unsigned int f, n;                               /* variáveis contadoras */
    int pid[N];                             /* id.'s dos processos filósofos */
    int id;                                    /* índice do 'array' de pid's */
    int chave;             /* chave de acesso à região de memória partilhada
                  ou ao grupo de semáforos */
    char num[3][12];      /* conversão de valor numérico (até 10 algarismos) */
    int status,                                      /* 'status' de execução */
        info;                                 /* informação de identificação */
    bool encontrou;                 /* sinalização de terminação do processo */

    /* leitura do nome do ficheiro de armazenamento e do n.º de iterações */

    do { 
        do { 
            printf ("\nNome do ficheiro de armazenamento do estado interno? ");
            t = scanf ("%20[^\n]", n_fic);
            scanf ("%*[^\n]");
            scanf ("%*c");
        } while (t == 0);
        fic = fopen (n_fic, "r");
        if (fic != NULL) { 
            fclose (fic);
            printf ("Já existe um ficheiro com esse nome! ");
            do { 
                printf ("Quer reescrevê-lo? ");
                scanf ("%c", &opt);
                if (opt != '\n') { 
                    scanf ("%*[^\n]");
                    scanf ("%*c");
                }
            } while ((opt == '\n') || ((opt != 'S') && (opt != 's') &&
                        (opt != 'N') && (opt != 'n')));
            if ((opt == 'S') || (opt == 's')) break;
        }
    } while (fic != NULL);

    do { 
        printf ("Número de iterações (<= %u)? ", NMAXITER);
        t = scanf ("%u", &niter);
        scanf ("%*[^\n]");
        scanf ("%*c");
    } while ((t == 0) || (niter > NMAXITER));

    /* composição da linha base de comando */

    sprintf (num[0], "%s", "0");
    sprintf (num[1], "%u", niter);
    if ((chave = ftok (".", 'a')) == -1) { 
        perror ("erro na geração da chave");
        exit (EXIT_FAILURE);
    }
    sprintf (num[2], "%u", chave);

    /* criação e inicialização da fila de mensagens e do ficheiro de
       armazenamento do estado interno */

    if ((msgid = msg_create (chave)) == -1) { 
        perror ("erro na criação da fila de mensagens");
        exit (EXIT_FAILURE);
    }
    for (f = 0; f < N; f++)           /* à partida, todos os filósofos estão */
        meaf.et.estado[f] = MEDITANDO;                            /* a meditar */

    if ((fic = fopen (n_fic, "w")) == NULL) { 
        perror ("erro na operação de criação do ficheiro de armazenamento");
        exit (EXIT_FAILURE);
    }
    fprintf (fic, "%14cProblema do jantar dos filósofos\n\n", ' ');
    for (f = 0; f < N; f++)
        fprintf (fic, "  MEDITANDO ");
    fprintf (fic, "\n");
    if (fclose (fic) == EOF) { 
        perror ("erro na operação de fecho do ficheiro de armazenamento");
        exit (EXIT_FAILURE);
    }

    /* lançamento dos processos filósofos */

    for (f = 0; f < N; f++) { 
        if ((pid[f] = fork ()) < 0) { 
            perror ("erro na duplicacao do processo");
            exit (EXIT_FAILURE);
        }
        num[0][0] = '0'+f;
        n_fic_err[strlen(n_fic_err)-1] = '0'+f;
        if (pid[f] == 0)
            if (execl (FILOSOFO, FILOSOFO, num[0], num[1], n_fic, num[2],
                        n_fic_err, NULL) < 0) { 
                perror ("erro no lancamento do processo filósofo");
                exit (EXIT_FAILURE);
            }
    }

    /* sinalização de início de operações */

    if (msg_send_nb (msgid, &meaf, sizeof (CORPO)) == -1) { 
        perror ("erro no envio da mensagem de estado interno");
        exit (EXIT_FAILURE);
    }

    /* espera pela terminacao de todos os processos filósofos */

    printf ("\nRelatório final\n");
    n = 0;
    do { 
        info = wait (&status);
        encontrou = false;
        for (f = 0; f < N; f++)
            if (info == pid[f]) { 
                encontrou = true;
                id = f;
            }
        if (!encontrou) { 
            perror ("erro na espera por um processo filósofo");
            exit (EXIT_FAILURE);
        }
        printf ("o processo filósofo, com id %d, já terminou: ", id);
        if (WIFEXITED (status))
            printf ("o seu status de saída foi %d\n", WEXITSTATUS (status));
        n += 1;
    } while (n < N);

    /* destruição da fila de mensagens */

    if (msg_destroy (msgid) == -1) { 
        perror ("erro na destruição da fila de mensagens");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}

