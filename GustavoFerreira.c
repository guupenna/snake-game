#include <stdio.h>
#include <stdlib.h>

#define TAM_MAX 100

// Mapa
#define MAP_COMIDA '*'
#define MAP_PAREDE '#'
#define MAP_DINHEIRO '$'

typedef struct {
    int tamLinhas;
    int tamColunas;
    char posicoes[TAM_MAX][TAM_MAX];
} tMapa;

// Cobra
#define COBRA_DIREITA '>'
#define COBRA_ESQUERDA '<'
#define COBRA_CIMA '^'
#define COBRA_BAIXO 'v'
#define COBRA_CORPO 'o'
#define COBRA_MORTE 'X'

typedef struct {
    int lcabeca;
    int ccabeca;
    char corpo[TAM_MAX][TAM_MAX];
    int tamanho;
    int cresceuAgr;
    char direcao;
} tCobra;

// Jogo
typedef struct {
    tMapa mapa;
    tCobra cobra;
    int pts;
    int gameOver;
} tJogo;

// Inicialização
tJogo InicializaJogo(tJogo jogo, FILE *fmapa, FILE *finicializacao);

tMapa LeTamanhoMapa(tMapa mapa, FILE *fmapa);

tJogo IdentificaMapaECobra(tMapa mapa, tCobra cobra, FILE *fmapa);

// Execução jogo
typedef struct {
    int i;
    int j;
} tPosicao;     // Utilizado para marcar a posicao da cabeça e das partes do corpo

typedef struct {
    char movimento;
    int numero;
} tJogada;      // Utilizado para mapear e montar histórico de jogadas

void RealizaJogo(tJogo jogo, FILE *fsaida);

tJogo ExecutaJogada(tJogo jogo, tJogada jogada, tPosicao pos[]);

tJogo AtualizaJogo(tJogo jogo, tJogada jogada, tPosicao pos[]);

void ImprimeSaida(tJogo jogo, tJogada jogada, FILE *fsaida);

// Movimento e ações com a cobra
void MarcaPosAnteriorCabeca(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra MoveCabeca(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra MantemCobraNoMapa(tCobra cobra, tMapa mapa);

tCobra MoveCorpo(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra TrocaPosicao(tCobra cobra, tPosicao posVazia, tPosicao posCorpo);

tCobra CresceCobra(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra MataCobra(tCobra cobra, tMapa mapa);

// Auxiliares

tPosicao ObtemPosicaoCabeca(tCobra cobra);

int ObtemTamanhoLinhas(tMapa mapa);

int ObtemTamanhoColunas(tMapa mapa);

tJogo PoeDentroDetJogo(tMapa mapa, tCobra cobra, tJogo jogo);

int CresceuAgr(tCobra cobra);

int EhGameOver(tJogo jogo);

int EhPosicaoCabeca(tCobra cobra, int i, int j);

int EhPosicaoCorpo(tCobra cobra, int i, int j);

int IdentificaAcao(char sign);

tCobra ResetaCresceuAgr(tCobra cobra);

tMapa PoeNoMapaNaPosicao(tMapa mapa, int i, int j, char c);

int EhUltimaParteCorpo(tCobra cobra, tJogada jogada, tPosicao pos[]);

char ObtemCabeca(tCobra cobra);

char RetornaCorpoNaPosicao(tCobra cobra, int i, int j);

char RetornaMapaNaPosicao(tMapa mapa, int i, int j);

int EhVitoria(tMapa mapa);

int main(int argc, char * argv[]) {
    FILE *fileMapa, *fileInicializacao, *fileSaida;
    char diretorio[1000];
    tJogo jogo;

    // Verifica se foi inserido o nome do diretório
    if (argv[1]==NULL) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 0;
    }
    
    // Abre o arquivo que contem o mapa
    sprintf(diretorio, "%s/mapa.txt", argv[1]);
    fileMapa = fopen(diretorio, "r");

    if (fileMapa == NULL) {
        printf("Nao foi possivel ler o arquivo que possui esse caminho: %s\n", diretorio);
        return 0;
    }

    // Cria e abre o arquivo (no diretorio "saida/") e realiza a inicialização do jogo
    sprintf(diretorio, "%s/saida/inicializacao.txt", argv[1]);
    fileInicializacao = fopen(diretorio, "w");

    jogo = InicializaJogo(jogo, fileMapa, fileInicializacao);

    //
    sprintf(diretorio, "%s/saida/saida.txt", argv[1]);
    fileSaida = fopen(diretorio, "w");

    RealizaJogo(jogo, fileSaida);

    fclose(fileMapa);
    fclose(fileInicializacao);
    fclose(fileSaida);

    return 0;
}

tJogo InicializaJogo(tJogo jogo, FILE *fmapa, FILE *finicializacao) {

    jogo.mapa = LeTamanhoMapa(jogo.mapa, fmapa);

    jogo = IdentificaMapaECobra(jogo.mapa, jogo.cobra, fmapa);

    int i, j, l, c;
    l = ObtemTamanhoLinhas(jogo.mapa);
    c = ObtemTamanhoColunas(jogo.mapa);

    // Imprime mapa e cobra
    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            if (EhPosicaoCabeca(jogo.cobra, i, j)) {
                char cabeca = ObtemCabeca(jogo.cobra);
                fprintf(finicializacao, "%c", cabeca);
                continue;
            }
            char mapaNaPos = RetornaMapaNaPosicao(jogo.mapa, i, j);
            fprintf(finicializacao, "%c", mapaNaPos);
        }
        fprintf(finicializacao, "\n");
    }

    tPosicao posicao = ObtemPosicaoCabeca(jogo.cobra);
    fprintf(finicializacao, "A cobra comecara o jogo na linha %d e coluna %d\n", posicao.i+1, posicao.j+1);

    return jogo;
}

tPosicao ObtemPosicaoCabeca(tCobra cobra) {
    tPosicao posicao;

    posicao.i = cobra.lcabeca;
    posicao.j = cobra.ccabeca;
}

tMapa LeTamanhoMapa(tMapa mapa, FILE *fmapa) {
    fscanf(fmapa, "%d %d", &mapa.tamLinhas, &mapa.tamColunas);
    fscanf(fmapa, "%*[\n]");

    return mapa;
}

tJogo IdentificaMapaECobra(tMapa mapa, tCobra cobra, FILE *fmapa) {
    int i, j, l, c;
    char aux;

    l = ObtemTamanhoLinhas(mapa);
    c = ObtemTamanhoColunas(mapa);

    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            fscanf(fmapa, "%c", &aux);
            if (aux == COBRA_DIREITA || aux == COBRA_ESQUERDA || aux == COBRA_CIMA || aux == COBRA_BAIXO) {
                mapa.posicoes[i][j] = ' ';
                cobra.lcabeca = i;
                cobra.ccabeca = j;
                cobra.direcao = aux;
                cobra.tamanho = 1;
            } else {
                mapa.posicoes[i][j] = aux;
            }
        }
        fscanf(fmapa, "%*[\n]");
    }
    
    tJogo jogo = PoeDentroDetJogo(mapa, cobra, jogo);

    return jogo;
}

