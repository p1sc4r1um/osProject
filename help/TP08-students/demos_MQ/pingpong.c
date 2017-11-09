
/*
 * Programa que ilustra o uso de message queues para comunicar.
 *
 * Dois processos trocam uma mensagem de "ping-pong" via uma
 * message queue do sistema.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

typedef struct
{
  /* Tipo da mensagem */
  long mtype;
  /* Conteudo da mensagem */
  int total_msgs;                     /* Total de mensagens  */
  int um_valor;                       /* Valor ping-pong     */
} pingpong_msg;


int mqid;                             /* Id da message queue */


#define PING	1                     /* Processo PING */
#define PONG	2                     /* Processo PONG */


/*****************************************************/

static void ping()
{
  int novo_valor;
  pingpong_msg my_msg;

  my_msg.total_msgs = 0;
  my_msg.um_valor   = random() % 100;
  my_msg.mtype      = PONG;
  srandom(getpid());

  while (1)
    {
      /* Envia a mensagem */
      msgsnd(mqid, &my_msg, sizeof(pingpong_msg), 0);

      /* Espera receber uma mensagem com o seu id (=PING) */
      msgrcv(mqid, &my_msg, sizeof(pingpong_msg), PING, 0);

      printf("[Ping] Recebi uma mensagem!\n");
      printf("---------------------------\n");
      printf("Total mensagens = %d\n", my_msg.total_msgs);
      printf("Valor recebido  = %d\n", my_msg.um_valor);

      novo_valor = random() % 100;
      printf("Valor a enviar  = %d\n\n", novo_valor);

      ++my_msg.total_msgs;
      my_msg.um_valor = novo_valor;
      my_msg.mtype    = PONG;

      sleep(4);
    }
}

static void pong()
{
  int novo_valor;
  pingpong_msg my_msg;

  srandom(getpid());

  while (1)
    {
      /* Espera receber uma mensagem com o seu id (=PONG) */
      msgrcv(mqid, &my_msg, sizeof(pingpong_msg), PONG, 0);

      printf("[Pong] Recebi uma mensagem!\n");
      printf("---------------------------\n");
      printf("Total mensagens = %d\n", my_msg.total_msgs);
      printf("Valor passado   = %d\n", my_msg.um_valor);

      novo_valor = random() % 100;
      printf("Valor a enviar  = %d\n\n", novo_valor);

      ++my_msg.total_msgs;
      my_msg.um_valor = novo_valor;
      my_msg.mtype    = PING;

      sleep(4);

      /* Envia a mensagem */
      msgsnd(mqid, &my_msg, sizeof(pingpong_msg), 0);
    }
}


/*****************************************************/


static void cleanup()
{
  //msgctl(mqid, IPC_RMID, 0);

  /* Garante que todos os processos recebem um SIGTERM */
  kill(0, SIGTERM);
  exit(0);
}

static void cria_ipcs()
{
  /* Redirecional alguns sinais graves para uma rotina de cleanup */
  signal(SIGINT, cleanup);
  signal(SIGHUP, cleanup);
  signal(SIGQUIT, cleanup);
  signal(SIGTERM, cleanup);

  /* Cria uma message queue */
  mqid = msgget(IPC_PRIVATE, IPC_CREAT|0777);
  if (mqid < 0)
    {
      perror("Ao criar a message queue");
      exit(0);
    }
}

static void cria_processos()
{
  printf("Carregue em ^C para terminar...\n\n");

  if (fork() == 0)
    {
      ping();
      exit(0);
    }
  else
    {
      pong();
      exit(0);
    }
}

int main()
{
  cria_ipcs();
  cria_processos();

  return 0;
}


