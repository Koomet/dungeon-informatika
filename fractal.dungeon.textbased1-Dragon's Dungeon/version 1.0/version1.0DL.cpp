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
    cout << "\n> Pilihanmu: ";
    while (!(cin >> choice) || choice < min || choice > max) {
        cout << "Masukkan angka antara " << min << " dan " << max << ": ";
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
    cout << "                      game by horazonka" << endl;
    cout << "===========================================" << endl;
}

void showCredits() {
    cout << "\n================ CREDITS ================" << endl;
    cout << "Game Design & Programming: Habibi" << endl;
    cout << "Story & Dialogs: Habibi" << endl;
    cout << "Testing & Feedback: Nabil, Habibi" << endl;
    cout << "Special Thanks to: All friends who supported this project!" << endl;
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
            narrate("Kamu mendapatkan " + to_string(qty) + "x " + name + ".");
            return;
        }
    }
    if ((int)inv.items.size() >= inv.maxSlots) {
        narrate("Tasmu penuh! Tidak bisa menambah item baru.");
        return;
    }
    inv.items.push_back({name, qty, desc});
    narrate("Kamu mendapatkan " + to_string(qty) + "x " + name + ".");
}

void showInventory(const Inventory &inv) {
    cout << "\n======= INVENTORY =======" << endl;
    if (inv.items.empty()) {
        narrate("Tas kamu kosong...");
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
        narrate("Inventory kosong.");
        return "";
    }

    showInventory(inv);
    narrate("Pilih item dengan nomor (atau 0 untuk batal): ");
    int choice;

    while (true) {
        cout << "> ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            narrate("Input tidak valid.");
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) return "";
        if (choice >= 1 && choice <= (int)inv.items.size())
            return inv.items[choice - 1].name;

        narrate("Masukkan nomor yang benar.");
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
Enemy slime        = {"Slime", 25, 25, 5, 1, 1, 5, "T1"};
Enemy goblin       = {"Goblin", 40, 40, 8, 2, 4, 15, "T1"};
Enemy skeleton     = {"Skeleton", 60, 60, 10, 5, 3, 20, "T2"};
Enemy darkWolf     = {"Dark Wolf", 90, 90, 15, 6, 6, 35, "T3"};
Enemy cursedKnight = {"Cursed Knight", 140, 140, 22, 10, 5, 60, "T4"};
Enemy INFORMATIKAJAHAT = {"INFORMATIKA JAHAT", 300, 300, 15, 10, 8, 200, "T5"};

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
        narrate("Seranganmu meleset!");
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
        narrate(e.name + " meleset menyerangmu!");
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
        narrate("⬆ LEVEL UP! Kamu naik ke level " + to_string(p.level) + "!");
        p.maxHP += 10;
        p.attack += 3;
        p.defense += 2;
        p.speed += 1;
        p.hp = p.maxHP;
        p.exp -= required;
    }
}

void pressEnter() {
    cout << "\n( Tekan ENTER untuk melanjutkan... )";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ================= ZONES / ENCOUNTERS =================

// SOME ZONE THINGY THAT KEEP BUGGING!
//WHAT THE BUG! -Habibi

void zone1(Player &player) {
    narrate("=== Zona 1: Ancient Grassland ===");
    showHUD(player);
    dialog(player.name," : jadi ini dia dungeon yang katanya menyimpan harta karun legendaris");
    dialog(player.name," : paling paling cuma monster lemah seperti slime atau goblin");
    narrate("Kamu memasuki area pertama dungeon...");

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
            narrate("Kamu berjalan... tapi tidak terjadi apa-apa.");
            narrate("kamu kembali melanjutkan perjalanan.");
            pressEnter();
        }
    }
}

void zone2(Player &player) {
    narrate("=== Zona 2: Ruined Corridor ===");
    showHUD(player);
    dialog(player.name,"wah ruangan baru, ini sepertinya kayak koridor panjang gitu");
    dialog(player.name,"semoga gak ada jebakan atau apalah disini");
    narrate("Kamu melangkah ke koridor yang mencekam...");

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
            narrate("Kamu berjalan... tapi tidak terjadi apa-apa.");
            narrate("kamu kembali melanjutkan perjalanan.");
            pressEnter();
        }
    }
}