int ObtemTamanhoLinhas(tMapa mapa) {
    return mapa.tamLinhas;
}

int ObtemTamanhoColunas(tMapa mapa) {
    return mapa.tamColunas;
}

tJogo PoeDentroDetJogo(tMapa mapa, tCobra cobra, tJogo jogo) {
    jogo.mapa = mapa;
    jogo.cobra = cobra;

    return jogo;
}

void RealizaJogo(tJogo jogo, FILE *fsaida) {
    tJogada jogada;
    tPosicao pos[1000] = {'\0'};
    jogada.numero = 0;
    jogo.pts = 0;
    jogo.gameOver = 0;

    while (scanf("%c", &jogada.movimento) == 1) {
        scanf("%*c");
        
        jogo = ExecutaJogada(jogo, jogada, pos);

        ImprimeSaida(jogo, jogada, fsaida);

        if (EhGameOver(jogo)) {
            printf("Game over!\n");
            break;
        }
        if (EhVitoria(jogo.mapa)) {
            printf("Voce venceu!\n");
            break;
        }

        jogada.numero++;
    }
    printf("Pontuacao final: %d\n", jogo.pts);
}

tJogo ExecutaJogada(tJogo jogo, tJogada jogada, tPosicao pos[]) { 
    // Marcar posicao da cabeca antes do movimento, para realizar a troca de posicoes sequencial para movimentar o corpo
    MarcaPosAnteriorCabeca(jogo.cobra, jogada, pos);

    jogo.cobra = MoveCabeca(jogo.cobra, jogada, pos);

    jogo.cobra = MantemCobraNoMapa(jogo.cobra, jogo.mapa);

    jogo = AtualizaJogo(jogo, jogada, pos);

    // Primeiro movimenta a cabeça para identificar e realizar ação necessária, e depois movimenta o corpo
    if (!CresceuAgr(jogo.cobra)) {
        jogo.cobra = MoveCorpo(jogo.cobra, jogada, pos);
    }

    if (EhGameOver(jogo)) {
        jogo.cobra = MataCobra(jogo.cobra, jogo.mapa);
    }

    return jogo;
}

int CresceuAgr(tCobra cobra) {
    return cobra.cresceuAgr;
}

