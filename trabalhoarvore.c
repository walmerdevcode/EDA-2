#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define MAXIMO_N 10000
#define AMOSTRAS 10
#define MAX_CHAVE 1000000
#define PASSO_N 10

// -----------------------------
// Tempo em segundos
// -----------------------------
static inline double tempo_segundos() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// ==============================
//             ÁRVORE AVL
// ==============================
typedef struct NoAVL {
    int chave;
    struct NoAVL *esquerda;
    struct NoAVL *direita;
    int altura;
} NoAVL;

static inline int maximo(int a, int b) { return (a > b) ? a : b; }
static inline int alturaAVL(NoAVL *no) { return no ? no->altura : 0; }

NoAVL* criarNoAVL(int chave) {
    NoAVL* no = (NoAVL*)malloc(sizeof(NoAVL));
    if (!no) { perror("malloc NoAVL"); exit(EXIT_FAILURE); }
    no->chave = chave;
    no->esquerda = no->direita = NULL;
    no->altura = 1;
    return no;
}

NoAVL* rotacionarDireitaAVL(NoAVL* y) {
    NoAVL* x = y->esquerda;
    NoAVL* T2 = x->direita;
    x->direita = y;
    y->esquerda = T2;
    y->altura = maximo(alturaAVL(y->esquerda), alturaAVL(y->direita)) + 1;
    x->altura = maximo(alturaAVL(x->esquerda), alturaAVL(x->direita)) + 1;
    return x;
}

NoAVL* rotacionarEsquerdaAVL(NoAVL* x) {
    NoAVL* y = x->direita;
    NoAVL* T2 = y->esquerda;
    y->esquerda = x;
    x->direita = T2;
    x->altura = maximo(alturaAVL(x->esquerda), alturaAVL(x->direita)) + 1;
    y->altura = maximo(alturaAVL(y->esquerda), alturaAVL(y->direita)) + 1;
    return y;
}

int balanceamentoAVL(NoAVL* no) {
    return no ? alturaAVL(no->esquerda) - alturaAVL(no->direita) : 0;
}

NoAVL* inserirAVL(NoAVL* no, int chave) {
    if (!no) return criarNoAVL(chave);

    if (chave < no->chave)
        no->esquerda = inserirAVL(no->esquerda, chave);
    else if (chave > no->chave)
        no->direita = inserirAVL(no->direita, chave);
    else
        return no;

    no->altura = maximo(alturaAVL(no->esquerda), alturaAVL(no->direita)) + 1;
    int bal = balanceamentoAVL(no);

    if (bal > 1 && chave < no->esquerda->chave)
        return rotacionarDireitaAVL(no);
    if (bal < -1 && chave > no->direita->chave)
        return rotacionarEsquerdaAVL(no);
    if (bal > 1 && chave > no->esquerda->chave) {
        no->esquerda = rotacionarEsquerdaAVL(no->esquerda);
        return rotacionarDireitaAVL(no);
    }
    if (bal < -1 && chave < no->direita->chave) {
        no->direita = rotacionarDireitaAVL(no->direita);
        return rotacionarEsquerdaAVL(no);
    }
    return no;
}

NoAVL* noMinimoAVL(NoAVL* no) {
    NoAVL* atual = no;
    while (atual && atual->esquerda) atual = atual->esquerda;
    return atual;
}

NoAVL* removerAVL(NoAVL* raiz, int chave) {
    if (!raiz) return NULL;

    if (chave < raiz->chave)
        raiz->esquerda = removerAVL(raiz->esquerda, chave);
    else if (chave > raiz->chave)
        raiz->direita = removerAVL(raiz->direita, chave);
    else {
        if (!raiz->esquerda || !raiz->direita) {
            NoAVL* temp = raiz->esquerda ? raiz->esquerda : raiz->direita;
            if (!temp) {
                free(raiz);
                return NULL;
            } else {
                NoAVL* filho = temp;
                raiz->chave = filho->chave;
                raiz->esquerda = filho->esquerda;
                raiz->direita = filho->direita;
                raiz->altura = filho->altura;
                free(filho);
            }
        } else {
            NoAVL* temp = noMinimoAVL(raiz->direita);
            raiz->chave = temp->chave;
            raiz->direita = removerAVL(raiz->direita, temp->chave);
        }
    }

    if (!raiz) return NULL;

    raiz->altura = maximo(alturaAVL(raiz->esquerda), alturaAVL(raiz->direita)) + 1;
    int bal = balanceamentoAVL(raiz);

    if (bal > 1 && balanceamentoAVL(raiz->esquerda) >= 0)
        return rotacionarDireitaAVL(raiz);
    if (bal > 1 && balanceamentoAVL(raiz->esquerda) < 0) {
        raiz->esquerda = rotacionarEsquerdaAVL(raiz->esquerda);
        return rotacionarDireitaAVL(raiz);
    }
    if (bal < -1 && balanceamentoAVL(raiz->direita) <= 0)
        return rotacionarEsquerdaAVL(raiz);
    if (bal < -1 && balanceamentoAVL(raiz->direita) > 0) {
        raiz->direita = rotacionarDireitaAVL(raiz->direita);
        return rotacionarEsquerdaAVL(raiz);
    }

    return raiz;
}

