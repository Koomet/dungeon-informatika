
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

static string toLowerTrim(const string &s)
{
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start]))
        start++;
    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1]))
        end--;
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
void dialog(const string &speaker, const string &text);
void showHUD(const Player &p);

// Compatibility wrapper: some calls use the older/alternate name draconicMasterBoss
inline void draconicMasterBoss(Player &player) { bossBattle_DraconicMaster(player); }

int rng(int a, int b)
{
    return rand() % (b - a + 1) + a;
}
// utilty=
void typeText(const string &text, int delay = 25)
{
    for (char c : text)
    {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
    cout << endl;
}
void narrate(const string &text)
{
    cout << text << endl;
}
void dialog(const string &speaker, const string &text)
{
    cout << speaker << " " << text << endl;
}
int chooseOption(int min, int max)
{
    int choice;
    cout << "\n> Choose: ";
    while (!(cin >> choice) || choice < min || choice > max)
    {
        cout << "Pick a number between " << min << " and " << max << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // bersihkan buffer
    return choice;
}

//  banner
void showTitle()
{
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
    cout << "                      талулу ver." << endl;
    cout << "===========================================" << endl;
}

void showCredits()
{
    cout << "\n================ CREDITS ================" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "All thanks to her" << endl;
    cout << "=========================================" << endl;
    cout << "=========================================\n"
         << endl;
}

void DragonSari()
{
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
struct Item
{
    string name;
    int quantity;
    string description;
};
struct Inventory
{
    int maxSlots = 20;
    vector<Item> items;
};

void addItem(Inventory &inv, const string &name, int qty, const string &desc)
{
    for (auto &it : inv.items)
    {
        if (it.name == name)
        {
            it.quantity += qty;
            narrate("You picked up " + to_string(qty) + "x " + name + ".");
            return;
        }
    }
    if ((int)inv.items.size() >= inv.maxSlots)
    {
        narrate("Your bag is full, cant pick up no more.");
        return;
    }
    inv.items.push_back({name, qty, desc});
    narrate("You picked up " + to_string(qty) + "x " + name + ".");
}

void showInventory(const Inventory &inv)
{
    cout << "\n======= INVENTORY =======" << endl;
    if (inv.items.empty())
    {
        narrate("Bag is empty...");
        return;
    }
    int idx = 1;
    for (const auto &it : inv.items)
    {
        cout << idx << ". " << it.name << " x" << it.quantity
             << " — " << it.description << endl;
        idx++;
    }
}

string selectItemFromInventory(Inventory &inv)
{
    if (inv.items.empty())
    {
        narrate("Empty inventory.");
        return "";
    }

    showInventory(inv);
    narrate("Choose the item you want to use: ");
    int choice;

    while (true)
    {
        cout << "> ";
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            narrate("Unvalid input.");
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0)
            return "";
        if (choice >= 1 && choice <= (int)inv.items.size())
            return inv.items[choice - 1].name;

        narrate("Come on man.");
    }
}

void cleanInventory(Inventory &inv)
{
    // Remove any items that have zero or negative quantity to keep inventory tidy
    inv.items.erase(std::remove_if(inv.items.begin(), inv.items.end(),
                                   [](const Item &it)
                                   { return it.quantity <= 0; }),
                    inv.items.end());
}

// ================= PLAYER & ENEMY =================
struct Enemy
{
    string name;
    int maxHP;
    int hp;
    int attack;
    int defense;
    int speed;
    int expReward;
    string tier;
};

struct Player
{
    string name;
    int maxHP = 100;
    int hp = 100;
    int attack = 12;
    int defense = 6;
    int speed = 5;
    int level = 1;
    int talulah = 0;
    int exp = 0;

    Inventory inv;
};
Enemy xplizard = {"Glowzard", 15, 15, 5, 1, 1, 150, "T1"};
Enemy xplizard2 = {"Glowerzard", 15, 15, 5, 1, 1, 250, "T1"};
Enemy slug1 = {"Slug", 25, 25, 5, 1, 1, 5, "T1"};
Enemy slug2 = {"Infected slug", 30, 30, 7, 1, 1, 10, "T1"};
Enemy wolf = {"Wolf", 40, 40, 8, 2, 4, 15, "T1"};
Enemy wolf2 = {"Infected Wolf", 50, 50, 10, 2, 4, 25, "T2"};
Enemy snake = {"Snake", 40, 40, 10, 2, 8, 20, "T2"};
Enemy snake2 = {"Infected Snake", 50, 50, 12, 4, 8, 25, "T2"};
Enemy frog = {"Giant Frog", 60, 60, 8, 6, 5, 25, "T2"};
Enemy frog2 = {"Giant Infected Frog", 65, 65, 9, 10, 3, 35};
Enemy crocodile = {"Crocodile", 60, 60, 12, 13, 5, 35, "T2"};
Enemy crocodile2 = {"Infected Crocodile", 65, 65, 12, 14, 8, 40, "T2"};
Enemy snake3 = {"Giant Snake", 65, 65, 10, 8, 5, 25, "T3"};
Enemy bird = {"Giant Eagle", 55, 55, 8, 1, 10, 25, "T3"};
Enemy bird2 = {"Giant Infected Eagle", 55, 55, 12, 3, 9, 35, "T3"};
Enemy griffin = {"Griffin", 76, 76, 15, 5, 11, 50, "T3"};
Enemy salder = {"Salamander", 65, 65, 11, 5, 7, 35, "T3"};
Enemy salder2 = {"Infected Salamander", 65, 65, 14, 7, 7, 35, "T3"};
Enemy spider = {"Spider", 35, 35, 15, 5, 6, 20, "T2"};
Enemy zombie = {"Zombie", 68, 68, 6, 9, 9, 30, "T2"};
Enemy zombie2 = {"Armored Zombie", 68, 68, 6, 9, 15, 40, "T2"};
Enemy skeleton = {"Skeleton", 70, 70, 8, 5, 5, 30, "T2"};
Enemy skeleton2 = {"Crystallized Skeleton", 90, 90, 11, 5, 5, 40, "T2"};
Enemy darkorb = {"Dark Orb", 90, 90, 15, 6, 6, 35, "T3"};
Enemy darkorb2 = {"Crystal Orb", 100, 100, 17, 6, 6, 45, "T3"};
Enemy livingarmor = {"Living Armor", 140, 140, 22, 10, 5, 60, "T4"};
Enemy livingarmor2 = {"Living Crystal", 150, 160, 25, 10, 5, 80, "T4"};

// ================= COMBAT MATH =================
int calculateDamage(int atk, int def)
{
    double base = atk - (def * 0.5);
    if (base < 1)
        base = 1;
    double randomMultiplier = (rand() % 31 + 85) / 100.0; // 0.85 - 1.15
    int dmg = (int)(base * randomMultiplier);
    if (dmg < 1)
        dmg = 1;
    return dmg;
}
bool isCritical() { return (rand() % 100) < 10; } // 10%
bool isMiss() { return (rand() % 100) < 5; }      // 5%

int playerAttack(Player &p, Enemy &e)
{
    if (isMiss())
    {
        narrate("You miss!");
        if (p.talulah == 1)
        {
            narrate("Talulah attack with her sword!");
            int dmg = 65;
        }
        else
        {
            return 0;
        }
    }
    bool crit = isCritical();
    int dmg = calculateDamage(p.attack, e.defense);
    if (crit)
    {
        dmg *= 2;
        narrate("Critical Hit!");
        if (p.talulah == 1)
        {
            narrate("Talulah throws a Fireball!");
            dmg += 15;
        }
    }
    e.hp -= dmg;
    return dmg;
}
int enemyAttack(Player &p, Enemy &e)
{
    if (isMiss())
    {
        narrate(e.name + " Miss!");
        return 0;
    }
    bool crit = isCritical();
    int dmg = calculateDamage(e.attack, p.defense);
    if (crit)
    {
        dmg *= 2;
        narrate("[!] " + e.name + " LANDS A CRITICAL HIT!");
    }
    p.hp -= dmg;
    return dmg;
}

// ================= EXP / LEVEL =================
void addExp(Player &p, int exp)
{
    p.exp += exp;
    narrate("Kamu mendapat " + to_string(exp) + " EXP!");
    int required = p.level * 50;
    if (p.exp >= required)
    {
        p.level++;
        narrate("⬆ LEVEL UP! level " + to_string(p.level - 1) + "-->" + to_string(p.level) + "!");
        p.maxHP += 10;
        p.attack += 3;
        p.defense += 2;
        p.speed += 1;
        p.hp = p.maxHP;
        p.exp -= required;
    }
}

void pressEnter()
{
    cout << "\n( Press ENTER... )";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ================= ZONES / ENCOUNTERS =================

// SOME ZONE THINGY THAT KEEP BUGGING!
// WHAT THE BUG! -Habibi

void zone1(Player &player)
{
    narrate("=== Zona 1: Ancient Grassland ===");
    showHUD(player);
    dialog(player.name, " : How in the world is there a big ass grass field in here as well");
    dialog(player.name, " : ...is that the sun in the sky? why is the sky inside?");
    dialog(player.name, " : Am i still outside? im pretty sure i've went inside the cave");
    narrate("You continue to move forward...");
    narrate("Youre pretty sure youre already inside the 'Cave'");
    pressEnter();
    for (int i = 0; i < 5; i++)
    {
        if (i == 1)
        {
            narrate("You came across a weird corpse.");
            pressEnter();
            narrate("Some kind of rock seems to grow on its skin.");
            narrate("You have a feeling that touching it would be a very bad thing to do.");
            dialog(player.name, "");
            pressEnter();
        }
        int roll = rng(1, 100);

        if (roll <= 20)
        {
            showHUD(player);
            openChestZone1(player);
            pressEnter();
        }
        else if (roll <= 80)
        {
            showHUD(player);
            vector<Enemy> pool = {slug1, spider, slug2, wolf, xplizard};
            vector<int> chance = {25, 25, 25, 20, 5};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
        else
        {
            narrate("You keep walking and nothing fortunate or unfortunate happen.");
            narrate("Moving along.");
            pressEnter();
        }
    }
    narrate("it become darker as the fake sun starts to set.");
    narrate("You decided to stop by a remnant of a camp.");
    narrate("You start the campfire.");
    if (player.hp != player.maxHP)
    {
        (player.hp + player.maxHP);
        narrate("You feel well rested, HP fully Recovered.");
    }
    else
    {
        narrate("Youre not tired but the break is nice");
    }
    pressEnter();
    narrate("You see a figure covered in cloak approaching the campsite from a distance.");
    narrate("As the figure get closer youre able to make out 'her' feature, A pair of horn on top of a grayish hair, a pale skin and something swishing around behind her.");
    dialog(player.name, ": Hello... ");
    typeText("??? : \"Hello... a nice place you got here, mind having some company?.\"");
    pressEnter();
    dialog(player.name, ": No, go ahead.");
    typeText("??? : Thank you.");
    narrate("She sat down on the floor and sets down her sword.");
    dialog(player.name, ": Youre looking for the Golden Amulet too?.");
    typeText("??? : Golden Amulet? No, im here looking for someone actually");
    dialog(player.name, ": Why would you be looking in the dungeon?.");
    typeText("??? : ..I dont really know, i just thought i would find them here");
    dialog(player.name, ": Well, i hope you find them quickly and without accident.");
    typeText("??? : And i hope you find that Golden Amulet quickly as well.");
    narrate("You stand up and dust off your your clothes.");
    dialog(player.name, ": It was nice meeting you, but i have to go now.");
    typeText("??? : Alright, Good luck.");
    pressEnter();
}

void zone2(Player &player)
{
    narrate("=== Zona 2: Salty Marshes ===");
    showHUD(player);
    narrate("The ground becomes more and more muddy with increasingly larger puddle scattered around as you walk further...");
    dialog(player.name, " : I do not like this kind of place");
    dialog(player.name, " : At least the sun is still here");
    narrate("You continue to march on...");
    narrate("You have a feeling this is not an ordinary dungeon");

    for (int i = 0; i < 4; i++)
    {
        if (i == 3)
        {
            narrate("You saw a cluster of that odd growing rock moving.");
            pressEnter();
            narrate("Its a crocodile with dark rock growing from its scales.");
            narrate("It suddenly stops moving as if it was dead  before exploding into black dust.");
            narrate("You hurriedly continued on your way");
            int roll = rng(1, 100);
            {

                if (roll <= 20)
                {
                    showHUD(player);
                    openChestZone1(player);
                    pressEnter();
                }
                else if (roll <= 80)
                {
                    showHUD(player);
                    vector<Enemy> pool = {snake, snake2, crocodile, crocodile2, xplizard};
                    vector<int> chance = {30, 25, 25, 15, 5};
                    randomEncounter(player, pool, chance);
                    pressEnter();
                }

                else
                {
                    narrate("You keep walking and nothing fortunate or unfortunate happen.");
                    narrate("Moving along.");
                    pressEnter();
                }
            }
        }
    }
    narrate("After what feels like days of walking you find another abandoned campsite on dryland...");
    narrate("You light the fire and rest your tired body");
    if (player.hp != player.maxHP)
    {
        (player.hp + player.maxHP);
        narrate("You feel well rested, HP fully Recovered.");
    }
    else
    {
        narrate("Youre not tired but the break is nice");
    }
    pressEnter();
    narrate("You spot a familiar figure approaching from the distance.");
    typeText("??? : We cross path again it seems, mind if i take a seat?.");
    dialog(player.name, ": No problem, go ahead.");
    typeText("Thanks again.");
    narrate("She sit down on a nearby log");
    typeText("..I realized that i havent introduced myself before, My name is Talulah Artorius, You can call me Talulah if you like.");
    narrate(player.name + "My name is " + player.name + " nice to meet you Talulah");
    typeText("Talulah : Nice to meet you too " + player.name);
    dialog(player.name, ": So Talulah, where are you from?.");
    typeText("Talulah : Im from the Tundra in the north, its a cold place unlike here");
    typeText("Talulah : Where are You from anyway?");
    dialog(player.name, ": Im from the town south of here, an nice little town.");
    dialog(player.name, ": You said youre looking for someone right?, can you give me some description so maybe i could help a little");
    typeText("Talulah : Sadly, i cant tell you that, i barely know anything about them either");
    dialog(player.name, ": Well its alright, but are you planning to that big tree over there?");
    narrate("The tree itself is located much much further into the marsh and probably stands at hundreds of meters tall by the size of it, its still the biggest tree you have ever seen even from this distance");
    typeText("Talulah : Yes but im planning on staying around these marshes a bit longer, however much i hate this place with all its muds and bugs");
    dialog(player.name, ": I know, right?");
    pressEnter();
}

void zone3(Player &player)
{
    narrate("=== Zona 3: THE Tree ===");
    showHUD(player);
    dialog(player.name, " : It already looks pretty big from afar but i didnt know it would be THIS big up close ");
    dialog(player.name, " : Aww rat, of course there would be some big ass birds as well");
    dialog(player.name, " : Huh, what a conveniently human sized hole on this tree");
    narrate("You continue to move your leg forward...");
    narrate("This is definitely not a normal dungeon");
    narrate("You never expected the inside of the tree to be a large maze");

    for (int i = 0; i < 4; i++)
    {
        if (i == 2)
        {
            narrate("You see 2 giant eagle fighting each other with one of them being quite smaller than the other.");
            pressEnter();
            narrate("And yet the smaller one seems to overpowering the bigger one before biting into its neck and ending its life.");
            narrate("It doesnt take you long to notice the dark rock poking above the feather of the smaller one.");
            narrate("You decide that its best to leave it alone.");
            dialog(player.name, "");
            pressEnter();
            int roll = rng(1, 100);
            if (roll <= 20)
            {
                showHUD(player);
                openChestZone1(player);
                pressEnter();
            }
            else if (roll <= 80)
            {
                showHUD(player);
                vector<Enemy> pool = {bird, bird2, snake3, griffin, xplizard};
                vector<int> chance = {30, 30, 20, 10, 5};
                randomEncounter(player, pool, chance);
                pressEnter();
            }

            else
            {
                narrate("You keep walking and nothing fortunate or unfortunate happen.");
                narrate("Moving along.");
                pressEnter();
            }
        }
    }
    narrate("You smell the scent of burning wood");
    pressEnter();
    narrate("Its a campfire, with a...familiar? person sitting near it, and some kind of portal behind her");
    typeText("Talulah : Hello " + player.name + " youre a little late arent you?");
    narrate("Its just Talulah with her cloak discarded, turns out she was wearing some kind of formal militaristic-noble dress underneath.");
    dialog(player.name, " : I didnt know youre an officer");
    typeText("Talulah : Oh this?, its a...gift!, Yes a gift from my father for my birthday!, and my cloak is ruined because of that stupid griffin");
    dialog(player.name, " : Why would you wear a cloak here anyway, its not like its gonna rain inside");
    typeText("Talulah : thats the thing!, im not wearing it but those bird shit on it while i was taking a break");
    typeText("Talulah : Its doesnt really worth much but its still my cloak you know?");
    dialog(player.name, " : I could see where youre coming from");
    narrate("You sat down near the fire opposite of her seat");
    dialog(player.name, " : But at least theres no mosquito here or any kind of bug actually, Hmm");
    typeText("Talulah : You do have a point there but thats all the better for us");
    typeText("Talulah : Though ive heard that the next 'Area' is very different than this one not");
    pressEnter();
    typeText("Talulah : Say " + player.name + " do you want to travel together? we could help each other out along the way");
    dialog(player.name, " : Sure, why not, i need to rest a bit though");
    typeText("Talulah : No problem");
    narrate("You noticed her tail moves slightly");
    pressEnter();
    if (player.hp != player.maxHP)
    {
        (player.hp + player.maxHP);
        narrate("You feel well rested, HP fully Recovered.");
    }
    else
    {
        narrate("Youre not tired but the break is nice");
    }
    narrate("You set off into the portal along with Talulah");
    narrate("You have a friend now!");
    player.maxHP += 25;
    player.attack += 15;
    player.talulah += 1;
}

void zone4(Player &player)
{
    narrate("=== Zona 4: Ruined Corridor ===");
    narrate("You walk down a staircase...");
    showHUD(player);
    dialog(player.name, "...Why is it so bright even though theres no lamp?");
    dialog(player.name, "I sure hope the ceiling wont fall on me...");
    dialog(player.name, "and I swear to God almighty if theres trap in here...");
    narrate("You walk further into the odd corridor...");

    for (int i = 0; i < 4; i++)
    {
        if (i == 1)
        {
            typeText("Talulah : The ceilings seems awfully eager of falling on us");
            pressEnter();
        }

        else if (i == 2)
        {
            typeText("Talulah : Why are there lizard in here?");
            pressEnter();
        }
        else if (i == 3)
        {
            typeText("Talulah : What? Never seen a tail before?");
            pressEnter();
        }
        else if (i == 4)
        {
            typeText("Talulah : look ");
            narrate("A rock covered salamander crawls sluggishly");
            typeText("Talulah : Do you know what those rocks are? ");
            dialog(player.name, " : No? why?");
            typeText("Talulah : ...Its nothing, forget it ");
            pressEnter();
        }
        int roll = rng(1, 100);

        if (roll <= 50)
        {
            showHUD(player);
            openChestZone1(player);
            pressEnter();
        }
        else if (roll <= 80)
        {
            showHUD(player);
            vector<Enemy> pool = {salder, salder2, zombie, darkorb, xplizard2};
            vector<int> chance = {25, 25, 25, 20, 5};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
        else
        {
            narrate("You keep walking and nothing fortunate or unfortunate happen.");
            narrate("Moving along.");
            pressEnter();
        }
    }
    narrate("The campfire was lit and the body is about rest");
    if (player.hp != player.maxHP)
    {
        (player.hp + player.maxHP);
        narrate("You feel well rested, HP fully Recovered.");
    }
    else
    {
        narrate("Youre not tired but the break is nice");
    }
    pressEnter();
    typeText("Talulah : Hey, do you remember about those rocks i ask you about?");
    dialog(player.name, " : Yeah, what about them?");
    pressEnter();
    typeText("Talulah : We have the same thing in my hometown");
    dialog(player.name, " : Monsters with rock sounds quite problematic, ");
    narrate("Talulah shakes her head");
    typeText("Talulah : ...Those things also grow on people");
    dialog(player.name, " : ..Elaborate?");
    pressEnter();
    typeText("Talulah : Its a disease, it spread though rocks itself.");
    typeText("Talulah : Though your skin is usually enough to protect you from it.");
    typeText("Talulah : That is unless you breath in the rock dust or got cut by one of the rock, for even the smallest of paper cut is enough to spread it.");
    pressEnter();
    typeText("Talulah : The symptom is simple, at first those dark rock will start growing on your skin.");
    typeText("Talulah : Then it would spread around your body while also getting bigger.");
    typeText("Talulah : but it wont only grow outside, your organ will slowly getting replaced by these rocks which would slowly but surely kill you.");
    typeText("Talulah : And finally, when most of your body has already been replaced you will simply explode into rock dust that could still infect other...");
    pressEnter();
    typeText("Talulah : Right now, we still havent found the cure for the disease.");
    typeText("Talulah : For these reason the infected is always discriminated againt, it doesnt matter who you are before you got infected.");
    typeText("Talulah : In their eyes youre just another dangerous infected.");
    typeText("Talulah : ............");
    typeText("Talulah : Im sorry for bringing up such a sad topic.");
    dialog(player.name, " : Its fine, it is a bit depressing but you seems a little better after telling me about it");
    typeText("Talulah : Thank you for listening, truly.");
    dialog(player.name, " : So uh, are you still gonna look for whoever youre looking for? how would you find them anyway?");
    typeText("Talulah : They just say that the one im looking for will give me the strength to change the fate of the infected");
    dialog(player.name, " : That.. doesnt help much");
    narrate("She just shrugs in response");
    typeText("Talulah : Though might have an idea of who im looking for");
    narrate("Her tails wags slightly");
    pressEnter();
}

void zone5(Player &player)
{
    narrate("=== Zona 5: Cadaver Cavern ===");
    showHUD(player);
    dialog(player.name, " : Of course theres another cave at the end");
    dialog(player.name, " : Though this does make a bit more sense, an actually dark cave and without grass or sun");
    narrate("You went into the hole on the wall..."); // is it spelling correct? belulang atau berulang
    // shut up its "is the spelling correct?" -Nabil

    for (int i = 0; i < 7; i++)
    {
        if (i == 1)
        {
            typeText("Talulah : Dont worry the my fire wont burn up the oxygen");
            pressEnter();
        }

        else if (i == 2)
        {
            typeText("Talulah : Come on!, follow me!");
            pressEnter();
        }

        else if (i == 4)
        {
            typeText("Imagine theres a cave with a lot of corpse, how would you name it?");
            pressEnter();
        }
        int roll = rng(1, 100);

        if (roll <= 60)
        {
            showHUD(player);
            openChestZone3(player);
            pressEnter();
        }
        else if (roll <= 90)
        {
            showHUD(player);
            vector<Enemy> pool = {zombie, zombie2, skeleton, skeleton2, darkorb};
            vector<int> chance = {30, 15, 30, 15, 10};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
        else
        {
            narrate("You keep walking and nothing fortunate or unfortunate happen.");
            narrate("Moving along.");
            pressEnter();
        }
    }
    narrate("You found another abandoned campsite, but for some reason the fire is still lit");
    if (player.hp != player.maxHP)
    {
        (player.hp + player.maxHP);
        narrate("You feel well rested, HP fully Recovered.");
    }
    else
    {
        narrate("Youre not tired but the break is nice");
    }
    pressEnter();
    typeText("Talulah : " + player.name + " i have something to tell you");
    dialog(player.name, " : What is it?");
    pressEnter();
    narrate("She pull up her sleeve slightly, revealing her wrist with dark rock growing on it");
    typeText("Talulah : I am... an Infected");
    dialog(player.name, " : What, but how?");
    typeText("Talulah : In my country the infected are treated as if they were nothing but pests");
    typeText("Talulah : In my country the infected are treated as if they were nothing but pests");
    typeText("Talulah : So i try support them whether secretly or by taking part in their peaceful protest");
    typeText("Talulah : But my... Father have other plans for me, so one day i decide to infect myself with a piece of the rock");
    narrate("She points to a lesion on her wrist");
    typeText("Talulah : I did this so my father would see me as a worthless asset for his plans");
    dialog(player.name, " : Your father sounds like a pretty bad person");
    dialog(player.name, " : Dont worry though youre still the same Talulah from before in my head");
    typeText("Talulah : Thanks");
    dialog(player.name, " : When we got out, maybe i could help you out supporting the infected?");
    typeText("Talulah : I would greatly appreciate that" + player.name);
}

void zone6(Player &player)
{
    narrate("=== Zona 6: Forsaken Depths ===");
    showHUD(player);
    dialog(player.name, " : This place starting to give me the chill man");
    narrate("But regardless you continue on..."); // no this game is unballace btw
    // Talulah is my favorite npc ever fr fr -Nabil
    // true -Nabil
    pressEnter();
    // shit man , this thing is hard!
    for (int i = 0; i < 8; i++)
    {
        if (i == 1)
        {
            typeText("Talulah : Dont tell me youre scared of a cave, come on!");
            pressEnter();
        }

        else if (i == 2)
        {
            typeText("Talulah : i see that you really like looking at my tail, what are you?, a cat?");
            pressEnter();
        }

        else if (i == 4)
        {
            typeText("Your amulet is probably at the end of this cave on some statue neck");
            pressEnter();
        }
        else if (i == 6)
        {
            typeText("Boo!, did i scare you?");
            pressEnter();
        }
        else if (i == 8)
        {
            typeText("Talulah : All this cave and not a single gold to be found");
            pressEnter();
        }
        int roll = rng(1, 100);

        if (roll <= 60)
        {
            showHUD(player);
            openChestZone3(player);
            pressEnter();
        }
        else
        {
            showHUD(player);
            vector<Enemy> pool = {darkorb, darkorb2, livingarmor, livingarmor2, xplizard};
            vector<int> chance = {25, 25, 25, 15, 5};
            randomEncounter(player, pool, chance);
            pressEnter();
        }
    }
}
// what is this - nabil

void randomEncounter(Player &player, vector<Enemy> pool, vector<int> probs)
{
    int roll = rng(1, 100);
    int acc = 0;

    for (int i = 0; i < pool.size(); i++)
    {
        acc += probs[i];
        if (roll <= acc)
        {
            battle(player, pool[i]);
            return;
        }
    }
}

// SOME LOOT SHIT
// shit indeed -Nabil
void openChestZone1(Player &player)
{
    int roll = rng(1, 100);

    if (roll <= 33)
    {
        addItem(player.inv, "Potion", 1, "Heal 40 HP");
        narrate("You found Potion!");
    }
    else if (roll <= 66)
    {
        addItem(player.inv, "Bomb", 1, "Bomb 50 dmg");
        narrate("You found Bomb!");
    }
    else if (roll <= 88)
    {
        player.maxHP += 50;
        narrate("You found Heart Crystal! Max HP +50!");
    }
    else
    {
        player.attack += 25;
        narrate("You found X-Attack Potion! Attack +25 permanen!");
    }
}

void openChestZone3(Player &player)
{
    int roll = rng(1, 100);

    if (roll <= 33)
    {
        addItem(player.inv, "Bomb", 1, "Bomb 50 dmg");
        narrate("You found Bomb!");
    }
    else if (roll <= 77)
    { // 33 + 44
        player.maxHP += 50;
        narrate("Kamu menemukan Heart Crystal! Max HP +50!");
    }
    else if (roll <= 89)
    { // 77 + 12
        player.attack += 25;
        narrate("Kamu menemukan X-Attack Potion! Attack +25 permanen!");
    }
    else
    {
        addItem(player.inv, "Armor", 1, "Mengurangi 50% damage untuk 1 turn");
        narrate("Kamu menemukan Dragonite Armor! Mengurangi 50% dmg.");
    }
}

// ================= ITEM EFFECTS (in-battle) =================
// you know what yeah this is the most buggiest thing ive code
bool useItemInBattle(Player &player, Enemy &enemy, const string &itemNameInput)
{
    if (itemNameInput.empty())
        return false;
    string key = toLowerTrim(itemNameInput);

    for (auto &it : player.inv.items)
    {
        if (toLowerTrim(it.name) == key && it.quantity > 0)
        {
            // apply effect
            it.quantity--;
            narrate("You use " + it.name + ".");

            if (toLowerTrim(it.name) == "potion")
            {
                int heal = 40;
                player.hp = min(player.maxHP, player.hp + heal);
                narrate("HP Recovered +" + to_string(heal) + ".");
            }
            else if (toLowerTrim(it.name) == "bomb")
            {
                int dmg = 50;
                enemy.hp -= dmg;
                narrate("The Bomb explode! " + enemy.name + " takes " + to_string(dmg) + " damage!");
            }
            else if (toLowerTrim(it.name) == "sword")
            {
                int inc = 5;
                player.attack += inc;
                narrate("The blade is sharpened! Attack +" + to_string(inc) + ". Permanently");
            }
            else if (toLowerTrim(it.name) == "armor")
            {
                // armor memberikan cuma se turn
                narrate("Armor activated, reduce incoming damage by half once (50%).");
                // ngebug bruh
                player.defense = player.defense * 2;
            }
            else
            {
                narrate("This has no use in here.");
            }
            return true;
        }
    }
    narrate("Item is either gone or you run out.");
    return false;
}

// this function is from github and i dont know when this will be , so yeah ill stick it there and see if in future this will be usefull
bool useItemOutOfBattle(Player &player, const string &itemNameInput)
{
    if (itemNameInput.empty())
        return false;
    string key = toLowerTrim(itemNameInput);

    for (auto &it : player.inv.items)
    {
        if (toLowerTrim(it.name) == key && it.quantity > 0)
        {
            if (key == "sword")
            {
                it.quantity--;
                player.attack += 5;
                narrate(" You strap the sword on your hip. Attack +5.");
                return true;
            }
            else if (key == "potion")
            {
                narrate("Potion could only be used in battle...for some reason.");
                return false;
            }
            else if (key == "bomb")
            {
                narrate("Bomb is unactivate for now, could be used in battle obviously.");
                return false;
            }
            else
            {
                narrate("This has no use here.");
                return false;
            }
        }
    }
    narrate("Item is either gone or you run out.");
    return false;
}

void showHUD(const Player &p)
{
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
        if (i.name == "Golden Amulet")
            hasAmulet = true; // bruh

    cout << "Amulet   : " << (hasAmulet ? "✓ Dimiliki" : "-") << endl;

    cout << "======================\n"
         << endl;
}

// battlin`
void battle(Player &player, Enemy enemy)
{ // psstt use bomb, its powerful
    system("cls");
    narrate("Fight!");
    narrate(player.name + " Against " + enemy.name);

    while (player.hp > 0 && enemy.hp > 0)
    {
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

        if (choice == 1)
        {
            int dmg = playerAttack(player, enemy);
            narrate("The enemy takes " + to_string(dmg) + " damage!");
        }
        else if (choice == 2)
        {
            string selected = selectItemFromInventory(player.inv);
            if (selected.empty())
            {
                narrate("You put it back inside.");
                continue;
            }

            bool used = useItemInBattle(player, enemy, selected);
            cleanInventory(player.inv);

            if (!used)
            {
                narrate("You put it back inside.");
                continue;
            }

            // if enemy dies due to bomb, skip enemy turn
            if (enemy.hp <= 0)
                break;
        }
        else if (choice == 3)
        {
            narrate("You bring up your shield, Defense + 30%!");
            player.defense = (int)(player.defense * 1.3);
        }
        else if (choice == 4)
        {
            if (rand() % 100 < 40)
            {
                narrate("Escaped Successfully!");
                return;
            }
            else
            {
                narrate("Escaped Unsuccessfully!");
            }
        }

        // Enemy turn (if alive) yeah if alive
        if (enemy.hp > 0)
        {
            int edmg = enemyAttack(player, enemy);
            narrate(enemy.name + " Deal " + to_string(edmg) + " damage!");
        }

        player.defense = origDefense;
    }

    if (player.hp <= 0)
    {
        narrate("\n You die...");
        return;
    }

    narrate("\n You killed " + enemy.name + "!");
    addExp(player, enemy.expReward);
}

// BOSS
// AI: 3 moves: Attack(+20 flat), Heal(+10), Dodge (20% evade for 1 turn).
void bossBattle_DraconicMaster(Player &player)
{
    system("cls");
    narrate(" You reach yet another cave...");
    narrate("The air feels heavy, there is no light, and the ground is shaking, something is wrong here");
    narrate("Suddenly hundreds of torches on the wall lit up revealing a giant dragon charging at you while roaring its lungs out");
    typeText("DRACONIC MASTER is here...");
    // petah...the hors is herh
    DragonSari();
    Enemy boss = {"DRACONIC MASTER", 300, 300, 25, 10, 8, 200, "Boss"};
    bool bossDodge = false;
    int dodgeCooldown = 0;

    while (player.hp > 0 && boss.hp > 0)
    {
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
        if (choice == 1)
        {
            if (bossDodge && (rand() % 100 < 20))
            {
                narrate("DRACONIC MASTER dodge your attack!");
            }
            else
            {
                int dmg = playerAttack(player, boss);
                narrate("You deal " + to_string(dmg) + " damage!");
            }
        }
        else if (choice == 2)
        {
            string selected = selectItemFromInventory(player.inv);
            if (!selected.empty())
            {
                useItemInBattle(player, boss, selected);
                cleanInventory(player.inv);
                if (boss.hp <= 0)
                    break;
            }
            else
            {
                narrate("You put it back inside.");
            }
        }
        else if (choice == 3)
        {
            narrate("You bring up your shield, Defense + 30%!");
            player.defense = (int)(player.defense * 1.3);
        }

        bossDodge = false;

        // Boss AI move
        if (boss.hp > 0)
        {
            int move;
            if (boss.hp < boss.maxHP * 0.4 && (rand() % 100 < 55))
                move = 2; // heal more likely
            else if (dodgeCooldown == 0 && (rand() % 100 < 20))
                move = 3; // dodge
            else
                move = 1; // attack

            if (move == 1)
            {
                narrate("\nDRACONIC MASTER uses Dragon Strike!");
                int dmg = calculateDamage(boss.attack + 20, player.defense);
                player.hp -= dmg;
                narrate("Kamu takes " + to_string(dmg) + " damage!");
            }
            else if (move == 2)
            {
                narrate("\nDRACONIC MASTER uses Dragon Heal!");
                boss.hp += 10;
                if (boss.hp > boss.maxHP)
                    boss.hp = boss.maxHP;
                narrate("Boss recover 10 HP!");
            }
            else if (move == 3)
            {
                narrate("\nDRACONIC MASTER uses Dragon Dodge! (20% evade next player attack)");
                bossDodge = true;
                dodgeCooldown = 3;
            }
        }

        // reset player's defense if used defend
        player.defense = origDef;
        if (dodgeCooldown > 0)
            dodgeCooldown--;
    }

    if (player.hp <= 0)
    {
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
    narrate("\n=== GOOD ENDING — ts so corny ;deadfloweremoi;===");
    typeText("Hey " + player.name + " i think i found the person im looking for.");
    pressEnter();
    narrate("Talulah cups your cheek with her hands");
    typeText("The person im looking for is You!");
    narrate("and then they kiss and seggs 8 hour and bla bla bla may allah forgive me for this");
    showCredits();
    pressEnter();
}

// ================= STORY ACTS =================
void act0()
{
    narrate("Dungeon Legends...");
    narrate("Prepare to fun.");
}

void act1(const string &name)
{
    narrate("\nFor no particular reason you decided to get out of bed that day.");
    narrate("You might as well look for that mysterious Golden amulet in the all-famous 'DRACONIC DUNGEON'.");
    narrate("???: Go ahead");
    narrate("1. Okay\n2. No?");

    int c = chooseOption(1, 2);

    switch (c)
    {
    case 1:
        narrate("Turns out, the 'dungeon' is nothing but a small cave in the middle of nowhere.");
        narrate("It is surrounded by a field of grass.");
        narrate("You see an old man waiting by the entrance under the shade of a tree.");
        typeText("Old man: \"Hello there...\"");
        pressEnter();
        narrate("He quickly pulls out his sword and held it against your throat");
        typeText("Old man: \"Give me everything you have or you'll die\"");
        narrate("You hear a creak before suddenly the tree fell down, crushing the old man beneath it");
        narrate("After a moment or two you decide to loot his still-warm body");
        pressEnter();
        break;

    case 2:
        narrate("The words seems to get stuck in your throat");
        narrate("You feel a tinge of sadness from...somewhere");
        narrate("You decided to turn bac-");
        pressEnter();
        narrate("=== ENDING: Im sorry ===");
        exit(0);
        break;

    default:
        narrate("Not that one.");
        break;
    }
}

// ================= MAIN =================
int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    system("cls");
    showTitle();
    cout << "\nWelcome!\n(Press Enter)";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    act0();
    narrate("\nI know that YOU know exactly what this is.");

    narrate("\nYou hear a voice from the beyond");
    cout << "\n???: Who are you?" << endl
         << "> ";
    string playerName;
    getline(cin, playerName);
    if (playerName.empty())
        playerName = "Adventurer";

    // create player
    Player player;
    player.name = playerName;
    player.maxHP = 100;
    player.hp = 100;
    player.attack = 12;
    player.defense = 6;
    int talulah = 0;
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
    zone5(player);
    zone6(player);
    bossBattle_DraconicMaster(player);
    narrate("\n=== Thanks        ===");
    narrate("\n===               ===");
    narrate("\n===       -Talulah===");
    pressEnter();
    return 0;
}
