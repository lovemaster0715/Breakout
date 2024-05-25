#include "raylib.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define screenWidth 800
#define screenHeight 450

typedef struct {
    int cordx; // coordenada x
    int cordy; // coordenada y
    char cor;
    int largura;
    int altura;
    int destroi; // flag de destruição
} retangulo;

typedef struct {
    char nome[51]; // nome do jogador
    int ponto; // quantia de pontos
    retangulo raq; //raquete do jogador
} jogador;

typedef struct {
    int r;
    int cordx; // coordenada x
    int cordy; // coordenada y
    int cor;
    int velx; // velocidade x
    int vely; // velocidade y
} bola;

typedef struct{
    int ponto;
    char nome[51];
}score;

void pos0 (jogador *p1)
// Função para definir a posição inicial do jogador
{
    p1->raq.cordx= (float)screenWidth/2; // seta a coordenada x para metade da largura da tela
    p1->raq.cordy= (float)screenHeight*7/8; // seta a coordenada y para sete oitavos da tela
}

void desenha_raquete (jogador *p1)
// Função para desenhar a raquete do jogador
{
    // Desenha um retangulo com 100 de largura, 15 de altura e vermelho com as coordenadas dadas
    DrawRectangle(p1->raq.cordx, p1->raq.cordy, p1->raq.largura=100, p1->raq.altura=15, RED);
}

void move_raquete (jogador *p1)
// Função para mover a raquete considerando colisões com a borda da tela
{
    if (IsKeyDown(KEY_RIGHT))
    // Move para a direita se a seta direita for apertada e não ultrapassar o limite da tela
    {
        if ((p1->raq.cordx+p1->raq.largura) >= GetScreenWidth())
            {
                p1->raq.cordx=GetScreenWidth()-p1->raq.largura;
            }
        else p1->raq.cordx += 10.0f;
    }
    if (IsKeyDown(KEY_LEFT))
    // Move para a esquerda se a seta esquerda for apertada e não ultrapassar o limite da tela
    {
        if (p1->raq.cordx<=0)
            {
                p1->raq.cordx= 0;
            }
        else p1->raq.cordx -= 10.0f;
    }
}
void morte (jogador *p1)
//função que salva a pontuação do jogador se for maior que as maiores pontuações
{
    score highscores[6], aux;
    int i,j;
    FILE *arq;

    arq = fopen("highscores.bin", "r+b");

    for(i=0;i<5;i++){ //salva as informacoes da .bin em uma matriz de struct
        fread(&highscores[i], sizeof(score), 1, arq);
    }

    highscores[5].ponto = p1->ponto; //adiciona a pontuação do jogador na matriz de struct
    strcpy(highscores[5].nome, p1->nome);

    for (i = 0; i < 6; i++){// ordena a matriz de struct
        for (j = i; j < 6; j++){
            if (highscores[j].ponto > highscores[i].ponto){
                aux = highscores[i];
                highscores[i] = highscores[j];
                highscores[j] = aux;
            }
        }
    }

    rewind(arq);
    for(i=0;i<5;i++){//escreve no arquivo apenas os primeiros 5 componentes da struct
        fwrite(&highscores[i], sizeof(score), 1, arq);
    }

    fclose(arq);

    return;
}

void bola_inicial (bola *bball)
// Função para definir a posição inicial da bola
{
   // bball->cordBola {(float)screenWidth/2 ,(float)screenHeight*2/3};
    bball->cordx=(float)screenWidth/2;
    bball->cordy=(float)screenHeight*2/3;
}

void desenha_bola (bola *bball)
// Função para desenhar a bola
{
    DrawCircle(bball->cordx, bball->cordy, bball->r=8, WHITE);

}
void vel_bola_inicial (bola *bball)
// Função que define a velocidade inicial da bola
{
    bball->velx = 5;
    bball->vely = -5;
}

void move_bola (bola *bball, jogador *p1, retangulo tijolos[][10], int *vidas)
// Função para movimentar a bola e checar contra colisões com a tela
// colisões com tijolos e raquetes e somar pontos
{
    int i,j;
    //Colisão com os lados da tela
    if (((bball->cordx+bball->r)>= GetScreenWidth())||bball->cordx<=0)
        {
            bball->velx*= -1; // inverte a velocidade y
        }
    //Colisão com o teto da tela
    if (bball->cordy<=0)
        {
            bball->vely*= -1;  // inverte a velocidade y
        }
    //Colisão com a raquete do jogador
    if (((bball->cordx+bball->r) > (p1->raq.cordx)) &&
        ((bball->cordx+bball->r) < (p1->raq.cordx+(p1->raq.largura))) &&
        ((bball->cordy+bball->r) > (p1->raq.cordy)))
    {
        bball->vely*= -1;  // inverte a velocidade y
    }
    //Colisão com os tijolos e aumenta os pontos caso tocar em um tijolo
    for (i=0;i<5;i++)
    {
        for (j=0;j<10;j++)
        {
            if (tijolos[i][j].destroi!=1)
                if ((bball->cordx < (tijolos[i][j].cordx+tijolos[i][j].largura)) &&
                    (bball->cordx+bball->r > tijolos[i][j].cordx) &&
                    (bball->cordy < tijolos[i][j].cordy + tijolos[i][j].altura) &&
                    ((bball->cordy+bball->r) > (tijolos[i][j].cordy)))
                    {
                        bball->vely*= -1;  // inverte a velocidade y
                        tijolos[i][j].destroi=1;  // acende a flag de destruição
                        p1->ponto += 20;  // aumenta a quantia de pontos do jogador
                    }
        }
    }
    bball->cordx+=bball->velx;
    bball->cordy+=bball->vely;

    if ( bball->cordy > screenHeight)
    // Caso a bola passe da raquete
    {
        *vidas -= 1; // perde uma vida
        bball->cordx=(float)screenWidth/2; //reseta as coordenadas da bola
        bball->cordy=(float)screenHeight*2/3;
        bball->velx = 5; // reseta a velocidade da bola
        bball->vely = -5;
    }
}

