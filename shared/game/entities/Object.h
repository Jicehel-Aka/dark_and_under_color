// Object.h — Objet ramassable (potion, clé, parchemin magique).
//
// Porté depuis Object.pde (version couleur Processing du jeu, fournie
// par Jicehel avec l'autorisation de l'auteur). Pas une couche de
// compatibilité comme pour Dark & Under 1 (Arduboy) -- Processing n'a
// aucun rapport avec l'API AKA, donc réécriture directe de la logique
// en C++, pas d'émulation d'API source.
//
// Différences volontaires avec l'original :
// - Les 4 PImage (icône d'inventaire, icône pleine taille, moyenne,
//   petite) deviennent des pointeurs vers des buffers RGB565 constants
//   (voir assets/), chargés une fois pour toutes en flash -- Processing
//   charge ses PNG depuis le disque à l'exécution (loadImage()), l'AKA
//   n'a pas de filesystem PNG à décoder à la volée pour ce cas d'usage.
// - Pas de champ "dialogueItem" ici : le texte affiché lors de la
//   collecte est une responsabilité de l'UI/Dialogue, pas de l'objet
//   lui-même (voir le futur portage de Dialogue.pde).
// - checkCollision()/collect() dans l'original ne faisaient déjà presque
//   rien (la majorité du corps de collect() était en commentaire, un
//   travail visiblement inachevé côté Processing) -- repris tel quel en
//   l'état (juste la détection de collision, pas d'effet de jeu), à
//   compléter quand la logique de ramassage sera portée pour de vrai.
#pragma once

#include <cstdint>

enum class ObjectType : uint8_t {
    Potion,  // 'P' dans l'original
    Key,     // 'K'
    Magic,   // 'M' (parchemin)
};

class GameObject {
  public:
    GameObject( int colX, int rowY, ObjectType type, const char* id );

    // true si les coordonnées (colX, rowY) passées correspondent à la
    // position actuelle du joueur -- signature identique à l'original
    // (qui reçoit des coordonnées externes plutôt que de lire this->,
    // permettant de tester une position différente de l'objet lui-même
    // si besoin ; conservé tel quel).
    bool checkCollision( int checkCol, int checkRow, int playerCol, int playerRow ) const;

    int getCol() const { return objectCol; }
    int getRow() const { return objectRow; }
    int getX() const { return objectX; }
    int getY() const { return objectY; }
    ObjectType getType() const { return objectType; }
    bool isActive() const { return active; }
    void setActive( bool value ) { active = value; }
    const char* getId() const { return id; }

  private:
    int objectX;
    int objectY;
    int objectRow;
    int objectCol;
    bool active = false;
    ObjectType objectType;
    const char* id;
};