void liberarAVL(NoAVL* no) {
    if (!no) return;
    liberarAVL(no->esquerda);
    liberarAVL(no->direita);
    free(no);
}

// ==============================
//        ÁRVORE RUBRO-NEGRA 
// ==============================
typedef enum { VERMELHO, PRETO } Cor;

typedef struct NoRN {
    int chave;
    Cor cor;
    struct NoRN *esquerda, *direita, *pai;
} NoRN;

NoRN* criarNoRN(int chave) {
    NoRN* no = (NoRN*)malloc(sizeof(NoRN));
    if (!no) { perror("malloc NoRN"); exit(EXIT_FAILURE); }
    no->chave = chave;
    no->cor = VERMELHO;
    no->esquerda = no->direita = no->pai = NULL;
    return no;
}

void rot_dir_rn(NoRN **raiz, NoRN *y) {
    NoRN *x = y->esquerda;
    if (!x) return;
    y->esquerda = x->direita;
    if (x->direita) x->direita->pai = y;
    x->pai = y->pai;
    if (!y->pai) *raiz = x;
    else if (y == y->pai->esquerda) y->pai->esquerda = x;
    else y->pai->direita = x;
    x->direita = y;
    y->pai = x;
}

void rot_esq_rn(NoRN **raiz, NoRN *x) {
    NoRN *y = x->direita;
    if (!y) return;
    x->direita = y->esquerda;
    if (y->esquerda) y->esquerda->pai = x;
    y->pai = x->pai;
    if (!x->pai) *raiz = y;
    else if (x == x->pai->esquerda) x->pai->esquerda = y;
    else x->pai->direita = y;
    y->esquerda = x;
    x->pai = y;
}

void inserir_fixup_rn(NoRN **raiz, NoRN *z) {
    while (z->pai && z->pai->cor == VERMELHO) {
        NoRN *pai = z->pai;
        NoRN *avo = pai->pai;
        if (!avo) break;
        if (pai == avo->esquerda) {
            NoRN *y = avo->direita;
            if (y && y->cor == VERMELHO) {
                pai->cor = PRETO;
                y->cor = PRETO;
                avo->cor = VERMELHO;
                z = avo;
            } else {
                if (z == pai->direita) {
                    z = pai;
                    rot_esq_rn(raiz, z);
                    pai = z->pai;
                    avo = pai ? pai->pai : NULL;
                }
                if (pai) pai->cor = PRETO;
                if (avo) {
                    avo->cor = VERMELHO;
                    rot_dir_rn(raiz, avo);
                }
            }
        } else {
            NoRN *y = avo->esquerda;
            if (y && y->cor == VERMELHO) {
                pai->cor = PRETO;
                y->cor = PRETO;
                avo->cor = VERMELHO;
                z = avo;
            } else {
                if (z == pai->esquerda) {
                    z = pai;
                    rot_dir_rn(raiz, z);
                    pai = z->pai;
                    avo = pai ? pai->pai : NULL;
                }
                if (pai) pai->cor = PRETO;
                if (avo) {
                    avo->cor = VERMELHO;
                    rot_esq_rn(raiz, avo);
                }
            }
        }
    }
    if (*raiz) (*raiz)->cor = PRETO;
}

NoRN* inserirRN(NoRN* raiz, int chave) {
    NoRN *z = criarNoRN(chave);
    NoRN *y = NULL;
    NoRN *x = raiz;
    while (x) {
        y = x;
        if (chave < x->chave) x = x->esquerda;
        else if (chave > x->chave) x = x->direita;
        else { free(z); return raiz; }
    }
    z->pai = y;
    if (!y) raiz = z;
    else if (chave < y->chave) y->esquerda = z;
    else y->direita = z;
    inserir_fixup_rn(&raiz, z);
    return raiz;
}

