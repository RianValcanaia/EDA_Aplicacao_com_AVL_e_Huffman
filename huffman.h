#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <queue>
#include <vector>
#include <map>
#include <sstream>

struct Huffman {
    //====================================== ESTRUTURAS ANINHADAS ======================================
    struct NoHuffman {
        char dado;
        unsigned frequencia;
        NoHuffman *esq, *dir;

        // construtor
        NoHuffman(char d, unsigned f) : dado(d), frequencia(f), esq(nullptr), dir(nullptr) {}
    };

    // obj de comparacao p fila de prioridade 
    struct CompararNo {
        bool operator()(NoHuffman* esq, NoHuffman* dir) {
            return esq->frequencia > dir->frequencia;
        }
    };

    //========================================= ATRIBUTOS ==========================================
private:
    NoHuffman* raiz;
    std::map<char, std::string> codigos;

    //=========================================== MÉTODOS ============================================
public:
    // construtor
    Huffman() : raiz(nullptr) {}

    // destrutor
    ~Huffman() {
        destruirArvore(raiz);
    }

    void construir(const std::string& texto_fonte) {
        if (texto_fonte.empty()) return;

        std::map<char, unsigned> mapa_frequencia;
        for (char c : texto_fonte) {
            mapa_frequencia[c]++;
        }

        std::priority_queue<NoHuffman*, std::vector<NoHuffman*>, CompararNo> fila_prioridade;
        for (auto const& [chave, valor] : mapa_frequencia) {
            fila_prioridade.push(new NoHuffman(chave, valor));
        }
        
        if (fila_prioridade.size() == 1) {
            NoHuffman* no_esq = fila_prioridade.top(); fila_prioridade.pop();
            NoHuffman* no_interno = new NoHuffman('$', no_esq->frequencia);
            no_interno->esq = no_esq;
            fila_prioridade.push(no_interno);
        }

        while (fila_prioridade.size() > 1) {
            NoHuffman* no_esq = fila_prioridade.top(); fila_prioridade.pop();
            NoHuffman* no_dir = fila_prioridade.top(); fila_prioridade.pop();

            NoHuffman* no_interno = new NoHuffman('$', no_esq->frequencia + no_dir->frequencia);
            no_interno->esq = no_esq;
            no_interno->dir = no_dir;
            fila_prioridade.push(no_interno);
        }

        raiz = fila_prioridade.top();
        gerarCodigos(raiz, "");
    }

    std::string comprimir(const std::string& texto_para_comprimir) {
        std::stringstream ss;
        for (char c : texto_para_comprimir) {
            if (codigos.count(c)) {
                ss << codigos.at(c);
            }
        }
        return ss.str();
    }
    
    std::string descomprimir(const std::string& texto_comprimido) {
        if (!raiz) return "";

        std::stringstream resultado_ss;
        NoHuffman* no_atual = raiz;
        
        if (raiz->esq && !raiz->dir) {
            no_atual = raiz->esq;
        }

        for (char bit : texto_comprimido) {
            if (bit == '0') {
                if (no_atual->esq) no_atual = no_atual->esq;
            } else {
                if (no_atual->dir) no_atual = no_atual->dir;
            }

            if (!no_atual->esq && !no_atual->dir) {
                resultado_ss << no_atual->dado;
                no_atual = raiz;
                if (raiz->esq && !raiz->dir) no_atual = raiz->esq;
            }
        }
        return resultado_ss.str();
    }

private:
    void gerarCodigos(NoHuffman* no, const std::string& codigo_atual) {
        if (!no) return;

        if (!no->esq && !no->dir) {
            codigos[no->dado] = codigo_atual.empty() ? "0" : codigo_atual;
        }

        gerarCodigos(no->esq, codigo_atual + "0");
        gerarCodigos(no->dir, codigo_atual + "1");
    }

    // Método auxiliar recursivo para liberar a memória
    void destruirArvore(NoHuffman* no) {
        if (!no) return;
        destruirArvore(no->esq);
        destruirArvore(no->dir);
        delete no;
    }
};

#endif 