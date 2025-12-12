#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;
int rng(int a, int b) {
    return rand() % (b - a + 1) + a;
}


void showTitle() {
    cout << "========================================== \n";
    cout << "  ___  _   _ _  _  ___ ___ ___  _  _  \n";
    cout << " |   \\| | | | \\| |/ __| __/ _ \\| \\| | \n";
    cout << " | |) | |_| | .` | (_ | _| (_) | .` | \n";
    cout << " |___/ \\___/|_|\\_|\\___|___\\___/|_|\\_| \n";
    cout << "  /  \\ ___| |_\\__\\__\\__\\__\\__\\__\\__\\_\\ \n";
    cout << " | () |___| (_-<_-<_-<_-<_-<_-<_-< '_|/\n";
    cout << "  \\__/    |_/__/__/__/__/__/__/__/__/\n";
    cout << " _______________________________\n";
    cout << " | |  | __/ __| __| \\| |   \\/ __|     \n";
    cout << " | |__| _| (_ | _|| .` | |) \\__ \\     \n";
    cout << " |____|___\\___|___|_|\\_|___/|___/     \n";
    cout << "                      game by horazonka" << endl;
    cout << "===========================================";
}    
    
// ===== Efek Ketikan (HANYA UNTUK DIALOG) =====
void typeText(const string& text, int delay = 30) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
    cout << endl;
}

// ===== Narasi Cepat (Muncul Langsung) =====
void narrate(const string& text) {
    cout << text << endl;
}

// ===== Pilihan Input =====
int chooseOption(int min, int max) {
    int choice;
    cout << "\n> Pilihanmu: ";
    cin >> choice;
    while (choice < min || choice > max) {
        cout << "Masukkan angka antara " << min << " dan " << max << ": ";
        cin >> choice;
    }
    return choice;
}

// ===== Dialog Karakter (nama + teks diketik) =====
void dialog(const string& speaker, const string& line, int delay = 25) {
    cout << speaker << ": "; 
    typeText(line, delay);    
}

// ===============================
// ===== INVENTORY SYSTEM ========
// ===============================

struct Item {
    string name;
    int quantity;
    string description;
};

struct Inventory {
    int maxSlots = 20; // bisa kamu ubah nanti
    vector<Item> items;
};

// Tambah item
void addItem(Inventory &inv, string name, int qty, string desc) {
    for (auto &it : inv.items) {
        if (it.name == name) {
            it.quantity += qty;
            narrate("Kamu mendapatkan " + to_string(qty) + "x " + name + ".");
            return;
        }
    }

    if ((int)inv.items.size() >= inv.maxSlots) {
        narrate("Tas kamu penuh! Tidak bisa menambah item baru.");
        return;
    }

    inv.items.push_back({name, qty, desc});
    narrate("Kamu mendapatkan " + to_string(qty) + "x " + name + ".");
}

// Tampilkan inventory
void showInventory(const Inventory &inv) {
    narrate("\n======= INVENTORY =======");

    if (inv.items.empty()) {
        narrate("Tas kamu kosong...");
        return;
    }

    for (auto &it : inv.items) {
        cout << "• " << it.name << " x" << it.quantity
             << " — " << it.description << endl;
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
        narrate("Kamu menemukan Mystic Armor! Mengurangi 50% dmg.");
    }
}

void npcEncounter(Player &player) {
    narrate("Seorang kesatria wanita muncul...");
    dialog("???:", "hei siapa namamu?...");
    dialog(player.name, " : Aku " + player.name + ", siapa kamu?");
    dialog("???:", "Aku Talullah, kesatria pelindung dungeon ini.");
    dialog("Talullah:", "sepertinya kamu juga mencari harta karun itu ya atau kamu cuma ingin mencari petualangan?.");
    dialog(player.name, " : Aku di sini untuk mencari 'Gold Amulet'.");
    dialog("Talullah:", "Hmmm... baiklah aku akan membantumu, tapi dengan satu syarat.");
    dialog("Talullah:", "Saat kamu menghadapi bos terakhir nanti, aku ingin bertarung di sisimu.");
    dialog(player.name, " : Tentu saja, aku akan mengalahkan boss itu.");
    dialog("Talullah:", "Bagus! Sebagai tanda persahabatan kita, terimalah ini.");
    dialog("Talullah:", "mungkin armor dan pedang ini saja yang bisa ku beri. Aku menuggu diluar dulu, aku lelah dan ingin beristirahat diluar dungeon.");
    
    player.maxHP += 150;
    player.attack += 50;

    narrate("Max HP +150!");
    narrate("Attack +50!");
    narrate("dia pergi meninggalkanmu...");
}



