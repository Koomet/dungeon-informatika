#include <bits/stdc++.h>
using namespace std;


// ----------------------------- Utilities -----------------------------
std::mt19937 &rng() {
    static std::mt19937 gen((unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());
    return gen;
}
int randInt(int a, int b) { // inclusive
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng());
}
double randReal() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng());
}
string toLower(const string &s) {
    string res = s;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

// ----------------------------- Game Types -----------------------------
enum class Rarity { COMMON, RARE, EPIC };
string rarityName(Rarity r) {
    switch (r) {
        case Rarity::COMMON: return "Common";
        case Rarity::RARE:   return "Rare";
        case Rarity::EPIC:   return "Epic";
    }
    return "Unknown";
}

struct Item {
    string name;
    Rarity rarity;
    int power;    // for weapons -> damage bonus, for potions -> healing amount, for trinkets -> value
    string type;  // "Weapon", "Potion", "Trinket"
    string toString() const {
        return name + " (" + type + ", " + rarityName(rarity) + (type=="Weapon" ? (", +" + to_string(power) + " ATK)") : (type=="Potion" ? (", Heal " + to_string(power) + ")") : (", value " + to_string(power) + ")")));
    }
};

// Loot Chest generator
vector<Item> generateLootChest(int chestQuality = 0) {
    // chestQuality could bias rarity: -1 => poor, 0 => normal, 1 => good
    vector<Item> loot;
    int attempts = randInt(1, 3);
    for (int i = 0; i < attempts; ++i) {
        double r = randReal();
        double epicChance = 0.05 + 0.05*chestQuality;
        double rareChance = 0.25 + 0.05*chestQuality;
        Rarity rar;
        if (r < epicChance) rar = Rarity::EPIC;
        else if (r < epicChance + rareChance) rar = Rarity::RARE;
        else rar = Rarity::COMMON;

        // pick a type
        double t = randReal();
        if (t < 0.4) {
            // Weapon
            int base = (rar==Rarity::COMMON?1: rar==Rarity::RARE?3:6);
            int power = base + randInt(0, base);
            string name;
            if (rar==Rarity::COMMON) name = (randReal()<0.5? "Iron Sword":"Rusty Dagger");
            else if (rar==Rarity::RARE) name = (randReal()<0.5? "Elven Blade":"War Axe");
            else name = (randReal()<0.5? "Dragonbite": "Crystal Greatsword");
            loot.push_back({name, rar, power, "Weapon"});
        } else if (t < 0.75) {
            // Potion
            int heal = (rar==Rarity::COMMON?20: rar==Rarity::RARE?50:120) + randInt(0,10);
            string name = (rar==Rarity::COMMON? "Small Potion" : rar==Rarity::RARE? "Elixir":"Phoenix Elixir");
            loot.push_back({name, rar, heal, "Potion"});
        } else {
            // Trinket / gold
            int val = (rar==Rarity::COMMON?10: rar==Rarity::RARE?50:200) + randInt(0,20);
            string name = (rar==Rarity::COMMON? "Bronze Amulet" : rar==Rarity::RARE? "Silver Locket":"Crystal Amulet");
            loot.push_back({name, rar, val, "Trinket"});
        }
    }
    // chance to include extra gold as trinket
    if (randReal() < 0.3) {
        int goldVal = randInt(5, 50) * (1 + chestQuality);
        loot.push_back({"Gold Pouch", Rarity::COMMON, goldVal, "Trinket"});
    }
    return loot;
}

// ----------------------------- Player -----------------------------
struct Player {
    string name = "Hero";
    int level = 1;
    int maxHp = 100;
    int hp = 100;
    int baseAtk = 6;
    int gold = 0;
    vector<Item> inventory;
    string equippedWeaponName = "";
    int storyState = 0; // branching story integer

    int weaponBonus() const {
        for (auto &it : inventory) {
            if (it.type=="Weapon" && it.name == equippedWeaponName) return it.power;
        }
        return 0;
    }

    void addItem(const Item &it) {
        if (it.name == "Gold Pouch") {
            gold += it.power;
            return;
        }
        if (it.type == "Trinket" && it.name=="Gold Pouch") { gold += it.power; return; }
        inventory.push_back(it);
    }

    void showInventory() const {
        cout << "---- Inventory ----\n";
        if (inventory.empty()) cout << "(empty)\n";
        else {
            for (size_t i=0;i<inventory.size();++i) {
                cout << i+1 << ") " << inventory[i].toString();
                if (inventory[i].name == equippedWeaponName) cout << " [Equipped]";
                cout << "\n";
            }
        }
        cout << "Gold: " << gold << "\n";
    }

    void heal(int amount) {
        hp = min(maxHp, hp + amount);
    }

    int attackValue() const {
        return baseAtk + weaponBonus();
    }
};

// ----------------------------- Zone & Encounters -----------------------------
enum class EncounterType { ENEMY, NPC, DISCOVERY };

struct Zone {
    string id;
    string name;
    string description;
    vector<EncounterType> possibleEncounters;
    vector<string> neighbors; // zone ids for travel

    Zone() = default;
    Zone(const string &_id, const string &_name, const string &_desc) : id(_id), name(_name), description(_desc) {}
};

// Enemy structure
struct Enemy {
    string name;
    int hp;
    int atk;
    int goldDrop;
};

// Minimal map of zones
struct World {
    unordered_map<string, Zone> zones;
    void addZone(const Zone &z) { zones[z.id] = z; }
    Zone* getZone(const string &id) {
        auto it = zones.find(id);
        if (it==zones.end()) return nullptr;
        return &it->second;
    }
    vector<string> zoneList() const {
        vector<string> r;
        for (auto &p : zones) r.push_back(p.first);
        return r;
    }
};

// ----------------------------- Game Engine -----------------------------
struct Game {
    Player player;
    World world;
    string currentZoneId;
    bool running = true;

    // Initialize world with some zones and connections
    void initWorld() {
        Zone forest("forest", "Haunted Forest", "Misty trees and twisted roots. Shadows move oddly.");
        forest.possibleEncounters = {EncounterType::ENEMY, EncounterType::NPC, EncounterType::DISCOVERY};
        forest.neighbors = {"village", "cave"};

        Zone cave("cave", "Ancient Cave", "Cold stone and echoing halls. Something glints in the dark.");
        cave.possibleEncounters = {EncounterType::ENEMY, EncounterType::DISCOVERY};
        cave.neighbors = {"forest", "ruins"};

        Zone village("village", "Merchant Town", "A small town with friendly faces and a market.");
        village.possibleEncounters = {EncounterType::NPC, EncounterType::DISCOVERY};
        village.neighbors = {"forest"};

        Zone ruins("ruins", "Forgotten Ruins", "Collapsed pillars and strange runes. An old secret sleeps here.");
        ruins.possibleEncounters = {EncounterType::ENEMY, EncounterType::NPC, EncounterType::DISCOVERY};
        ruins.neighbors = {"cave"};

        world.addZone(forest);
        world.addZone(cave);
        world.addZone(village);
        world.addZone(ruins);

        currentZoneId = "village";
    }

    // Display main menu
    void mainMenu() {
        while (true) {
            cout << "=== Text RPG Adventure ===\n";
            cout << "1) Start New Game\n";
            cout << "2) Load Game\n";
            cout << "3) Exit\n";
            cout << "Choose: ";
            string choice; getline(cin, choice);
            if (choice=="1") {
                startNewGame();
                gameLoop();
            } else if (choice=="2") {
                if (loadGame()) {
                    cout << "Game loaded.\n";
                    gameLoop();
                } else {
                    cout << "Failed to load save.\n";
                }
            } else if (choice=="3" || toLower(choice)=="exit") {
                cout << "Goodbye.\n";
                break;
            } else {
                cout << "Invalid option.\n";
            }
        }
    }

    void startNewGame() {
        initWorld();
        cout << "Enter your name: ";
        string name; getline(cin, name);
        if (name.empty()) name = "Hero";
        player = Player();
        player.name = name;
        // give starter items
        player.inventory.push_back({"Rusty Dagger", Rarity::COMMON, 2, "Weapon"});
        player.equippedWeaponName = "Rusty Dagger";
        player.inventory.push_back({"Small Potion", Rarity::COMMON, 20, "Potion"});
        player.gold = 30;
        currentZoneId = "village";
        player.storyState = 0;
        cout << "Welcome, " << player.name << ". Your adventure begins!\n";
    }

    // Save/load simple text format
    bool saveGame(const string &fname="savegame.txt") {
        ofstream ofs(fname);
        if (!ofs) return false;
        ofs << player.name << '\n';
        ofs << player.level << ' ' << player.maxHp << ' ' << player.hp << ' ' << player.baseAtk << '\n';
        ofs << player.gold << '\n';
        ofs << player.equippedWeaponName << '\n';
        ofs << player.storyState << '\n';
        ofs << currentZoneId << '\n';
        ofs << player.inventory.size() << '\n';
        for (auto &it : player.inventory) {
            ofs << it.name << '|' << (int)it.rarity << '|' << it.power << '|' << it.type << '\n';
        }
        return true;
    }

    bool loadGame(const string &fname="savegame.txt") {
        ifstream ifs(fname);
        if (!ifs) return false;
        initWorld();
        string line;
        getline(ifs, player.name);
        int lvl, maxhp, hp, atk;
        ifs >> lvl >> maxhp >> hp >> atk;
        player.level = lvl; player.maxHp = maxhp; player.hp = hp; player.baseAtk = atk;
        int gold; ifs >> gold; player.gold = gold;
        string tmp; getline(ifs, tmp); // consume newline
        getline(ifs, player.equippedWeaponName);
        ifs >> player.storyState;
        getline(ifs, tmp); // consume newline
        getline(ifs, currentZoneId);
        size_t invCount;
        ifs >> invCount;
        getline(ifs, tmp);
        player.inventory.clear();
        for (size_t i=0;i<invCount;++i) {
            string line; getline(ifs, line);
            if (line.empty()) continue;
            stringstream ss(line);
            string name, rstr, pstr, type;
            getline(ss, name, '|');
            getline(ss, rstr, '|');
            getline(ss, pstr, '|');
            getline(ss, type);
            Item it;
            it.name = name;
            it.rarity = (Rarity)stoi(rstr);
            it.power = stoi(pstr);
            it.type = type;
            player.inventory.push_back(it);
        }
        return true;
    }

    // Generate an enemy appropriate for zone & player level
    Enemy generateEnemyForZone(const Zone &z) {
        // basic variety by zone id
        if (z.id == "forest") {
            if (randReal() < 0.5) return {"Forest Wolf", 40 + randInt(0,10), 6 + randInt(0,3), randInt(8,20)};
            return {"Ghostly Stalker", 30 + randInt(0,15), 8 + randInt(0,4), randInt(10,30)};
        } else if (z.id == "cave") {
            if (randReal() < 0.5) return {"Cave Spider", 30 + randInt(0,10), 7 + randInt(0,3), randInt(6,18)};
            return {"Stone Golem", 60 + randInt(0,20), 10 + randInt(0,6), randInt(20,40)};
        } else if (z.id == "village") {
            if (randReal() < 0.6) return {"Drunken Thug", 35 + randInt(0,10), 6 + randInt(0,2), randInt(5,15)};
            return {"Bandit Scout", 28 + randInt(0,8), 7 + randInt(0,3), randInt(8,20)};
        } else if (z.id == "ruins") {
            if (randReal() < 0.5) return {"Guardian Wight", 50 + randInt(0,20), 9 + randInt(0,4), randInt(15,35)};
            return {"Ancient Specter", 45 + randInt(0,20), 11 + randInt(0,6), randInt(20,50)};
        }
        return {"Wild Beast", 30, 6, 10};
    }

    // Combat mini-game
    bool battle(Enemy enemy) {
        cout << "Battle Start! " << enemy.name << " appears (HP " << enemy.hp << ")\n";
        while (enemy.hp > 0 && player.hp > 0) {
            cout << "\nYour HP: " << player.hp << "/" << player.maxHp << " | Enemy HP: " << enemy.hp << "\n";
            cout << "Actions: 1) Attack  2) Use Potion  3) Flee\n";
            cout << "Choose: ";
            string choice; getline(cin, choice);
            if (choice == "1") {
                int dmg = player.attackValue() + randInt(0,3);
                cout << "You strike for " << dmg << " damage.\n";
                enemy.hp -= dmg;
            } else if (choice == "2") {
                // find first potion
                auto it = find_if(player.inventory.begin(), player.inventory.end(), [](const Item &it){ return it.type=="Potion"; });
                if (it == player.inventory.end()) {
                    cout << "No potion available!\n";
                } else {
                    cout << "You drink " << it->name << " and heal " << it->power << " HP.\n";
                    player.heal(it->power);
                    player.inventory.erase(it);
                }
            } else if (choice == "3") {
                if (randReal() < 0.5) {
                    cout << "You fled successfully.\n";
                    return false;
                } else {
                    cout << "Failed to flee!\n";
                }
            } else {
                cout << "Invalid.\n";
                continue;
            }
            if (enemy.hp > 0) {
                int edmg = enemy.atk + randInt(0,3);
                cout << enemy.name << " hits you for " << edmg << " damage.\n";
                player.hp -= edmg;
            }
        }
        if (player.hp <= 0) {
            cout << "You were defeated...\n";
            return false;
        }
        cout << "You defeated " << enemy.name << "! You gain " << enemy.goldDrop << " gold.\n";
        player.gold += enemy.goldDrop;
        // small chance to drop loot
        if (randReal() < 0.35) {
            cout << "The enemy dropped a chest!\n";
            auto loot = generateLootChest(0);
            for (auto &it : loot) {
                cout << "  Found: " << it.toString() << "\n";
                player.addItem(it);
            }
        }
        // after win, small XP/level system (simple)
        if (randReal() < 0.4) {
            player.level++;
            player.maxHp += 8;
            player.baseAtk += 1;
            player.hp = player.maxHp;
            cout << "You leveled up! Now level " << player.level << ". HP/ATK increased.\n";
        }
        return true;
    }

    // NPC encounter - dialogue with choices that affect storyState or reward
    void npcEncounter(const Zone &z) {
        // simple variety
        double r = randReal();
        if (r < 0.33) {
            cout << "You meet a wounded traveler. He begs for help.\n";
            cout << "1) Help him (cost 10 gold)\n2) Ignore\n3) Steal from him\nChoose: ";
            string ch; getline(cin, ch);
            if (ch == "1" && player.gold >= 10) {
                player.gold -= 10;
                cout << "You help him. He gives you a potion and a rumor.\n";
                player.addItem({"Small Potion", Rarity::COMMON, 20, "Potion"});
                // rumor unlocks a hint - increase storyState
                player.storyState = max(player.storyState, 1);
            } else if (ch == "2") {
                cout << "You walk away. The traveler curses you.\n";
            } else if (ch == "3") {
                if (randReal() < 0.5) {
                    int stolen = min(player.gold, 15);
                    player.gold += stolen;
                    cout << "You steal " << stolen << " gold successfully.\n";
                } else {
                    cout << "He cries for guard! You are fined 10 gold.\n";
                    player.gold = max(0, player.gold - 10);
                }
            } else {
                cout << "Invalid choice.\n";
            }
        } else if (r < 0.66) {
            cout << "You encounter a traveling merchant. He offers goods.\n";
            cout << "Merchant: 'I sell weapons and potions.'\n";
            cout << "1) Buy Iron Sword (20g)\n2) Buy Potion (10g)\n3) Leave\nChoose: ";
            string ch; getline(cin, ch);
            if (ch == "1" && player.gold >= 20) {
                player.gold -= 20;
                player.addItem({"Iron Sword", Rarity::COMMON, 4, "Weapon"});
                cout << "Bought Iron Sword.\n";
            } else if (ch == "2" && player.gold >= 10) {
                player.gold -= 10;
                player.addItem({"Small Potion", Rarity::COMMON, 20, "Potion"});
                cout << "Bought potion.\n";
            } else {
                cout << "Transaction cancelled or insufficient gold.\n";
            }
        } else {
            cout << "A mysterious sage offers you a choice that will shape your path.\n";
            cout << "1) Seek glory (join the ruins quest)\n2) Seek wealth (favor merchants)\nChoose: ";
            string ch; getline(cin, ch);
            if (ch == "1") {
                player.storyState = 2; // ruins path
                cout << "Sage: 'The old stones call to you.'\n";
            } else if (ch == "2") {
                player.storyState = 3; // merchant path
                cout << "Sage: 'Gold opens many doors.'\n";
            } else {
                cout << "Sage disappears.\n";
            }
        }
    }

    // Discovery encounter - chests, traps, shrines
    void discoveryEncounter(const Zone &z) {
        double r = randReal();
        if (r < 0.5) {
            cout << "You find a hidden chest!\n";
            auto loot = generateLootChest((z.id=="ruins")?1:0);
            for (auto &it : loot) {
                cout << "  " << it.toString() << "\n";
                player.addItem(it);
            }
        } else if (r < 0.8) {
            cout << "It's a trap! You trigger spikes.\n";
            int dmg = randInt(5, 20);
            player.hp -= dmg;
            cout << "You take " << dmg << " damage.\n";
            if (player.hp <= 0) {
                cout << "You died from the trap.\n";
            }
        } else {
            cout << "You discover an old shrine. You feel restored.\n";
            player.heal(30);
        }
    }

    // Handle a random encounter in current zone
    void handleRandomEncounter() {
        Zone *z = world.getZone(currentZoneId);
        if (!z) return;
        if (z->possibleEncounters.empty()) return;
        // Choose random encounter type
        EncounterType et = z->possibleEncounters[randInt(0, (int)z->possibleEncounters.size()-1)];
        // bias: if storyState indicates certain path, favor certain events
        if (player.storyState == 2 && currentZoneId == "ruins" && randReal() < 0.6) {
            et = EncounterType::DISCOVERY; // ruins path more discoveries
        }

        if (et == EncounterType::ENEMY) {
            Enemy e = generateEnemyForZone(*z);
            bool survived = battle(e);
            if (!survived) {
                cout << "Game over. Returning to main menu.\n";
                running = false;
            }
        } else if (et == EncounterType::NPC) {
            npcEncounter(*z);
        } else if (et == EncounterType::DISCOVERY) {
            discoveryEncounter(*z);
        }
    }

    // Travel between zones
    void travel() {
        Zone *z = world.getZone(currentZoneId);
        if (!z) return;
        cout << "You can travel to:\n";
        for (size_t i=0;i<z->neighbors.size();++i) {
            string nid = z->neighbors[i];
            auto nz = world.getZone(nid);
            if (nz) cout << i+1 << ") " << nz->name << " - " << nz->description << "\n";
        }
        cout << "Choose number or 0 to cancel: ";
        string ch; getline(cin, ch);
        int sel = 0;
        try { sel = stoi(ch); } catch(...) { sel = -1; }
        if (sel > 0 && (size_t)sel <= z->neighbors.size()) {
            currentZoneId = z->neighbors[sel-1];
            cout << "You travel to " << world.getZone(currentZoneId)->name << ".\n";
            // Chance for immediate event upon arrival
            if (randReal() < 0.5) handleRandomEncounter();
        } else {
            cout << "Travel cancelled.\n";
        }
    }

    // Explore current zone (triggers random encounter)
    void explore() {
        cout << "You explore the area...\n";
        if (randReal() < 0.2) {
            cout << "Nothing of note.\n";
            return;
        }
        handleRandomEncounter();
    }

    // Quick actions menu while in game
    void gameLoop() {
        running = true;
        while (running) {
            if (player.hp <= 0) {
                cout << "You have no HP left. Would you like to load last save? (y/n): ";
                string ans; getline(cin, ans);
                if (toLower(ans) == "y") {
                    if (!loadGame()) {
                        cout << "No save found. Returning to main menu.\n";
                        break;
                    }
                } else break;
            }
            Zone *z = world.getZone(currentZoneId);
            cout << "\n-- Location: " << (z? z->name : currentZoneId) << " --\n";
            if (z) cout << z->description << "\n";
            cout << "HP: " << player.hp << "/" << player.maxHp << "  Gold: " << player.gold << "  Level: " << player.level << "\n";
            cout << "Actions:\n";
            cout << "1) Explore\n2) Travel\n3) Inventory\n4) Equip Weapon\n5) Save Game\n6) Quit to Main Menu\nChoose: ";
            string choice; getline(cin, choice);
            if (choice == "1") {
                explore();
                // check for story branching triggers (example)
                if (player.storyState == 1 && currentZoneId == "forest" && randReal() < 0.2) {
                    cout << "Your earlier rumor leads you to a hidden glen. A wondrous item awaits.\n";
                    auto loot = generateLootChest(1);
                    for (auto &it: loot) { cout << "  " << it.toString() << "\n"; player.addItem(it); }
                    player.storyState = 4; // mark completed
                }
            } else if (choice == "2") {
                travel();
            } else if (choice == "3") {
                player.showInventory();
            } else if (choice == "4") {
                player.showInventory();
                cout << "Choose item number to equip as weapon (0 cancel): ";
                string ch; getline(cin, ch);
                int n = 0;
                try { n = stoi(ch); } catch(...) { n = -1; }
                if (n > 0 && (size_t)n <= player.inventory.size()) {
                    if (player.inventory[n-1].type == "Weapon") {
                        player.equippedWeaponName = player.inventory[n-1].name;
                        cout << "Equipped " << player.equippedWeaponName << ".\n";
                    } else {
                        cout << "Not a weapon.\n";
                    }
                } else {
                    cout << "Cancelled.\n";
                }
            } else if (choice == "5") {
                if (saveGame()) cout << "Saved.\n"; else cout << "Save failed.\n";
            } else if (choice == "6") {
                cout << "Return to main menu (progress saved automatically)? (y/n): ";
                string ans; getline(cin, ans);
                if (toLower(ans) == "y") saveGame();
                break;
            } else {
                cout << "Unknown choice.\n";
            }

            // Check for special ending conditions based on storyState and location
            if (player.storyState == 2 && currentZoneId == "ruins") {
                // special ruins ending possibility
                if (randReal() < 0.15) {
                    cout << "\nYou discover the heart of the ruins. An ancient spirit offers you glory.\n";
                    cout << "1) Accept (become legendary)\n2) Refuse (leave)\nChoose: ";
                    string ch; getline(cin, ch);
                    if (ch == "1") {
                        cout << "You merge with the ruins' power. You become a legend. ENDING: The Ascendant\n";
                        running = false;
                        break;
                    } else {
                        cout << "You walk away, life continues.\n";
                        player.storyState = 5;
                    }
                }
            } else if (player.storyState == 3 && currentZoneId == "village") {
                if (randReal() < 0.12) {
                    cout << "\nA wealthy merchant offers you a partnership for riches.\n1) Accept\n2) Decline\nChoose: ";
                    string ch; getline(cin, ch);
                    if (ch == "1") {
                        cout << "You accept and live a prosperous life. ENDING: The Merchant Lord\n";
                        running = false;
                        break;
                    } else {
                        cout << "You refuse and keep your independence.\n";
                        player.storyState = 6;
                    }
                }
            }
        }
    }
};

// ----------------------------- Main -----------------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Game game;
    game.mainMenu();
    return 0;
}