int EhGameOver(tJogo jogo) {
    return jogo.gameOver;
}

void MarcaPosAnteriorCabeca(tCobra cobra, tJogada jogada, tPosicao pos[]) {
    pos[jogada.numero].i=cobra.lcabeca;
    pos[jogada.numero].j=cobra.ccabeca;
}

tCobra MoveCabeca(tCobra cobra, tJogada jogada, tPosicao pos[]) {
    if (cobra.direcao == COBRA_DIREITA) {
        switch(jogada.movimento) {
            case 'c':
            cobra.ccabeca++;
            break;

            case 'h':
            cobra.lcabeca++;
            cobra.direcao = COBRA_BAIXO;
            break;

            case 'a':
            cobra.lcabeca--;
            cobra.direcao = COBRA_CIMA;
            break;
        }
    } else if (cobra.direcao == COBRA_ESQUERDA) {
        switch(jogada.movimento) {
            case 'c':
            cobra.ccabeca--;
            break;

            case 'h':
            cobra.lcabeca--;
            cobra.direcao = COBRA_CIMA;
            break;

            case 'a':
            cobra.lcabeca++;
            cobra.direcao = COBRA_BAIXO;
            break;
        }
    } else if (cobra.direcao == COBRA_BAIXO) {
        switch(jogada.movimento) {
            case 'c':
            cobra.lcabeca++;
            break;

            case 'h':
            cobra.ccabeca--;
            cobra.direcao = COBRA_ESQUERDA;
            break;

            case 'a':
            cobra.ccabeca++;
            cobra.direcao = COBRA_DIREITA;
            break;
        }
    } else if (cobra.direcao == COBRA_CIMA) {
        switch(jogada.movimento) {
            case 'c':
            cobra.lcabeca--;
            break;

            case 'h':
            cobra.ccabeca++;
            cobra.direcao = COBRA_DIREITA;
            break;

            case 'a':
            cobra.ccabeca--;
            cobra.direcao = COBRA_ESQUERDA;
            break;
        }
    }

    return cobra;
}

tCobra MantemCobraNoMapa(tCobra cobra, tMapa mapa) {
    if (cobra.lcabeca >= mapa.tamLinhas) {
        cobra.lcabeca -= mapa.tamLinhas;
    }
    if (cobra.lcabeca < 0) {
        cobra.lcabeca += mapa.tamLinhas;
    }
    if (cobra.ccabeca >= mapa.tamColunas) {
        cobra.ccabeca -= mapa.tamColunas;
    }
    if (cobra.ccabeca < 0) {
        cobra.ccabeca += mapa.tamColunas;
    }

    return cobra;
}

tJogo AtualizaJogo(tJogo jogo, tJogada jogada, tPosicao pos[]) {
    int i, j, l, c, temAcao;

    l = ObtemTamanhoLinhas(jogo.mapa);
    c = ObtemTamanhoColunas(jogo.mapa);

    jogo.cobra = ResetaCresceuAgr(jogo.cobra);

    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {

            if (EhPosicaoCabeca(jogo.cobra, i, j)) {
                if (!EhPosicaoCorpo(jogo.cobra, i, j)) {
                    char mapaNaPos = RetornaMapaNaPosicao(jogo.mapa, i, j);
                    temAcao = IdentificaAcao(mapaNaPos);

                    if (temAcao == 1) {
                        jogo.cobra = CresceCobra(jogo.cobra, jogada, pos);
                        jogo.pts++;
                    }
                    if (temAcao == 2) {
                        jogo.pts+=10;
                    }
                    if (temAcao == -1) {
                        jogo.gameOver = 1;
                    }
                    char vazio = ' ';
                    jogo.mapa = PoeNoMapaNaPosicao(jogo.mapa, i, j, vazio);
                } else if (!EhUltimaParteCorpo(jogo.cobra, jogada, pos)) // Verifica se a cabeca não está na posicao da ultima parte do corpo. Caso esteja, a cobra não morre, já que o corpo ainda será movido e aquela parte será liberada.
                    {
                        jogo.gameOver = 1;
                    }
                return jogo;
            }
        }
    }
    return jogo;
}

tCobra ResetaCresceuAgr(tCobra cobra) {
    cobra.cresceuAgr = 0;
    
    return cobra;
}

tMapa PoeNoMapaNaPosicao(tMapa mapa, int i, int j, char c) {
    mapa.posicoes[i][j] = c;

    return mapa;
}