void zone3(Player &player) {
    narrate("=== Zona 3: Cadaver Cavern ===");
    showHUD(player);
    dialog(player.name,"ini tempatnya agak gelap dan menyeramkan");
    narrate("Kamu memasuki gua yang dipenuhi tulang tulang belulang..."); //is it spelling correct? belulang atau berulang

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
            narrate("Kamu berjalan... tapi tidak terjadi apa-apa.");
            narrate("kamu kembali melanjutkan perjalanan.");
            pressEnter();
        }
    }
}

void zone4(Player &player) {
    narrate("=== Zona 4: Forsaken Depths ===");
    showHUD(player);
    dialog(player.name,"sepertinya ini adalah zona paling berbahaya disini");
    narrate("Kamu memasuki kedalaman dungeon yang lebih berbahaya..."); // no this game is unballace btw
    // Talulah is my favorite npc ever fr fr -Nabil
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
void openChestZone1(Player &player) {
    int roll = rng(1, 100);

    if (roll <= 33) {
        addItem(player.inv, "Potion", 1, "Heal 40 HP");
        narrate("Kamu menemukan Potion!");
    }
    else if (roll <= 66) {
        addItem(player.inv, "Bomb", 1, "Bomb 50 dmg");
        narrate("Kamu menemukan Bomb!");
    }
    else if (roll <= 88) {
        player.maxHP += 50;
        narrate("Kamu menemukan Heart Crystal! Max HP +50!");
    }
    else {
        player.attack += 25;
        narrate("Kamu menemukan X-Attack Potion! Attack +25 permanen!");
    }
}

void openChestZone3(Player &player) {
    int roll = rng(1, 100);

    if (roll <= 33) {
        addItem(player.inv, "Bomb", 1, "Bomb 50 dmg");
        narrate("Kamu menemukan Bomb!");
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
    narrate("Seorang kesatria wanita muncul..."); // da best character ever -Nabil
    typeText("???: hei siapa namamu?...");
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
            narrate("Kamu menggunakan " + it.name + ".");

            if (toLowerTrim(it.name) == "potion") {
                int heal = 40;
                player.hp = min(player.maxHP, player.hp + heal);
                narrate("HP pulih +" + to_string(heal) + ".");
            }
            else if (toLowerTrim(it.name) == "bomb") {
                int dmg = 50;
                enemy.hp -= dmg;
                narrate("Bom meledak! " + enemy.name + " menerima " + to_string(dmg) + " damage!");
            }
            else if (toLowerTrim(it.name) == "sword") {
                int inc = 5;
                player.attack += inc;
                narrate("Pedang diasah! Attack permanen +" + to_string(inc) + ".");
            }
            else if (toLowerTrim(it.name) == "armor") {
                // armor memberikan cuma se turn
                narrate("Armor aktif! Mengurangi damage sekali ini (50%).");
                // ngebug bruh
                player.defense = player.defense * 2;
            }
            else {
                narrate("Item ini belum punya efek di pertempuran.");
            }
            return true;
        }
    }
    narrate("Item tidak ditemukan atau sudah habis.");
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
                narrate(" Kamu mengikat pedang ke punggungmu. Attack +5.");
                return true;
            } else if (key == "potion") {
                narrate("Potion hanya bisa digunakan di battle.");
                return false;
            } else if (key == "bomb") {
                narrate("Bomb disimpan dengan aman. Gunakan saat bertarung.");
                return false;
            } else {
                narrate("Item tidak punya aksi di luar pertempuran.");
                return false;
            }
        }
    }
    narrate("Item tidak ditemukan atau sudah habis.");
    return false;
}


