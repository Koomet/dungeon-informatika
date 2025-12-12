void bossBattle_DraconicMaster(Player &player) {
    system("cls");
    narrate("🔥 Kamu telah mencapai ruangan terdalam dari Dungeon...");
    narrate("Suara berat menggema dari kegelapan...");
    typeText("???: \"...Manusia. Apakah kau yang mengusik tidurku?\"");
    typeText("DRACONIC MASTER muncul dengan aura menggetarkan.");

    Enemy boss = {
        "DRACONIC MASTER",
        300,     // maxHP
        300,     // hp
        25,      // attack
        10,      // defense
        8,       // speed
        200,     // exp reward
        "Boss"
    };

    bool bossDodge = false;     
    int dodgeCooldown = 0;

    narrate("\n⚔️ FINAL BATTLE DIMULAI! ⚔️");

    while (player.hp > 0 && boss.hp > 0) {
        cout << "\n=== STATUS ===\n";
        cout << "HP Kamu : " << player.hp << "/" << player.maxHP << endl;
        cout << "HP Boss : " << boss.hp << "/" << boss.maxHP << endl;

        cout << "\nAksi kamu?" << endl;
        cout << "1. Serang" << endl;
        cout << "2. Gunakan Item" << endl;
        cout << "3. Bertahan" << endl;

        int choice = chooseOption(1, 3);

        // ========== PLAYER TURN ==========
        if (choice == 1) {
            int dmg = calculateDamage(player.attack, boss.defense);

            if (bossDodge && (rand() % 100 < 20)) {
                narrate("DRACONIC MASTER menghindari seranganmu!");
            } else {
                boss.hp -= dmg;
                narrate("Kamu memberi " + to_string(dmg) + " damage!");
            }
        }
        else if (choice == 2) {
            showInventory(player.inv);
            narrate("Gunakan item apa?");
            string it;
            cin >> it;
            useItem(player.inv, it);
        }
        else if (choice == 3) {
            narrate("Kamu bertahan. Defense +30% giliran ini.");
            player.defense = (int)(player.defense * 1.3);
        }

        // Reset dodge jika aktif sebelumnya
        bossDodge = false;

        // ========== BOSS TURN ==========
        if (boss.hp > 0) {
            int moveChoice;

            // AI behavior:
            if (boss.hp < boss.maxHP * 0.4 && (rand() % 100 < 45)) {
                moveChoice = 2; // heal priority
            }
            else if (dodgeCooldown == 0 && (rand() % 100 < 15)) {
                moveChoice = 3; // dodge chance
            }
            else {
                moveChoice = 1; // attack default
            }

            // === EXECUTE MOVE ===
            if (moveChoice == 1) {
                narrate("\n🔥 DRACONIC MASTER menggunakan *Dragon Strike*!");
                int dmg = calculateDamage(boss.attack + 20, player.defense);
                player.hp -= dmg;
                narrate("Kamu terkena " + to_string(dmg) + " damage!");
            }
            else if (moveChoice == 2) {
                narrate("\n💚 DRACONIC MASTER menggunakan *Dragon Heal*!");
                boss.hp += 10;
                if (boss.hp > boss.maxHP) boss.hp = boss.maxHP;
                narrate("Boss memulihkan 10 HP!");
            }
            else if (moveChoice == 3) {
                narrate("\n⚡ DRACONIC MASTER menggunakan *Dragon Dodge*!");  
                narrate("Boss meningkatkan peluang menghindar 20% untuk 1 turn!");
                bossDodge = true;
                dodgeCooldown = 3;
            }
        }

        // Defense reset
        if (choice == 3) player.defense = player.defense / 1.3;

        // Cooldown decrease
        if (dodgeCooldown > 0) dodgeCooldown--;
    }

    // ========== RESULT ==========
    if (player.hp <= 0) {
        narrate("\n💀 Kamu dikalahkan oleh DRACONIC MASTER...");
        narrate("Dungeon kembali gelap...");
        narrate("BAD ENDING — The Dragon's Dominion");
        return;
    }

    narrate("\n🔥 Kamu berhasil mengalahkan DRACONIC MASTER!");
    narrate("Sosok raksasa itu runtuh dan meninggalkan cahaya emas...");
    typeText("Kamu mendapatkan **GOLDEN AMULET**!");
    addItem(player.inv, "Golden Amulet", 1, "Artefak legendaris. Bukti kemenanganmu.");

    narrate("\n=== GOOD ENDING — DRAGON'S LEGACY ===");
}
