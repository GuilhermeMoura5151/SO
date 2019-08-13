/* Problema do jantar dos filósofos de Dijkstra.
   Solução documentada em  "Modern Operating Systems" de A. S. Tanenbaum.
   Sincronização baseada em semáforos e memória partilhada.
   Implementação com primitivas SVIPC.

   Processos filósofos - f = 0, 1, ... , N-1
*/


/* alusão a módulos ou bibliotecas externas */

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <math.h>
#include  "semaphore.h"                  /* módulo de gestão dos semáforos */
#include  "sharedMemory.h"       /* módulo de gestão de memória partilhada */


/* caracterização do estado interno */

#define  N            5              /* n.º de filósofos (menor do que 10) */
#define  MEDITANDO    0                       /* o filósofo está a meditar */
#define  COM_FOME     1                        /* o filósofo está com fome */
#define  COMENDO      2                         /* o filósofo está a comer */
#define  ESQ  (f+N-1)%N     /* ident. do filósofo à esquerda do filósofo f */
#define  DIR    (f+1)%N      /* ident. do filósofo à direita do filósofo f */
#define  NMAXITER    20                         /* n.º máximo de iterações */


/* estrutura de dados partilhada e variáveis associadas */

typedef struct
        { unsigned int acesso,     /* id. do semáforo de protecção da r.c. */
                       vez[N];       /* id. dos semáforos de sincronização
                                                                individual */
          unsigned int estado[N];           /* estado actual dos filósofos */
        } DADOS_PART;

static DADOS_PART *dp;      /* ponteiro para a região de dados partilhados */

static int shmid,       /* ident. de acesso à região de memoria partilhada */
           semgid;               /* ident. de acesso ao grupo de semáforos */
static char *n_fic; /* nome do ficheiro de armazenamento do estado interno */


/* alusão a funções locais */

static void meditar (void);
static void comer (void);
static void pegar_nos_garfos (unsigned int);
static void pousar_os_garfos (unsigned int);
static void avaliar_situacao (unsigned int);
static void guardar_estado (void);


int main (int argc, char *argv[])

{
  unsigned int f;                    /* identificador do processo filósofo */
  int chave;             /* chave de acesso à região de memória partilhada
                                                  ou ao grupo de semáforos */
  unsigned int niter;                               /* número de iterações */
  char *tinp;                             /* teste de parâmetros numéricos */
  unsigned int i;

  /* validação da passagem de parâmetros */

  if (argc != 6)
     { freopen ("erro", "a", stderr);
       fprintf (stderr, "n.º de parâmetros incorrecto\n");
       exit (EXIT_FAILURE);
     }
     else freopen (argv[5], "w", stderr);
  f = (unsigned int) strtol (argv[1], &tinp, 0);
  if ((*tinp != '\0') || (f >= N))
     { fprintf (stderr, "erro na identificação do processo filósofo\n");
       exit (EXIT_FAILURE);
     }
  niter = (unsigned int) strtol (argv[2], &tinp, 0);
  if ((*tinp != '\0') || (niter == 0) || (niter > NMAXITER))
     { fprintf (stderr, "erro no número de iterações\n");
       exit (EXIT_FAILURE);
     }
  n_fic = argv[3];
  chave = (unsigned int) strtol (argv[4], &tinp, 0);
  if (*tinp != '\0')
     { fprintf (stderr, "erro no número de iterações\n");
       exit (EXIT_FAILURE);
     }

  /* ligação ao grupo de semáforos e à região de memória partilhada e anexação
     da região de memória partilhada ao espaço de endereçamento próprio */

  if ((semgid = semConnect (chave)) == -1)
     { perror ("erro na ligação ao grupo de semáforos");
       exit (EXIT_FAILURE);
     }
  if ((shmid = shmemConnect (chave)) == -1)
     { perror ("erro na ligação à região de memória partilhada");
       exit (EXIT_FAILURE);
     }
  if (shmemAttach (shmid, (void **) &dp) == -1)
     { perror ("erro na anexação da região de memória partilhada");
       exit (EXIT_FAILURE);
     }

  /* simulação da situação propriamente dita - ciclo de vida dos filósofos */

  srandom ((unsigned int) getpid ());
  for (i = 0; i < niter; i++)
  { meditar ();
    pegar_nos_garfos (f);
    comer ();
    pousar_os_garfos (f);
  }

  /* desanexação da região de memória partilhada do espaço de endereçamento
     próprio */

  if (shmemDettach (dp) == -1)
     { perror ("erro na desanexação da região de memória partilhada");
       exit (EXIT_FAILURE);
     }

  exit (EXIT_SUCCESS);
}