// Gunakan item (template, logic bisa kamu isi)
bool useItem(Inventory &inv, string targetItem) {
    for (auto &it : inv.items) {
        if (it.name == targetItem && it.quantity > 0) {
            it.quantity--;

            narrate("Kamu menggunakan " + it.name + ".");

            // Tambahkan logika efek item nanti:
            // contoh: heal HP, tambah attack, dll

            return true;
        }
    }
    narrate("Item tidak ditemukan atau sudah habis.");
    return false;
}

// SOME ZONE THINGY THAT KEEP BUGGING!
//WHAT THE BUG! -Habibi

void zone1(Player &player) {
    narrate("=== Zona 1: Ancient Grassland ===");

    for (int i = 0; i < 5; i++) {

        int roll = rng(1, 100);

        if (roll <= 20) {
            openChestZone1(player);
        }
        else if (roll <= 80) {
            vector<Enemy> pool = {slime, goblin};
            vector<int> chance = {50, 50};
            randomEncounter(player, pool, chance);
        }
        else {
            narrate("Kamu berjalan... tapi tidak terjadi apa-apa.");
        }
    }
}

void zone2(Player &player) {
    narrate("=== Zona 2: Ruined Corridor ===");

    for (int i = 0; i < 3; i++) {
        int roll = rng(1, 100);

        if (roll <= 50) {
            openChestZone1(player);
        }
        else {
            vector<Enemy> pool = {slime, goblin};
            vector<int> chance = {50, 50};
            randomEncounter(player, pool, chance);
        }
    }
}

void zone3(Player &player) {
    narrate("=== Zona 3: Lower Crypt ===");

    for (int i = 0; i < 5; i++) {
        int roll = rng(1, 100);

        if (roll <= 60) {
            openChestZone3(player);
        }
        else {
            vector<Enemy> pool = {slime, skeleton, goblin};
            vector<int> chance = {30, 40, 30};
            randomEncounter(player, pool, chance);
        }
    }
}

void zone4(Player &player) {
    narrate("=== Zona 4: Forsaken Depths ===");

    // Encounter pertama selalu NPC
    npcEncounter(player);

    // 3 encounter lagi
    for (int i = 0; i < 3; i++) {
        int roll = rng(1, 100);

        if (roll <= 60) {
            openChestZone3(player);
        }
        else {
            vector<Enemy> pool = {darkWolf, cursedKnight};
            vector<int> chance = {50, 50};
            randomEncounter(player, pool, chance);
        }
    }
}



// =======================================
// ======   RPG MATHEMATICAL COMBAT  =====
// =======================================

struct Enemy {
    string name;
    int maxHP;
    int hp;
    int attack;
    int defense;
    int speed;
    int expReward; // optional: EXP system
    string tier;
};

struct Player {
    string name;
    int maxHP = 100;
    int hp    = 100;
    int attack = 12;
    int defense = 6;
    int speed   = 5;
    int level   = 1;
    int exp     = 0;

    Inventory inv;
};
Enemy slime        = {"Slime", 25, 25, 5, 1, 1, 5, "T1"};
Enemy goblin       = {"Goblin", 40, 40, 8, 2, 4, 15, "T1"};
Enemy skeleton     = {"Skeleton", 60, 60, 10, 5, 3, 20, "T2"};
Enemy darkWolf     = {"Dark Wolf", 90, 90, 15, 6, 6, 35, "T3"};
Enemy cursedKnight = {"Cursed Knight", 140, 140, 22, 10, 5, 60, "T4"};


// ===== Damage Formula =====
// Rumus: 
// base = (Atk - Def/2)
// random_multiplier = 0.85 - 1.15
// crit = 3x damage
// miss chance = 5%

int calculateDamage(int atk, int def) {
    double base = atk - (def * 0.5);
    if (base < 1) base = 1;

    double randomMultiplier = (rand() % 31 + 85) / 100.0;
    int dmg = (int)(base * randomMultiplier);
    if (dmg < 1) dmg = 1;
    return dmg;
}

// ===== Critical Chance =====
bool isCritical() {
    return (rand() % 100) < 10; // 10%
}

// ===== Miss Chance =====
bool isMiss() {
    return (rand() % 100) < 5;  // 5%
}


// ===== Player Attack Sequence =====
int playerAttack(Player &p, Enemy &e) {
    if (isMiss()) {
        narrate("Seranganmu meleset!");
        return 0;
    }

    bool crit = isCritical();
    int dmg = calculateDamage(p.attack, e.defense);

    if (crit) {
        dmg *= 2;
        narrate("💥 Serangan critical!");
    }

    e.hp -= dmg;
    return dmg;
}


// ===== Enemy Attack Sequence =====
int enemyAttack(Player &p, Enemy &e) {
    if (isMiss()) {
        narrate(e.name + " meleset menyerangmu!");
        return 0;
    }

    bool crit = isCritical();
    int dmg = calculateDamage(e.attack, p.defense);

    if (crit) {
        dmg *= 2;
        narrate("⚠️ " + e.name + " melakukan serangan critical!");
    }

    p.hp -= dmg;
    return dmg;
}


