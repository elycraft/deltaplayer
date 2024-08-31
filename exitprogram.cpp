#include "exitprogram.h"

// Ajoute une fonction de sortie à la liste
void ExitProgram::addExitFunction(const std::function<void()>& func) {
    exitFunctions.push_back(func);
}

// Appelle toutes les fonctions de la liste
void ExitProgram::appExit() {
    for (const auto& func : exitFunctions) {
        func(); // Appelle chaque fonction
    }
}
