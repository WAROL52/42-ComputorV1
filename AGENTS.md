# AGENTS.md

## Périmètre d'écriture

Le dépôt appartient à l'utilisateur. **Tu n'écris que dans `tests/unit/`.**

Tout le reste — `src/`, `include/`, `main.cpp`, `Makefile`, `tests/e2e/` — est hors périmètre. Quand tu y repères un défaut, tu le décris dans ta réponse finale sous **Remarques** (fichier, ligne, symptôme, correctif proposé en diff) et tu laisses le fichier intact.

Une autorisation explicite de l'utilisateur dans la conversation débloque un fichier précis pour cette fois seulement. Elle ne s'étend pas aux tours suivants.

## Tests unitaires

Chaque fichier de test est **une seule unité de compilation** : il figure dans `TEST_SRCS` du `Makefile` et n'est jamais `#include` depuis un autre `.cpp`. Un `#include` duplique l'enregistrement des tests, car `VTEST_SUITE` s'exécute à l'initialisation statique — le fichier apparaît alors deux fois dans la sortie.

Un test rouge est un résultat valide : il documente un bug du code de production. Tu laisses le test rouge et tu remontes le bug en remarque ; tu ne touches pas au code pour le faire passer au vert.

Terminé quand : `make unit` compile avec les flags du projet (`-Wall -Wextra -Werror -std=c++11`) et la suite s'exécute ; ta réponse donne le compte passed/failed et les remarques.

## Style

- Français pour les noms de tests, les commentaires et les messages.
- Indentation à l'onglets, accolades ouvrantes sur la ligne de la déclaration.
- Bibliothèque de test : `tests/unit/Vtest.hpp` — `Vtest::describe`, `Vtest::it`, `Vtest::expect`.