NoRN* minimoRN(NoRN *node) {
    while (node && node->esquerda) node = node->esquerda;
    return node;
}

void transplantRN(NoRN **raiz, NoRN *u, NoRN *v) {
    if (!u->pai) *raiz = v;
    else if (u == u->pai->esquerda) u->pai->esquerda = v;
    else u->pai->direita = v;
    if (v) v->pai = u->pai;
}

void remover_fixup_rn(NoRN **raiz, NoRN *x) {
    while (x && x != *raiz && x->cor == PRETO) {
        if (x == x->pai->esquerda) {
            NoRN *w = x->pai->direita;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO; x->pai->cor = VERMELHO;
                rot_esq_rn(raiz, x->pai);
                w = x->pai->direita;
            }
            if ((!w) || ((!(w->esquerda) || w->esquerda->cor == PRETO) && (!(w->direita) || w->direita->cor == PRETO))) {
                if (w) w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (!w->direita || w->direita->cor == PRETO) {
                    if (w->esquerda) w->esquerda->cor = PRETO;
                    w->cor = VERMELHO;
                    rot_dir_rn(raiz, w);
                    w = x->pai->direita;
                }
                if (w) w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                if (w && w->direita) w->direita->cor = PRETO;
                rot_esq_rn(raiz, x->pai);
                x = *raiz;
            }
        } else {

            NoRN *w = x->pai->esquerda;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO; x->pai->cor = VERMELHO;
                rot_dir_rn(raiz, x->pai);
                w = x->pai->esquerda;
            }
            if ((!w) || ((!(w->esquerda) || w->esquerda->cor == PRETO) && (!(w->direita) || w->direita->cor == PRETO))) {
                if (w) w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (!w->esquerda || w->esquerda->cor == PRETO) {
                    if (w->direita) w->direita->cor = PRETO;
                    w->cor = VERMELHO;
                    rot_esq_rn(raiz, w);
                    w = x->pai->esquerda;
                }
                if (w) w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                if (w && w->esquerda) w->esquerda->cor = PRETO;
                rot_dir_rn(raiz, x->pai);
                x = *raiz;
            }
        }
    }
    if (x) x->cor = PRETO;
}

NoRN* removerRN(NoRN* raiz, int chave) {
    NoRN *z = raiz;
    while (z && z->chave != chave) {
        if (chave < z->chave) z = z->esquerda;
        else z = z->direita;
    }
    if (!z) return raiz;
    NoRN *y = z;
    Cor y_original_cor = y->cor;
    NoRN *x = NULL;
    if (!z->esquerda) {
        x = z->direita;
        transplantRN(&raiz, z, z->direita);
    } else if (!z->direita) {
        x = z->esquerda;
        transplantRN(&raiz, z, z->esquerda);
    } else {
        y = minimoRN(z->direita);
        y_original_cor = y->cor;
        x = y->direita;
        if (y->pai == z) {
            if (x) x->pai = y;
        } else {
            transplantRN(&raiz, y, y->direita);
            y->direita = z->direita;
            if (y->direita) y->direita->pai = y;
        }
        transplantRN(&raiz, z, y);
        y->esquerda = z->esquerda;
        if (y->esquerda) y->esquerda->pai = y;
        y->cor = z->cor;
    }
    if (y_original_cor == PRETO) remover_fixup_rn(&raiz, x);

    return raiz;
}

void liberarRN(NoRN* no) {
    if (!no) return;
    liberarRN(no->esquerda);
    liberarRN(no->direita);
    free(no);
}

// ==============================
//           ÁRVORE  B
// ==============================
typedef struct NoB {
    int n;
    int folha;
    int *chave;
    struct NoB **filho;
} NoB;

typedef struct ArvoreB {
    NoB *raiz;
    int t;
} ArvoreB;

NoB *criarNoB(int t, int folha) {
    NoB *no = (NoB*) malloc(sizeof(NoB));
    if (!no) { perror("malloc NoB"); exit(EXIT_FAILURE); }
    no->folha = folha;
    no->n = 0;
    no->chave = (int*) malloc(sizeof(int) * (2*t - 1));
    no->filho = (NoB**) malloc(sizeof(NoB*) * (2*t));
    if (!no->chave || !no->filho) { perror("malloc NoB arrays"); exit(EXIT_FAILURE); }
    for (int i = 0; i < 2*t; i++) no->filho[i] = NULL;
    return no;
}

