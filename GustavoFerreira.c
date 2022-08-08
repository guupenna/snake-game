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

// Estatisticas
typedef struct {
    int movSemPonto;
    int movBaixo;
    int movCima;
    int movEsquerda;
    int movDireita;
    int movTotal;
} tEstatistica;

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
    tEstatistica estatisticas;
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

void RealizaJogo(tJogo jogo, FILE *fsaida, FILE *fresumo, FILE *franking, FILE *festatisticas, FILE *fheatmap);

tJogo ExecutaJogada(tJogo jogo, tJogada jogada, tPosicao pos[], FILE *fresumo);

tJogo AtualizaJogo(tJogo jogo, tJogada jogada, tPosicao pos[], FILE *fresumo);

void ImprimeSaida(tJogo jogo, tJogada jogada, FILE *fsaida);

// Movimento e ações com a cobra
void MarcaPosicaoCabeca(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra MoveCabeca(tCobra cobra, tEstatistica estatisticas, tJogada jogada, tPosicao pos[]);

tCobra MantemCobraNoMapa(tCobra cobra, tMapa mapa);

tCobra MoveCorpo(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra TrocaPosicao(tCobra cobra, tPosicao posVazia, tPosicao posCorpo);

tCobra CresceCobra(tCobra cobra, tJogada jogada, tPosicao pos[]);

tCobra MataCobra(tCobra cobra, tMapa mapa);

// Resumo
void ImprimeNoResumo(FILE *fresumo, tJogada jogada, tCobra cobra, int acao);

// Heatmap
void ImprimeHeatmap(int l, int c, int heatmap[l][c], tJogada jogada, tPosicao pos[], FILE *fheatmap);

// Ranking
void ImprimeRanking(int l, int c, int heatmap[l][c], FILE *franking);

// Estatisticas
tEstatistica ObtemEstatisticas(tCobra cobra);

tCobra AdicionaMovSemPontoEstatistica (tCobra cobra, tEstatistica estatistica);

void ImprimeEstatisticas(tCobra cobra, FILE *festatisticas);

// Auxiliares
tPosicao ObtemPosicaoCabeca(tCobra cobra);

int ObtemTamanhoLinhas(tMapa mapa);

int ObtemTamanhoColunas(tMapa mapa);

tJogo PoeDentroDetJogo(tMapa mapa, tCobra cobra, tJogo jogo);

tCobra InicializaCorpo(tCobra cobra);

tCobra ZeraEstatisticas(tCobra cobra);

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
    FILE *fileMapa, *fileInicializacao, *fileResumo, *fileRanking, *fileEstatisticas, *fileHeatmap, *fileSaida;
    char diretorio[1000];
    tJogo jogo;

    // Verifica se foi inserido o nome do diretório
    if (argv[1] == NULL) {
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

    //Abrir todos os arquivos a serem utilizados para a realização do jogo
    sprintf(diretorio, "%s/saida/saida.txt", argv[1]);
    fileSaida = fopen(diretorio, "w");

    sprintf(diretorio, "%s/saida/resumo.txt", argv[1]);
    fileResumo = fopen(diretorio, "w");

    sprintf(diretorio, "%s/saida/ranking.txt", argv[1]);
    fileRanking = fopen(diretorio, "w");

    sprintf(diretorio, "%s/saida/estatisticas.txt", argv[1]);
    fileEstatisticas = fopen(diretorio, "w");

    sprintf(diretorio, "%s/saida/heatmap.txt", argv[1]);
    fileHeatmap = fopen(diretorio, "w");

    RealizaJogo(jogo, fileSaida, fileResumo, fileRanking, fileEstatisticas, fileHeatmap);

    // Fecha todos os arquivos abertos anteriormente
    fclose(fileMapa);
    fclose(fileInicializacao);
    fclose(fileResumo);
    fclose(fileRanking);
    fclose(fileEstatisticas);
    fclose(fileHeatmap);
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

    return posicao;
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

void RealizaJogo(tJogo jogo, FILE *fsaida, FILE *fresumo, FILE *franking, FILE *festatisticas, FILE *fheatmap) {
    tJogada jogada;
    tPosicao pos[TAM_MAX*TAM_MAX] = {'\0'};
    jogada.numero = 1;
    jogo.pts = 0;
    jogo.gameOver = 0;
    jogo.cobra = InicializaCorpo(jogo.cobra);
    jogo.cobra = ZeraEstatisticas(jogo.cobra);

    while (scanf("%c", &jogada.movimento) == 1) {
        scanf("%*c");
        
        jogo = ExecutaJogada(jogo, jogada, pos, fresumo);

        ImprimeSaida(jogo, jogada, fsaida);

        jogada.numero++;

        if (EhGameOver(jogo)) {
            fprintf(fsaida, "Game over!\n");
            break;
        }
        if (EhVitoria(jogo.mapa)) {
            fprintf(fsaida, "Voce venceu!\n");
            break;
        }
    }
    fprintf(fsaida, "Pontuacao final: %d\n", jogo.pts);

    MarcaPosicaoCabeca(jogo.cobra, jogada, pos);    // Para marcar a posição final da cabeça, que fez o jogo terminar

    int l, c;

    l = ObtemTamanhoLinhas(jogo.mapa);
    c = ObtemTamanhoColunas(jogo.mapa);

    int heatmap[l][c];

    ImprimeHeatmap(l, c, heatmap, jogada, pos, fheatmap);
    ImprimeRanking(l, c, heatmap, franking);

    ImprimeEstatisticas(jogo.cobra, festatisticas);
}

tCobra InicializaCorpo(tCobra cobra) {
    int i, j;

    for (i=0; i<TAM_MAX; i++) {
        for (j=0; j<TAM_MAX; j++) {
            cobra.corpo[i][j] = '\0';
        }
    }

    return cobra;
}

tCobra ZeraEstatisticas(tCobra cobra) {
    tEstatistica estatistica = {0};
    cobra.estatisticas = estatistica;

    return cobra;
}

tJogo ExecutaJogada(tJogo jogo, tJogada jogada, tPosicao pos[], FILE *fresumo) { 
    // Marcar posicao da cabeca antes do movimento, para realizar a troca de posicoes sequencial para movimentar o corpo
    MarcaPosicaoCabeca(jogo.cobra, jogada, pos);

    tEstatistica estatistica = ObtemEstatisticas(jogo.cobra);
    jogo.cobra = MoveCabeca(jogo.cobra, estatistica, jogada, pos);

    jogo.cobra = MantemCobraNoMapa(jogo.cobra, jogo.mapa);

    jogo = AtualizaJogo(jogo, jogada, pos, fresumo);

    // Primeiro movimenta a cabeça para identificar e realizar ação necessária, e depois movimenta o corpo
    if (!CresceuAgr(jogo.cobra)) {
        jogo.cobra = MoveCorpo(jogo.cobra, jogada, pos);
    }

    if (EhGameOver(jogo)) {
        jogo.cobra = MataCobra(jogo.cobra, jogo.mapa);
    }

    return jogo;
}

tEstatistica ObtemEstatisticas(tCobra cobra) {
    return cobra.estatisticas;
}

int CresceuAgr(tCobra cobra) {
    return cobra.cresceuAgr;
}

int EhGameOver(tJogo jogo) {
    return jogo.gameOver;
}

void MarcaPosicaoCabeca(tCobra cobra, tJogada jogada, tPosicao pos[]) {
    pos[jogada.numero].i = cobra.lcabeca;
    pos[jogada.numero].j = cobra.ccabeca;
}

tCobra MoveCabeca(tCobra cobra, tEstatistica estatisticas, tJogada jogada, tPosicao pos[]) {
    if (cobra.direcao == COBRA_DIREITA) {
        switch(jogada.movimento) {
            case 'c':
            cobra.ccabeca++;
            estatisticas.movDireita++;
            break;

            case 'h':
            cobra.lcabeca++;
            cobra.direcao = COBRA_BAIXO;
            estatisticas.movBaixo++;
            break;

            case 'a':
            cobra.lcabeca--;
            cobra.direcao = COBRA_CIMA;
            estatisticas.movCima++;
            break;
        }
    } else if (cobra.direcao == COBRA_ESQUERDA) {
        switch(jogada.movimento) {
            case 'c':
            cobra.ccabeca--;
            estatisticas.movEsquerda++;
            break;

            case 'h':
            cobra.lcabeca--;
            cobra.direcao = COBRA_CIMA;
            estatisticas.movCima++;
            break;

            case 'a':
            cobra.lcabeca++;
            cobra.direcao = COBRA_BAIXO;
            estatisticas.movBaixo++;
            break;
        }
    } else if (cobra.direcao == COBRA_BAIXO) {
        switch(jogada.movimento) {
            case 'c':
            cobra.lcabeca++;
            estatisticas.movBaixo++;
            break;

            case 'h':
            cobra.ccabeca--;
            cobra.direcao = COBRA_ESQUERDA;
            estatisticas.movEsquerda++;
            break;

            case 'a':
            cobra.ccabeca++;
            cobra.direcao = COBRA_DIREITA;
            estatisticas.movDireita++;
            break;
        }
    } else if (cobra.direcao == COBRA_CIMA) {
        switch(jogada.movimento) {
            case 'c':
            cobra.lcabeca--;
            estatisticas.movCima++;
            break;

            case 'h':
            cobra.ccabeca++;
            cobra.direcao = COBRA_DIREITA;
            estatisticas.movDireita++;
            break;

            case 'a':
            cobra.ccabeca--;
            cobra.direcao = COBRA_ESQUERDA;
            estatisticas.movEsquerda++;
            break;
        }
    }

    cobra.estatisticas = estatisticas;

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

tJogo AtualizaJogo(tJogo jogo, tJogada jogada, tPosicao pos[], FILE *fresumo) {
    int i, j, l, c, temAcao;

    l = ObtemTamanhoLinhas(jogo.mapa);
    c = ObtemTamanhoColunas(jogo.mapa);

    jogo.cobra = ResetaCresceuAgr(jogo.cobra);
    
    tEstatistica estatistica = ObtemEstatisticas(jogo.cobra);

    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {

            if (EhPosicaoCabeca(jogo.cobra, i, j)) {
                if (!EhPosicaoCorpo(jogo.cobra, i, j)) {
                    char mapaNaPos = RetornaMapaNaPosicao(jogo.mapa, i, j);
                    temAcao = IdentificaAcao(mapaNaPos);

                    if (!temAcao) {
                        jogo.cobra = AdicionaMovSemPontoEstatistica(jogo.cobra, estatistica);
                    }
                    if (temAcao == -1) {
                        jogo.gameOver = 1;
                        jogo.cobra = AdicionaMovSemPontoEstatistica(jogo.cobra, estatistica);
                    }
                    if (temAcao == 1) {
                        jogo.pts += 10;
                    }
                    if (temAcao == 2) {
                        jogo.pts++;
                        jogo.cobra = CresceCobra(jogo.cobra, jogada, pos);
                    }
                    // Substitui a posição no mapa que a cabeça da cobra passou, por um espaço vazio
                    char vazio = ' ';
                    jogo.mapa = PoeNoMapaNaPosicao(jogo.mapa, i, j, vazio);

                    if (EhVitoria(jogo.mapa)) {
                        temAcao = 3;
                    }

                } else {
                    jogo.cobra = AdicionaMovSemPontoEstatistica(jogo.cobra, estatistica);
                    if (!EhUltimaParteCorpo(jogo.cobra, jogada, pos)) // Verifica se a cabeca não está na posicao da ultima parte do corpo. Caso esteja, a cobra não morre, já que o corpo ainda será movido e aquela parte será liberada.
                    {
                        jogo.gameOver = 1;
                        temAcao = -1;
                    }
                }
                ImprimeNoResumo(fresumo, jogada, jogo.cobra, temAcao);
                return jogo;
            }
        }
    }
    return jogo;
}

tCobra AdicionaMovSemPontoEstatistica (tCobra cobra, tEstatistica estatistica) {
    estatistica.movSemPonto++;
    cobra.estatisticas = estatistica;

    return cobra;
}

void ImprimeNoResumo(FILE *fresumo, tJogada jogada, tCobra cobra, int acao) {
    switch (acao) {
        case -1:
        fprintf(fresumo, "Movimento %d (%c) resultou no fim de jogo por conta de colisao\n", jogada.numero, jogada.movimento);
        break;

        case 1:
        fprintf(fresumo, "Movimento %d (%c) gerou dinheiro\n", jogada.numero, jogada.movimento);
        break;

        case 2:
        fprintf(fresumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d\n", jogada.numero, jogada.movimento, cobra.tamanho);
        break;

        case 3:
        fprintf(fresumo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d, terminando o jogo\n", jogada.numero, jogada.movimento, cobra.tamanho);
        break;
    }
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
    if (sign == MAP_PAREDE) {
        return -1;
    } else if (sign == MAP_DINHEIRO) {
        return 1;
    } else if (sign == MAP_COMIDA) {
        return 2;
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

    fprintf(fsaida, "\nEstado do jogo apos o movimento '%c':\n", jogada.movimento);
    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            if (EhPosicaoCorpo(jogo.cobra, i, j)) {
                char corpoNaPosicao = RetornaCorpoNaPosicao(jogo.cobra, i, j);
                fprintf(fsaida, "%c", corpoNaPosicao);
                continue;
            }
            if (EhPosicaoCabeca(jogo.cobra, i, j)) {
                char cabeca = ObtemCabeca(jogo.cobra);
                fprintf(fsaida, "%c", cabeca);
                continue;
            }
            char mapaNaPosicao = RetornaMapaNaPosicao(jogo.mapa, i, j);
            fprintf(fsaida, "%c", mapaNaPosicao);
        }
        fprintf(fsaida, "\n");
    }
    fprintf(fsaida, "Pontuacao: %d\n", jogo.pts);
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

void ImprimeHeatmap(int l, int c, int heatmap[l][c], tJogada jogada, tPosicao pos[], FILE *fheatmap) {
    int i, j, k = 1;

    // Preenche o heatmap com '0'
    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            heatmap[i][j] = 0;
        }
    }

    // Verifica todas as posicoes que a cabeca passou em jogadas anteriores e atribui ao heatmap com a frequencia correspondente
    while ( k<=jogada.numero ) {
        i = pos[k].i;
        j = pos[k].j;
        heatmap[i][j]++;
        k++;
    }

    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            fprintf(fheatmap, "%d", heatmap[i][j]);
            if (j == c-1) {
                fprintf(fheatmap, "\n");
            } else {
                fprintf(fheatmap, " "); 
            }
        }
    }
}

void ImprimeRanking(int l, int c, int heatmap[l][c], FILE *franking) {
    int maiorValor = 0, i, j, n;

    // Marca o maior valor para ter um ponto de partida na verificação das posições mais frequentes
    for (i=0; i<l; i++) {
        for (j=0; j<c; j++) {
            if (heatmap[i][j] > maiorValor) {
                maiorValor = heatmap[i][j];
            }
        }
    }
    for (n=maiorValor; n>=1; n--)
        for (i=0; i<l; i++) {
            for (j=0; j<c; j++) {
                if (heatmap[i][j] == n) {
                    fprintf(franking, "(%d, %d) - %d\n", i, j, n);
                }
            }
        }
}

void ImprimeEstatisticas(tCobra cobra, FILE *festatisticas) {
    tEstatistica estatistica = ObtemEstatisticas(cobra);

    estatistica.movTotal = estatistica.movBaixo + estatistica.movCima + estatistica.movDireita + estatistica.movEsquerda;

    fprintf(festatisticas, "Numero de movimentos: %d\nNumero de movimentos sem pontuar: %d\nNumero de movimentos para baixo: %d\nNumero de movimentos para cima: %d\nNumero de movimentos para esquerda: %d\nNumero de movimentos para direita: %d\n", estatistica.movTotal, estatistica.movSemPonto, estatistica.movBaixo, estatistica.movCima, estatistica.movEsquerda, estatistica.movDireita);
}