// =====Level Up System =====
void addExp(Player &p, int exp) {
    p.exp += exp;
    narrate("Kamu mendapat " + to_string(exp) + " EXP!");

    int required = p.level * 50;

    if (p.exp >= required) {
        narrate("⬆ LEVEL UP! Kamu naik ke level " + to_string(p.level + 1) + "!");
        p.level++;
        p.maxHP += 10;
        p.attack += 3;
        p.defense += 2;
        p.speed += 1;
        p.hp = p.maxHP;
        p.exp -= required;
    }
}


// ===== MAIN BATTLE FUNCTION =====
void battle(Player &player, Enemy enemy) {
    system("cls");
    narrate("⚔️ Pertempuran dimulai!");
    narrate(player.name + " VS " + enemy.name);

    while (player.hp > 0 && enemy.hp > 0) {
        cout << "\n=== STATUS ===" << endl;
        cout << "HP Kamu : " << player.hp << "/" << player.maxHP << endl;
        cout << "HP Musuh: " << enemy.hp << "/" << enemy.maxHP << endl;

        cout << "\nAksi kamu?" << endl;
        cout << "1. Serang" << endl;
        cout << "2. Gunakan Item" << endl;
        cout << "3. Bertahan (+Defense sementara)" << endl;
        cout << "4. Kabur (40% chance)" << endl;

        int choice = chooseOption(1, 4);

        if (choice == 1) {
            int dmg = playerAttack(player, enemy);
            narrate("Kamu memberi " + to_string(dmg) + " damage!");
        }
        else if (choice == 2) {
            showInventory(player.inv);
            narrate("Gunakan item apa?");
            string it;
            cin >> it;

            useItem(player.inv, it);

            // Kamu bisa tambahkan heal/efek di sini
        }
        else if (choice == 3) {
            narrate("Kamu bersiap bertahan. Defense meningkat 30% giliran ini.");
            player.defense = (int)(player.defense * 1.3);
        }
        else if (choice == 4) {
            if (rand() % 100 < 40) {
                narrate("Berhasil kabur!");
                return;
            } else {
                narrate("Gagal kabur!");
            }
        }

        // Enemy Turn (jika belum mati)
        if (enemy.hp > 0) {
            int enemyDamage = enemyAttack(player, enemy);
            narrate(enemy.name + " menyerang dan memberi " + to_string(enemyDamage) + " damage!");
        }

        // Reset temporary defense if player defend
        if (choice == 3) {
            player.defense = player.defense / 1.3;
        }
    }

    if (player.hp <= 0) {
        narrate("💀 Kamu tewas...");
        return;
    } else {
        narrate("🔥 Kamu mengalahkan " + enemy.name + "!");
        addItem(player.inv, "Monster Fang", 1, "Taring musuh yang dikalahkan.");
        addExp(player, enemy.expReward);
    }
}



// ===== Fungsi Cerita =====
void act0() {
  
    narrate("Dungeon Legends — sebuah game RPG berbasis teks penuh pilihan dan misteri...");
    narrate("Mungkin tidak banyak yang bisa dikatakan, jadi... nikmatilah petualanganmu.");
}

void act1(const string& name) {
    
    narrate("Kamu, yaaaa kamu " + name + ", hanyalah seseorang yang bermimpi menjadi petualang sejati.");
    narrate("Di tengah langit sore yang tenang, kamu melihat peta lama menuju sebuah dungeon kuno...");
    narrate("Tulisan samar di gerbangnya berbunyi: 'DRACONIC DUNGEON'.");

    narrate("\nApakah kamu ingin masuk ke dalam Dungeon tersebut?");
    narrate("1. Masuk ke dalam Dungeon.");
    narrate("2. Mengurungkan niat dan pergi.");

    int choice = chooseOption(1, 2);
    system("cls"); // ubah ke "clear" jika di Linux

    if (choice == 2) {
        
        narrate("Kamu menatap gerbang itu sekali lagi, lalu memutuskan untuk mundur.");
        narrate("Mungkin belum saatnya... atau mungkin kamu hanya takut.");
        narrate("\n=== Ending 1: 'Embrace the Cowardly' ===");
        exit(0);
    }

    // Jika memilih masuk
   
    narrate("Udara lembab, suara tetesan air terdengar dari dinding batu tua.");
    narrate("Tiba-tiba, seorang pria tua dengan jubah lusuh muncul dari kegelapan.");

    dialog("Renville", "Salam, wahai petualang. Siapa namamu?");
    dialog(name, "Namaku " + name + ".");
    dialog("Renville", "Ah, nama yang bagus untuk anak muda penuh tekad seperti dirimu.");
    dialog("Renville", "Aku adalah penjaga dungeon ini. Mungkin... juru kuncinya.");
    dialog("Renville", "Aku akan memberimu sesuatu untuk memulai perjalananmu.");

    narrate("Kamu menerima: Pedang ×1, Potion ×5, Bom ×5");
    addItem(player.inv, "Sword", 1, "Pedang dasar. Menambah Attack permanen.");
    addItem(player.inv, "Potion", 5, "Menyembuhkan 40 HP.");
    addItem(player.inv, "Bomb", 5, "Mengakibatkan 50 damage pada musuh.");

    dialog("Renville", "Dungeon ini menyimpan harta bernama 'Gold Amulet'. Hanya yang layak yang bisa menemukannya.");
    dialog("Renville", "Aku rasa... itu mungkin kamu, " + name + ".");
    dialog("Renville", "Berhati-hatilah di dalam. Setiap langkah bisa jadi ujian.");
    dialog("Renville", "Sampai jumpa, petualang.");

    narrate("\nKamu melangkah lebih dalam ke dalam kegelapan...");

}

