/* ================================================================
   SISTEMA CRUD DE PESSOAS - TRABALHO ACADEMICO
   Disciplina: Engenharia de Software
   Descricao : Sistema de cadastro com armazenamento em arquivo
   Dados     : Nome, Data de Nascimento e Telefone
   Linguagem : C (padrao C99)
   Compilador: GCC
   Como compilar: gcc crud_pessoas.c -o crud_pessoas
   Como executar : ./crud_pessoas  (Linux/Mac)
                   crud_pessoas.exe (Windows)
   ================================================================ */

/* ----------------------------------------------------------------
   BIBLIOTECAS UTILIZADAS
   stdio.h  -> funcoes de entrada/saida (printf, scanf, fopen...)
   stdlib.h -> funcoes gerais (system, exit...)
   string.h -> funcoes de texto (strcmp, strcpy, strlen...)
   ---------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------
   CONSTANTES
   Define tamanhos fixos para os campos de texto.
   Usar constantes evita numeros "magicos" espalhados pelo codigo.
   ---------------------------------------------------------------- */
#define TAM_NOME       60
#define TAM_DATA       11   /* DD/MM/AAAA = 10 chars + barra + nulo */
#define TAM_TELEFONE   20
#define ARQUIVO        "pessoas.txt"   /* Nome do arquivo de dados  */

/* ================================================================
   ESTRUTURA DE DADOS (struct)

   A struct Pessoa agrupa todos os campos de uma pessoa em um
   unico tipo. Assim, podemos passar uma "Pessoa" inteira para
   funcoes, salvar no arquivo, etc.
   ================================================================ */
typedef struct {
    int  id;                      /* Identificador unico de cada pessoa */
    char nome[TAM_NOME];          /* Nome completo                      */
    char dataNascimento[TAM_DATA + 1];/* Data no formato DD/MM/AAAA         */
    char telefone[TAM_TELEFONE];  /* Numero de telefone                 */
} Pessoa;

/* ================================================================
   DECLARACAO DAS FUNCOES (prototipos)
   Boa pratica: declarar antes de usar, assim o compilador conhece
   as funcoes mesmo que elas apareçam depois do main().
   ================================================================ */
void cadastrarPessoa();
void listarPessoas();
void atualizarPessoa();
void excluirPessoa();
int  proximoId();
void limparBuffer();
void pausar();

/* ================================================================
   FUNCAO PRINCIPAL - main()

   Ponto de entrada do programa. Exibe o menu e chama a funcao
   correspondente a escolha do usuario em loop, ate ele sair.
   ================================================================ */
int main() {
    int opcao;

    /* Loop principal: continua ate o usuario escolher sair (opcao 5) */
    do {
        /* Limpa a tela para melhor visualizacao */
        system("cls || clear");

        /* Exibe o menu de opcoes */
        printf("========================================\n");
        printf("     SISTEMA DE CADASTRO DE PESSOAS     \n");
        printf("========================================\n");
        printf("  1. Cadastrar nova pessoa\n");
        printf("  2. Listar todas as pessoas\n");
        printf("  3. Atualizar dados de uma pessoa\n");
        printf("  4. Excluir uma pessoa\n");
        printf("  5. Sair\n");
        printf("========================================\n");
        printf("  Digite a opcao desejada: ");
        scanf("%d", &opcao);
        limparBuffer(); /* Limpa o '\n' que sobra no buffer do teclado */

        /* Direciona para a funcao certa conforme a opcao */
        switch (opcao) {
            case 1:
                cadastrarPessoa();
                break;
            case 2:
                listarPessoas();
                break;
            case 3:
                atualizarPessoa();
                break;
            case 4:
                excluirPessoa();
                break;
            case 5:
                printf("\nSaindo do sistema. Ate logo!\n\n");
                break;
            default:
                printf("\nOpcao invalida! Tente novamente.\n");
                pausar();
        }

    } while (opcao != 5);

    return 0;
}

