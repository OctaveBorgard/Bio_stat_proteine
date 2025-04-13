#define _USE_MATH_DEFINES
#include <iostream> // Inclut la bibliothèque standard pour les entrées et sorties (cin, cout).
// std::cout permet d'afficher des messages dans la console. std::endl insère une fin de ligne ("\n").
#include <fstream> // Inclut la bibliothèque pour manipuler les fichiers (lecture et écriture).
// std::ifstream permet d'ouvrir et lire un fichier, et std::cerr affiche les erreurs dans la console.
#include <vector>  // Inclut la bibliothèque pour utiliser les conteneurs dynamiques comme std::vector.
// std::vector est un conteneur dynamique similaire à un tableau extensible. Il est déclaré avec le type des éléments qu'il contient.
#include <sstream> // Inclut la bibliothèque pour traiter les chaînes comme des flux (std::stringstream).
// std::stringstream permet de manipuler des chaînes de caractères comme des flux d'entrée/sortie.
#include <set> 
#include <cmath>
#include <filesystem>

// Définition d'une classe Matrix pour stocker et manipuler les données sous forme de matrice.
class Matrix {
private:
    std::vector<std::vector<std::string>> data; // Déclare un vecteur de vecteurs de chaînes pour représenter la matrice.
    std::string name;
    // Chaque std::vector<std::string> représente une ligne, et data est la collection de toutes les lignes.

public:

    void change_name(const std::string& name_file){
        std::string supr = "Angles_info_" ;
        std::string supr2 = ".txt" ;
        std::string new_name = name_file;
        size_t pos1 = new_name.find(supr);
        if (pos1 != std::string::npos) {  // Vérifie si trouvé
            new_name.erase(pos1, supr.length());
        }

        size_t pos2 = new_name.find(supr2);
        if (pos2 != std::string::npos) {  // Vérifie si trouvé
            new_name.erase(pos2, supr2.length());
        }

        for (char &c : new_name) {
            if (c == '\\') c = '/';
        }
    
        // Trouver la dernière occurrence de '/'
        size_t pos = new_name.find_last_of("/");
    
        // Extraire la dernière partie du chemin
        new_name = (pos != std::string::npos) ? new_name.substr(pos + 1) : new_name;

        name = new_name ;
    }

    void affiche_name(){
        std::cout<<name<<std::endl;
    }

    int indice(const std::string& nomCol){
        // const signifie que cette méthode ne modifie pas l'objet courant.
        if (data.empty() || data[0].empty()){ //si la matrice est vide ou si la première ligne est vide on retourne -1
            return -1;
        }
        const auto& firstRow=data[0]; // Récupère la première ligne du tableau de données
        for (size_t i=0; i<firstRow.size();i++){ // Parcourt chaque élément de la première ligne
            if(firstRow[i]==nomCol){ // Vérifie si l'élément correspond au nom de la colonne recherchée
                return static_cast<int>(i); // Retourne l'index de la colonne sous forme d'entier
            }
        }
        return -1;
    }

    // Méthode pour ajouter une ligne à la matrice.
    void addRow(const std::vector<std::string>& row) {
        data.push_back(row); // Ajoute la ligne au vecteur principal "data".
        // push_back() est une méthode de std::vector qui ajoute un élément à la fin du vecteur.
    }

    // Méthode pour supprimer une colonne à un index donné.
    void removeColumn(int colIndex) {
        for (auto& row : data) { // Parcourt chaque ligne de la matrice.
            // auto déduit automatiquement le type de la variable. Ici, row est de type std::vector<std::string>&.
            if (colIndex < row.size()) { // Vérifie si l'index de la colonne existe pour cette ligne.
                row.erase(row.begin() + colIndex); // Supprime l'élément à l'index spécifié dans la ligne.
                // erase() est une méthode de std::vector qui supprime un élément à un index donné.
                // row.begin() retourne un itérateur pointant sur le premier élément de la ligne.
            }
        }
    }

    void removeRow(int rowindex) {
        if (rowindex < data.size()){
            data.erase(data.begin()+rowindex) ;
        }
    }

    void remove0(int cistrans_index){
        if (nb_col() >= cistrans_index){
            for (auto it = begin(data); it != end(data);){
               if ((*it)[cistrans_index] == "0"){
                    it = data.erase(it);
                }
                else {
                    it++ ;
                }
          }
        }
        else std::cout<<"Colonne non existante"<<std::endl;
    }

