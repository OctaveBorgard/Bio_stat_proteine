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
#include <random>
#include <opencv2/opencv.hpp>
#include <functional>


struct Angles {
    float Phi_res1;
    float Phi_res2;
};


// Fonction pour résoudre le système linéaire
std::vector<float> solve(std::vector<std::vector<float>> A, std::vector<float> Y) {
    int n = A.size();

    for (int i = 0; i < n; i++) {
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (abs(A[k][i]) > abs(A[maxRow][i])) {
                maxRow = k;
            }
        }

        std::swap(A[i], A[maxRow]);
        std::swap(Y[i], Y[maxRow]);
        for (int k = i + 1; k < n; k++) {
            float c = A[k][i] / A[i][i];
            for (int j = i; j < n; j++) {
                A[k][j] -= c * A[i][j];
            }
            Y[k] -= c * Y[i];
        }
    }
    std::vector<float> X(n);
    for (int i = n - 1; i >= 0; i--) {
        X[i] = Y[i];
        for (int j = i + 1; j < n; j++) {
            X[i] -= A[i][j] * X[j];
        }
        X[i] /= A[i][i];
    }

    return X;
}

std::vector<float> interpolerLinear(float x1, float y1, float x2, float y2) {
    std::vector<float> X(2);
    X[0] = (y2-y1)/(x2-x1) ;
    X[1] = y1-X[0]*x1 ;
    return X ;
}
// Fonction d'interpolation quadratique
std::vector<float> interpolerQuadratique(float x1, float y1, float x2, float y2, float x3, float y3) {
    std::vector<std::vector<float>> A = {
        {x1 * x1, x1, 1},
        {x2 * x2, x2, 1},
        {x3 * x3, x3, 1}
    };

    std::vector<float> Y = {y1, y2, y3};
    return solve(A, Y);
}


// Fonction densité f(x) = ax^2 + bx + c
float density_quadratique(float x, float a, float b, float c) {
    return a*x*x + b*x + c;
}

float density_lineaire(float x, float a, float b) {
    return a*x + b;
}

float methodeRejet_lineaire(float centroide, float precision, float x1, float y1, float x2, float y2, float x3, float y3) {
    std::random_device rd;
    std::mt19937 gen(rd());
    double xmin = centroide-precision/2 ;
    double xmax = centroide+precision/2 ;
    float zeros = 0.0;
    xmin =  std::max(xmin, -M_PI);
    xmax =  std::min(xmax, M_PI) ;
    std::uniform_real_distribution<> dist_x(xmin, xmax);
    //std::cout<<"min, max "<<xmin<< " "<<xmax<<std::endl;
    float a1, b1, a2, b2 ;
    std::vector<float> coef_tmp = interpolerLinear(x1, y1, x2, y2) ;
    a1 = coef_tmp[0];
    b1 = coef_tmp[1] ;
    coef_tmp = interpolerLinear(x1, y1, x3, y3) ;
    a2 = coef_tmp[0];
    b2 = coef_tmp[1] ;
    //std::cout<<"a1, b1, a2, b2 "<<a1<<" "<<b1<<" "<<a2<<" "<<b2<< " "<<std::endl;
    // Trouver le maximum de f(x) sur [xmin, xmax]
    float fmax ;
    fmax = std::max({density_lineaire(xmin, a1, b1), density_lineaire(xmax, a2, b2), density_lineaire(centroide, a1, b1)});
    
    //std::cout<<"fmax "<<density_lineaire(xmin, a1, b1)<< " " << density_lineaire(xmax, a2, b2) << " "<<density_lineaire(centroide, a1, b1) <<std::endl;

    std::uniform_real_distribution<> dist_y(0.0, fmax);

    int secure = 0 ;
    while (secure <= 10000) {
        float x = dist_x(gen);
        float y = dist_y(gen);

        if ((x<= centroide & y <= density_lineaire(x, a1, b1))||(x>=centroide & y<=density_lineaire(x, a2, b2))) {
            return x;
        }
        secure += 1;
    }
    std::cout << "Aucun tirage réussi" <<std::endl;
}