int fim_jogo (retangulo tijolos[][10])
{ //verifica se todos os blocos foram destruidos
	int fim=0, i, j, contador=0;
	for(i=0;i<5;i++){
		for(j=0;j<10;j++){
			if(tijolos[i][j].destroi == 1){//verifica o .destruido de todos os tijolos
				contador++;
			}
		}
	}
	if(contador == 50){
        fim = 1; //retorna 1 se todos os blocos forem destruidos
	}
	return fim;
}

void desenha_tijolo (retangulo tijolos[][10])
// função que, a partir da cor da estrutura do tijolo, desenha um tijolo na tela
// se a flag do tijolo for diferente de 1 a partir da cor
{
    int i,j;
    for (i=0;i<5;i++)
        {
             for (j=0;j<10;j++)
                {
                    if (tijolos[i][j].destroi!=1)
                        {
                            switch (toupper(tijolos[i][j].cor))
                            {
                                case 'R': DrawRectangle(tijolos[i][j].cordx, tijolos[i][j].cordy, tijolos[i][j].largura, tijolos[i][j].altura, RED);
                                            break;
                                case 'G': DrawRectangle(tijolos[i][j].cordx, tijolos[i][j].cordy, tijolos[i][j].largura, tijolos[i][j].altura, GREEN);
                                            break;
                                case 'B': DrawRectangle(tijolos[i][j].cordx, tijolos[i][j].cordy, tijolos[i][j].largura, tijolos[i][j].altura, BLUE);
                                            break;
                                case 'Y': DrawRectangle(tijolos[i][j].cordx, tijolos[i][j].cordy, tijolos[i][j].largura, tijolos[i][j].altura, YELLOW);
                                            break;
                                case 'X': DrawRectangle(tijolos[i][j].cordx, tijolos[i][j].cordy, tijolos[i][j].largura, tijolos[i][j].altura, PURPLE);
                                            break;
                                case '-': tijolos[i][j].destroi = 1;
                                            break;
                            }
                        }
                }
        }
}

void vivo_tijolo (retangulo tijolos[][10])
// função para mudar a flag de destruição para 0
{
    int i,j;
    for (i=0;i<5;i++)
    {
        for (j=0;j<10;j++)
        {
            tijolos[i][j].destroi = 0;
        }
    }
}

void cord_tijolo (retangulo tijolos[][10])
// função para definir as coordenadas iniciais dos tijolos
{
    int i,j;
    for (i=0;i<5;i++)
    {
        for (j=0;j<10;j++)
        {
            tijolos[i][j].largura= 65;
            tijolos[i][j].altura= 10;
        }
    }

    for (i=0;i<5;i++)
    {
        tijolos[i][0].cordx = 39;
        for (j=0;j<10;j++)
        {
            if( !(j==0) )
            {
                tijolos[i][j].cordx = tijolos[i][j-1].cordx + 65 + 8;
            }
        }
    }
    for (j=0;j<10;j++)
    {
        tijolos[0][j].cordy = 20;
        for (i=0;i<5;i++)
        {
            if( !(i==0) )
            {
                tijolos[i][j].cordy = tijolos[i-1][j].cordy + 10 + 8;
            }
        }
    }
}

void nivel (int nivel, retangulo tijolos[][10]){
    char c, cnivel, onivel[11]={'n','i','v','e','l','1','.','t','x','t'};
    int i=0, j=0;
    FILE *level;
    cnivel = nivel+'0';
    onivel[5]=cnivel; //cria uma string nivelx.txt, onde x pode ser atualizada pelo parametro da funcao
    printf("%s\n", onivel);
    level = fopen(onivel, "r"); //abre o arquivo do nivel correspondente
     if (level == NULL){
        printf("Problema ao abrir nivel. Fatal.\n");
    }

    for(i=0;i<5;i++){ //atualiza a matriz de struct com os valores lidos do arquivo
        for(j=0;j<10;j++){
            do{
                c = fgetc(level);
                tijolos[i][j].cor = c;
            }while(c == '\n');
        }
    }
    fclose(level);
    return;
}