void showHUD(const Player &p) {
    cout << "\n======================" << endl;
    cout << "   STATUS PLAYER" << endl;
    cout << "======================" << endl;
    cout << "Nama     : " << p.name << endl;
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
    narrate("Pertempuran dimulai!");
    narrate(player.name + " VS " + enemy.name);

    while (player.hp > 0 && enemy.hp > 0) {
        cout << "\n=== STATUS ===" << endl;
        cout << "HP Kamu : " << player.hp << "/" << player.maxHP << endl;
        cout << "HP Musuh: " << max(0, enemy.hp) << "/" << enemy.maxHP << endl;

        cout << "\nApa yang akan kamu lakukan?" << endl;
        cout << "1. Serang" << endl;
        cout << "2. Gunakan Item" << endl;
        cout << "3. Bertahan (+30% defense 1 turn)" << endl;
        cout << "4. Kabur (40% chance)" << endl;

        int choice = chooseOption(1, 4);
        int origDefense = player.defense;

if (choice == 1) {
    int dmg = playerAttack(player, enemy);
    narrate("Kamu memberi " + to_string(dmg) + " damage!");
} else if (choice == 2) {
    string selected = selectItemFromInventory(player.inv);
    if (selected.empty()) {
        narrate("Batal menggunakan item.");
        continue;
    }

    bool used = useItemInBattle(player, enemy, selected);
    cleanInventory(player.inv);

    if (!used) {
        narrate("Gagal menggunakan item.");
        continue;
    }

    // if enemy dies due to bomb, skip enemy turn
    if (enemy.hp <= 0) break;
} else if (choice == 3) {
            narrate("Kamu bertahan dengan perisai mu... Defense naik 30%!");
            player.defense = (int)(player.defense * 1.3);
        } else if (choice == 4) {
            if (rand() % 100 < 40) {
                narrate("Berhasil kabur!");
                return;
            } else {
                narrate("Gagal kabur!");
            }
        }

        // Enemy turn (if alive) yeah if alive
        if (enemy.hp > 0) {
            int edmg = enemyAttack(player, enemy);
            narrate(enemy.name + " menyerang dan memberi " + to_string(edmg) + " damage!");
        }
        
        player.defense = origDefense;
    }

    if (player.hp <= 0) {
        narrate("\n Kamu mati...");
        return;
    }

    narrate("\n Kamu membunuh " + enemy.name + "!");
    addExp(player, enemy.expReward);
}

