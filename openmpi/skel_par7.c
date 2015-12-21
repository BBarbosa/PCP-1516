#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define ROW 5000        //define o máximo de linhas
#define COL 5000        //define o máximo de colunas 

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

char tipo[3];          //tipo de ficheiro
int matrix[ROW][COL];    //matriz onde será guardada a imagem
int linhas,colunas;    //garantir que linha < ROW e coluna < COL, #linhas e #colunas efectivamente usadas

/**
 * Carrega imagem do tipo .pbm
 * @param path
 * @return
 */
int carregaImagemPBM(char *path) {
    FILE *fp;
    int l,c,i,j;
    int conta=0;

    fp = fopen(path,"r");

    if(fp) {
        fscanf(fp,"%s",tipo);     //ok
        fscanf(fp,"%d %d",&c,&l); //ok
        linhas = l;
        colunas = c;

        for(i=0;i<l;i++) {
            for(j=0;j<c;j++) {
                fscanf(fp,"%d ",&matrix[i][j]);
                if(matrix[i][j]==1)
                conta++;
            }
        }
    }
    fclose(fp);

    return conta;
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
            fprintf(fp,"%d ",matrix[i][j]);
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
        totalP,
        dest,
        mtype,
        rows,
        averow, extra, offset,
        i, ii, j, k, rc, lim;
    MPI_Status status;
    
    char matI[ROW][COL];
    char mat[ROW][COL];
    char borda[COL];
    
    double procTime,mProcTime=0,sendTime,recTime,barTime,commTime = 0,totalTime;
    
    if(argc < 2) {
      printf("#ERRO: Insira uma imagem\n");
      exit(1);
    } 
    
    for(i=0;i<COL;i++){
      borda[i]=0;
    }
    
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &taskid );
    MPI_Comm_size( MPI_COMM_WORLD, &numtasks );
    //MPI_Comm_split( MPI_COMM_WORLD, rank == 0, 0, &new_comm );

    if(numtasks < 2) {
      printf("#ERRO: Precisa de, pelo menos, 2 tasks!\n");
      MPI_Abort(MPI_COMM_WORLD, rc);
      exit(1);
    }
    
    /* numworkers */
    numworkers = numtasks-1;
    
    /* preenche matriz local */
    totalP = carregaImagemPBM(argv[1]);
    
    int t = totalP /numworkers;
    int offs[64] ;
    int off=0;
    int ind=0;  
    int rowAct=0;
    int offAct=0;
      for(i=0;i<linhas;i++) {
      if(off > t)
      off=0;
      for(j=0;j<colunas;j++) {
        
        mat[i][j] = matrix[i][j];
        if(mat[i][j]==1){
        off++;
        }
        if(off==t){
        offs[ind]=i;
        ind++;
        off++;
        }
}}



    
    /******************** MASTER *********************/
    if (taskid == MASTER) {
      totalTime = MPI_Wtime();
      //printf("MPI iniciado com %d tasks.\n",numtasks);
      /* envia os dados da matriz*/
      averow = linhas / numworkers;
      extra = linhas % numworkers;
      offset = 0;
      mtype = FROM_MASTER;
      rows = averow;

      
    }
      
      MPI_Barrier(MPI_COMM_WORLD);
      //INICIA TEMPO
      
      if (taskid == MASTER) {
      
      for(dest=1; dest<=numworkers; dest++) {
        //rows = (dest <= extra) ? averow+1 : averow;
        
        if(dest==1){
        
        offset=0;
        rows= offs[dest-1];
        }else{ if(dest==numworkers){offset=offs[dest-1] ; if(offset==0){offset=offs[dest-2];} ; rows=linhas-offset;} else{ offset= offs[dest-2];rows = offs[dest-1]-offset;  }}
        
        sendTime = MPI_Wtime() - sendTime;
        //printf("A enviar %d linhas para a task %d offset=%d\n",rows,dest,offset);
        MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
        MPI_Send(mat[offAct], rows*colunas, MPI_BYTE, dest, mtype,MPI_COMM_WORLD);
       // offset = offset + rows;
        sendTime = MPI_Wtime() - sendTime;
      }
      
      }
      
      

    /******************** WORKER ********************/
    if(taskid > MASTER) {
    
    
      mtype = FROM_MASTER;
      
      
      
      MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      MPI_Recv(mat[offset], rows*colunas, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
      
      
      //printf("recTime %lf\n",recTime);

      /* Processamento */
      int alterou=1,vizinhos,transicoes,complementos;
      int p2,p3,p4,p5,p6,p7,p8,p9;
      
      lim = rows + offset;
      
      procTime = MPI_Wtime(); 
      while(alterou) {
          alterou = 0;
          /* Primeira passagem */
          for(i=offset+1; i<lim; i++) {
              for(j=1; j<colunas-1; j++) {
                  /* Se o pixel for diferente de zero */
                  if(mat[i][j]) {
                      /*if(i==offset+1){
                        p2=0;
                        p3=0;
                        p9=0;
                      }*/
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
          for(i=offset+1; i<lim; i++) {
              for(j=1; j<colunas-1; j++) {
                  /* Se o pixel for diferente de zero */
                  if(mat[i][j]) {
                  /*if(i==offset+1){
                        p2=0;
                        p3=0;
                        p9=0;
                      }*/
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
      procTime = MPI_Wtime() - procTime;
      //printf("procTime %lf\n",procTime);
      
      mtype = FROM_WORKER;
      
      //TEMPO
      
      
      MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      MPI_Send(mat[offset], rows*colunas, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
      
      
      
    }

    /* Recebe resultados */
      if (taskid == MASTER) {
      mtype = FROM_WORKER;
      for(i=1; i<=numworkers; i++) {
        source = i;
        recTime = MPI_Wtime();
        MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&mat[offset][0], rows*colunas, MPI_INT, source, mtype,MPI_COMM_WORLD, &status);
        //printf("Recebi resultados da task %d\n",source);
        recTime = MPI_Wtime() - recTime;
        
        totalTime = MPI_Wtime() - totalTime;
        
        for(ii=offset;ii<offset+rows;ii++) {
          for(j=0;j<colunas;j++) {
            matrix[ii][j] = mat[ii][j];
          }
        }
      }
      
      /* Imprime resultados */
      
      imprimeMatriz();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    
    if(taskid == MASTER) {
      
      printf("totalTime %lf\n",totalTime);
      printf("%lf\n",totalTime-sendTime);
      printf("%lf\n",sendTime);
      
    }
    
    
    return 0;
}
