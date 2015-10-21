#include<stdio.h>
#include<stdlib.h>

#define ROW 10
#define COL 10

int mat[ROW][COL];

/**
 * Carrega a imagem para a matriz
 * @param path
 * @return 
 */
int carregaImagem(char *path) {
    FILE *fp;
    int i,j;
    
    fp = fopen(path,"r");
    
    if(fp != NULL) {
        for(i=0; i<ROW;i++) {
            for(j=0;j<COL;j++) {
                fscanf(fp,"%d",&mat[i][j]);
            }
        }
    } else {
        return -1;
    }
    fclose(fp);
    
    return 1;
}

/**
 * Escreve o resultado num ficheiro
 * @param path
 * @return 
 */
int escreveResultado(char *path) {
    FILE *fp;
    int i,j;
    
    fp = fopen(path,"w");
    
    for(i=0; i<ROW;i++) {
        for(j=0;j<COL;j++) {
            fprintf(fp,"%d",mat[i][j]);
        }
        fprintf(fp,"\n");
    }
    
    return 1;
}

/**
 * Imprime a matriz (testes)
 */
void imprimeMatriz() {
    int i,j;
    
    for(i=0;i<ROW;i++) {
        for(j=0;j<COL;j++) {
            printf("%d",mat[i][j]);
        }
        printf("\n");
    }
}

/**
 * Calcula o complementar de um bit
 * @param num
 * @return 
 */
int comp(int num) {
    return abs(num-1);
}

int main() {
    int alterou=0,i,j,primeira=1,vizinhos=0,transicoes,complementos;
    
    /* le ficheiro de input */
    carregaImagem("quadrado10.txt");
    
    while(alterou) {
        alterou = 0;
        
        /* Primeira passagem */
        for(i=1; i<ROW-1; i++) {
            for(j=1; j<COL-1; j++) {
                /* vizinhos */
                vizinhos = mat[i][j+1] + mat[i+1][j+1] + mat[i+1][j] + mat[i+1][j-1] + mat[i][j-1] + mat[i-1][j-1] + mat[i-1][j] + mat[i-1][j+1]; 
                if(vizinhos >= 2 && vizinhos <= 6) {
                    /* transicçoes */
                    transicoes = 1;
                    if(transicoes == 1) {
                        /* complementos */
                        complementos = comp(mat[i+1][j]) + comp(mat[i][j-1]) + comp(mat[i][j+1]) * comp(mat[i-1][j]); 
                        if(complementos == 1) {
                            mat[i][j] = 0;
                            alterou = 1;
                        }
                    }
                    
                }
            }
        }
        
        /* Segunda passagem */
        for(i=1; i<ROW-1; i++) {
            for(j=1; j<COL-1; j++) {
                /* vizinhos */
                vizinhos = mat[i][j+1] + mat[i+1][j+1] + mat[i+1][j] + mat[i+1][j-1] + mat[i][j-1] + mat[i-1][j-1] + mat[i-1][j] + mat[i-1][j+1]; 
                if(vizinhos >= 2 && vizinhos <= 6) {
                    /* transicçoes */
                    transicoes = 1;
                    if(transicoes == 1) {
                        /* complementos */
                        complementos = comp(mat[i][j+1]) + comp(mat[i-1][j]) + comp(mat[i+1][j]) * comp(mat[i][j-1]);
                        if(complementos == 1) {
                            mat[i][j] = 0;
                            alterou = 1;
                        }
                    }
                    
                }
            }
        }
        
    }
    
    /* escreve a matriz no ficheiro de output */
    imprimeMatriz();
    
    return 0;
}