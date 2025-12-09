#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <limits>
#include <algorithm>
#include <cctype>
using namespace std;


static string toLowerTrim(const string &s) {
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end-1])) end--;
    string out;
    for (size_t i = start; i < end; ++i) 
        out.push_back((char)tolower((unsigned char)s[i]));
    return out;
}

// Forward declarations
struct Player;
struct Enemy;
void openChestZone1(Player &player);
void openChestZone3(Player &player);
void npcEncounter(Player &player);
void randomEncounter(Player &player, vector<Enemy> pool, vector<int> probs);
void battle(Player &player, Enemy enemy);
void bossBattle_DraconicMaster(Player &player);
void dialog(const string& speaker, const string& text);
void showHUD(const Player &p);

// Compatibility wrapper: some calls use the older/alternate name draconicMasterBoss
inline void draconicMasterBoss(Player &player) { bossBattle_DraconicMaster(player); }

int rng(int a, int b) {
    return rand() % (b - a + 1) + a;
}
// utilty=
void typeText(const string& text, int delay = 25) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
    cout << endl;
}
void narrate(const string& text) {
    cout << text << endl;
}
void dialog(const string& speaker, const string& text) {
    cout << speaker << " " << text << endl;
}
int chooseOption(int min, int max) {
    int choice;
    cout << "\n> Choose: ";
    while (!(cin >> choice) || choice < min || choice > max) {
        cout << "Pick a number between " << min << " and " << max << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // bersihkan buffer
    return choice;
}

//  banner
void showTitle() {
    cout << "========================================== \n";
    cout << "  ___  _   _ _  _  ___ ___ ___  _  _  \n";
    cout << " |   \\| | | | \\| |/ __| __/ _ \\| \\| | \n";
    cout << " | |) | |_| | .` | (_ | _| (_) | .` | \n";
    cout << " |___/ \\___/|_|\\_|\\___|___\\___/|_|\\_| \n";
    cout << "  /  \\ ___| |_\\__\\__\\__\\__\\__\\__\\__\\_\\ \n";
    cout << " | () |___| (_-<_-<_-<_-<_-<_-<_-< '_|/\n";
    cout << "  \\__/    |_/__/__/__/__/__/__/__/__/\n";
    cout << "  _    ___ ___ ___ _  _ ___  ___        \n";
    cout << " | |  | __/ __| __| \\| |   \\/ __|     \n";
    cout << " | |__| _| (_ | _|| .` | |) \\__ \\     \n";
    cout << " |____|___\\___|___|_|\\_|___/|___/     \n";
    cout << " Game by nabil    \n";
    cout << "                      Talulu ver." << endl;
    cout << "===========================================" << endl;
}

void showCredits() {
    cout << "\n================ CREDITS ================" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "=========================================" << endl;
    cout << "=========================================\n" << endl;
}

void DragonSari() {
    cout << "<>=======() " << endl;
    cout << "(/\\___   /|\\\\          ()==========<>_" << endl;
    cout << "      \\_/ | \\\\        //|\\   ______/ \\)" << endl;
    cout << "        \\_|  \\\\      // | \\_/" << endl;
    cout << "          \\|\\/|\\\\_   //  /\\/" << endl;
    cout << "           (oo)\\ \\_//  /" << endl;
    cout << "          //_/\\_\\\\/ /  |" << endl;
    cout << "         @@/  |=\\  \\  |" << endl;
    cout << "              \\_=\\_ \\ |" << endl;
    cout << "                \\==\\ \\|\\\\_" << endl;
    cout << "             __(\\===\\(  )\\" << endl;
    cout << "            (((~) __(_/   |" << endl;
    cout << "                 (((~) \\  /" << endl;
    cout << "                 ______/ /" << endl;
    cout << "                 '------'" << endl;
}

// inventory system
struct Item {
    string name;
    int quantity;
    string description;
};
struct Inventory {
    int maxSlots = 20;
    vector<Item> items;
};

void addItem(Inventory &inv, const string& name, int qty, const string& desc) {
    for (auto &it : inv.items) {
        if (it.name == name) {
            it.quantity += qty;
            narrate("You picked up " + to_string(qty) + "x " + name + ".");
            return;
        }
    }
    if ((int)inv.items.size() >= inv.maxSlots) {
        narrate("Your bag is full, cant pick up no more.");
        return;
    }
    inv.items.push_back({name, qty, desc});
    narrate("You picked up " + to_string(qty) + "x " + name + ".");
}

void showInventory(const Inventory &inv) {
    cout << "\n======= INVENTORY =======" << endl;
    if (inv.items.empty()) {
        narrate("Bag is empty...");
        return;
    }
    int idx = 1;
    for (const auto &it : inv.items) {
        cout << idx << ". " << it.name << " x" << it.quantity 
             << " — " << it.description << endl;
        idx++;
    }
}

string selectItemFromInventory(Inventory &inv) {
    if (inv.items.empty()) {
        narrate("Empty inventory.");
        return "";
    }

    showInventory(inv);
    narrate("Choose the item you want to use: ");
    int choice;

    while (true) {
        cout << "> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            narrate("Unvalid input.");
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) return "";
        if (choice >= 1 && choice <= (int)inv.items.size())
            return inv.items[choice - 1].name;

        narrate("Come on man.");
    }
}


