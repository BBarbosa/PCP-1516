#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

#define ROW 500
#define COL 500

int mat[ROW][COL];
int linha,coluna; //garantir que linha < ROW e coluna < COL

int criaFicheiro(char *nome, int l) {
    FILE *fp = fopen(nome,"w");
    int i,j,flag=1;
    
    fp = fopen(nome,"w");

    for(i=0; i<l-1;i++) {
        fprintf(fp,"%d",0);
        for(j=0;j<l-2;j++) {
            if(flag) {
                fprintf(fp,"%d",0);
            } else {
                fprintf(fp,"%d",1);
            }
            
        }
        flag = 0;
        fprintf(fp,"%d\n",0);
    }
    
    for(j=0;j<l;j++) {
        fprintf(fp,"%d",0);
    }
    
    fprintf(fp,"\n");
    
    return 0;
}

/**
 * Carrega a imagem para a matriz
 * @param path
 * @return
 */
int carregaImagem(char *path) {
    int fp,i,j,flag=1;
    char b = '1';

    fp = open(path,O_RDONLY);

    i = j = 0;
    while(read(fp,&b,1)) {
        if(b == '\n') {
            if(flag) {
                coluna = j;
                flag = 0;
            }
            i++;
            j=0;
        } else {
            mat[i][j] = atoi(&b);
            j++;
        }
    }
    
    linha = i;
    close(fp);

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

    for(i=0; i<linha;i++) {
        for(j=0;j<coluna;j++) {
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

    for(i=0;i<linha;i++) {
        for(j=0;j<coluna;j++) {
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
    
    criaFicheiro("teste.txt",100);
    
    /* le ficheiro de input - verificar nargs */
    if(argc != 2) {
        alterou = 0;
        printf("#ERRO!");
    } else {
        carregaImagem("teste.txt");
    }

    while(alterou) {
        alterou = 0;

        /* Primeira passagem */
        for(i=1; i<ROW-1; i++) {
            for(j=1; j<COL-1; j++) {
                if(mat[i][j]) {
                    /* vizinhos */
                    vizinhos = mat[i][j+1] + mat[i+1][j+1] + mat[i+1][j] + mat[i+1][j-1] + mat[i][j-1] + mat[i-1][j-1] + mat[i-1][j] + mat[i-1][j+1];
                    if(vizinhos >= 2 && vizinhos <= 6) {
                        /* transicçoes */
                        transicoes = trans(mat[i+1][j+1],mat[i][j+1]) + trans(mat[i+1][j],mat[i+1][j+1]) + trans(mat[i+1][j-1],mat[i+1][j]) + trans(mat[i][j-1],mat[i+1][j-1]) +
                                     trans(mat[i-1][j-1],mat[i][j-1]) + trans(mat[i-1][j],mat[i-1][j-1]) + trans(mat[i-1][j+1],mat[i-1][j]) + trans(mat[i][j+1],mat[i-1][j+1]);
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
        }

        /* Segunda passagem */
        for(i=1; i<ROW-1; i++) {
            for(j=1; j<COL-1; j++) {
                if(mat[i][j]) {
                    /* vizinhos */
                    vizinhos = mat[i][j+1] + mat[i+1][j+1] + mat[i+1][j] + mat[i+1][j-1] + mat[i][j-1] + mat[i-1][j-1] + mat[i-1][j] + mat[i-1][j+1];
                    if(vizinhos >= 2 && vizinhos <= 6) {
                        /* transicçoes */
                        transicoes = trans(mat[i+1][j+1],mat[i][j+1]) + trans(mat[i+1][j],mat[i+1][j+1]) + trans(mat[i+1][j-1],mat[i+1][j]) + trans(mat[i][j-1],mat[i+1][j-1]) +
                                     trans(mat[i-1][j-1],mat[i][j-1]) + trans(mat[i-1][j],mat[i-1][j-1]) + trans(mat[i-1][j+1],mat[i-1][j]) + trans(mat[i][j+1],mat[i-1][j+1]);
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

    }

    /* escreve a matriz no ficheiro de output */
    imprimeMatriz();

    return 0;
}