/* ================================================================
   CREATE - cadastrarPessoa()

   Solicita os dados da nova pessoa ao usuario, gera um ID unico
   automaticamente e salva o registro no final do arquivo de texto.

   Como funciona o arquivo:
   - Abrimos com "a" (append = adicionar ao final).
   - Cada pessoa ocupa uma linha no formato:
     id|nome|dataNascimento|telefone
   - O separador "|" facilita a leitura depois com fscanf.
   ================================================================ */
void cadastrarPessoa() {
    Pessoa p;           /* Cria uma variavel do tipo Pessoa */
    FILE   *arquivo;    /* Ponteiro para o arquivo          */

    system("cls || clear");
    printf("========================================\n");
    printf("         CADASTRAR NOVA PESSOA          \n");
    printf("========================================\n");

    /* Leitura dos dados digitados pelo usuario */
    printf("Nome completo    : ");
    fgets(p.nome, TAM_NOME, stdin);
    p.nome[strcspn(p.nome, "\n")] = '\0'; /* Remove o '\n' do final */

    printf("Data nascimento  : ");
    fgets(p.dataNascimento, TAM_DATA + 1, stdin);
    p.dataNascimento[strcspn(p.dataNascimento, "\n")] = '\0';
    limparBuffer();

    printf("Telefone         : ");
    fgets(p.telefone, TAM_TELEFONE, stdin);
    p.telefone[strcspn(p.telefone, "\n")] = '\0';

    /* Gera o proximo ID disponivel */
    p.id = proximoId();

    /* Abre o arquivo em modo "append" (adiciona ao final sem apagar) */
    arquivo = fopen(ARQUIVO, "a");
    if (arquivo == NULL) {
        printf("\nERRO: Nao foi possivel abrir o arquivo!\n");
        pausar();
        return;
    }

    /* Salva a pessoa no arquivo, cada campo separado por "|" */
    fprintf(arquivo, "%d|%s|%s|%s\n",
            p.id, p.nome, p.dataNascimento, p.telefone);

    fclose(arquivo); /* Fecha o arquivo - IMPORTANTE sempre fechar! */

    printf("\n>>> Pessoa cadastrada com sucesso! ID: %d <<<\n", p.id);
    pausar();
}

/* ================================================================
   READ - listarPessoas()

   Le o arquivo linha por linha e exibe todos os registros na tela.

   Como funciona:
   - Abrimos com "r" (read = somente leitura).
   - fscanf le cada campo separado por "|".
   - Se o arquivo nao existir ou estiver vazio, avisa o usuario.
   ================================================================ */
void listarPessoas() {
    Pessoa p;        /* Variavel temporaria para ler cada registro */
    FILE   *arquivo;
    int     encontrou = 0; /* Controla se encontrou ao menos 1 registro */

    system("cls || clear");
    printf("========================================\n");
    printf("          LISTA DE PESSOAS              \n");
    printf("========================================\n");

    /* Abre o arquivo em modo leitura */
    arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        printf("Nenhuma pessoa cadastrada ainda.\n");
        pausar();
        return;
    }

    /* Le registro por registro ate chegar no fim do arquivo (EOF) */
    while (fscanf(arquivo, "%d|%59[^|]|%10[^|]|%19[^\n]\n",
                  &p.id, p.nome, p.dataNascimento, p.telefone) == 4) {

        /* Exibe os dados de cada pessoa formatados */
        printf("\n  ID          : %d\n",  p.id);
        printf("  Nome        : %s\n",   p.nome);
        printf("  Nascimento  : %s\n",   p.dataNascimento);
        printf("  Telefone    : %s\n",   p.telefone);
        printf("  ----------------------------------------\n");
        encontrou = 1;
    }

    fclose(arquivo);

    if (!encontrou) {
        printf("Nenhuma pessoa cadastrada ainda.\n");
    }

    pausar();
}