/* procedimento meditar */

static void meditar (void)

{
  usleep((unsigned int) floor (1 + 100000.0 * random () / RAND_MAX));
}


/* procedimento comer */

static void comer (void)

{
  usleep((unsigned int) floor (1 + 100000.0 * random () / RAND_MAX));
}


/* procedimento de pegar nos garfos */

static void pegar_nos_garfos (unsigned int f)

{
  if (semDown (semgid, dp->acesso) == -1)    /* entrada na região crítica */
     { perror ("erro na operação down do semáforo acesso");
       exit (EXIT_FAILURE);
     }
  dp->estado[f] = COM_FOME;             /* o filósofo assumiu que tem fome */
  guardar_estado ();
  avaliar_situacao (f);    /* o filósofo tenta garantir a posse dos garfos */
  if (semUp (semgid, dp->acesso) == -1)        /* saída da região crítica */
     { perror ("erro na operação up do semáforo acesso");
       exit (EXIT_FAILURE);
     }
  if (semDown (semgid, dp->vez[f]) == -1)    /* aguarda, se não conseguiu
                                                           obter os garfos */
     { perror ("erro na operação down do semáforo vez[f]");
       exit (EXIT_FAILURE);
     }
}

/* procedimento de pousar nos garfos */

static void pousar_os_garfos (unsigned int f)

{
  if (semDown (semgid, dp->acesso) == -1)    /* entrada na região crítica */
     { perror ("erro na operação down do semáforo acesso");
       exit (EXIT_FAILURE);
     }
  dp->estado[f] = MEDITANDO;         /* o filósofo prepara-se para meditar */
  guardar_estado ();
  avaliar_situacao (ESQ);      /* avalia o caso do filósofo à sua esquerda */
  avaliar_situacao (DIR);       /* avalia o caso do filósofo à sua direita */
  if (semUp (semgid, dp->acesso) == -1)        /* saída da região crítica */
     { perror ("erro na operação up do semáforo acesso");
       exit (EXIT_FAILURE);
     }
}

/* procedimento de avaliar a situação */

static void avaliar_situacao (unsigned int f)

{
  if ((dp->estado[f] == COM_FOME) &&
      (dp->estado[ESQ] != COMENDO) && (dp->estado[DIR] != COMENDO))
     { dp->estado[f] = COMENDO;           /* os garfos estão efectivamente
                                                              na sua posse */
       guardar_estado ();
       if (semUp (semgid, dp->vez[f]) == -1) /* o processo de continuação
                                                               é garantido */
          { perror ("erro na operação up do semáforo vez[f]");
            exit (EXIT_FAILURE);
          }
     }
}


/* procedimento de armazenamento do estado interno */

static void guardar_estado (void)

{
  FILE *fic;                                      /* descritor do ficheiro */
  unsigned int i;

  if ((fic = fopen (n_fic, "a")) == NULL)
     { perror ("erro na operação de abertura do ficheiro de armazenamento");
       exit (EXIT_FAILURE);
     }
  for (i = 0; i < N; i++)
    switch (dp->estado[i])
    { case MEDITANDO: fprintf (fic, "  MEDITANDO ");
                      break;
      case COM_FOME:  fprintf (fic, "  COM_FOME  ");
                      break;
      case COMENDO:   fprintf (fic, "   COMENDO  ");
    }
  fprintf (fic, "\n");
  if (fclose (fic) == EOF)
     { perror ("erro na operação de fecho do ficheiro de armazenamento");
       exit (EXIT_FAILURE);
     }
}
