#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <chrono>
#include <thread>
#include "avl.h"
#include "huffman.h"

using namespace std;

#define tempo 200

// FUNCOES SECUNDARIAS
void limpa_tela(){ 
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif
}

void limpa_buffer() {  // limpa buffer de entrada
    cin.clear();               
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void entrada(int ini, int fim, int *opcao){  // verifica se a entrada esta dentro do intervalo
    int a;
    cout << "Digite uma opção: ";
    cin >> a;
    while (a < ini || a > fim){
        cout << "Digite nomamente, intervalo aceito [" << ini << ',' << fim << ']' << endl;
        cout << "Digite uma opção: ";
        cin >> a;
    }
    *opcao = a;
}

void carrega_arquivo(string nome_arquivo, AVL *arvore, vector<AVL::Info>& vetor){
    ifstream arquivo(nome_arquivo);
    if(!arquivo.is_open()){
        cout << "Erro ao abrir o arquivo. Aperte enter par voltar.";
        cin.ignore();
        cin.get();
        return;
    }

    string linha;
    getline(arquivo, linha);
    int tam = stoi(linha);

    while(getline(arquivo, linha)){
        stringstream ss(linha);
        string id_str, nome_str, preco_str;

        if (getline(ss, id_str, ',') && getline(ss, nome_str, ',') && getline(ss, preco_str, ';')){
            AVL::Info *produto = new AVL::Info(stoi(id_str), nome_str, stof(preco_str));
            vetor.push_back(*produto);
            arvore->inserir(produto);
        }
    }

    cout << "Banco de dados carregado com sucesso. Aperte enter para continuar.";
    cin.get();

    arquivo.close();
}

void salva_arquivo(const vector<AVL::Info> &vetor, const string &nome_arquivo){
    ofstream arquivo(nome_arquivo);

    if (!arquivo){
        cout << "Erro ao salvar o arquivo. Aperte enter para voltar";
        limpa_buffer();
        cin.get();
        return;
    }

    for (const auto &item: vetor){
        arquivo << item.id << "," << item.nome << "," << item.preco << ";" << endl;

    }

    arquivo.close();
}

long calculaTamArquivo(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        return -1;
    }
    return file.tellg();
}

int main(){
    bool continuar = true;
    int opcao;
    AVL *arvore = new AVL();
    vector<AVL::Info> vetor;

    carrega_arquivo("DB.txt", arvore, vetor);

    while(continuar){
        limpa_tela();

        cout << "MENU: " << endl
            << "1 - Busca um produto na AVL" <<endl
            << "2 - Busca linear de um produto" << endl
            << "3 - Comprimir banco de dados com Huffman" << endl
            << "4 - Sair" << endl;

        entrada(1,4,&opcao);
        limpa_tela();
        switch (opcao){
            case 1:{
                cout << "=== Buscar produto na AVL ===" << endl << endl;;
                cout << "Digite o código do produto que deseja buscar: ";
                int codigo;
                cin >> codigo;

                auto inicio_tempo = chrono::steady_clock::now();
                AVL::No *nodo = arvore->buscar(codigo); 
                auto fim_tempo = chrono::steady_clock::now();
                chrono::duration<double> tempo_total = fim_tempo - inicio_tempo;
                
                if (nodo){
                    cout << "\u2714 " << "Produto: " << endl
                        << "\t Código: " << nodo->dado.id << endl
                        << "\t Nome: " << nodo->dado.nome << endl
                        << "\t Preço: " << fixed << setprecision(2) << nodo->dado.preco << endl; 
                }else cout << "\u2718 Código de produto não encontrado."; ;

                cout << "\n Tempo decorrido: " << fixed << setprecision(2) << tempo_total.count() << " segundos" << endl;

                break;
            }
            case 2:{
                cout << "=== Busca linear de um produto ===" << endl << endl;
                cout << "Digite o código do produto que deseja buscar: ";
                int indice = -1, codigo;
                cin >> codigo;

                auto inicio_tempo = chrono::steady_clock::now();

                for (int i = 0; i < vetor.size(); i++){
                    this_thread::sleep_for(chrono::milliseconds(tempo));
                    if (vetor[i].id == codigo) {
                        indice = i;
                        break;
                    }
                }

                auto fim_tempo = chrono::steady_clock::now();
                chrono::duration<double> tempo_total = fim_tempo - inicio_tempo;

                if (indice != -1) {
                    cout << "\u2714 " << "Produto: " << endl
                        << "\t Código: " << vetor[indice].id << endl
                        << "\t Nome: " << vetor[indice].nome << endl
                        << "\t Preço: " << fixed << setprecision(2) << vetor[indice].preco << endl;
                }else cout << "\u2718 Código de produto não encontrado."; 

                cout << "\n Tempo decorrido: " << fixed << setprecision(2) << tempo_total.count() << " segundos" << endl;

                break;
            }
            case 3: {
                cout << "=== Relatório de Compressão do Banco de Dados ===" << endl << endl;
                
                if (vetor.empty()) {
                    cout << "O banco de dados está vazio. Nada para analisar." << endl;
                    break;
                }

                string texto_completo;
                for (const auto& info : vetor) {
                    texto_completo += info.nome;
                }
                
                Huffman huffman_compressor;
                huffman_compressor.construir(texto_completo);
                cout << "Árvore de Huffman construída com base nos nomes dos produtos." << endl;

                long long total_bits_originais = 0;
                long long total_bits_comprimidos = 0;

                for (const auto& info : vetor) {
                    total_bits_originais += info.nome.length() * 8;

                    string nome_comprimido = huffman_compressor.comprimir(info.nome);
                    total_bits_comprimidos += nome_comprimido.length();
                }

                long tamanho_original_bytes = total_bits_originais / 8;
                long tamanho_comprimido_bytes = (total_bits_comprimidos + 7) / 8; // (+7 para arredondar para cima)

                if (tamanho_original_bytes > 0) {
                    double reducao = 100.0 * (1.0 - (double)tamanho_comprimido_bytes / tamanho_original_bytes);
                    cout << "--- Relatório de Compressão (Apenas para os Nomes dos Produtos) ---" << endl;
                    cout << "Tamanho Original dos Nomes: \t" << tamanho_original_bytes << " bytes" << endl;
                    cout << "Tamanho Comprimido: \t" << tamanho_comprimido_bytes << " bytes" << endl;
                    cout << "Fator de Redução: \t\t" << fixed << setprecision(2) << reducao << "%" << endl;
                } else {
                    cout << "Não foi possível gerar o relatório (sem dados)." << endl;
                }

                break;
            }
            case 4:
                continuar = false;
                break;
        }

        if (continuar){
            cout << "Aperte enter para voltar.";
            limpa_buffer();
            cin.get();
        } 
    }

    delete arvore;
}
