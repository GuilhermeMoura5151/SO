/* Problema do jantar dos filósofos de Dijkstra.
   Solução documentada em  "Modern Operating Systems" de A. S. Tanenbaum.
   Sincronização baseada em semáforos e memória partilhada.
   Implementação com primitivas SVIPC.

   Processo gerador dos processos filósofos
*/


#define   FILOSOFO     "semFilos"          /* nome dos processos filósofos */

/* alusão a módulos ou bibliotecas externas */

#include  <stdio.h>
#include  <stdlib.h>
#include  <stdbool.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <string.h>

#include  "semaphore.h"                   /* módulo de gestão de semáforos */
#include  "sharedMemory.h"       /* módulo de gestão de memória partilhada */


/* caracterização do estado interno */

#define  N          5                /* n.º de filósofos (menor do que 10) */
#define  MEDITANDO  0                         /* o filósofo está a meditar */
#define  COM_FOME   1                          /* o filósofo está com fome */
#define  COMENDO    2                           /* o filósofo está a comer */
#define  NMAXITER  20                           /* n.º máximo de iterações */


/* estrutura de dados partilhada e variáveis associadas */

typedef struct
        { unsigned int acesso,     /* id. do semáforo de protocolo da r.c. */
                       vez[N];       /* id. dos semáforos de sincronização
                                                                individual */
          unsigned int estado[N];           /* estado actual dos filósofos */
        } DADOS_PART;

static DADOS_PART *dp;      /* ponteiro para a região de dados partilhados */


int  main (void)

{
  char n_fic[21];                  /* nome do ficheiro de armazenamento do
                                                            estado interno */
  char n_fic_err[] = "err ";           /* base do nome do ficheiro de erro */
  FILE *fic;                                      /* descritor do ficheiro */
  int shmid,            /* ident. de acesso à região de memória partilhada */
      semgid;                    /* ident. de acesso ao grupo de semáforos */
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

  do
  { do
    { printf ("\nNome do ficheiro de armazenamento do estado interno? ");
      t = scanf ("%20[^\n]", n_fic);
      scanf ("%*[^\n]");
      scanf ("%*c");
    } while (t == 0);
    fic = fopen (n_fic, "r");
    if (fic != NULL)
       { fclose (fic);
         printf ("Já existe um ficheiro com esse nome! ");
         do
         { printf ("Quer reescrevê-lo? ");
           scanf ("%c", &opt);
           if (opt != '\n')
              { scanf ("%*[^\n]");
                scanf ("%*c");
              }
         } while ((opt == '\n') || ((opt != 'S') && (opt != 's') &&
                                    (opt != 'N') && (opt != 'n')));
         if ((opt == 'S') || (opt == 's')) break;
       }
  } while (fic != NULL);

  do
  { printf ("Número de iterações (<= %u)? ", NMAXITER);
    t = scanf ("%u", &niter);
    scanf ("%*[^\n]");
    scanf ("%*c");
  } while ((t == 0) || (niter > NMAXITER));

  /* composição da linha base de comando */

  sprintf (num[0], "%s", "0");
  sprintf (num[1], "%u", niter);
  if ((chave = ftok (".", 'a')) == -1)
     { perror ("erro na geração da chave");
       exit (EXIT_FAILURE);
     }
  sprintf (num[2], "%u", chave);

  /* criação e inicialização da região de memória partilhada e do ficheiro
     de armazenamento do estado interno */

  if ((shmid = shmemCreate (chave, sizeof (DADOS_PART))) == -1)
     { perror ("erro na criação da região de memória partilhada");
       exit (EXIT_FAILURE);
     }
  if (shmemAttach (shmid, (void **) &dp) == -1)
     { perror ("erro na anexação da região de memória partilhada");
       exit (EXIT_FAILURE);
     }
  dp->acesso = N+1;                /* id. do semáforo de protecção da r.c. */
  for (f = 0; f < N; f++)            /* id. dos semáforos de sincronização */
    dp->vez[f] = f+1;                                        /* individual */
  for (f = 0; f < N; f++)           /* à partida, todos os filósofos estão */
    dp->estado[f] = MEDITANDO;                                /* a meditar */

  if ((fic = fopen (n_fic, "w")) == NULL)
     { perror ("erro na operação de criação do ficheiro de armazenamento");
       exit (EXIT_FAILURE);
     }
  fprintf (fic, "%14cProblema do jantar dos filósofos\n\n", ' ');
  for (f = 0; f < N; f++)
    fprintf (fic, "  MEDITANDO ");
  fprintf (fic, "\n");
  if (fclose (fic) == EOF)
     { perror ("erro na operação de fecho do ficheiro de armazenamento");
       exit (EXIT_FAILURE);
     }

  /* criação e inicialização do grupo de semáforos */

  if ((semgid = semCreate (chave, N+1)) == -1)
     { perror ("erro na criação do grupo de semáforos");
       exit (EXIT_FAILURE);
     }
  if (semUp (semgid, dp->acesso) == -1)     /* possibilitar acesso à r.c. */
     { perror ("erro na operação up do semáforo acesso");
       exit (EXIT_FAILURE);
     }

  /* lançamento dos processos filósofos */

  for (f = 0; f < N; f++)
  { if ((pid[f] = fork ()) < 0)
       { perror ("erro na duplicação do processo");
         exit (EXIT_FAILURE);
       }
    num[0][0] = '0'+f;
    n_fic_err[strlen(n_fic_err)-1] = '0'+f;
    if (pid[f] == 0)
       if (execl (FILOSOFO, FILOSOFO, num[0], num[1], n_fic, num[2],
                    n_fic_err, NULL) < 0)
          { perror ("erro no lançamento do processo filósofo");
            exit (EXIT_FAILURE);
          }
  }

  /* sinalização de início de operações */

  if (semSignal (semgid) == -1)
     { perror ("erro na sinalização de início de operações");
       exit (EXIT_FAILURE);
     }

  /* espera pela terminação de todos os processos filósofos */

  printf ("\nRelatório final\n");
  n = 0;
  do
  { info = wait (&status);
    encontrou = false;
    for (f = 0; f < N; f++)
      if (info == pid[f])
         { encontrou = true;
           id = f;
         }
    if (!encontrou)
       { perror ("erro na espera por um processo filósofo");
         exit (EXIT_FAILURE);
       }
    printf ("o processo filósofo, com id %d, já terminou: ", id);
    if (WIFEXITED (status))
       printf ("o seu status de saída foi %d\n", WEXITSTATUS (status));
    n += 1;
  } while (n < N);

  /* destruição do grupo de semáforos e da região de memória partilhada */

  if (semDestroy (semgid) == -1)
     { perror ("erro na destruição do grupo de semáforos");
       exit (EXIT_FAILURE);
     }
  if (shmemDettach (dp) == -1)
     { perror ("erro na desanexação da região de memória partilhada");
       exit (EXIT_FAILURE);
     }
  if (shmemDestroy (shmid) == -1)
     { perror ("erro na destruição da região de memória partilhada");
       exit (EXIT_FAILURE);
     }

  exit (EXIT_SUCCESS);
}