    // Méthode pour afficher la matrice dans la console.
    void affiche() const {
        // const signifie que cette méthode ne modifie pas l'objet courant.
        for (const auto& row : data) { // Parcourt chaque ligne de la matrice.
            // const auto& garantit que chaque ligne n'est pas modifiée.
            for (const std::string& value : row) { // Parcourt chaque élément de la ligne.
                std::cout << value << " "; // Affiche l'élément suivi d'un espace.
            }
            std::cout << std::endl; // Passe à la ligne suivante après avoir affiché une ligne entière.
        }
    }

    int nb_row() const{
        return data.size() ;
    }

    int nb_col() const{ //faire test a 0
        if (nb_row()!=0){
            return data[0].size() ;
        }
        else{
            return 0 ;
        }
    }

    void discretisation(float precision){ //La matrice echantillon est passée en référence (Matrix& echantillon).

        if (nb_col() != 2) { // Vérifie que la matrice contient exactement 2 colonnes.
            std::cerr << "Erreur : la matrice doit avoir exactement 2 colonnes pour l'échantillonnage." << std::endl;
            return;  // Quitte la fonction si la matrice n'a pas 2 colonnes.
        }
        
        std::string folderName = "Distributions_" + std::to_string(precision);
        std::filesystem::path outputDir = "C:/Users/Octave/Desktop/INSA4A_2/projet/code/" + folderName;
        std::filesystem::create_directories(outputDir); // Crée le dossier si nécessaire

        //std::filesystem::path outputDir = "./Distributions03";
        std::string nomFichier = (outputDir / (name + "_" + std::to_string(precision) + ".txt")).string();

        // Ouvrir le fichier en mode lecture et écriture (création si inexistant)
        std::fstream fichier(nomFichier, std::ios::in | std::ios::out | std::ios::app);

        // Vérifier si l'ouverture a réussi
        if (!fichier) {
            std::cerr << "Erreur lors de l'ouverture du fichier !" << std::endl;
            return;
        }

        std::vector<std::vector<int>> table; // Déclare un vecteur de vecteurs de chaînes pour représenter la matrice.
        std::vector<int> zeroRow(static_cast<int>(std::round(2*M_PI / precision)+1), 0);

        for (int i=1; i <= std::round(2*M_PI/precision)+1; i++){ //Créer la matrice remplit de 0.
            table.push_back(zeroRow);
        }
        /*std::cout<<"table"<<std::endl;
        for (const auto& row : table) {
            for (const auto& value : row) {
                std::cout << value << " ";
            }
            std::cout << std::endl;
        }*/
        
        // Parcourir les lignes de la matrice en ignorant la première ligne (en-tête).
        for (size_t i = 1; i < data.size(); i++) { 
            try {
                // Convertit la première colonne en float (X).
                float x = std::stof(data[i][0]) + M_PI;  

                // Convertit la deuxième colonne en float (Y).
                float y = std::stof(data[i][1]) + M_PI;  

                // Arrondi les valeurs en fonction de la précision demandée.
                int x_round = std::round(x / precision);
                int y_round = std::round(y / precision);
                //std::cout<<"x, y"<< std::ceil(2*M_PI / precision) << " "<< x_round<<" "<<y_round<<std::endl;
                //remplissage de la matrice
                if (x_round < 0 || x_round >= table.size() || y_round < 0 || y_round >= table.size()) {
                    std::cerr << "Erreur : indices hors limites ! x_round=" << x_round << ", y_round=" << y_round << std::endl;
                } else {
                    table[y_round][x_round] += 1;
                }
                

            } 
            catch (const std::exception& e) { // Capture les erreurs éventuelles (ex: conversion impossible)
                std::cerr << "Erreur de conversion à la ligne " << i << ": " << e.what() << std::endl;
            }
        }
        //Ecrire la matrice dans le txt
        for (auto& row : table){
            for (auto& value : row){
                fichier << value << " ";
            }
            fichier << std::endl;
        }    
        if (fichier.is_open()) {
            fichier.close();
        }
        else {std::cout<<"fichier non ouvert ?"<<std::endl;} 
        //std::cout << "Fini !"<<std::endl;
    } // fin discretisation
    

    
    
}; //fin de la class Matrix



