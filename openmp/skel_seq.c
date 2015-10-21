#include<stdio.h>
#include<opencv2/highgui/highgui.hpp>
#define ROW 500
#define COL 500

int mat[ROW][COL];
int linha,coluna;

int load(String *path) {
    
}


int main() {
    int alterou=0,i,j,primeira=1,vizinhos=0;
    
    while(alterou) {
        alterou = 0;
        for(i=1; i<linhas-1; i++) {
            for(j=1; j<colunas-1; j++) {
                vizinhos = ; 
                if(vizinhos >= 2 && vizinhos <= 6) {
                    /* transicçoes */
                    if(transicoes = 1) {
                        /* complementos */
                        if( ) {
                            mat[i][j] = 0;
                            alterou = 1;
                        }
                    }
                    
                }
            }
        }
    }
    
    IplImage* img = cvLoadImage("imagens/a.gif",CV_LOAD_IMAGE_COLOR);
    cvNamedWindow("opencvtest",CV_WINDOW_AUTOSIZE);
    cvShowImage("opencvtest",img);
    cvWaitKey(0);
    cvReleaseImage(&img);
    
    return 0;
}