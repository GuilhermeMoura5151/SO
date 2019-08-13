/* Problema do jantar dos filósofos de Dijkstra.
   Adaptação da solução documentada em  "Modern Operating Systems" de A. S.
   Tanenbaum a uma sincronização baseada em monitores.
   Implementação de threads e do monitor usando primitivas da biblioteca pthread.

   Threads filósofos - f = 0, 1, ... , N-1
*/


/* alusão a módulos ou bibliotecas externas */

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <sys/types.h>
#include  <pthread.h>
#include  <math.h>

#include  "monFilos.h"


/* estrutura de dados de comunicação com os threads filósofos */

typedef struct
          { unsigned int fid;                    /* id. do thread filósofo */
            unsigned int niter;                     /* número de iterações */
          } THREAD_PAR;


/* caracterização do estado interno */

#define  N          5                /* n.º de filósofos (menor do que 10) */
#define  MEDITANDO  0                         /* o filósofo está a meditar */
#define  COM_FOME   1                          /* o filósofo está com fome */
#define  COMENDO    2                           /* o filósofo está a comer */
#define  NMAXITER  20                           /* n.º máximo de iterações */


/* descritor do ficheiro de armazenamento do estado interno
   'array' de status de retorno dos threads filósofos
     variáveis globais, acessíveis pelos threads filósofos
*/

FILE *fic;                                        /* descritor do ficheiro */
static int status[N];                             /* status' de retorno */


/* alusão a funções locais */

void *filosofo (void *);
static void meditar (void);
static void comer (void);


/*   thread principal responsável pelo lançamento da simulação
     e gerador dos threads filósofos
*/

int  main (void)

{
  char n_fic[21];                  /* nome do ficheiro de armazenamento do
                                                            estado interno */
  int t;                                               /* teste de leitura */
  char opt;                                         /* resposta a pergunta */
  unsigned int niter;                               /* número de iterações */
  unsigned int f;                                    /* variável contadora */
  pthread_t tid[N];                         /* id.'s dos threads filósofos
              note-se que o tipo pthread_t é um tipo meramente operacional */
  THREAD_PAR val[N];                             /* 'array' de comunicação */
  int *p_status;                   /* ponteiro para o 'status' de execução */

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

  /* criação e abertura do ficheiro de armazenamento do estado interno
     inicialização do 'array' de comunicação */

  if ((fic = fopen (n_fic, "w")) == NULL)
     { perror ("erro na operação de criação do ficheiro de armazenamento");
       exit (EXIT_FAILURE);
     }
  for (f = 0; f < N; f++)
  { val[f].fid = f;
    val[f].niter = niter;
  }

  /* lançamento dos threads filósofos */

  for (f = 0; f < N; f++)
  { if (pthread_create (&tid[f], NULL, filosofo, &val[f]) != 0)
       { perror ("erro no lançamento do thread");
         exit (EXIT_FAILURE);
       }
  }

  /* espera pela terminação de todos os threads filósofos */

  printf ("\nRelatório final\n");
  printf ("O meu pid é %d\n", getpid());
  for (f = 0; f < N; f++)
  { if (pthread_join (tid[f], (void **) &p_status) != 0)
       { perror ("erro na espera por um processo filósofo");
         exit (EXIT_FAILURE);
       }
    printf ("o thread filósofo, com id %u, já terminou: ", f);
    printf ("o seu status de saída foi %d\n", *p_status);
  }

  /* fecho do ficheiro de armazenamento do estado interno */

  if (fclose (fic) == EOF)
     { perror ("erro na operação de fecho do ficheiro de armazenamento");
       exit (EXIT_FAILURE);
     }

  exit (EXIT_SUCCESS);
}


/*   thread filósofo
     simula o ciclo de vida de cada filósofo

     Parâmetros de entrada:
        val_p --- ponteiro para uma estrutura que contém
                     f   --- identificação do filósofo
                   niter --- n.º de iterações
*/

void *filosofo (void *val_p)
{
  unsigned int i;                                     /* variável contadora */

  srandom ((unsigned int) pthread_self ());
  for (i = 0; i < ((THREAD_PAR *) val_p)->niter; i++)
  { meditar ();
    if ((status[((THREAD_PAR *) val_p)->fid] =
                       pegar_nos_garfos (((THREAD_PAR *) val_p)->fid)) == ERRO)
       { fprintf (stderr, "filósofo %u\n", ((THREAD_PAR *) val_p)->fid);
         perror ("erro na operação de pegar nos garfos");
         pthread_exit (&status[((THREAD_PAR *) val_p)->fid]);
       }
    comer ();
    if ((status[((THREAD_PAR *) val_p)->fid] =
                       pousar_os_garfos (((THREAD_PAR *) val_p)->fid)) == ERRO)
       { fprintf (stderr, "filósofo %u\n", ((THREAD_PAR *) val_p)->fid);
         perror ("erro na operação de pegar nos garfos");
         pthread_exit (&status[((THREAD_PAR *) val_p)->fid]);
       }
  }
  status[((THREAD_PAR *) val_p)->fid] = EXIT_SUCCESS;
  pthread_exit ((void *) &status[((THREAD_PAR *) val_p)->fid]);
}


/* procedimento meditar */

static void meditar (void)

{
  usleep((unsigned int) floor (1 + 200000.0 * random () / RAND_MAX));
}


/* procedimento comer */

static void comer (void)

{
  usleep((unsigned int) floor (1 + 100000.0 * random () / RAND_MAX));
}