// ===== Act 2 Placeholder =====
void act2(const string& name) {
    system("cls");
    
    narrate("Langkah kakimu bergema di lorong batu yang gelap...");
    narrate("Sebuah suara aneh terdengar dari kejauhan, seperti raungan makhluk lapar...");
    narrate("Kamu menggenggam pedangmu erat-erat.");

// ================= BOSS: DRACONIC MASTER =================
// AI: 3 moves: Attack(+20 flat), Heal(+10), Dodge (20% evade for 1 turn).
void bossBattle_DraconicMaster(Player &player) {
    system("cls");
    narrate("🔥 Kamu telah mencapai ruang terdalam...");
    dialog("Suara berat menggema: \"Siapa yang berani menggangguku?\"");
    dialog("DRACONIC MASTER muncul, menggeram...");

    Enemy boss = {"DRACONIC MASTER", 300, 300, 25, 10, 8, 200, "Boss"};
    bool bossDodge = false;
    int dodgeCooldown = 0;

    while (player.hp > 0 && boss.hp > 0) {
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
            showInventory(player.inv);
            narrate("Gunakan item apa? (Potion / Bomb / Sword)");
            string it;
            getline(cin, it);
            if (!it.empty()) useItemInBattle(player, boss, it);
            if (boss.hp <= 0) break;
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
                narrate("\n🔥 DRACONIC MASTER uses Dragon Strike!");
                int dmg = calculateDamage(boss.attack + 20, player.defense);
                player.hp -= dmg;
                narrate("Kamu menerima " + to_string(dmg) + " damage!");
            } else if (move == 2) {
                narrate("\n💚 DRACONIC MASTER uses Dragon Heal!");
                boss.hp += 10;
                if (boss.hp > boss.maxHP) boss.hp = boss.maxHP;
                narrate("Boss memulihkan 10 HP!");
            } else if (move == 3) {
                narrate("\n⚡ DRACONIC MASTER uses Dragon Dodge! (20% evade next player attack)");
                bossDodge = true;
                dodgeCooldown = 3;
            }
        }

        // reset player's defense if used defend
        player.defense = origDef;
        if (dodgeCooldown > 0) dodgeCooldown--;
    }

    if (player.hp <= 0) {
        narrate("\n💀 Kamu dikalahkan oleh DRACONIC MASTER...");
        narrate("BAD ENDING — The Dragon's Dominion");
        return;
    }

    // Victory
    narrate("\n🔥 Kamu berhasil mengalahkan DRACONIC MASTER!");
    typeText("Sosok raksasa runtuh... sebuah benda emas jatuh di hadapanmu.");
    addItem(player.inv, "Golden Amulet", 1, "Artefak legendaris - bukti kemenanganmu.");
    narrate("\n=== GOOD ENDING — DRAGON'S LEGACY ===");
}

    
}

// ===== MAIN GAME =====
int main() {
    string playerName;

    system("cls");
    showTitle();      
    cout << "\nSelamat datang di Dungeon Legends!\n";
    cout << "(Tekan ENTER untuk memulai...)" << endl;
    cin.ignore(); // tunggu ENTER dari user
    act0();

    cout << "\nMasukkan nama karaktermu: \n> "; // Prompt input dibuat langsung
    getline(cin, playerName);
    if (playerName.empty()) playerName = "Petualang";

    system("cls");
    act1(playerName);
    act2(playerName);
    zone1(player);
    zone2(player);
    zone3(player);
    zone4(player);
    bossBattle_DraconicMaster(player);


    narrate("\n=== ♥Thanks for playing♥ ===");
    narrate("Incomplete : 20%");
    return 0;
}