ArvoreB *criarArvoreB(int t) {
    if (t < 2) t = 2;
    ArvoreB *arv = (ArvoreB*) malloc(sizeof(ArvoreB));
    if (!arv) { perror("malloc ArvoreB"); exit(EXIT_FAILURE); }
    arv->t = t;
    arv->raiz = criarNoB(t, 1);
    return arv;
}

void dividirFilhoB(NoB *pai, int idx, int t) {
    NoB *y = pai->filho[idx];
    NoB *z = criarNoB(t, y->folha);
    z->n = t - 1;
    for (int j = 0; j < t - 1; j++) z->chave[j] = y->chave[j + t];
    if (!y->folha) for (int j = 0; j < t; j++) z->filho[j] = y->filho[j + t];
    y->n = t - 1;
    for (int j = pai->n; j >= idx + 1; j--) pai->filho[j+1] = pai->filho[j];
    pai->filho[idx + 1] = z;
    for (int j = pai->n - 1; j >= idx; j--) pai->chave[j+1] = pai->chave[j];
    pai->chave[idx] = y->chave[t - 1];
    pai->n++;
}

void inserirNaoCheioB(NoB *no, int k, int t) {
    int i = no->n - 1;
    if (no->folha) {
        while (i >= 0 && k < no->chave[i]) {
            no->chave[i+1] = no->chave[i];
            i--;
        }
        no->chave[i+1] = k;
        no->n++;
    } else {
        while (i >= 0 && k < no->chave[i]) i--;
        i++;
        if (!no->filho[i]) no->filho[i] = criarNoB(t, 1);
        if (no->filho[i]->n == 2*t - 1) {
            dividirFilhoB(no, i, t);
            if (k > no->chave[i]) i++;
        }
        inserirNaoCheioB(no->filho[i], k, t);
    }
}

void inserirB(ArvoreB *arv, int k) {
    if (!arv) return;
    NoB *r = arv->raiz;
    if (r->n == 2*arv->t - 1) {
        NoB *s = criarNoB(arv->t, 0);
        s->filho[0] = r;
        arv->raiz = s;
        dividirFilhoB(s, 0, arv->t);
        inserirNaoCheioB(s, k, arv->t);
    } else inserirNaoCheioB(r, k, arv->t);
}


int buscarChaveB(NoB *no, int k) {
    int idx = 0;
    while (idx < no->n && no->chave[idx] < k) idx++;
    return idx;
}

int getAntecessorB(NoB *no) {
    NoB *cur = no;
    while (!cur->folha) cur = cur->filho[cur->n];
    return cur->chave[cur->n - 1];
}

int getSucessorB(NoB *no) {
    NoB *cur = no;
    while (!cur->folha) cur = cur->filho[0];
    return cur->chave[0];
}

void pegarDoAnteriorB(NoB *no, int idx, int t) {
    NoB *filho = no->filho[idx];
    NoB *irm = no->filho[idx - 1];
    for (int i = filho->n - 1; i >= 0; i--) filho->chave[i+1] = filho->chave[i];
    if (!filho->folha) for (int i = filho->n; i >= 0; i--) filho->filho[i+1] = filho->filho[i];
    filho->chave[0] = no->chave[idx - 1];
    if (!filho->folha) filho->filho[0] = irm->filho[irm->n];
    no->chave[idx - 1] = irm->chave[irm->n - 1];
    filho->n++;
    irm->n--;
}

void pegarDoProximoB(NoB *no, int idx, int t) {
    NoB *filho = no->filho[idx];
    NoB *irm = no->filho[idx + 1];
    filho->chave[filho->n] = no->chave[idx];
    if (!filho->folha) filho->filho[filho->n + 1] = irm->filho[0];
    no->chave[idx] = irm->chave[0];
    for (int i = 1; i < irm->n; i++) irm->chave[i-1] = irm->chave[i];
    if (!irm->folha) for (int i = 1; i <= irm->n; i++) irm->filho[i-1] = irm->filho[i];
    filho->n++;
    irm->n--;
}