float methodeRejet_quadra(float a, float b, float c, float xmin, float xmax) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_x(xmin, xmax);

    // Trouver le maximum de f(x) sur [xmin, xmax]
    float fmax ;
    if (a!=0 & xmin <-b/(2*a) & -b/(2*a)<xmax){
        fmax = std::max({density_quadratique(xmin, a, b, c), density_quadratique(xmax, a, b, c),
            density_quadratique(-b/(2*a), a, b, c)}); // sommet de la parabole
    } else{
        fmax = std::max({density_quadratique(xmin, a, b, c), density_quadratique(xmax, a, b, c)});
    }

    std::uniform_real_distribution<> dist_y(0.0, fmax);

    int secure = 0 ;
    while (secure <= 10000) {
        float x = dist_x(gen);
        float y = dist_y(gen);

        if (y <= density_quadratique(x, a, b, c)) {
            return x;
        }
        secure += 1;
    }
    std::cout << "Aucun tirage réussi" <<std::endl;
}



class Matrix_Distrib{
private :
    std::vector<std::vector<float>> data;
    std::string name ;
    float precision ; 
public :
    void change_precision(const float p){
        precision = p ;
    }
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

    void affiche_nom(){
        std::cout<<name<<std::endl;
    }

    void addRow(const std::vector<float>& row) {
        data.push_back(row);
    }

    void affiche(){
        for (auto& row : data){
            for (float value : row){
                std::cout<<value<<" " ;
            }
            std::cout<<std::endl;
        }
    }

    void ReadFrom_txt(const std::string& filename){
        std::ifstream file(filename); // Ouvre un fichier en mode lecture.
        if (!file) { // Vérifie si le fichier n'a pas pu être ouvert.
            std::cerr << "Erreur d'ouverture du fichier." << std::endl; // Affiche un message d'erreur sur la sortie standard d'erreur.
            return; // Quitte la fonction en cas d'erreur.
        }
            
        std::string line; // Variable pour stocker chaque ligne du fichier.
        while (std::getline(file, line)) { // Lit le fichier ligne par ligne.
            std::vector<float> row; // Déclare un vecteur pour stocker les éléments de la ligne.
            std::stringstream ss(line); // Crée un flux de chaînes pour traiter la ligne comme un flux de données.
            std::string value; // Variable pour stocker chaque élément de la ligne.
            while (ss >> value) { // Extrait chaque mot ou valeur (séparé par des espaces) du flux.
                row.push_back(stoi(value)); // Ajoute l'élément extrait au vecteur "row".
            }
                
            if (!row.empty()) { // Vérifie si la ligne n'est pas vide.
                data.push_back(row);
            }
        }
        std::cout << filename << std::endl;
        change_name(filename);
        size_t pos = filename.find_last_of("_");
        precision = stof((pos != std::string::npos) ? filename.substr(pos + 1) : filename);
        //std::cout<<"Precision "<<precision<<std::endl;
        file.close(); // Ferme le fichier après lecture.
        
    }

    float norme(){
        float norm = 0;
        for (auto& row : data){
            for (auto& value : row){
                norm += value;
            }
        }
        return norm ;
    }

    void normalisation(){
        float norm = norme();
        for (auto& row : data){
            for (auto& value : row){
                value /= norm;
            }
        }
    }

    Angles tiragePalier(){
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_real_distribution<float> distrib(0.0, 1.0);

        double random_number = distrib(gen);

        float compte = 0;
        float line = 0 ;
        float col = 0 ;
        for (auto& row : data){
            for (auto& value : row){
                compte += value;
                //std::cout<<compte<<" <=  "<<random_number<<std::endl;
                if (compte>=random_number){
                    //std::cout<<precision<<"   "<<col<< "   "<<line<<std::endl;
                    Angles a = {(col)*precision-M_PI, (line)*precision-M_PI} ;
                    return a ;
                }
                col += 1.0 ;
            }
            line += 1.0 ;
            col = 0.0 ;
        }
        return {0, 0};
    }