/* ================================================================
   UPDATE - atualizarPessoa()

   Busca a pessoa pelo ID informado, permite alterar os dados
   e reescreve o arquivo inteiro com a alteracao aplicada.

   Estrategia de atualizacao em arquivo texto:
   - Nao e possivel editar uma linha no meio de um arquivo texto
     sem reescrever tudo. Por isso:
     1. Lemos todos os registros para um vetor em memoria.
     2. Alteramos o registro correto no vetor.
     3. Reescrevemos o arquivo inteiro do zero com "w".
   ================================================================ */
void atualizarPessoa() {
    Pessoa  registros[200]; /* Vetor para armazenar ate 200 pessoas em RAM */
    Pessoa  p;
    FILE   *arquivo;
    int     total    = 0;  /* Quantos registros foram lidos               */
    int     idBusca  = 0;  /* ID que o usuario quer atualizar             */
    int     achado   = 0;  /* Indica se o ID foi encontrado               */
    int     i;

    system("cls || clear");
    printf("========================================\n");
    printf("         ATUALIZAR PESSOA               \n");
    printf("========================================\n");

    printf("Digite o ID da pessoa a atualizar: ");
    scanf("%d", &idBusca);
    limparBuffer();

    /* Passo 1: Le todos os registros do arquivo para o vetor */
    arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        printf("\nNenhuma pessoa cadastrada ainda.\n");
        pausar();
        return;
    }

    while (fscanf(arquivo, "%d|%59[^|]|%10[^|]|%19[^\n]\n",
                  &p.id, p.nome, p.dataNascimento, p.telefone) == 4) {
        registros[total] = p; /* Copia o registro para o vetor */
        total++;
    }
    fclose(arquivo);

    /* Passo 2: Procura o ID no vetor e atualiza se encontrar */
    for (i = 0; i < total; i++) {
        if (registros[i].id == idBusca) {

            /* Exibe dados atuais para referencia */
            printf("\n  Dados atuais:\n");
            printf("  Nome        : %s\n", registros[i].nome);
            printf("  Nascimento  : %s\n", registros[i].dataNascimento);
            printf("  Telefone    : %s\n", registros[i].telefone);
            printf("\n  Digite os novos dados:\n");

            /* Leitura dos novos dados */
            printf("  Novo nome        : ");
            fgets(registros[i].nome, TAM_NOME, stdin);
            registros[i].nome[strcspn(registros[i].nome, "\n")] = '\0';

            printf("  Novo nascimento  : ");
            fgets(registros[i].dataNascimento, TAM_DATA + 1, stdin);
            registros[i].dataNascimento[strcspn(registros[i].dataNascimento, "\n")] = '\0';
            limparBuffer();

            printf("  Novo telefone    : ");
            fgets(registros[i].telefone, TAM_TELEFONE, stdin);
            registros[i].telefone[strcspn(registros[i].telefone, "\n")] = '\0';

            achado = 1;
            break; /* Para de procurar apos encontrar */
        }
    }

    if (!achado) {
        printf("\nID %d nao encontrado!\n", idBusca);
        pausar();
        return;
    }

    /* Passo 3: Reescreve o arquivo inteiro com os dados atualizados */
    arquivo = fopen(ARQUIVO, "w"); /* "w" apaga e recria o arquivo */
    if (arquivo == NULL) {
        printf("\nERRO ao salvar alteracoes!\n");
        pausar();
        return;
    }

    for (i = 0; i < total; i++) {
        fprintf(arquivo, "%d|%s|%s|%s\n",
                registros[i].id,
                registros[i].nome,
                registros[i].dataNascimento,
                registros[i].telefone);
    }

    fclose(arquivo);

    printf("\n>>> Dados atualizados com sucesso! <<<\n");
    pausar();
}

/* ================================================================
   DELETE - excluirPessoa()

   Remove um registro do arquivo pelo ID. A logica e a mesma do
   UPDATE: carrega tudo em memoria, ignora o registro a excluir
   e reescreve o arquivo sem ele.
   ================================================================ */