// BOSS
// AI: 3 moves: Attack(+20 flat), Heal(+10), Dodge (20% evade for 1 turn).
void bossBattle_DraconicMaster(Player &player) {
    system("cls");
    narrate(" Kamu telah mencapai ruang terdalam...");
    narrate("suasana disini begitu mencekam, ruangan gelap dan juga agak lambab,seperti kau bisa merasakan kejanggalan disini");
    narrate("kemudian dengan secara tiba tiba ruangan menjadi terang karena obor obor dan lampu menyala--membara dengan api yang datang entah darimana");
    typeText("Suara berat menggema: \"Siapa yang berani menggangguku?\"");
    typeText("DRACONIC MASTER muncul, menggeram...");
    DragonSari();
    Enemy boss = {"DRACONIC MASTER", 300, 300, 25, 10, 8, 200, "Boss"};
    bool bossDodge = false;
    int dodgeCooldown = 0;

    while (player.hp > 0 && boss.hp > 0) {
        showHUD(player);
        cout << "\n=== STATUS ===" << endl;
        cout << player.name << " HP: " << player.hp << "/" << player.maxHP << endl;
        cout << boss.name << " HP: " << boss.hp << "/" << boss.maxHP << endl;

        cout << "\nAksi kamu?" << endl;
        cout << "1. Serang" << endl;
        cout << "2. Gunakan Item" << endl;
        cout << "3. Bertahan" << endl;

        int choice = chooseOption(1, 3);
        int origDef = player.defense;

    // Player turn
    if (choice == 1) {
        if (bossDodge && (rand() % 100 < 20)) {
            narrate("DRACONIC MASTER menghindari seranganmu!");
        } else {
            int dmg = playerAttack(player, boss);
            narrate("Kamu memberi " + to_string(dmg) + " damage!");
        }
    } else if (choice == 2) {
        string selected = selectItemFromInventory(player.inv);
        if (!selected.empty()) {
            useItemInBattle(player, boss, selected);
            cleanInventory(player.inv);
            if (boss.hp <= 0) break;
        } else {
            narrate("Batal menggunakan item.");
        }
    } else if (choice == 3) {
        narrate("Kamu bertahan. Defense +30% giliran ini.");
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
                narrate("Kamu menerima " + to_string(dmg) + " damage!");
            } else if (move == 2) {
                narrate("\nDRACONIC MASTER uses Dragon Heal!");
                boss.hp += 10;
                if (boss.hp > boss.maxHP) boss.hp = boss.maxHP;
                narrate("Boss memulihkan 10 HP!");
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
        narrate("\nKamu terbunuh oleh DRACONIC MASTER...");
        narrate("BAD ENDING — The Dragon's Dominion");
        narrate("yap kamu kalah dan game over");
        narrate("Nerf the dragon"); //-Nabil
        narrate("                 -Nabil");
        pressEnter();
        narrate("bukan.... ini bukan akhir , yaaa kamu memang mati ,tapi sadarkah kamu bahwa mati disini lebih terhormat daripada dizona awal");
        narrate("paling tidak ini usaha terbaik mu " + player.name);
        pressEnter();
        return;
    }

    // Victory
    narrate("\n Kamu berhasil mengalahkan DRACONIC MASTER!");
    typeText("Sosok raksasa runtuh... sebuah benda emas jatuh di hadapanmu.");
    addItem(player.inv, "Golden Amulet", 1, "Artefak legendaris - bukti kemenanganmu.");
    narrate("\n=== GOOD ENDING — DRAGON'S LEGACY ===");
    showCredits();
    pressEnter();
}

// ================= STORY ACTS =================
void act0() {
    narrate("Dungeon Legends — sebuah game RPG berbasis teks penuh pilihan dan misteri...");
    narrate("Nikmati petualanganmu.");
}

void act1(const string &name) {
    narrate("\nKamu, " + name + ", bermimpi menjadi petualang sejati.");
    narrate("Di suatu hari kamu menemukan peta menuju 'DRACONIC DUNGEON'.");
    narrate("Apakah kamu ingin masuk?");
    narrate("1. Masuk\n2. Pergi");

    int c = chooseOption(1,2);

    switch (c) {
        case 1:
            narrate("Kamu memasuki dungeon... seorang pria tua bernama Renville muncul.");
            typeText("Renville: \"Salam...petualang\"");
            typeText("Renville memberikan beberapa barang untuk memulai perjalananmu.");
            narrate("Kamu menerima: Pedang ×1, Potion ×5, Bom ×5");
            break;

        case 2:
            narrate("Kamu pergi dari gerbang tersebut menuju ke tempat lain. Untuk suatu petualangan yang lain… .");
            narrate("So yeah kamu kehilangan kesempatan...");
            pressEnter();
            narrate("=== ENDING: Coward's giving up ===");
            showCredits();
            pressEnter();
            exit(0);
            break;

        default:
            narrate("Pilihan tidak valid.");
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
    cout << "\nSelamat datang di Dungeon Legends!\n(Enter untuk lanjut)";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    act0();
    narrate("\nDungeon Legends. Sebuah game rpg yang dikemas dalam konsep text based game.");
    narrate("\nMungkin tidak banyak yang aku bisa katakan untuk game ini yang sebenarnya adalah tugas informatika, jadi ya enjoy… nikmati gameplay nya dan storynya");

    cout << "\nMasukkan nama karaktermu: " << endl << "> ";
    string playerName;
    getline(cin, playerName);
    if (playerName.empty()) playerName = "Petualang";

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
    addItem(player.inv, "Sword", 1, "Pedang dasar. Menambah Attack permanen (+5 saat dipakai).");
    addItem(player.inv, "Potion", 5, "Menyembuhkan 40 HP.");
    addItem(player.inv, "Bomb", 5, "Memberi 50 damage langsung ke musuh.");

    // omg this is frustating!
    zone1(player);
    zone2(player);
    zone3(player);
    zone4(player);
    bossBattle_DraconicMaster(player);
    narrate("\n=== Terima kasih sudah bermain ===");
    pressEnter();
    return 0;
}
// so its turns out that i forgot to ballance the game, you can spam 1 to just beat it