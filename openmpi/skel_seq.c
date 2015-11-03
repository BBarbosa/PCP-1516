#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<ctype.h>
#include<omp.h>

#define ROW 4000        //define o máximo de linhas
#define COL 4000        //define o máximo de colunas

char tipo[3];          //tipo de ficheiro
int mat[ROW][COL];     //matriz onde será guardada a imagem
int linhas,colunas;    //garantir que linha < ROW e coluna < COL
                       //#linhas e #colunas efectivamente usadas

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
    
    fp = fopen("output.ascii.pbm","w");
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

int main(int argc, char **argv) {
    int alterou=1,i,j,vizinhos,transicoes,complementos;
    int p2,p3,p4,p5,p6,p7,p8,p9;
    
    /* le ficheiro de input - verificar nargs */
    if(argc != 2) {
        alterou = 0;
        printf("#ERRO: Insira uma imagem\n");
    } else {
        carregaImagemPBM(argv[1]);
    }

    /* Processamento - iniciar timer */
    double time = omp_get_wtime();
    while(alterou) {
        alterou = 0;
        /* Primeira passagem */
        for(i=1; i<linhas-1; i++) {
            for(j=1; j<colunas-1; j++) {
                /* Se o pixel for diferente de zero */
                if(mat[i][j]) {
                    p2 = mat[i-1][j]; p3 = mat[i-1][j+1]; p4 = mat[i][j+1]; p5 = mat[i+1][j+1]; p6 = mat[i+1][j];
                    p7 = mat[i+1][j-1]; p8 = mat[i][j-1]; p9 = mat[i-1][j-1];
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
        for(i=1; i<linhas-1; i++) {
            for(j=1; j<colunas-1; j++) {
                /* Se o pixel for diferente de zero */
                if(mat[i][j]) {
                    p2 = mat[i-1][j]; p3 = mat[i-1][j+1]; p4 = mat[i][j+1]; p5 = mat[i+1][j+1]; p6 = mat[i+1][j];
                    p7 = mat[i+1][j-1]; p8 = mat[i][j-1]; p9 = mat[i-1][j-1];
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
    /* Terminar timer */
    time = omp_get_wtime() - time;
    printf("Tempo: %lf\n",time);
    /* escreve a matriz no ficheiro de output */
    imprimeMatriz();

    return 0;
}