int main()
{
    jogador p1; // jogador
    bola bball; // bola
    retangulo tijolos[5][10] ; // matriz de tijolos
    int vidas = 5; // vidas
    p1.ponto = 0; // setar pontos do jogador como 0
    int modo = 0; // modo de jogo setado como 0
    FILE *arq;
    bool entrar = false; // flag para entrar no modo de jogo
    score highscores[5];
    int i, level=1;
    printf("Digite seu nome: "); // nome do jogador
    fgets(p1.nome,52,stdin);
    fflush(stdin);
    p1.nome[strlen(p1.nome)-1]='\0';
    InitWindow(screenWidth, screenHeight, "BreakoutINF");
    arq = fopen("highscores.bin", "rb");
    for(i=0;i<5;i++)
    {
        fread(&highscores[i], sizeof(score), 1, arq);
    }
    fclose(arq);
    SetTargetFPS(60);

   // newlevel:

    nivel(level,tijolos); // inicia o nível 1
    cord_tijolo(tijolos); // seta as posições iniciais dos tijolos
    vivo_tijolo(tijolos); // atualiza a flag de destruição
    pos0 (&p1); // posição inicial do jogador
    bola_inicial(&bball); // posição inicial da bola
    vel_bola_inicial(&bball); // velocidade inicial da bola

    while (!WindowShouldClose())
    {
        // atualizações do menu

        // checa qual a opção do menu pela flag "modo"
        if (IsKeyPressed(KEY_DOWN))
            {
                modo++;
                if (modo > (2)) modo = 0;

            }
        else if (IsKeyPressed(KEY_UP))
            {
                modo--;
                if (modo < 0) modo = 2;
            }
        // checa se vai entrar na opção do menu pelo booleano "entrar"
        // "ENTER" para entrar
        if (IsKeyPressed(KEY_ENTER))
            {
                entrar = true;
            }
        // "BACKSPACE" para sair
        if (IsKeyPressed(KEY_BACKSPACE))
            {
                entrar = false;
            }

        BeginDrawing();

            ClearBackground(BLACK);
            if (entrar == false)
            // menu inicial
            {
                //ImageDraw(&breakout, breakout, (Rectangle){ 0, 0, 200, 100},(Rectangle){ 30, 40, 200*1.5f, 100*1.5f }, WHITE);
                DrawText( TextFormat("NOVO JOGO"), (screenWidth/2)- 50 , screenHeight*5/10, 20, (modo == 0 ? DARKBLUE : DARKGRAY));
                DrawText( TextFormat("HIGHSCORE"), (screenWidth/2)- 50 , screenHeight*6/10, 20, (modo == 1 ? DARKBLUE : DARKGRAY));
                DrawText( TextFormat("SAIR"), (screenWidth/2)- 50 , screenHeight*7/10, 20, (modo == 2 ? DARKBLUE : DARKGRAY));
            }
            if (modo == 0 && entrar == true)
            // jogo
            {
                DrawText(TextFormat("%d", p1.ponto), 20, 380, 20, LIGHTGRAY);
                DrawText(TextFormat("%d", vidas), 750, 380, 20, GOLD);
                desenha_tijolo(tijolos);
                desenha_raquete (&p1);
                desenha_bola (&bball);
                move_bola(&bball,&p1,tijolos, &vidas);
                move_raquete (&p1);
                if (vidas==0)
                    {
                        morte(&p1);
                        return 0;
                    }
            }
            if (modo == 1 && entrar == true)
            // menu de high scores
            {
                DrawText(TextFormat("%s    %d", highscores[0].nome, highscores[0].ponto), (screenWidth/2)- 50 , screenHeight*3/10, 20, GOLD);
                DrawText(TextFormat("%s    %d", highscores[1].nome, highscores[1].ponto), (screenWidth/2)- 50 , screenHeight*4/10, 20, RAYWHITE);
                DrawText(TextFormat("%s    %d", highscores[2].nome, highscores[2].ponto), (screenWidth/2)- 50 , screenHeight*5/10, 20, BEIGE);
                DrawText(TextFormat("%s    %d", highscores[3].nome, highscores[3].ponto), (screenWidth/2)- 50 , screenHeight*6/10, 20, DARKGRAY);
                DrawText(TextFormat("%s    %d", highscores[4].nome, highscores[4].ponto), (screenWidth/2)- 50 , screenHeight*7/10, 20, DARKGRAY);
            }
            if (modo == 2 && entrar == true)
            {
                return 0;
            }

            if(fim_jogo(tijolos))
            // se fim de jogo acontecer, reinicia no nível seguinte
            {
                level++;
                nivel(level,tijolos);
                cord_tijolo(tijolos);
                vivo_tijolo(tijolos);
                pos0 (&p1);
                bola_inicial(&bball);
                vel_bola_inicial(&bball);
            } //verifica se todos os blocos foram destruidos, caso afirmativo, goto para fora do while do jogo para carregar um novo level

        EndDrawing();

    }

    CloseWindow();


    return 0;
}
