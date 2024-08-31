#ifndef EXIT_PROGRAM_H
#define EXIT_PROGRAM_H

#include "QObject"
#include <functional> // Pour std::function
#include <vector>     // Pour std::vector

class ExitProgram : public QObject {
Q_OBJECT
public slots:
    void appExit();
public:
    // Constructeur par défaut
    ExitProgram() = default;

    // Méthode pour ajouter une fonction
    void addExitFunction(const std::function<void()>& func);

    // Méthode pour appeler toutes les fonctions de sortie


private:
    // Liste des fonctions à exécuter lors de la sortie
    std::vector<std::function<void()>> exitFunctions;
};

#endif // EXIT_PROGRAM_H
