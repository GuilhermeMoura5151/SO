/* Módulo definidor das operações associadas com o acesso a um monitor de
   tipo Lampson / Redell que gere a atribuição dos garfos.
   São usadas primitivas da biblioteca pthread.

   Operações implementadas:
     - pegar nos garfos;
     - pousar os garfos.

   Nome: monitor_mesa.c

   Implementação do módulo
*/

/* alusão a módulos ou bibliotecas externas */

#include <stdio.h>
#include <pthread.h>
#include <math.h>


/* alusão ao descritor do ficheiro (variável global) */

extern FILE *fic;


/* mensagens de saída das primitivas do monitor */

#define  OK           0
#define  ERRO        -1


/* caracterização do estado interno */

#define  N             5              /* n.º de filósofos (menor do que 10) */
#define  MEDITANDO     0                       /* o filósofo está a meditar */
#define  COM_FOME      1                        /* o filósofo está com fome */
#define  COMENDO       2                         /* o filósofo está a comer */
#define  ESQ ((f+N-1)%N)     /* ident. do filósofo à esquerda do filósofo f */
#define  DIR   ((f+1)%N)      /* ident. do filósofo à direita do filósofo f */


/* estrutura de dados interna */

/* variàveis que descrevem a situação dos filósofos num dado instante */

static unsigned int estado[N];              /* estado actual dos filósofos */
static pthread_cond_t vez[N];                    /* ponto de sincronização */

/* implementação do monitor */

static pthread_mutex_t acesso = PTHREAD_MUTEX_INITIALIZER;  /* flag de locking */
static pthread_once_t init = PTHREAD_ONCE_INIT;       /* flag de inicialização */


/* alusão a funções locais */

static void inicializacao (void);
static int avaliar_situacao (unsigned int, unsigned int);
static void guardar_estado (void);


/* primitivas de acesso ao monitor */

/* função de pegar nos garfos
   o filósofo tenta tomar posse dos garfos que necessita para comer
   bloqueia, se tal não for poss?vel

   parâmetro de entrada:
       f  --- identificação do filósofo

   valor devolvido:
       0 - sucesso
      -1 - erro
           a situação concreta é descrita em errno
*/

int pegar_nos_garfos (unsigned int f)

{
  if (pthread_mutex_lock (&acesso) != 0)              /* entrada no monitor */
      return (ERRO);
  pthread_once (&init, inicializacao);       /* inicializa as estruturas de */
                     /* dados internas, se for o primeiro acesso ao monitor */
  estado[f] = COM_FOME;                  /* o filósofo assumiu que tem fome */
  guardar_estado ();
  if (avaliar_situacao (f, f) != OK)   /* o filósofo tenta garantir a posse
                                                                 dos garfos */
     return (ERRO);
  if (pthread_mutex_unlock (&acesso) != 0)              /* saída no monitor */
      return (ERRO);
  return (OK);
}

/* função de pousar nos garfos
   o filósofo pousa na mesa os garfos usados para comer
   avalia a seguir a situação dos seus vizinhos, acordando um deles se tal se
   propiciar

   parâmetro de entrada:
      f   --- identificação do filósofo

   valor devolvido:
       0 - sucesso
      -1 - erro
           a situação concreta é descrita em errno
*/

int pousar_os_garfos (unsigned int f)

{
  if (pthread_mutex_lock (&acesso) != 0)              /* entrada no monitor */
      return (ERRO);
  pthread_once (&init, inicializacao);       /* inicializa as estruturas de */
                     /* dados internas, se for o primeiro acesso ao monitor */
  estado[f] = MEDITANDO;              /* o filósofo prepara-se para meditar */
  guardar_estado ();
  if (avaliar_situacao (f, ESQ) != OK)       /* avalia o caso do filósofo à
                                                               sua esquerda */
     return (ERRO);
  if (avaliar_situacao (f, DIR) != OK)       /* avalia o caso do filósofo à
                                                                sua direita */
     return (ERRO);
  if (pthread_mutex_unlock (&acesso) != 0)              /* saída no monitor */
      return (ERRO);
  return (OK);
}

/* primitivas internas ao monitor */

/* procedimento de inicialização da estrutura de dados interna */

static void inicializacao (void)

{
  unsigned int f;                                     /* variável contadora */

  for (f = 0; f < N; f++)
  { estado[f] = MEDITANDO;          /* os filósofos começam todos a meditar */
    pthread_cond_init (&vez[f], NULL);
  }

  /* inicialização do ficheiro de armazenamento */

  fprintf (fic, "%14cProblema do jantar dos filósofos\n\n", ' ');
  for (f = 0; f < N; f++)
    fprintf (fic, "  MEDITANDO ");
  fprintf (fic, "\n");
}

/* procedimento de avaliar a situação
   o filósofo pousa na mesa os garfos usados para comer
   avalia a seguir a situação dos seus vizinhos, acordando um deles se tal se
   propiciar

   parâmetros de entrada:
      fo --- identificação do filósofo que invoca a operação
      f  --- identificação do filósofo sobre quem é invocada a operação

   valor devolvido:
       0 - sucesso
      -1 - erro
           a situação concreta é descrita em errno
*/

static int avaliar_situacao (unsigned int fo, unsigned int f)

{
  if ((estado[f] == COM_FOME) &&
      (estado[ESQ] != COMENDO) && (estado[DIR] != COMENDO))
     { estado[f] = COMENDO;   /* o filósofo f toma posse efectiva dos garfos */
       guardar_estado ();
       if (fo != f)
          { if (pthread_cond_signal (&vez[f]) != 0)    /* o filósofo vizinho
                                                                  é acordado */
               return (ERRO);
          }
     }
     else  if (fo == f)
              { if (pthread_cond_wait (&vez[fo], &acesso) != 0)        /* o
                                     filósofo aguarda pela posse dos grafos */
                   return (ERRO);
              }
  return (OK);
}

/* procedimento de armazenamento do estado interno no ficheiro
   é escrita uma linha
*/

static void guardar_estado (void)

{
  unsigned int f;                                     /* variável contadora */

  for (f = 0; f < N; f++)
    switch (estado[f])
    { case MEDITANDO: fprintf (fic, "  MEDITANDO ");
                      break;
      case COM_FOME:  fprintf (fic, "  COM_FOME  ");
                      break;
      case COMENDO:   fprintf (fic, "   COMENDO  ");
    }
  fprintf (fic, "\n");
}
