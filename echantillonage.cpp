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


struct Angles {
    float Phi_res1;
    float Phi_res2;
};


class Matrix_Distrib{
private :
    std::vector<std::vector<float>> data;
    std::string name ;
    float precision ; 
public :

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
                data.push_back(row); // Ajoute la ligne à la matrice en appelant la méthode addRow().
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

    Angles tirage(){
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_real_distribution<float> distrib(0.0, 1.0);

        double random_number = distrib(gen);

        float compte = 0;
        float line = 0 ;
        float col = 0 ;
        for (auto& row : data){
            line += 1.0 ;
            for (auto& value : row){
                col += 1.0 ;
                compte += value;
                //std::cout<<compte<<" <=  "<<random_number<<std::endl;
                if (compte>=random_number){
                    std::cout<<precision<<"   "<<col<< "   "<<line<<std::endl;
                    Angles a = {col*precision-M_PI, line*precision-M_PI} ;
                    
                    return a ;
                }
            col = 0.0 ;
            }
        }
        return {0, 0};

        }

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
                                  cv::Point(j * scale, i * scale),
                                  cv::Point((j + 1) * scale - 1, (i + 1) * scale - 1),
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
    ds.ReadFrom_txt("C:/Users/Octave/Desktop/INSA4A_2/projet/code/Distributions/ALA_ALA_ALA_0.300000.txt");
    ds.normalisation() ;
    //ds.affiche();
    //std::cout<<ds.norme()<<std::endl;

    Angles test = ds.tirage() ;
    std::cout << "Point : (" << test.Phi_res1 << ", " << test.Phi_res2 << ")" << std::endl;
    std::cout<<"gfdsjhklm"<<std::endl;
    ds.affiche_image();

    return 0;
}
