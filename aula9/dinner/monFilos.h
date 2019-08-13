/* Módulo definidor das operações associadas com o acesso a um monitor de
   tipo Lampson / Redell que gere a atribuição dos garfos.
   São usadas primitivas da biblioteca pthread.

   Operações implementadas:
     - pegar nos garfos;
     - pousar os garfos.

   Nome: monitorMesa.h

   Interface do módulo
*/

#ifndef MONFILOS_H_
#define MONFILOS_H_

/* mensagens de saída */

#define  OK           0
#define  ERRO        -1

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

extern int pegar_nos_garfos (unsigned int);

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

extern int pousar_os_garfos (unsigned int);

#endif /* MONFILOS_H_ */
