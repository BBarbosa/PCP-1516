#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROW 5000        //define o máximo de linhas
#define COL 5000        //define o máximo de colunas

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

char tipo[3];          //tipo de ficheiro
int mat[ROW][COL];    //matriz onde será guardada a imagem
int linhas,colunas;    //garantir que linha < ROW e coluna < COL, #linhas e #colunas efectivamente usadas

/**
 * Carrega imagem do tipo .pbm
 * @param path
 * @return
 */
int carregaImagemPBM(char *path) {
    FILE *fp;
    int l,c,i,j;

    fp = fopen(path,"r");

    if(fp) {
        fscanf(fp,"%s",tipo);     //ok
        fscanf(fp,"%d %d",&c,&l); //ok
        linhas = l;
        colunas = c;

        for(i=0;i<l;i++) {
            for(j=0;j<c;j++) {
                fscanf(fp,"%d ",&mat[i][j]);
            }
        }
    }
    fclose(fp);

    return 1;
}

/**
 * Cria a imagem do esqueleto
 */
void imprimeMatriz() {
    int i,j;
    FILE *fp;

    fp = fopen("output_par.ascii.pbm","w");
    fprintf(fp,"%s\n",tipo);
    fprintf(fp,"%d %d\n",colunas,linhas);

    for(i=0;i<linhas;i++) {
        for(j=0;j<colunas;j++) {
            fprintf(fp,"%d ",mat[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}

/**
 * Calcula o complementar de um bit
 * @param num
 * @return
 */
int comp(int num) {
    return (num == 0 ? 1 : 0);
}

/**
 * Verifica se houve transicao
 * @param act
 * @param ant
 * @return
 */
int trans(int act, int ant) {
  return (act == 1 && ant == 0);
}

/**
 * Calcula o valor inteiro de um dígito de 0 a 9
 * @param n
 * @return
 */
int val(int n) {
    return n;
}

/**
 * main
 */
int main(int argc, char **argv )
{
    int numtasks,
        taskid,
        numworkers,
        source,
        dest,
        mtype,
        rows,
        averow, extra, offset,
        i, j, k, rc;
    MPI_Status status;

    if(arcg < 2) {
      printf("#ERRO: Insira uma imagem\n");
      exit(1);
    } else {
      carregaImagemPBM(argv[1]);
    }

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &taskid );
    MPI_Comm_size( MPI_COMM_WORLD, &numtasks )
    //MPI_Comm_split( MPI_COMM_WORLD, rank == 0, 0, &new_comm );

    if(numtasks < 2) {
      printf("#ERRO: Precisa de, pelo menos, 2 tasks!\n");
      MPI_Abort(MPI_COMM_WORLD, rc);
      exit(1);
    }

    numworkers = numtasks-1;

    /******************** MASTER *********************/
    if (taskid == MASTER) {
      printf("MPI iniciado com %d tasks.\n",numtasks);
      /* envia os dados da matriz*/
      averow = linhas / numworkers;
      extra = linhas % numworkes;
      offset = 0;
      mytype = FROM_MASTER;

      for(dest=1; dest<=numworkes; dest++) {
        rows = (dest <= extra) ? averow+1 : averow;
        printf("A enviar %d linhas para a task %d offset=%d\n",rows,dest,offset);
        MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
        MPI_Send(&mat[offset][0], rows*colunas, MPI_INT, dest, mtype,
                  MPI_COMM_WORLD);
      }

      /* Recebe resultados */
      mytype = FROM_WORKER;
      for(i=1; i<=numworkes; i++) {
        source = i;
        MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&mat[offset][0], rows*colunas, MPI_INT, source, mtype,
                 MPI_COMM_WORLD, &status);
        printf("Recebi resultados da task %d\n",source);
      }

      /* Imprime resultados */
      imprimeMatriz();
    }

    /******************** WORKER ********************/
    if(taskid > MASTER) {
      mytype = FROM_MASTER;
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&mat, rows*colunas, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

      /* Processamento */
      int alterou=1,vizinhos,transicoes,complementos;
      int p2,p3,p4,p5,p6,p7,p8,p9;

      while(alterou) {
          alterou = 0;
          /* Primeira passagem */
          for(i=1; i<linhas-1; i++) {
              for(j=1; j<colunas-1; j++) {
                  /* Se o pixel for diferente de zero */
                  if(mat[i][j]) {
                      p2 = mat[i-1][j]; p3 = mat[i-1][j+1]; p4 = mat[i][j+1]; p5 = mat[i+1][j+1];
                      p6 = mat[i+1][j]; p7 = mat[i+1][j-1]; p8 = mat[i][j-1]; p9 = mat[i-1][j-1];
                      /* vizinhos */
                      vizinhos = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
                      if(vizinhos >= 2 && vizinhos <= 6) {
                          /* transicçoes */
                          transicoes = trans(p3,p2) + trans(p4,p3) + trans(p5,p4) + trans(p6,p5) +
                                       trans(p7,p6) + trans(p8,p7) + trans(p9,p8) + trans(p2,p9);
                          if(transicoes == 1) {
                              /* complementos */
                              complementos = ((p2 && p4 && p6)==0 && (p4 && p6 && p8)==0 );
                              if(complementos == 1) {
                                  mat[i][j] = 0;
                                  if(!alterou) {
                                      alterou = 1;
                                  }
                              }
                          }

                      }
                  }
              }
          }

          /* Segunda passagem */
          for(i=1; i<linhas-1; i++) {
              for(j=1; j<colunas-1; j++) {
                  /* Se o pixel for diferente de zero */
                  if(mat[i][j]) {
                      p2 = mat[i-1][j]; p3 = mat[i-1][j+1]; p4 = mat[i][j+1]; p5 = mat[i+1][j+1];
                      p6 = mat[i+1][j]; p7 = mat[i+1][j-1]; p8 = mat[i][j-1]; p9 = mat[i-1][j-1];
                      /* vizinhos */
                      vizinhos = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
                      if(vizinhos >= 2 && vizinhos <= 6) {
                          /* transicçoes */
                          transicoes = trans(p3,p2) + trans(p4,p3) + trans(p5,p4) + trans(p6,p5) +
                                       trans(p7,p6) + trans(p8,p7) + trans(p9,p8) + trans(p2,p9);
                          if(transicoes == 1) {
                              /* complementos */
                              complementos = ((p2 && p4 && p8)==0 && (p2 && p6 && p8)==0);
                              if(complementos == 1) {
                                  mat[i][j] = 0;
                                  if(!alterou) {
                                      alterou = 1;
                                  }
                              }
                          }

                      }
                  }
              }
          }
      }

      mtype = FROM_WORKER;
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&mat, rows*colunas, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}


/* This is the master */
int master(MPI_Comm master_comm, MPI_Comm comm, char **buf)
{
    int        i,j, size;
    MPI_Status status;

    MPI_Comm_size( master_comm, &size );
    for (j=1; j<=2; j++) {
	      for (i=1; i<size; i++) {
	         MPI_Recv( buf, (linhas*colunas), MPI_CHAR, i, 0, master_comm, &status );
	         imprimeMatriz();
	      }
    }
}

/* This is the slave */
int slave(MPI_Comm master_comm, MPI_Comm comm, int l, int c, char **buf)
{
    int alterou=1,i,j,vizinhos,transicoes,complementos;
    int p2,p3,p4,p5,p6,p7,p8,p9;
    int  rank;

    MPI_Comm_rank( comm, &rank );
    /**/
    while(alterou) {
        alterou = 0;
        /* Primeira passagem */
        for(i=1; i<l-1; i++) {
            for(j=1; j<c-1; j++) {
                /* Se o pixel for diferente de zero */
                if(mat[i][j]) {
                    p2 = val(mat[i-1][j]); p3 = val(mat[i-1][j+1]); p4 = val(mat[i][j+1]); p5 = val(mat[i+1][j+1]);
                    p6 = val(mat[i+1][j]); p7 = val(mat[i+1][j-1]); p8 = val(mat[i][j-1]); p9 = val(mat[i-1][j-1]);
                    /* vizinhos */
                    vizinhos = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
                    if(vizinhos >= 2 && vizinhos <= 6) {
                        /* transicçoes */
                        transicoes = trans(p3,p2) + trans(p4,p3) + trans(p5,p4) + trans(p6,p5) +
                                     trans(p7,p6) + trans(p8,p7) + trans(p9,p8) + trans(p2,p9);
                        if(transicoes == 1) {
                            /* complementos */
                            complementos = comp(p4) + comp(p6) + comp(p8) * comp(p2);
                            if(complementos == 1) {
                                mat[i][j] = 0;
                                if(!alterou) {
                                    alterou = 1;
                                }
                            }
                        }

                    }
                }
            }
        }

        /* Segunda passagem */
        for(i=1; i<l-1; i++) {
            for(j=1; j<c-1; j++) {
                /* Se o pixel for diferente de zero */
                if(mat[i][j]) {
                    p2 = val(mat[i-1][j]); p3 = val(mat[i-1][j+1]); p4 = val(mat[i][j+1]); p5 = val(mat[i+1][j+1]);
                    p6 = val(mat[i+1][j]); p7 = val(mat[i+1][j-1]); p8 = val(mat[i][j-1]); p9 = val(mat[i-1][j-1]);
                    /* vizinhos */
                    vizinhos = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
                    if(vizinhos >= 2 && vizinhos <= 6) {
                        /* transicçoes */
                        transicoes = trans(p3,p2) + trans(p4,p3) + trans(p5,p4) + trans(p6,p5) +
                                     trans(p7,p6) + trans(p8,p7) + trans(p9,p8) + trans(p2,p9);
                        if(transicoes == 1) {
                            /* complementos */
                            complementos = comp(p2) + comp(p8) + comp(p4) * comp(p6);
                            if(complementos == 1) {
                                mat[i][j] = 0;
                                if(!alterou) {
                                    alterou = 1;
                                }
                            }
                        }

                    }
                }
            }
        }

    }
    /**/
    MPI_Send( buf, (l*c), MPI_CHAR, 0, 0, master_comm );
}