void fundirB(NoB *no, int idx, int t) {
    NoB *filho = no->filho[idx];
    NoB *irm = no->filho[idx + 1];
    filho->chave[t - 1] = no->chave[idx];
    for (int i = 0; i < irm->n; i++) filho->chave[i + t] = irm->chave[i];
    if (!filho->folha) for (int i = 0; i <= irm->n; i++) filho->filho[i + t] = irm->filho[i];
    for (int i = idx + 1; i < no->n; i++) no->chave[i-1] = no->chave[i];
    for (int i = idx + 2; i <= no->n; i++) no->filho[i-1] = no->filho[i];
    filho->n += irm->n + 1;
    no->n--;
    free(irm->chave);
    free(irm->filho);
    free(irm);
}

void preencherB(NoB *no, int idx, int t) {
    if (idx != 0 && no->filho[idx - 1]->n >= t) pegarDoAnteriorB(no, idx, t);
    else if (idx != no->n && no->filho[idx + 1]->n >= t) pegarDoProximoB(no, idx, t);
    else {
        if (idx != no->n) fundirB(no, idx, t);
        else fundirB(no, idx - 1, t);
    }
}

void removerRecB(NoB *no, int k, int t) {
    int idx = buscarChaveB(no, k);
    if (idx < no->n && no->chave[idx] == k) {
        if (no->folha) {
            for (int i = idx; i < no->n - 1; i++) no->chave[i] = no->chave[i+1];
            no->n--;
            return;
        } else {
            if (no->filho[idx]->n >= t) {
                int pred = getAntecessorB(no->filho[idx]);
                no->chave[idx] = pred;
                removerRecB(no->filho[idx], pred, t);
            } else if (no->filho[idx+1]->n >= t) {
                int succ = getSucessorB(no->filho[idx+1]);
                no->chave[idx] = succ;
                removerRecB(no->filho[idx+1], succ, t);
            } else {
                fundirB(no, idx, t);
                removerRecB(no->filho[idx], k, t);
            }
            return;
        }
    } else {
        if (no->folha) return;
        bool ultimo = (idx == no->n);
        if (!no->filho[idx]) return;
        if (no->filho[idx]->n < t) preencherB(no, idx, t);
        if (ultimo && idx > no->n) removerRecB(no->filho[idx-1], k, t);
        else removerRecB(no->filho[idx], k, t);
    }
}

void removerB(ArvoreB *arv, int k) {
    if (!arv || !arv->raiz) return;
    removerRecB(arv->raiz, k, arv->t);
    if (arv->raiz->n == 0 && !arv->raiz->folha) {
        NoB *tmp = arv->raiz;
        arv->raiz = arv->raiz->filho[0];
        free(tmp->chave);
        free(tmp->filho);
        free(tmp);
    }
}

void liberarNoB(NoB* no) {
    if (!no) return;
    if (!no->folha) {
        for (int i = 0; i <= no->n; i++) {
            if (no->filho[i]) liberarNoB(no->filho[i]);
        }
    }
    free(no->chave);
    free(no->filho);
    free(no);
}

void liberarArvoreB(ArvoreB* arv) {
    if (!arv) return;
    if (arv->raiz) liberarNoB(arv->raiz);
    free(arv);
}

// ==============================
//       GERAÇÃO DE CHAVES
// ==============================
void gerar_chaves(int *arr, int n) {
    for (int i = 0; i < n; i++) arr[i] = rand() % MAX_CHAVE;
}