void cleanInventory(Inventory &inv) {
    // Remove any items that have zero or negative quantity to keep inventory tidy
    inv.items.erase(std::remove_if(inv.items.begin(), inv.items.end(),
        [](const Item &it) { return it.quantity <= 0; }), inv.items.end());
}

// ================= PLAYER & ENEMY =================
struct Enemy {
    string name;
    int maxHP;
    int hp;
    int attack;
    int defense;
    int speed;
    int expReward;
    string tier;
};

struct Player {
    string name;
    int maxHP = 100;
    int hp = 100;
    int attack = 12;
    int defense = 6;
    int speed = 5;
    int level = 1;
    int exp = 0;
    Inventory inv;
};
Enemy slime        = {"Slug", 25, 25, 5, 1, 1, 5, "T1"};
Enemy goblin       = {"Goblin", 40, 40, 8, 2, 4, 15, "T1"};
Enemy skeleton     = {"Skeleton", 60, 60, 10, 5, 3, 20, "T2"};
Enemy darkWolf     = {"Dark Wolf", 90, 90, 15, 6, 6, 35, "T3"};
Enemy cursedKnight = {"Cursed Knight", 140, 140, 22, 10, 5, 60,"T4"
//Enemy slime        = {"Infected Slug", 30, 30, 7, 1, 1, 5, "T1"};
//Enemy slime        = {"Slime", 35, 30, 7, 1, 1, 5, "T1"};
//Enemy slime        = {"Slime", 25, 25, 5, 1, 1, 5, "T1"};
//Enemy slime        = {"Slime", 25, 25, 5, 1, 1, 5, "T1"};
};
Enemy INFORMATIKAJAHAT = {"INFORMATIKA JAHAT", 300, 300, 15, 10, 8, 200, "T5"};//ini pak yang musuh baru(muncul di zona 4)

// ================= COMBAT MATH =================
int calculateDamage(int atk, int def) {
    double base = atk - (def * 0.5);
    if (base < 1) base = 1;
    double randomMultiplier = (rand() % 31 + 85) / 100.0; // 0.85 - 1.15
    int dmg = (int)(base * randomMultiplier);
    if (dmg < 1) dmg = 1;
    return dmg;
}
bool isCritical() { return (rand() % 100) < 10; } // 10%
bool isMiss()     { return (rand() % 100) < 5;  } // 5%

int playerAttack(Player &p, Enemy &e) {
    if (isMiss()) {
        narrate("You miss!");
        return 0;
    }
    bool crit = isCritical();
    int dmg = calculateDamage(p.attack, e.defense);
    if (crit) {
        dmg *= 2;
        narrate("Critical Hit!");
    }
    e.hp -= dmg;
    return dmg;
}
int enemyAttack(Player &p, Enemy &e) {
    if (isMiss()) {
        narrate(e.name + "Miss!");
        return 0;
    }
    bool crit = isCritical();
    int dmg = calculateDamage(e.attack, p.defense);
    if (crit) {
        dmg *= 2;
        narrate("[!] " + e.name + " CRITICAL HIT!");
    }
    p.hp -= dmg;
    return dmg;
}