void excluirPessoa() {
    Pessoa  registros[200];
    Pessoa  p;
    FILE   *arquivo;
    int     total   = 0;
    int     idBusca = 0;
    int     achado  = 0;
    int     i;
    char    confirmacao[4];

    system("cls || clear");
    printf("========================================\n");
    printf("           EXCLUIR PESSOA               \n");
    printf("========================================\n");

    printf("Digite o ID da pessoa a excluir: ");
    scanf("%d", &idBusca);
    limparBuffer();

    /* Le todos os registros para o vetor */
    arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        printf("\nNenhuma pessoa cadastrada ainda.\n");
        pausar();
        return;
    }

    while (fscanf(arquivo, "%d|%59[^|]|%10[^|]|%19[^\n]\n",
                  &p.id, p.nome, p.dataNascimento, p.telefone) == 4) {
        registros[total] = p;
        total++;
    }
    fclose(arquivo);

    /* Verifica se o ID existe e exibe os dados para confirmacao */
    for (i = 0; i < total; i++) {
        if (registros[i].id == idBusca) {
            printf("\n  Pessoa encontrada:\n");
            printf("  Nome        : %s\n", registros[i].nome);
            printf("  Nascimento  : %s\n", registros[i].dataNascimento);
            printf("  Telefone    : %s\n", registros[i].telefone);

            printf("\n  Confirma exclusao? (s/n): ");
            fgets(confirmacao, sizeof(confirmacao), stdin);

            /* So exclui se o usuario digitar 's' ou 'S' */
            if (confirmacao[0] != 's' && confirmacao[0] != 'S') {
                printf("\nOperacao cancelada.\n");
                pausar();
                return;
            }

            achado = 1;
            break;
        }
    }

    if (!achado) {
        printf("\nID %d nao encontrado!\n", idBusca);
        pausar();
        return;
    }

    /* Reescreve o arquivo PULANDO o registro excluido */
    arquivo = fopen(ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("\nERRO ao atualizar o arquivo!\n");
        pausar();
        return;
    }

    for (i = 0; i < total; i++) {
        /* Se o ID for o excluido, simplesmente nao escreve */
        if (registros[i].id != idBusca) {
            fprintf(arquivo, "%d|%s|%s|%s\n",
                    registros[i].id,
                    registros[i].nome,
                    registros[i].dataNascimento,
                    registros[i].telefone);
        }
    }

    fclose(arquivo);

    printf("\n>>> Pessoa excluida com sucesso! <<<\n");
    pausar();
}

/* ================================================================
   FUNCAO AUXILIAR - proximoId()

   Le o arquivo inteiro e retorna o maior ID encontrado + 1.
   Garante que cada novo cadastro receba um ID unico e crescente,
   mesmo que o programa seja fechado e reaberto.
   ================================================================ */
int proximoId() {
    Pessoa p;
    FILE  *arquivo;
    int    maiorId = 0; /* Se o arquivo estiver vazio, começa em 0 */

    arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        return 1; /* Primeiro ID sera 1 */
    }

    /* Percorre todos os registros procurando o maior ID */
    while (fscanf(arquivo, "%d|%59[^|]|%10[^|]|%19[^\n]\n",
                  &p.id, p.nome, p.dataNascimento, p.telefone) == 4) {
        if (p.id > maiorId) {
            maiorId = p.id;
        }
    }

    fclose(arquivo);

    return maiorId + 1; /* Proximo ID = maior encontrado + 1 */
}

/* ================================================================
   FUNCAO AUXILIAR - limparBuffer()

   Descarta todos os caracteres que sobraram no buffer do teclado
   apos uma leitura com scanf. Sem isso, o proximo fgets pode ler
   o '\n' que sobrou em vez de esperar o usuario digitar algo.
   ================================================================ */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ================================================================
   FUNCAO AUXILIAR - pausar()

   Exibe uma mensagem e espera o usuario pressionar Enter antes de
   voltar ao menu. Melhora a experiencia porque o usuario tem tempo
   de ler o resultado da operacao.
   ================================================================ */
void pausar() {
    printf("\nPressione ENTER para voltar ao menu...");
    getchar();
}