// ==============================
//            MAIN
// ==============================
int main(void) {
    srand((unsigned)time(NULL));

    printf("Iniciando benchmark: %d amostras, n = %d..%d step %d\n", AMOSTRAS, PASSO_N, MAXIMO_N, PASSO_N);
    bool avisou_t1 = false;

    FILE *fInserir = fopen("resultados_insercao.csv", "w");
    FILE *fRemover = fopen("resultados_remocao.csv", "w");
    if (!fInserir || !fRemover) { perror("abrir CSV"); return 1; }

    fprintf(fInserir, "n,AVL,RN,B1,B5,B10\n");
    fprintf(fRemover, "n,AVL,RN,B1,B5,B10\n");

    
    int *arr10000 = (int*) malloc(sizeof(int) * MAXIMO_N);
    if (!arr10000) { perror("malloc arr10000"); return 1; }

    
    int passos = MAXIMO_N / PASSO_N;
    
    for (int n = PASSO_N; n <= MAXIMO_N; n += PASSO_N) {
        double somaAVLIns = 0.0, somaRNIns = 0.0, somaB1Ins = 0.0, somaB5Ins = 0.0, somaB10Ins = 0.0;
        double somaAVLRem = 0.0, somaRNRem = 0.0, somaB1Rem = 0.0, somaB5Rem = 0.0, somaB10Rem = 0.0;

        for (int s = 0; s < AMOSTRAS; s++) {
            
            gerar_chaves(arr10000, MAXIMO_N);

            

            // -------- AVL --------
            NoAVL *raizAVL = NULL;
            for (int i = 0; i < n-1; i++) raizAVL = inserirAVL(raizAVL, arr10000[i]);
            double t0 = tempo_segundos();
            raizAVL = inserirAVL(raizAVL, arr10000[n-1]);
            double t1 = tempo_segundos();
            double dt_ins = t1 - t0;
            somaAVLIns += dt_ins;

            double t2 = tempo_segundos();
            raizAVL = removerAVL(raizAVL, arr10000[n-1]);
            double t3 = tempo_segundos();
            double dt_rem = t3 - t2;
            somaAVLRem += dt_rem;
            liberarAVL(raizAVL);

            // -------- RN --------
            NoRN *raizRN = NULL;
            for (int i = 0; i < n-1; i++) raizRN = inserirRN(raizRN, arr10000[i]);
            t0 = tempo_segundos();
            raizRN = inserirRN(raizRN, arr10000[n-1]);
            t1 = tempo_segundos();
            somaRNIns += (t1 - t0);

            t2 = tempo_segundos();
            raizRN = removerRN(raizRN, arr10000[n-1]);
            t3 = tempo_segundos();
            somaRNRem += (t3 - t2);
            liberarRN(raizRN);

            // -------- B1 
            int tparam1 = 1;
            if (tparam1 < 2) { tparam1 = 2; if (!avisou_t1) { printf("Aviso: B-tree com t=1 não é válida, ajustando para t=2\n"); avisou_t1 = true; } }
            ArvoreB *b1 = criarArvoreB(tparam1);
            for (int i = 0; i < n-1; i++) inserirB(b1, arr10000[i]);
            t0 = tempo_segundos();
            inserirB(b1, arr10000[n-1]);
            t1 = tempo_segundos();
            somaB1Ins += (t1 - t0);

            t2 = tempo_segundos();
            removerB(b1, arr10000[n-1]);
            t3 = tempo_segundos();
            somaB1Rem += (t3 - t2);
            liberarArvoreB(b1);

            // -------- B5 --------
            ArvoreB *b5 = criarArvoreB(5);
            for (int i = 0; i < n-1; i++) inserirB(b5, arr10000[i]);
            t0 = tempo_segundos();
            inserirB(b5, arr10000[n-1]);
            t1 = tempo_segundos();
            somaB5Ins += (t1 - t0);

            t2 = tempo_segundos();
            removerB(b5, arr10000[n-1]);
            t3 = tempo_segundos();
            somaB5Rem += (t3 - t2);
            liberarArvoreB(b5);

            // -------- B10 --------
            ArvoreB *b10 = criarArvoreB(10);
            for (int i = 0; i < n-1; i++) inserirB(b10, arr10000[i]);
            t0 = tempo_segundos();
            inserirB(b10, arr10000[n-1]);
            t1 = tempo_segundos();
            somaB10Ins += (t1 - t0);

            t2 = tempo_segundos();
            removerB(b10, arr10000[n-1]);
            t3 = tempo_segundos();
            somaB10Rem += (t3 - t2);
            liberarArvoreB(b10);
        } 

        
        fprintf(fInserir, "%d,%.9f,%.9f,%.9f,%.9f,%.9f\n", n,
                somaAVLIns/AMOSTRAS, somaRNIns/AMOSTRAS,
                somaB1Ins/AMOSTRAS, somaB5Ins/AMOSTRAS, somaB10Ins/AMOSTRAS);

        fprintf(fRemover, "%d,%.9f,%.9f,%.9f,%.9f,%.9f\n", n,
                somaAVLRem/AMOSTRAS, somaRNRem/AMOSTRAS,
                somaB1Rem/AMOSTRAS, somaB5Rem/AMOSTRAS, somaB10Rem/AMOSTRAS);

        printf("n=%d concluído\n", n);
        fflush(stdout);
    } 

    fclose(fInserir);
    fclose(fRemover);
    free(arr10000);

    printf("Execução completa. Arquivos gerados:\n - resultados_insercao.csv\n - resultados_remocao.csv\n");
    return 0;
}
