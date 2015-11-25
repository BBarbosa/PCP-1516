#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define ROW 4000        //define o máximo de linhas
#define COL 4000        //define o máximo de colunas

char tipo[3];          //tipo de ficheiro
char mat[ROW][COL];    //matriz onde será guardada a imagem
int linhas,colunas;    //garantir que linha < ROW e coluna < COL, #linhas e #colunas efectivamente usadas

/**
 * Carrega imagem do tipo .pbm
 * @param path
 * @return
 */
int carregaImagemPBM(char *path) {
    FILE *fp;
    //char tipo[3];
    int l,c,i,j;

    fp = fopen(path,"r");

    if(fp) {
        fscanf(fp,"%s",tipo);     //ok
        fscanf(fp,"%d %d",&c,&l); //ok
        linhas = l;
        colunas = c;
        //printf("Tipo %s\nLinhas %d Colunas %d\n",tipo,l,c);

        for(i=0;i<l;i++) {
            for(j=0;j<c;j++) {
                fscanf(fp,"%c ",&mat[i][j]);
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

    fp = fopen("output.ascii.pbm","w");
    fprintf(fp,"%s\n",tipo);
    fprintf(fp,"%d %d\n",colunas,linhas);

    for(i=0;i<linhas;i++) {
        for(j=0;j<colunas;j++) {
            fprintf(fp,"%c ",mat[i][j]);
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
int comp(char num) {
    return (num == '0' ? 1 : 0);
}

/**
 * Verifica se houve transicao
 * @param act
 * @param ant
 * @return
 */
int trans(char act, char ant) {
  return (act == '1' && ant == '0');
}

/**
 * Calcula o valor inteiro de um dígito de 0 a 9
 * @param n
 * @return
 */
int val(char n) {
    return (n - '0');
}



int main(int argc, char **argv )
{
    int rank, size;
    MPI_Comm new_comm;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_split( MPI_COMM_WORLD, rank == 0, 0, &new_comm );
    if (rank == 0)
	     master_io( MPI_COMM_WORLD, new_comm );
    else
	     slave_io( MPI_COMM_WORLD, new_comm , 5);

    MPI_Finalize();
    return 0;
}

/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm)
{
    int        i,j, size;
    char       buf[256];
    MPI_Status status;

    MPI_Comm_size( master_comm, &size );
    for (j=1; j<=2; j++) {
	      for (i=1; i<size; i++) {
	         MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status );
	         fputs( buf, stdout );
	      }
    }
}

/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm, int l, int c, char **buf)
{
    int alterou=1,i,j,vizinhos,transicoes,complementos;
    int p2,p3,p4,p5,p6,p7,p8,p9;
    //char buf[l][c];
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
    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );

    return 0;
}