    Angles tirageLineaire(){
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_real_distribution<float> distrib(0.0, 1.0);
        double random_number = distrib(gen);
        float compte = 0;
        float line = 0 ;
        float col = 0 ;
        float phi1, phi2;
        std::vector<float> coef_spline ;
        float a, b, c ;
        float zeros = 0.0 ;
        for (auto& row : data){
            for (auto& value : row){
                compte += value;
                if (compte>=random_number){
                    //std::cout<<"col line "<<col<<" "<<line<<std::endl;
                    if (col == 0){
                        //std::cout<<(col)*precision-M_PI<<" "<< value<< " "<<(-1)*precision-M_PI<<" "<< data[line][data.size()-1]<<" "<< (col+1)*precision-M_PI<<" "<< data[line][col+1]<<std::endl;
                        phi1 = methodeRejet_lineaire(col*precision-M_PI, precision, (col)*precision-M_PI, value, (-1)*precision-M_PI, data[line][data.size()-1], (col+1)*precision-M_PI, data[line][col+1]) ;
                    } else if (col+1 >= data.size()){
                        phi1 = methodeRejet_lineaire(col*precision-M_PI, precision,(col)*precision-M_PI, value, (col-1)*precision-M_PI, data[line][col-1], (col+1)*precision-M_PI, data[line][0]);
                    } else {
                        phi1 = methodeRejet_lineaire(col*precision-M_PI, precision,(col)*precision-M_PI, value, (col-1)*precision-M_PI, data[line][col-1], (col+1)*precision-M_PI, data[line][col+1]) ;
                    };
                    //std::cout<<"end phi1 "<<std::endl;

                    if (line == 0){
                        phi2 = methodeRejet_lineaire(line*precision-M_PI, precision, (line)*precision-M_PI, value, (-1)*precision-M_PI, data[data.size()-1][col], (line+1)*precision-M_PI, data[line+1][col]) ;
                    } else if (line+1 >= data.size()){
                        phi2 = methodeRejet_lineaire(line*precision-M_PI, precision, (line)*precision-M_PI, value, (line-1)*precision-M_PI, data[line-1][col], (line+1)*precision-M_PI, data[0][col]) ;
                    } else {
                        phi2 = methodeRejet_lineaire(line*precision-M_PI, precision, (line)*precision-M_PI, value, (line-1)*precision-M_PI, data[line-1][col], (line+1)*precision-M_PI, data[line+1][col]) ;
                    };
                    //std::cout<<"end phi2 "<<std::endl;

                    Angles angl = {phi1, phi2} ;
                
                    return angl ;
                }
                col += 1.0 ;
            }
            col = 0.0 ;
            line += 1.0 ;
        }
        return {0, 0};

    }
    

    Angles tirageQuadratique(){
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_real_distribution<float> distrib(0.0, 1.0);

        double random_number = distrib(gen);

        float compte = 0;
        float line = 0 ;
        float col = 0 ;
        float phi1, phi2;
        std::vector<float> coef_spline ;
        float a, b, c ;
        float zeros = 0.0 ;
        for (auto& row : data){
            for (auto& value : row){
                
                compte += value;
                if (compte>=random_number){
                    if (col == 0){
                        coef_spline = interpolerQuadratique((col)*precision-M_PI, value, (-1)*precision-M_PI, data[line][data.size()-1], (col+1)*precision-M_PI, data[line][col+1]) ;
                    } else if (col+1 >= data.size()){
                        coef_spline = interpolerQuadratique((col)*precision-M_PI, value, (col-1)*precision-M_PI, data[line][col-1], (col+1)*precision-M_PI, data[line][0]) ;
                    } else {
                        coef_spline = interpolerQuadratique((col)*precision-M_PI, value, (col-1)*precision-M_PI, data[line][col-1], (col+1)*precision-M_PI, data[line][col+1]) ;
                    }   
                    a = coef_spline[0];
                    b = coef_spline[1];
                    c = coef_spline[2];
                    phi1 = methodeRejet_quadra(a, b, c, std::max((col*precision-precision/2), zeros)-M_PI, std::min((col*precision+precision/2-M_PI), M_PI));

                    if (line == 0){
                        coef_spline = interpolerQuadratique((line)*precision-M_PI, value, (-1)*precision-M_PI, data[data.size()-1][col], (line+1)*precision-M_PI, data[line+1][col]) ;
                    } else if (line+1 >= data.size()){
                        coef_spline = interpolerQuadratique((line)*precision-M_PI, value, (line-1)*precision-M_PI, data[line-1][col], (line+1)*precision-M_PI, data[0][col]) ;
                    } else {
                        coef_spline = interpolerQuadratique((line)*precision-M_PI, value, (line-1)*precision-M_PI, data[line-1][col], (line+1)*precision-M_PI, data[line+1][col]) ;
                    }
                    a = coef_spline[0];
                    b = coef_spline[1];
                    c = coef_spline[2];
                    phi2 = methodeRejet_quadra(a, b, c, std::max((line*precision-precision/2), zeros)-M_PI, std::min((line*precision+precision/2-M_PI), M_PI));
                    Angles angl = {phi1, phi2} ;
                
                    return angl ;
                }
                col += 1.0 ;
            }
            col = 0.0 ;
            line += 1.0 ;
        }
        return {0, 0};

    }

