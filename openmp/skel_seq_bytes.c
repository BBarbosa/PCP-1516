#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<ctype.h>
#include<omp.h>

#define ROW 4000        //define o máximo de linhas
#define COL 4000        //define o máximo de colunas

char tipo[3];          //tipo de ficheiro
char mat[ROW][COL];     //matriz onde será guardada a imagem
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
    
    fp = fopen("output_seq_bytes.ascii.pbm","w");
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
 * Calcula o complementar de um bit (char)
 * @param num
 * @return
 */
int comp(char num) {
    return (num == '0' ? 1 : 0);
}

/**
 * Verifica se houve transicao (char)
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

int main(int argc, char **argv) {
    int alterou=1,i,j,vizinhos,transicoes,complementos;
    
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
                if(mat[i][j] == '1') {
                    /* vizinhos */
                    vizinhos = val(mat[i-1][j]) + val(mat[i-1][j+1]) + val(mat[i][j+1]) + val(mat[i+1][j+1]) + val(mat[i+1][j]) + val(mat[i+1][j-1]) + val(mat[i][j-1]) + val(mat[i-1][j-1]);
                    if(vizinhos >= 2 && vizinhos <= 6) {
                        /* transicçoes */
                        transicoes = trans(mat[i-1][j+1],mat[i-1][j]) + trans(mat[i][j+1],mat[i-1][j+1]) + trans(mat[i+1][j+1],mat[i][j+1]) + trans(mat[i+1][j],mat[i+1][j+1]) +
                                     trans(mat[i+1][j-1],mat[i+1][j]) + trans(mat[i][j-1],mat[i+1][j-1]) + trans(mat[i-1][j-1],mat[i][j-1]) + trans(mat[i-1][j],mat[i-1][j-1]);
                        if(transicoes == 1) {
                            /* complementos */
                            complementos = comp(mat[i][j+1]) + comp(mat[i+1][j]) + comp(mat[i-1][j]) * comp(mat[i][j-1]);
                            if(complementos == 1) {
                                mat[i][j] = '0';
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
                if(mat[i][j] == '1') {
                    /* vizinhos */
                    vizinhos = val(mat[i-1][j]) + val(mat[i-1][j+1]) + val(mat[i][j+1]) + val(mat[i+1][j+1]) + val(mat[i+1][j]) + val(mat[i+1][j-1]) + val(mat[i][j-1]) + val(mat[i-1][j-1]);
                    if(vizinhos >= 2 && vizinhos <= 6) {
                        /* transicçoes */
                        transicoes = trans(mat[i-1][j+1],mat[i-1][j]) + trans(mat[i][j+1],mat[i-1][j+1]) + trans(mat[i+1][j+1],mat[i][j+1]) + trans(mat[i+1][j],mat[i+1][j+1]) +
                                     trans(mat[i+1][j-1],mat[i+1][j]) + trans(mat[i][j-1],mat[i+1][j-1]) + trans(mat[i-1][j-1],mat[i][j-1]) + trans(mat[i-1][j],mat[i-1][j-1]);
                        if(transicoes == 1) {
                            /* complementos */
                            complementos = comp(mat[i-1][j]) + comp(mat[i][j-1]) + comp(mat[i][j+1]) * comp(mat[i+1][j]);
                            if(complementos == 1) {
                                mat[i][j] = '0';
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