// Fonction pour lire un fichier texte et remplir une matrice avec ses données.
void readFileToMatrix(const std::string& filename, Matrix& matrix) {
    std::string name ;
    // std::string est une classe de la bibliothèque standard pour manipuler des chaînes de caractères.
    std::ifstream file(filename); // Ouvre un fichier en mode lecture.
    // std::ifstream est utilisé pour lire des fichiers. Il est initialisé avec le nom du fichier.
    if (!file) { // Vérifie si le fichier n'a pas pu être ouvert.
        std::cerr << "Erreur d'ouverture du fichier." << std::endl; // Affiche un message d'erreur sur la sortie standard d'erreur.
        return; // Quitte la fonction en cas d'erreur.
    }
    
    std::string line; // Variable pour stocker chaque ligne du fichier.
    while (std::getline(file, line)) { // Lit le fichier ligne par ligne.
        std::vector<std::string> row; // Déclare un vecteur pour stocker les éléments de la ligne.
        std::stringstream ss(line); // Crée un flux de chaînes pour traiter la ligne comme un flux de données.
        std::string value; // Variable pour stocker chaque élément de la ligne.
        while (ss >> value) { // Extrait chaque mot ou valeur (séparé par des espaces) du flux.
            row.push_back(value); // Ajoute l'élément extrait au vecteur "row".
        }
        
        if (!row.empty()) { // Vérifie si la ligne n'est pas vide.
            matrix.addRow(row); // Ajoute la ligne à la matrice en appelant la méthode addRow().
        }
    }
    std::cout << filename << std::endl;
    matrix.change_name(filename);
    file.close(); // Ferme le fichier après lecture.
}

void Work_one_document(std::string filename, float precision){
    Matrix matrix; // Instancie un objet Matrix pour stocker les données.
    readFileToMatrix(filename, matrix); // Appelle la fonction pour lire les données du fichier .txt et remplir la matrice.

    if (matrix.nb_row() < 60) {
        std::cout << "Fichier ignoré (moins de 60 lignes) : " << filename << std::endl;
        return;  // On ne traite pas ce fichier
    }

    matrix.remove0(matrix.indice("CIS(0)/TRANS(1)")) ;
    matrix.removeColumn(matrix.indice("CIS(0)/TRANS(1)")); 
    matrix.removeColumn(matrix.indice("Res1")); 
    matrix.removeColumn(matrix.indice("Res2")); 
    matrix.removeColumn(matrix.indice("Res3")); 
    matrix.removeColumn(matrix.indice("Omega_res_1")); 
    matrix.removeColumn(matrix.indice("Omega_res_2"));
    matrix.removeColumn(matrix.indice("Omega_res_3"));
    matrix.removeColumn(matrix.indice("Phi_res_1"));
    matrix.removeColumn(matrix.indice("Psi_res_1"));
    matrix.removeColumn(matrix.indice("Phi_res_3"));
    matrix.removeColumn(matrix.indice("Psi_res_3")); // On ne garde que les colonnes Phi_res_2 et Psi_res_2
    //std::cout << "\nMatrice avant suppression :" << std::endl;
    //matrix.affiche(); // Appelle la méthode affiche() pour afficher la matrice avant modification.
    
    
    //std::cout << "\nMatrice après suppression :" << std::endl;
    //matrix.affiche_name();
    //matrix.affiche(); // Affiche la matrice après suppression de la colonne.
    

    //ECHANTILLONNAGE
    //float precision = 0.3;  // Définit la précision pour arrondir les valeurs
    matrix.discretisation(precision); // Appliquer la discretisation
}

int main() {
    //std::string dossier = "../angles_scop-95_2.07-LGBTS-MF_0.6_0.2"; // Remplace par le chemin du dossier contenant les fichiers
    std::string dossier = "C:/Users/Octave/Desktop/INSA4A_2/projet/angles_scop-95_2.07-LGBTS-MF_0.6_0.2";
    std::vector<float> precisions = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};  // Ajoute ici toutes les précisions souhaitées

    try {
        for (float precision : precisions) {
            for (const auto& entry : std::filesystem::directory_iterator(dossier)) {
                if (entry.is_regular_file()) {
                    Work_one_document(entry.path().string(), precision);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
    return 0;
}