    void create_echantillon(Matrix_Distrib& echantillon, int n = 4000){
        //std::filesystem::path outputDir = "./Echantillon_genere";
        //std::string nomFichier = (outputDir / (name + "_" + std::to_string(precision) + "_generation.txt")).string();
        std::string nomFichier = "C:/Users/Octave/Desktop/INSA4A_2/projet/code/Echantillon_genere/" + name + "_" + "_generation.txt";
        // Ouvrir le fichier en mode lecture et écriture (création si inexistant)
        std::fstream fichier(nomFichier, std::ios::in | std::ios::out | std::ios::app);
        // Vérifier si l'ouverture a réussi
        if (!fichier) {
            std::cerr << "Erreur lors de l'ouverture du fichier !" << std::endl;
            return;
        }
        for (int i =0;i<n;i++){
            Angles generation = tirageLineaire() ;
            std::vector<float> tmp ;
            tmp.push_back(generation.Phi_res1) ;
            tmp.push_back(generation.Phi_res2) ;
            echantillon.addRow(tmp);
            fichier << generation.Phi_res1 << " " << generation.Phi_res2 << std::endl;
        }
        if (fichier.is_open()) {
            fichier.close();
        }
        else {std::cout<<"fichier non ouvert ?"<<std::endl;} 
    }

    void discretisation(float precision){ //La matrice echantillon est passée en référence (Matrix& echantillon).
        //std::filesystem::path outputDir = "C:/Users/Octave/Desktop/INSA4A_2/projet/code/Distributions";
        //std::filesystem::path outputDir = "./Distributions_echantillon";
        std::string nomFichier = "./Distributions_echantillon/"+name + "_" + std::to_string(precision) + ".txt";

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
        
        for (size_t i = 0; i < data.size(); i++) { 
            try {
                // Convertit la première colonne en float (X).
                float x = data[i][0] + M_PI;  

                // Convertit la deuxième colonne en float (Y).
                float y = data[i][1] + M_PI;  

                // Arrondi les valeurs en fonction de la précision demandée.
                int x_round = std::round(x / precision);
                int y_round = std::round(y / precision);
                //std::cout<<"x, y"<< std::ceil(2*M_PI / precision) << " "<< x_round<<" "<<y_round<<std::endl;
                //remplissage de la matrice
                if (x_round < 0 || x_round >= table.size() || y_round < 0 || y_round >= table.size()) {
                    std::cout<<"valeur prob "<<x<<" "<<y<<std::endl;
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


    void affiche_image() {
        if (data.empty()) {
            std::cerr << "Aucune donnée à afficher." << std::endl;
            return;
        }    
        int rows = data.size();
        int cols = data[0].size();
        int scale = 10; // Facteur d'agrandissement
        cv::Mat img(rows * scale, cols * scale, CV_8UC1);    
        float max_val = 0;
        for (const auto& row : data) {
            for (float val : row) {
                if (val > max_val) {
                    max_val = val;
                }
            }
        }
    
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                uchar pixel_value = static_cast<uchar>((data[i][j] * 255) / max_val);
                cv::rectangle(img, 
                                cv::Point(j * scale, (rows - 1 - i) * scale), // Inversion des lignes
                                cv::Point((j + 1) * scale - 1, (rows - i) * scale - 1),
                                cv::Scalar(pixel_value),
                                cv::FILLED);
            }
        }    
        cv::Mat img_color;
        cv::applyColorMap(img, img_color, cv::COLORMAP_JET);    
        // Enregistrement de l'image
        std::string filename = name + ".png";
        cv::imwrite(filename, img_color);
        std::cout << "Image sauvegardée sous: " << filename << std::endl;    
        cv::imshow("Matrice Colorée", img_color);
        cv::waitKey(0);
    }
};



int main() {
    Matrix_Distrib ds;
    Matrix_Distrib echantillon ;
    ds.ReadFrom_txt("./Distributions/ALA_ALA_ALA_0.300000.txt");
        //C:/Users/Octave/Desktop/INSA4A_2/projet/code/Distributions_test/AAA_AAA_AAA_0.300000.txt");
    ds.normalisation() ;
    //ds.affiche();
    //std::cout<<ds.norme()<<std::endl;

    //Angles test = ds.tirageQuadratique() ;
    //std::cout << "Point : (" << test.Phi_res1 << ", " << test.Phi_res2 << ")" << std::endl;
    //ds.affiche_image();

    ds.create_echantillon(echantillon) ;
    echantillon.change_precision(0.3) ;
    echantillon.change_name("echantillon") ;
    echantillon.discretisation(0.3);/*
    Matrix_Distrib distrib_echantillon;
    distrib_echantillon.ReadFrom_txt("C:/Users/Octave/Desktop/INSA4A_2/projet/code/Distributions_echantillon/echantillon_0.300000.txt");
    distrib_echantillon.affiche_image() ;*/

    return 0;
}
