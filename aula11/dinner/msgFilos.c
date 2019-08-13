/* Problema do jantar dos filósofos de Dijkstra.
   Adaptação da solução documentada em  "Modern Operating Systems" de A. S.
   Tanenbaum a uma sincronização baseada em passagem de mensagens.
   Implementação com primitivas SVIPC.

   Processos filósofos - f = 0, 1, ... , N-1
 */


/* alusão a módulos ou bibliotecas externas */

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <math.h>

#include  "message.h"           /* módulo de gestão de filas de mensagens */


/* caracterização do estado interno */

#define  N            5              /* n.º de filósofos (menor do que 10) */
#define  MEDITANDO    0                       /* o filósofo está a meditar */
#define  COM_FOME     1                        /* o filósofo está com fome */
#define  COMENDO      2                         /* o filósofo está a comer */
#define  ESQ  (f+N-1)%N     /* ident. do filósofo à esquerda do filósofo f */
#define  DIR    (f+1)%N      /* ident. do filósofo à direita do filósofo f */
#define  NMAXITER    20                         /* n.º máximo de iterações */


/* estrutura das mensagens trocadas e variáveis associadas */

/* mensagem que reporta o estado actual dos filósofos */

typedef struct { 
    unsigned int estado[N];                 /* estado actual dos filósofos */
} CORPO;

typedef struct { 
    long destid;                         /* identificação do destinatário */
    CORPO et;                                        /* corpo da mensagem */
} MENSAGEM_EAF;

/* mensagem que simula a tomada de posse dos garfos */

typedef struct { 
    long destid;                         /* identificação do destinatário */
    char dummy;                                             /* corpo nulo */
} MENSAGEM_SINC;

static int msgid;                    /* identificador da fila de mensagens */
static char *n_fic; /* nome do ficheiro de armazenamento do estado interno */
static MENSAGEM_EAF meaf = { N+1 };    /* caracterização do estado interno */


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
    int chave;                        /* chave de acesso à fila de mensagens */
    unsigned int niter;                               /* número de iterações */
    char *tinp;                             /* teste de parâmetros numéricos */
    unsigned int i;

    /* validação da passagem de parâmetros */

    if (argc != 6) { 
        freopen ("erro", "a", stderr);
        fprintf (stderr, "n.º de parâmetros incorrecto\n");
        exit (EXIT_FAILURE);
    }
    else freopen (argv[5], "w", stderr);
    f = (unsigned int) strtol (argv[1], &tinp, 0);
    if ((*tinp != '\0') || (f >= N)) { 
        fprintf (stderr, "erro na identificação do processo filósofo\n");
        exit (EXIT_FAILURE);
    }
    niter = (unsigned int) strtol (argv[2], &tinp, 0);
    if ((*tinp != '\0') || (niter == 0) || (niter > NMAXITER)) { 
        fprintf (stderr, "erro no número de iterações\n");
        exit (EXIT_FAILURE);
    }
    n_fic = argv[3];
    chave = (unsigned int) strtol (argv[4], &tinp, 0);
    if (*tinp != '\0') { 
        fprintf (stderr, "erro no número de iterações\n");
        exit (EXIT_FAILURE);
    }

    /* ligação à fila de mensagens */

    if ((msgid = msg_connect (chave)) == -1) { 
        perror ("erro na ligação à fila de mensagens");
        exit (EXIT_FAILURE);
    }

    /* simulação da situação propriamente dita */

    srandom ((unsigned int) getpid ());
    for (i = 0; i < niter; i++) { 
        meditar ();
        pegar_nos_garfos (f);
        comer ();
        pousar_os_garfos (f);
    }

    exit (EXIT_SUCCESS);
}


/* procedimento meditar */

static void meditar (void)
{
    usleep((unsigned int) floor (10000.0 * random () / RAND_MAX));
}


/* procedimento comer */

static void comer (void)
{
    usleep((unsigned int) floor (10000.0 * random () / RAND_MAX));
}


/* procedimento de pegar nos garfos */

static void pegar_nos_garfos (unsigned int f)
{
    MENSAGEM_SINC msinc;                       /* tomada de posse dos garfos */

    msinc.destid = f+1;
    /* entrada na região crítica */
    if (msg_receive (msgid, &meaf, sizeof (CORPO), meaf.destid) == -1) { 
        perror ("erro na recepção da mensagem de estado interno");
        exit (EXIT_FAILURE);
    }
    meaf.et.estado[f] = COM_FOME;          /* o filósofo assumiu que tem fome */
    guardar_estado ();
    avaliar_situacao (f);    /* o filósofo tenta garantir a posse dos garfos */
    /* saída da região crítica */
    if (msg_send_nb (msgid, &meaf, sizeof (CORPO)) == -1) { 
        perror ("erro no envio da mensagem de estado interno");
        exit (EXIT_FAILURE);
    }
    /* aguarda, se não conseguiu obter os garfos */
    if (msg_receive (msgid, &msinc, 0, msinc.destid) == -1) { 
        perror ("erro na recepção da mensagem de obtenção dos garfos");
        exit (EXIT_FAILURE);
    }
}

/* procedimento de pousar nos garfos */

static void pousar_os_garfos (unsigned int f)

{
    /* entrada na região crítica */
    if (msg_receive (msgid, &meaf, sizeof (CORPO), meaf.destid) == -1) { 
        perror ("erro na recepção da mensagem de estado interno");
        exit (EXIT_FAILURE);
    }
    meaf.et.estado[f] = MEDITANDO;      /* o filósofo prepara-se para meditar */
    guardar_estado ();
    avaliar_situacao (ESQ);      /* avalia o caso do filósofo à sua esquerda */
    avaliar_situacao (DIR);       /* avalia o caso do filósofo à sua direita */
    /* saída da região crítica */
    if (msg_send_nb (msgid, &meaf, sizeof (CORPO)) == -1) { 
        perror ("erro no envio da mensagem de estado interno");
        exit (EXIT_FAILURE);
    }
}

/* procedimento de avaliar a situação */

static void avaliar_situacao (unsigned int f)

{
    MENSAGEM_SINC msinc;                       /* tomada de posse dos garfos */

    msinc.destid = f+1;

    if ((meaf.et.estado[f] == COM_FOME) &&
            (meaf.et.estado[ESQ] != COMENDO) && (meaf.et.estado[DIR] != COMENDO)) { 
        meaf.et.estado[f] = COMENDO;        /* os garfos estão efectivamente
                         na sua posse */
        guardar_estado ();
        /* o processo de continuação é garantido */
        if (msg_send_nb (msgid, &msinc, 0) == -1) { 
            perror ("erro no envio da mensagem de obtenção dos garfos");
            exit (EXIT_FAILURE);
        }
    }
}


/* procedimento de armazenamento do estado interno */

static void guardar_estado (void)

{
    FILE *fic;                                      /* descritor do ficheiro */
    unsigned int i;

    if ((fic = fopen (n_fic, "a")) == NULL) { 
        perror ("erro na operação de abertura do ficheiro de armazenamento");
        exit (EXIT_FAILURE);
    }
    for (i = 0; i < N; i++)
        switch (meaf.et.estado[i])
        { case MEDITANDO: fprintf (fic, "  MEDITANDO ");
            break;
            case COM_FOME:  fprintf (fic, "  COM_FOME  ");
                    break;
            case COMENDO:   fprintf (fic, "   COMENDO  ");
        }
    fprintf (fic, "\n");
    if (fclose (fic) == EOF) { 
        perror ("erro na operação de fecho do ficheiro de armazenamento");
        exit (EXIT_FAILURE);
    }
}
