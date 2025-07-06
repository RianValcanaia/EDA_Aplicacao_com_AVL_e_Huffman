#ifndef AVL_H
#define AVL_H
#include <cstring>  
#include <chrono>
#include <thread>

#define tempo 200
using namespace std;

struct AVL {
    //============================================ ATRIBUTOS ============================================
    struct Info{
        int id; // código identificador
        string nome;
        float preco;
        
        // contruto
        Info() : id(0), nome(""), preco(0.0){}

        Info(int i, string n, float p): id(i), nome(n), preco(p){}
    };

    struct No{
        Info dado;
        int FB; // fator de balanceamento
        No *pai;
        No *esq;
        No *dir;

        //construtores
        No(): FB(0), pai(nullptr), esq(nullptr), dir(nullptr) {}
        No(No *ptr_pai, const Info& dado):  dado(dado), FB(0), pai(ptr_pai), esq(nullptr), dir(nullptr) {}
        No(No *ptr_pai, No *ptr_esq, No *ptr_dir, const Info& dado): dado(dado), pai(ptr_pai), esq(ptr_esq), dir(ptr_dir) {
            this->atualiza_FB();
        }

        void atualiza_FB(){
            FB = altura(this->esq) - altura(this->dir);
        }

        private:
            int altura(No *nodo){ 
                if (nodo == nullptr) return 0;
                int altura_Esq = altura(nodo->esq);
                int altura_Dir = altura(nodo->dir);
                return 1 + (altura_Esq > altura_Dir ? altura_Esq : altura_Dir);
            }
    };

    No *raiz;

    //============================================ MÉTODOS ============================================
    //construtores
    AVL(): raiz(nullptr) {}
    AVL(No *nodo): raiz(nodo){}

    //destrutor
    ~AVL(){
        destruir(raiz);
    }

    bool vazia(){ 
        return (raiz == nullptr)? true: false;
    }

    bool reinicia() {
        reinicia_rec(raiz);
        raiz = nullptr;
        return true;
    }

    bool inserir(Info *novo){
        raiz = inserir_rec(raiz, novo);
        return raiz != nullptr;
    }

    bool remover(int chave){
        bool removeu = false;
        raiz = remover_rec(raiz, chave, &removeu);
        return removeu;
    }

    No *buscar(int chave){
        return buscar_id_rec(raiz, chave);
    }

    private:
        void destruir(No* nodo) {
            if (nodo != nullptr) {
                destruir(nodo->esq);
                destruir(nodo->dir);
                delete nodo;
            }
        }
        
        void reinicia_rec(No *nodo){
            if (nodo == nullptr) return;
            reinicia_rec(nodo->esq);
            reinicia_rec(nodo->dir);
            delete nodo;
            
        }

        No *RSE(No *nodo){ // (-2;-1 ou 0)
            No *temp = nodo->dir; 
            No *temp_esq = temp->esq;

            temp->esq = nodo;
            nodo->dir = temp_esq; 

            temp->pai = nodo->pai;
            nodo->pai = temp;
            if (temp_esq) temp_esq->pai = nodo;

            nodo->atualiza_FB();
            temp->atualiza_FB();
            return temp;
        }

        No *RSD(No *nodo){ // (2; 1 ou 0)
            No *temp = nodo->esq;
            No *temp_dir = temp->dir;

            temp->dir = nodo;
            nodo->esq = temp_dir;
            
            temp->pai = nodo->pai;
            nodo->pai = temp;
            if (temp_dir) temp_dir->pai = nodo;

            nodo->atualiza_FB();
            temp->atualiza_FB();
            return temp;
        }

        No *RDE (No *nodo){ // (-2; 1)
            nodo->dir = RSD(nodo->dir);
            return RSE(nodo);
        }

        No *RDD(No *nodo){ // (2, -1)
            nodo->esq = RSE(nodo->esq);
            return RSD(nodo);
        }

        No *balancear(No *nodo){
            if (nodo == nullptr) return nullptr;

            if(nodo->FB >= -1 && nodo->FB <= 1) return nodo;

            // rotacao esquerda
            if(nodo->FB < -1){  
                int fb_dir = (nodo->dir) ? nodo->dir->FB : 0;
                if (fb_dir <= 0) return RSE(nodo);
                else return RDE(nodo);
            } 
            
            // rotacao direta
            if (nodo->FB > 1) {  
                int fb_esq = (nodo->esq) ? nodo->esq->FB : 0;
                if (fb_esq >= 0) return RSD(nodo);
                else return RDD(nodo);
            }

            return nodo;
        }
        
        No *inserir_rec(No *nodo, Info *new_info){
            
            if (nodo == nullptr) {
                return new No(nullptr, *new_info);
            }
            if (new_info->id < nodo->dado.id){
                if (nodo->esq == nullptr){
                    No *new_no = new No(nodo, *new_info);
                    nodo->esq = new_no;
                    return nodo;
                }
                nodo->esq = inserir_rec(nodo->esq, new_info);
            }else if(new_info->id > nodo->dado.id){
                if (nodo->dir == nullptr){
                    No *new_no = new No(nodo, *new_info);
                    nodo->dir = new_no;
                    return nodo;
                }
                nodo->dir = inserir_rec(nodo->dir, new_info);
            }else return nodo;

            nodo->atualiza_FB();

            if (nodo->FB < -1 || nodo->FB > 1){
                nodo = balancear(nodo);
            }

            return nodo;
        }

        No* remover_rec(No* nodo, int chave, bool *removeu) {
            if (nodo == nullptr) return nullptr;

            if (chave < nodo->dado.id) {
                nodo->esq = remover_rec(nodo->esq, chave, removeu);
                if (nodo->esq) nodo->esq->pai = nodo;
            }
            else if (chave > nodo->dado.id) {
                nodo->dir = remover_rec(nodo->dir, chave, removeu);
                if (nodo->dir) nodo->dir->pai = nodo;
            }
            else {
                *removeu = true;
                if (!nodo->esq && !nodo->dir) {
                    delete nodo;
                    return nullptr;
                }
                else if (!nodo->esq) {
                    No* temp = nodo->dir;
                    temp->pai = nodo->pai;
                    delete nodo;
                    return temp;
                }
                else if (!nodo->dir) {
                    No* temp = nodo->esq;
                    temp->pai = nodo->pai;
                    delete nodo;
                    return temp;
                }
                else {
                    No* sucessor = minimo(nodo->dir);
                    nodo->dado = sucessor->dado; 
                    nodo->dir = remover_rec(nodo->dir, sucessor->dado.id, removeu);
                    if (nodo->dir) nodo->dir->pai = nodo;
                }
            }

            nodo->atualiza_FB();

            if (nodo->FB < -1 || nodo->FB > 1) {
                nodo = balancear(nodo); 
            }

            return nodo;
        }

        No* minimo(No* nodo) {
            while (nodo->esq != nullptr) {
                nodo = nodo->esq;
            }
            return nodo;
        }

        No *buscar_id_rec(No *nodo, int chave){
            if (nodo == nullptr) return nullptr;
            
            this_thread::sleep_for(chrono::milliseconds(tempo)); 

            if (chave < nodo->dado.id){
                return buscar_id_rec(nodo->esq, chave);
            }else if(chave > nodo->dado.id){
                return buscar_id_rec(nodo->dir, chave);
            }else return nodo;
        }
        
};

#endif