int EhUltimaParteCorpo(tCobra cobra, tJogada jogada, tPosicao pos[]) {
    tPosicao ultimaPtCorpo = pos[jogada.numero-cobra.tamanho+1];
    // O +1 serve para corrigir a posição, já que a cabeça da cobra tambem conta como 1 de tamanho

    if (cobra.lcabeca == ultimaPtCorpo.i && cobra.ccabeca == ultimaPtCorpo.j) { // Se a cabeça da cobra estiver na posição da ultima parte do corpo é verdadeiro
        return 1;
    }
    return 0;
}

int EhPosicaoCabeca(tCobra cobra, int i, int j) {
    return i == cobra.lcabeca && j == cobra.ccabeca;
}

int EhPosicaoCorpo(tCobra cobra, int i, int j) {
    return cobra.corpo[i][j] == COBRA_CORPO || cobra.corpo[i][j] == COBRA_MORTE;
}

int IdentificaAcao(char sign) {
    if (sign == MAP_COMIDA) {
        return 1;
    } else if (sign == MAP_DINHEIRO) {
        return 2;
    } else if (sign == MAP_PAREDE || sign == COBRA_CORPO) {
        return -1;
    } else {
        return 0;
    }
}

tCobra CresceCobra(tCobra cobra, tJogada jogada, tPosicao pos[]) {
    tPosicao ultimaPosicao = pos[jogada.numero];

    cobra.corpo[ultimaPosicao.i][ultimaPosicao.j] = COBRA_CORPO;

    cobra.tamanho++;
    cobra.cresceuAgr = 1;

    return cobra;
}

tCobra MoveCorpo(tCobra cobra, tJogada jogada, tPosicao pos[]) {
    int parteCorpo = cobra.tamanho-1;
    tPosicao posCorpo;
    tPosicao posVazia;

    while ( parteCorpo ) {    // Até a cabeça
        posVazia = pos[jogada.numero]; // Posicao vazia onde tenho que mandar o corpo anterior
        posCorpo = pos[jogada.numero-1]; // Identifica a posicao do corpo olhando a posicao da cabeça na jogada anterior
        cobra = TrocaPosicao(cobra, posVazia, posCorpo); // Realiza a troca de posicoes da vazia com o corpo;

        jogada.numero--;
        parteCorpo--;
    }

    return cobra;
}

tCobra TrocaPosicao(tCobra cobra, tPosicao posVazia, tPosicao posCorpo) {
    char vazio;

    vazio = cobra.corpo[posVazia.i][posVazia.j];
    cobra.corpo[posVazia.i][posVazia.j] = COBRA_CORPO;
    cobra.corpo[posCorpo.i][posCorpo.j] = vazio;

    return cobra;
}

void ImprimeSaida(tJogo jogo, tJogada jogada, FILE *fsaida) {
    int i, j, l, c;

    l = ObtemTamanhoLinhas(jogo.mapa);
    c = ObtemTamanhoColunas(jogo.mapa);

    printf("\nEstado do jogo apos o movimento '%c':\n", jogada.movimento);
    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            if (EhPosicaoCorpo(jogo.cobra, i, j)) {
                int corpoNaPosicao = RetornaCorpoNaPosicao(jogo.cobra, i, j);
                printf("%c", corpoNaPosicao);
                continue;
            }
            if (EhPosicaoCabeca(jogo.cobra, i, j)) {
                char cabeca = ObtemCabeca(jogo.cobra);
                printf("%c", cabeca);
                continue;
            }
            char mapaNaPosicao = RetornaMapaNaPosicao(jogo.mapa, i, j);
            printf("%c", mapaNaPosicao);
        }
        printf("\n");
    }
    printf("Pontuacao: %d\n", jogo.pts);
}

char ObtemCabeca(tCobra cobra) {
    return cobra.direcao;
}

char RetornaCorpoNaPosicao(tCobra cobra, int i, int j) {
    return cobra.corpo[i][j];
}

char RetornaMapaNaPosicao(tMapa mapa, int i, int j) {
    return mapa.posicoes[i][j];
}

int EhVitoria(tMapa mapa) {
    int i, j, l, c;

    l = ObtemTamanhoLinhas(mapa);
    c = ObtemTamanhoColunas(mapa);

    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            if (mapa.posicoes[i][j] == MAP_COMIDA) {
                return 0;
            }
        }
    }
    return 1;
}

tCobra MataCobra(tCobra cobra, tMapa mapa) {
    int i, j, l, c;

    l = ObtemTamanhoLinhas(mapa);
    c = ObtemTamanhoColunas(mapa);

    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            if ((cobra.corpo[i][j] == COBRA_CORPO)) {
                cobra.corpo[i][j] = COBRA_MORTE;
            }
            if (i == cobra.lcabeca && j == cobra.ccabeca) {
                cobra.direcao = COBRA_MORTE;
            }
        }
    }

    return cobra;
}