// ================= EXP / LEVEL =================
void addExp(Player &p, int exp) {
    p.exp += exp;
    narrate("Kamu mendapat " + to_string(exp) + " EXP!");
    int required = p.level * 50;
    if (p.exp >= required) {
        p.level++;
        narrate("⬆ LEVEL UP! level " + to_string(p.level - 1) + to_string(p.level) + "!");
        p.maxHP += 10;
        p.attack += 3;
        p.defense += 2;
        p.speed += 1;
        p.hp = p.maxHP;
        p.exp -= required;
    }
}

void pressEnter() {
    cout << "\n( Press ENTER... )";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ================= ZONES / ENCOUNTERS =================

// SOME ZONE THINGY THAT KEEP BUGGING!
//WHAT THE BUG! -Habibi

void zone1(Player &player) {
    narrate("=== Zona 1: Ancient Grassland ===");
    showHUD(player);
    dialog(player.name," : How in the world is there a big ass grass field in here as well");
    dialog(player.name," : ...is that the sun in the sky? why is the sky inside?");
    dialog(player.name," : Am i still outside? im pretty i've went inside the cave");
    narrate("You continue to move forward...");
    narrate("Youre pretty sure youre already inside the 'Cave'");

    for (int i = 0; i < 5; i++) {

        int roll = rng(1, 100);

        if (roll <= 20) {
            showHUD(player);
            openChestZone1(player);
            pressEnter();
        }
        else if (roll <= 80) {
            showHUD(player);
            vector<Enemy> pool = {slime, goblin};
            vector<int> chance = {50, 50};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
        else {
            narrate("You keep walking and nothing fortunate or unfortunate happen.");
            narrate("Moving along.");
            pressEnter();
        }
    }
}

void zone2(Player &player) {
    narrate("=== Zona 2: Ruined Corridor ===");
    narrate("You walk down a staircase...");
    showHUD(player);
    dialog(player.name,"...Why is it so bright even though theres no lamp?");
    dialog(player.name,"I sure hope the ceiling wont fall on me...");
    dialog(player.name,"and I swear to God almighty if theres trap in here...");
    narrate("You walk further into the odd corridor...");

    for (int i = 0; i < 3; i++) {
        int roll = rng(1, 100);

        if (roll <= 50) {
            showHUD(player);
            openChestZone1(player);
            pressEnter();
        }
        else if (roll <= 80) {
            showHUD(player);
            vector<Enemy> pool = {slime, goblin};
            vector<int> chance = {50, 50};
            randomEncounter(player, pool, chance);
            pressEnter();
        }

        else {
            narrate("You keep walking and nothing fortunate or unfortunate happen.");
            narrate("Moving along.");
            pressEnter();
        }
    }
}

void zone3(Player &player) {
    narrate("=== Zona 3: Cadaver Cavern ===");
    showHUD(player);
    dialog(player.name,"Of course theres another cave at the end");
    dialog(player.name,"Though this does make a bit more sense, an actually dark cave and without grass or sun");
    narrate("You went into the hole on the wall..."); //is it spelling correct? belulang atau berulang
    //shut up its "is the spelling correct?" -Nabil

    for (int i = 0; i < 7; i++) {
        int roll = rng(1, 100);

        if (roll <= 60) {
            showHUD(player);
            openChestZone3(player);
            pressEnter();
        }
        else if (roll <= 90) {
            showHUD(player);
            vector<Enemy> pool = {slime, skeleton, goblin};
            vector<int> chance = {10, 60, 30};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
        else {
            narrate("You keep walking and nothing fortunate or unfortunate happen.");
            narrate("Moving along.");
            pressEnter();
        }
    }
}

void zone4(Player &player) {
    narrate("=== Zona 4: Forsaken Depths ===");
    showHUD(player);
    dialog(player.name,"This place starts to give me the chill man");
    narrate("But regardless you continue on..."); // no this game is unballace btw
    // Talulah is my favorite npc ever fr fr -Nabil
    // true -Nabil
    pressEnter();
    npcEncounter(player);
    pressEnter();

    // shit man , this thing is hard!
    for (int i = 0; i < 8; i++) {
        int roll = rng(1, 100);

        if (roll <= 60) {
            showHUD(player);
            openChestZone3(player);
            pressEnter();
        }
        else {
            showHUD(player);
            vector<Enemy> pool = {darkWolf, cursedKnight, goblin, INFORMATIKAJAHAT};
            vector<int> chance = {25, 25, 25, 25};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
    }
}
//what is this - nabil

void randomEncounter(Player &player, vector<Enemy> pool, vector<int> probs) {
    int roll = rng(1, 100);
    int acc = 0;

    for (int i = 0; i < pool.size(); i++) {
        acc += probs[i];
        if (roll <= acc) {
            battle(player, pool[i]);
            return;
        }
    }
}

// SOME LOOT SHIT
// shit indeed -Nabil
void openChestZone1(Player &player) {
    int roll = rng(1, 100);

    if (roll <= 33) {
        addItem(player.inv, "Potion", 1, "Heal 40 HP");
        narrate("You found Potion!");
    }
    else if (roll <= 66) {
        addItem(player.inv, "Bomb", 1, "Bomb 50 dmg");
        narrate("You found Bomb!");
    }
    else if (roll <= 88) {
        player.maxHP += 50;
        narrate("You found Heart Crystal! Max HP +50!");
    }
    else {
        player.attack += 25;
        narrate("You found X-Attack Potion! Attack +25 permanen!");
    }
}

void openChestZone3(Player &player) {
    int roll = rng(1, 100);

    if (roll <= 33) {
        addItem(player.inv, "Bomb", 1, "Bomb 50 dmg");
        narrate("You found Bomb!");
    }
    else if (roll <= 77) { // 33 + 44
        player.maxHP += 50;
        narrate("Kamu menemukan Heart Crystal! Max HP +50!");
    }
    else if (roll <= 89) { // 77 + 12
        player.attack += 25;
        narrate("Kamu menemukan X-Attack Potion! Attack +25 permanen!");
    }
    else { 
        addItem(player.inv, "Armor", 1, "Mengurangi 50% damage untuk 1 turn");
        narrate("Kamu menemukan Dragonite Armor! Mengurangi 50% dmg.");
    }
}

void npcEncounter(Player &player) {
    narrate("Someone suddenly appear out of nowhere..."); // da best character ever -Nabil
    typeText("???: ?...");
    typeText(player.name + ": Aku " + player.name + ", siapa kamu?");
    typeText("???: Aku Talulah, kesatria pelindung dungeon ini.");
    typeText("Talulah: sepertinya kamu juga mencari harta karun itu ya atau kamu cuma ingin mencari petualangan?.");
    typeText(player.name + ": Aku di sini untuk mencari 'Gold Amulet'.");
    typeText("Talulah: Hmmm... baiklah aku akan membantumu, tapi dengan satu syarat.");
    typeText("Talulah: Saat kamu menghadapi bos terakhir nanti, aku ingin bertarung di sisimu.");
    typeText(player.name + ": Tentu saja, aku akan mengalahkan boss itu.");
    typeText("Talulah: Bagus! Sebagai tanda persahabatan kita, terimalah ini.");
    typeText("Talulah: mungkin armor dan pedang ini saja yang bisa ku beri. Aku menuggu diluar dulu, aku lelah dan ingin beristirahat diluar dungeon.");
    pressEnter();
    
    player.maxHP += 150;
    player.attack += 50;

    narrate("Max HP +150!");
    narrate("Attack +50!");
    narrate("dia pergi meninggalkanmu...");
}



// ================= ITEM EFFECTS (in-battle) =================
// you know what yeah this is the most buggiest thing ive code
bool useItemInBattle(Player &player, Enemy &enemy, const string &itemNameInput) {
    if (itemNameInput.empty()) return false;
    string key = toLowerTrim(itemNameInput);

    for (auto &it : player.inv.items) {
        if (toLowerTrim(it.name) == key && it.quantity > 0) {
            // apply effect
            it.quantity--;
            narrate("You use " + it.name + ".");

            if (toLowerTrim(it.name) == "potion") {
                int heal = 40;
                player.hp = min(player.maxHP, player.hp + heal);
                narrate("HP Recovered +" + to_string(heal) + ".");
            }
            else if (toLowerTrim(it.name) == "bomb") {
                int dmg = 50;
                enemy.hp -= dmg;
                narrate("The Bomb explode! " + enemy.name + " takes " + to_string(dmg) + " damage!");
            }
            else if (toLowerTrim(it.name) == "sword") {
                int inc = 5;
                player.attack += inc;
                narrate("The blade is sharpened! Attack permanen +" + to_string(inc) + ".");
            }
            else if (toLowerTrim(it.name) == "armor") {
                // armor memberikan cuma se turn
                narrate("Armor activated, reduce incoming damage by half once (50%).");
                // ngebug bruh
                player.defense = player.defense * 2;
            }
            else {
                narrate("This has no use in here.");
            }
            return true;
        }
    }
    narrate("Item is either gone or you run out.");
    return false;
}

// this function is from github and i dont know when this will be , so yeah ill stick it there and see if in future this will be usefull
bool useItemOutOfBattle(Player &player, const string &itemNameInput) {
    if (itemNameInput.empty()) return false;
    string key = toLowerTrim(itemNameInput);

    for (auto &it : player.inv.items) {
        if (toLowerTrim(it.name) == key && it.quantity > 0) {
            if (key == "sword") {
                it.quantity--;
                player.attack += 5;
                narrate(" You strap the swordon your hip. Attack +5.");
                return true;
            } else if (key == "potion") {
                narrate("Potion could only be used in battle...for some reason.");
                return false;
            } else if (key == "bomb") {
                narrate("Bomb is unactivate for now, could be used in battle obviously.");
                return false;
            } else {
                narrate("This has no use here.");
                return false;
            }
        }
    }
    narrate("Item is either gone or you run out.");
    return false;
}


void showHUD(const Player &p) {
    cout << "\n======================" << endl;
    cout << "   STATUS PLAYER" << endl;
    cout << "======================" << endl;
    cout << "Name     : " << p.name << endl;
    cout << "Level    : " << p.level << endl;
    cout << "HP       : " << p.hp << "/" << p.maxHP << endl;
    cout << "Attack   : " << p.attack << endl;
    cout << "Defense  : " << p.defense << endl;
    cout << "Speed    : " << p.speed << endl;
    cout << "EXP      : " << p.exp << " / " << (p.level * 50) << endl;

    // Item penting
    bool hasAmulet = false;
    for (auto &i : p.inv.items)
        if (i.name == "Golden Amulet") hasAmulet = true; // bruh

    cout << "Amulet   : " << (hasAmulet ? "✓ Dimiliki" : "-") << endl;

    cout << "======================\n" << endl;
}


// battlin`
void battle(Player &player, Enemy enemy) { //psstt use bomb, its powerful
    system("cls");
    narrate("Fight!");
    narrate(player.name + " Against " + enemy.name);

    while (player.hp > 0 && enemy.hp > 0) {
        cout << "\n=== STATUS ===" << endl;
        cout << "Your Health : " << player.hp << "/" << player.maxHP << endl;
        cout << "Enemy Health: " << max(0, enemy.hp) << "/" << enemy.maxHP << endl;

        cout << "\nWhat will you do?" << endl;
        cout << "1. Attack" << endl;
        cout << "2. Use Item" << endl;
        cout << "3. Defend" << endl;
        cout << "4. Escape (40% chance)" << endl;

        int choice = chooseOption(1, 4);
        int origDefense = player.defense;

if (choice == 1) {
    int dmg = playerAttack(player, enemy);
    narrate("Kamu deal " + to_string(dmg) + " damage!");
} else if (choice == 2) {
    string selected = selectItemFromInventory(player.inv);
    if (selected.empty()) {
        narrate("You put it back inside.");
        continue;
    }

    bool used = useItemInBattle(player, enemy, selected);
    cleanInventory(player.inv);

    if (!used) {
        narrate("You put it back inside.");
        continue;
    }

    // if enemy dies due to bomb, skip enemy turn
    if (enemy.hp <= 0) break;
} else if (choice == 3) {
            narrate("You bring up your shield, Defense + 30%!");
            player.defense = (int)(player.defense * 1.3);
        } else if (choice == 4) {
            if (rand() % 100 < 40) {
                narrate("Escaped Successfully!");
                return;
            } else {
                narrate("Escaped Unsuccessfully!");
            }
        }

        // Enemy turn (if alive) yeah if alive
        if (enemy.hp > 0) {
            int edmg = enemyAttack(player, enemy);
            narrate(enemy.name + " Deal " + to_string(edmg) + " damage!");
        }
        
        player.defense = origDefense;
    }

    if (player.hp <= 0) {
        narrate("\n You die...");
        return;
    }

    narrate("\n You killed " + enemy.name + "!");
    addExp(player, enemy.expReward);
}

// BOSS
// AI: 3 moves: Attack(+20 flat), Heal(+10), Dodge (20% evade for 1 turn).
void bossBattle_DraconicMaster(Player &player) {
    system("cls");
    narrate(" You reach yet another cave...");
    narrate("The air feels heavy, there is no light, and the ground is shaking, something is wrong here");
    narrate("Suddenly hundreds of torches on the wall lit up revealing a giant dragon charging at you while roaring its lungs out");
    typeText("DRACONIC MASTER is here...");
    DragonSari();
    Enemy boss = {"DRACONIC MASTER", 300, 300, 25, 10, 8, 200, "Boss"};
    bool bossDodge = false;
    int dodgeCooldown = 0;

    while (player.hp > 0 && boss.hp > 0) {
        showHUD(player);
        cout << "\n=== STATUS ===" << endl;
        cout << player.name << " HP: " << player.hp << "/" << player.maxHP << endl;
        cout << boss.name << " HP: " << boss.hp << "/" << boss.maxHP << endl;

        cout << "\nWhat will you do?" << endl;
        cout << "1. Attack" << endl;
        cout << "2. Use Item" << endl;
        cout << "3. Defend" << endl;

        int choice = chooseOption(1, 3);
        int origDef = player.defense;

    // Player turn
    if (choice == 1) {
        if (bossDodge && (rand() % 100 < 20)) {
            narrate("DRACONIC MASTER dodge your attack!");
        } else {
            int dmg = playerAttack(player, boss);
            narrate("You deal " + to_string(dmg) + " damage!");
        }
    } else if (choice == 2) {
        string selected = selectItemFromInventory(player.inv);
        if (!selected.empty()) {
            useItemInBattle(player, boss, selected);
            cleanInventory(player.inv);
            if (boss.hp <= 0) break;
        } else {
            narrate("You put it back inside.");
        }
    } else if (choice == 3) {
        narrate("You bring up your shield, Defense + 30%!");
        player.defense = (int)(player.defense * 1.3);
    }

        bossDodge = false;

        // Boss AI move
        if (boss.hp > 0) {
            int move;
            if (boss.hp < boss.maxHP * 0.4 && (rand() % 100 < 55)) move = 2; // heal more likely
            else if (dodgeCooldown == 0 && (rand() % 100 < 20)) move = 3; // dodge
            else move = 1; // attack

            if (move == 1) {
                narrate("\nDRACONIC MASTER uses Dragon Strike!");
                int dmg = calculateDamage(boss.attack + 20, player.defense);
                player.hp -= dmg;
                narrate("Kamu takes " + to_string(dmg) + " damage!");
            } else if (move == 2) {
                narrate("\nDRACONIC MASTER uses Dragon Heal!");
                boss.hp += 10;
                if (boss.hp > boss.maxHP) boss.hp = boss.maxHP;
                narrate("Boss recover 10 HP!");
            } else if (move == 3) {
                narrate("\nDRACONIC MASTER uses Dragon Dodge! (20% evade next player attack)");
                bossDodge = true;
                dodgeCooldown = 3;
            }
        }

        // reset player's defense if used defend
        player.defense = origDef;
        if (dodgeCooldown > 0) dodgeCooldown--;
    }

    if (player.hp <= 0) {
        narrate("\nYou died...");
        narrate("BAD ENDING — Failure");
        narrate("???: at least you tried");
        narrate("Nerf the dragon"); //-Nabil
        narrate("                 -Nabil");
        pressEnter();
        narrate("???: Im sorry but please try again " + player.name);
        pressEnter();
        return;
    }

    // Victory
    narrate("\n You defeated DRACONIC MASTER!");
    typeText("The dragon turns to dust before forming into a golden amulet .");
    addItem(player.inv, "Golden Amulet", 1, "The legendary Artifact.");
    narrate("\n=== GOOD ENDING — To Be Continued ===");
    showCredits();
    pressEnter();
}

// ================= STORY ACTS =================
void act0() {
    narrate("Dungeon Legends...");
    narrate("Prepare to fun.");
}

void act1(const string &name) {
    narrate("\nFor no particular reason you decided to get out of bed that day.");
    narrate("You might as well look for that mysterious 'DRACONIC DUNGEON' far up in the north.");
    narrate("???: Go ahead");
    narrate("1. Okay\n2. No?");

    int c = chooseOption(1,2);

    switch (c) {
        case 1:
            narrate("Turns out, the 'dungeon' is nothing but a small cave in the middle of nowhere.");
             narrate("It is surrounded by a field of grass.");
            narrate("You see an old man waiting by the entrance under the shade of a tree.");
            typeText("Old man: \"Hello there...\"");
            narrate("He pulls out his sword and held it against your throat");
            typeText("Old man: \"Give me everything you have or you'll die\"");
            narrate("You hear a creak before suddenly the tree fell down, crushing the old man beneath it");
            narrate("After a moment or two you decide to loot his still-warm body");
            break;

        case 2:
            narrate("The words seems to get stuck in your throat");
            narrate("You feel a tinge of dissapointment from...somewhere");
            narrate("You decided to turn bac--");
            pressEnter();
            narrate("=== ENDING: Im sorry ===");
            exit(0);
            break;

        default:
            narrate("Not that one.");
            break;
    }
}
void act1_placeholder() {
    narrate("Kamu memasuki dungeon... seorang pria tua bernama Renville muncul.");
    typeText("Renville: \"Salam...petualang\"");
    typeText("Renville memberikan beberapa barang untuk memulai perjalananmu.");
    narrate("Kamu menerima: Pedang x1, Potion x5, Bom x5");
    pressEnter();
}

void act2_placeholder(const string &name) {
    narrate("\nKamu melangkah lebih dalam ke dungeon..."); //this thing is useless -Habibi
    narrate("Tiba-tiba, sesuatu terjadi!");
    typeText(">"+ name +" : ANJIR ada goblin!");
    narrate("a wild goblin appeared!");
}

void act4(const string &name) {
    narrate("kamu melanjutkan perjalananmu lebih dalam ke dungeon...");
    narrate("kamu memasuki sebuah ruangan besar yang dipenuhi dengan tulang belulang dan reruntuhan kuno.");
    narrate("Nikmati petualanganmu di zona 2." + name);
}

// ================= MAIN =================
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    system("cls");
    showTitle();
    cout << "\nWelcome!\n(Enter untuk lanjut)";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    act0();
    narrate("\nI know that YOU know exactly what this is.");
    
    narrate("\nYou hear a voice from the beyond");
    cout << "\n???: Who are you?" << endl << "> ";
    string playerName;
    getline(cin, playerName);
    if (playerName.empty()) playerName = "Adventurer";

    // create player
    Player player;
    player.name = playerName;
    player.maxHP = 100;
    player.hp = 100;
    player.attack = 12;
    player.defense = 6;

    system("cls");
    act1(playerName);

    // give starting items (now via code)
    addItem(player.inv, "Sword", 1, "The most basic one (+5 Attack)");
    addItem(player.inv, "Potion", 1, "Heal 40 HP.");
    addItem(player.inv, "Bomb", 2, "Deal 50 damage to the enemy.");

    // omg this is frustating!
    zone1(player);
    zone2(player);
    zone3(player);
    zone4(player);
    bossBattle_DraconicMaster(player);
    narrate("\n=== Thanks        ===");
    narrate("\n===               ===");
    narrate("\n===       -Talulah===");
    pressEnter();
    return 0;
}
