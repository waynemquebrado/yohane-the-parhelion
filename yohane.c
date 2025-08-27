/*
    Description:    The program is a game called "Yohane The Parhelion! The Siren in the Mirror World!."
                    Where Aquor members started losing their voices and it Yohane's (Player) job to
                    take back their lost voices from the Siren.

    Programmed by:  Wayne Matthew V. Quebrado
                    Viktor Rafale T. Aya-ay

    Last Modified:  July 30, 2025

    Version:        2.0

    [Acknowledgements:  stackoverflow.com => For the math.h
                        w3schools.com => For the atan2() function usage
                        study.com => For the math implementation of the Siren
                        Playtesters => For the people who tested on running the program]       
*/

// <Preprocessor directives>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <time.h>
#include <dirent.h>
#include <conio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define MAX_FLOORS 4
#define MAX_ROWS 12
#define MAX_COLS 56
#define MAX_DUNGEONS 8
#define ACHIEVEMENT_COUNT 28
#define MAX_SHOP 9
#define MAPS_COUNT 8
#define MAX_BATS 8

// TEXT COLOR
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREY  "\x1b[90m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_ORANGE  "\x1b[38;5;208m"
#define COLOR_WHITE  "\x1b[37m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_CYAN "\x1b[36m"

// ITEM ID
#define TEARS_OF_A_FALLEN_ANGEL 1
#define NOPPO_BREAD 2
#define SHOVEL_UPGRADE 3
#define BAT_TAMER 4
#define AIR_SHOES 5
#define STEWSHINE 6
#define MIKAN_MOCHI 7
#define KUROSAWA_MACHA 8
#define CHOCO_MINT_ICE_CREAM 9

// ACHIEVEMENT ID 
#define YOHANE_DESCENDS 1
#define MIKAN_POWER 2
#define RIKO_CHAN_BEAM 3
#define YOUSORO 4
#define ITS_THE_FUTURE_ZURA 5
#define GANBARUBY 6
#define BUU_BUU_DESU_WA 7
#define HUG 8
#define SHINY 9
#define IN_THIS_UNSTABLE_WORLD 10
#define ONE_MORE_SUNSHINE_STORY 11
#define PIANOFORTE_MONOLOGUE 12
#define BEGINNER_SAILING 13
#define OYASUMINASAN 14
#define RED_GEM_WINK 15
#define WHITE_FIRST_LOVE 16
#define SAKANA_KA_NANDAKA 17
#define NEW_WINDING_ROAD 18
#define DEEP_RESONANCE 19
#define NO_10 20
#define CYARON 21
#define AZALEA 22
#define GUILTY_KISS 23
#define EIKYUU_HOURS 24
#define AOZORA_JUMPING_HEART 25
#define MITAIKEN_HORIZON 26
#define RUBY_CHAN_HAI_NANI_GA_SUKI 27
#define STEP_ZERO_TO_ONE 28

// DEATH ID
#define DEATH_BAT 1
#define DEATH_BURNED 2
#define DEATH_IMPALED 3
#define DEATH_SIREN 4

// SIREN PAHSE
#define MAX_PHASE 3

typedef char string10[11];
typedef char string20[21];
typedef char string50[51];
typedef char string100[101];

// Chika        Yasudaya Ryokan
// Riko         Numazu Deep Sea Aquarium
// You          Izu-Mito Sea Paradise
// Hanamaru     Shougetsu Confectionary
// Ruby         Nagahama Castle Ruins
// Dia          Numazugoyotei
// Kanan        Uchiura Bay Pier
// Mari         Awashima Marine Park

typedef struct {
    float HP;
    int maxHP;
    int gold;
    int tears, noppo, iceCream;
    char itemOnHand; // 'T' = tears, 'N' = noppo, 'I' = iceCream
    bool shovelUpgrade, batTamer, airShoes, stewShine, mikanMochi, kurosawaMacha; // If player has item
    int row, col;
} Player;

typedef struct {
    int moveInterval, movesSince;
    bool active;
    int row, col;
    bool hit;
} Bat;

typedef struct{
    bool active;
    int row, col;
    bool hit;
} Siren;

typedef struct{
    int row, col;
    bool pressed;
}Switch;

typedef struct{
    char map[MAX_ROWS][MAX_COLS + 1];
    Siren siren;
    Switch switches[2];
    int phase;
    bool barrier;
} FinalBattle;

typedef struct {
    string50 name; // Name of dungeon
    string10 idol; 
    int floors;
    char map[MAX_FLOORS][MAX_ROWS][MAX_COLS + 1];
    int clears;
} Dungeon;

// VIEW ACHIEVMENTS
typedef struct{
    string50 name;
    bool status;
    time_t achievementDate;
    string100 description;
} Achievement;

typedef struct {
    bool playthroughActive;
    bool died;
    bool hasSaved;
    bool runActive;
    
    Achievement achievements[ACHIEVEMENT_COUNT];

    FinalBattle sirenFinal;

    Player player;

    int selectedDungeons[3];
    bool currentDungeonClear[3];

    // Tracking the bats for save file
    int dungeonLevel;

    // ACHIEVMENT CONDITIONS
    // 1 // 20
    int numDungeonCleared;

    // 2-9 // 10-17 // 21-24
    Dungeon allDungeons[MAX_DUNGEONS];

    // 10 // 18 // 19 
    int numSirenKilled;

    // 25
    int noDamageDungeonClear;
    // 26
    int totalGoldSpent;
    // 27
    int savedByRubyIceCream;
    // 28
    int noGoldOnHand;

    Achievement tempAchievements[ACHIEVEMENT_COUNT];

    // FOR PLAYTHROUGH
    bool checkPlaythroughDungeonCleared[MAX_DUNGEONS];
    int numPlaythroughDungeonCleared;
    int numPlaythrough;
} GameState;

/*  Description: Used to clear the screen
    Precondition: N/A
    @param <N/A> <purpose>
    @return <void>
*/
void 
clearScreen() {
    printf("\033[2J");
}

/*  Description: Used to display the stats of the player
    Precondition: GameState *g is initialized
    @param <GameState *g> <To get access to the player stats in the struct to display>
    @return <void>
*/
void 
playerStats(GameState *g) {
    // for making HP print decimal when it has
    if ((int)g->player.HP == g->player.HP) {
        printf("HP: %d / %d\t\t\tTotal Gold: " COLOR_YELLOW "%dGP" COLOR_RESET "\n", (int)g->player.HP, g->player.maxHP, g->player.gold);
    } else {
        printf("HP: %.1f / %d\t\t\tTotal Gold: " COLOR_YELLOW "%dGP" COLOR_RESET "\n", g->player.HP, g->player.maxHP, g->player.gold);
    }

    if (g->player.itemOnHand == '.' || (g->player.tears == 0 && g->player.noppo == 0 && g->player.iceCream == 0)){
        printf("Item on Hand: N/A\n\n");
    } else {
        switch (g->player.itemOnHand) {
            case 'T':
                printf("Item on Hand: Tears of a Fallen Angel (%d)\n\n", g->player.tears);
                break;
            case 'N':
                printf("Item on Hand: Noppo Bread (%d)\n\n", g->player.noppo);
                break;
            case 'C':
                printf("Item on Hand: Choco-Mint Ice Cream (%d)\n\n", g->player.iceCream);
                break;
        }
    }
}

/*  Description: Used to display the whole achievemetns module from Main Menu
    Precondition: GameState *g is initialized and the member achievements[] is initialized
    @param <GameState *g> <To get access to the achievements in the struct to display and process>
    @return <void>
*/
void 
viewAchievements(GameState *g) {

    char input;
    char newInput;
    int i, j;
    int nFlag = 0;
    int page = 1;
    int viewingAchievement = 0;
    int inputConvert;
    int numOfAchievements = 0;
    int saveIndex;
    string50 status;
    int nFound;

    for (i = 0; i < ACHIEVEMENT_COUNT; i++){
        if (g->achievements[i].status == 1){
            numOfAchievements++;
        }
    }

    string50 achievementName[4][8] = {
        {"Yohane Descends!", "Mikan Power!", "Riko-chan BEAM!", "Yousoro!", "It's the future, zura!", "Ganbaruby!", "Buu-buu desu wa!", "Hug!!!"},
        {"Shiny!", "In This Unstable World!", "One more sunshine story!", "Pianoforte Monologue!", "Beginner's Sailing!", "Oyasuminasan!", "Red Gem Wink!", "White First Love!"},
        {"Sakana ka Nandaka!", "New Winding Road!", "Deep Resonance!", "No. 10!", "CYaRon!", "AZALEA!", "Guilty Kiss!", "Eikyuu Hours!"},
        {"Aozora Jumping Heart!", "Mitaiken Horizon!", "Ruby-chan! Hai? Nani ga suki?", "Step! ZERO to ONE!"}
    };

    string100 achievementDescription[4][8] = {
        {"Cleared first dungeon", "Rescued Chika for the first time", "Rescued Riko for the first time", "Rescue You for the first time", "Rescued Hanamaru for the first time", "Rescued Ruby for the first time", "Rescue Dia for the first time", "Rescued Kanan for the first time"},
        {"Rescued Mari for the first time", "Beat the Final boss for the first time", "Rescued Chika twice", "Rescued Riko twice", "Rescued You twice", "Rescued Hanamaru twice", "Rescued Ruby twice", "Rescued Dia twice"},
        {"Rescued Kanan twice", "Rescued Mari twice", "Beat the Final boss twice", "Clear 10 dungeons", "Rescued Chika, You, and Ruby (Not necessarily in one playthrough)", "Rescued Hanamaru, Dia, and Mari (Not necessarily in one playthrough)", "Rescued Riko and Mari (Not necessarily in one playthrough)", "Have Yohane rescue all Aqours members for the first time"},
        {"Clear a dungeon without incurring any damage", "Accumulate a total of 5000G spent on Hanamaru's stores across multiple playthroughs", "Get saved by a fatal blow from Ruby's choco-mint ice cream item", "Complete a playthrough with 0G on-hand at the end"}
    };

    int achievementsPerPage[4] = {8, 8, 8, 4}; // Number of achievements per page

    do{
        clearScreen();
        if (page >= 1 && page <= 4){
            for (i = 0; i < 60; i++){
                printf("*");
            }
            printf("\n");
        
            printf("%19cAchievements module\n", ' ');
            printf("%22cObtained: %d / %d\n", ' ', numOfAchievements, ACHIEVEMENT_COUNT);

            for (i = 0; i < 60; i++){
                printf("*");
            }
            printf("\n");

            if (viewingAchievement == 0){
                for (i = 0; i < achievementsPerPage[page - 1]; i++) {
                    nFound = 0;
                    for (j = 0; j < ACHIEVEMENT_COUNT && nFound == 0; j++) {
                        // Removed achievementName[page - 1][i] &&
                        if (strcmp(achievementName[page - 1][i], g->achievements[j].name) == 0) {
                            saveIndex = j;
                            nFound = 1;
                        }
                    }
                      

                    if (!g->achievements[saveIndex].status) {
                        strcpy(status, "NOT EARNED!");
                    } 
                    else {
                        strcpy(status, "EARNED!");
                    }
                    printf("[%d] %-42s%s\n", i + 1, achievementName[page - 1][i], status);
                }


                printf("\nPage %d of 4\n\n", page);

                printf("[N]ext Page\n");
                printf("[P]revious Page\n");
                printf("[R]eturn to Main Menu\n\n");

                printf("Choice: ");

                input = getchar();
                while (getchar() != '\n');
            }

            else{
                if (inputConvert <= achievementsPerPage[page- 1] && inputConvert >= 1){
                    for (i = 0; i < ACHIEVEMENT_COUNT; i++){
                        if (strcmp(achievementName[page - 1][inputConvert - 1], g->achievements[i].name)== 0){
                            saveIndex = i;
                        }
                    }

                    if (g->achievements[saveIndex].status == 0){
                        strcpy(status, "NOT EARNED!");
                    }
                    else{
                        strcpy(status, "EARNED!");
                    }
                    
                    printf("Achievement Name: %s\n\n", achievementName[page - 1][inputConvert - 1]);
                    printf("Status: %s\n\n", status);
                    if (g->achievements[saveIndex].achievementDate != -1){
                        printf("Date Earned: %s\n\n", ctime(&g->achievements[saveIndex].achievementDate));
                    }
                    printf("Description:\n");
                    printf("%s\n\n\n", achievementDescription[page - 1][inputConvert - 1]);

                    printf("[R]eturn to Achievements Module\n\n");

                    printf("Choice: ");

                    newInput = getchar();
                    while (getchar() != '\n');
                }
            }

            if (viewingAchievement == 0){
                switch(input){
                    case 'n': case 'N':
                        page++;
                        break;
                    case 'p': case 'P':
                        page--;
                        break;
                    case 'r': case 'R':
                        nFlag = 1;
                        break;
                    
                    case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
                        //viewingAchievement = 1;
                        inputConvert = input - '0';

                        if (inputConvert <= achievementsPerPage[page- 1] && inputConvert >= 1){
                            viewingAchievement = 1;
                        }

                        else{
                            clearScreen();
                        }

                        break;
                }
            }

            else{
                if (newInput == 'r' || newInput == 'R'){
                    viewingAchievement = 0;
                }
            }
        }
        else{
            page = 1;
        }
    }while(nFlag == 0);
}

/*  Description: Used to update the achievement module specifically when a criteria for the achievement is met
    Precondition: GameState *g is initialized and achievementID is a valid correcti achievement ID from ACHIEVEMENT ID defines
    @param <GameState *g> <To get access to the achievements in the struct>
    @param <achievementID> <To use as index for the achievements in the GameState struct>
    @return <void>
*/
void 
achievementUpdater(GameState *g, int achievementID){
    g->achievements[achievementID-1].status = true;
    g->achievements[achievementID-1].achievementDate = time(NULL);

    g->tempAchievements[achievementID-1] = g->achievements[achievementID - 1];
} 

/*  Description: Used to check if a player has a save file
    Precondition: The parameter is initialized and string20 player is a valid filename/string
    @param <string20 player> <Used to compare on files form the directory with ".bin">
    @return <int>
*/
int 
checkSaveFile(string20 player) {

    int exists = 0;
    
    DIR* d = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        // check if player has played before
        if (strcmp(entry->d_name, player) == 0) {
            exists = 1;
        }
    }

    closedir(d);
    return exists;
}

/*  Description: Used to save from the struct from Main Menu's Save and Quit
    Precondition: GameState *g is initialized and filename contains a valid string
    @param <GameState *g> <Used to access the struct to be able to copy the members to the GameState save>
    @param <string20 filename> <Used as the name of the .bin file>
    @return <void>
*/
void 
saveRun(GameState *g, string20 filename) {
    printf("Saving game...\n");

    int i;
    GameState save = {
        .playthroughActive = g->playthroughActive,
        .died = g->died,
        .hasSaved = g->hasSaved,
        .runActive = g->runActive,
        
        .sirenFinal = g->sirenFinal,

        .player = g->player,


        .numDungeonCleared = g->numDungeonCleared,
        .numSirenKilled = g->numSirenKilled,
        .noDamageDungeonClear = g->noDamageDungeonClear,
        .totalGoldSpent = g->totalGoldSpent,
        .savedByRubyIceCream = g->savedByRubyIceCream,
        .noGoldOnHand = g->noGoldOnHand,

        .numPlaythroughDungeonCleared = g-> numPlaythroughDungeonCleared,
        .numPlaythrough = g->numPlaythrough,
        .dungeonLevel = g->dungeonLevel
    };

    for (i = 0; i < ACHIEVEMENT_COUNT; i++) {
        save.achievements[i] = g->achievements[i];
        save.tempAchievements[i] = g->tempAchievements[i];
    }
    for (i = 0; i < 3; i++) {
        save.selectedDungeons[i] = g->selectedDungeons[i];
        save.currentDungeonClear[i] = g->currentDungeonClear[i];
    }
    for (i = 0; i < MAX_DUNGEONS; i++) {
        save.allDungeons[i] = g->allDungeons[i];
        save.checkPlaythroughDungeonCleared[i] = g->checkPlaythroughDungeonCleared[i];
    }

    FILE* savefile = fopen(filename, "wb");
    fwrite(&save, sizeof(GameState), 1, savefile);
    fclose(savefile);

    g->hasSaved = true;

    printf("Game saved!\n");
    printf("Press any key to continue...\n");
    getch();
}

/*  Description:    Used to save from the struct from the Final Battle and everytime a player dies.
                    Used to make the program not close and to properly load the file everytime the player dies, or completes the playthrough
    Precondition: GameState *g is initialized and filename contains a valid string
    @param <GameState *g> <Used to access the struct to be able to copy the members to the GameState save>
    @param <string20 filename> <Used as the name of the .bin file>
    @return <void>
*/
void 
autoSave(GameState *g, string20 filename) {
    printf("Saving game...\n");

    int i;
    GameState save = {
        .playthroughActive = g->playthroughActive,
        .died = g->died,
        .runActive = g->runActive,
        
        .sirenFinal = g->sirenFinal,

        .player = g->player,

        .numDungeonCleared = g->numDungeonCleared,
        .numSirenKilled = g->numSirenKilled,
        .noDamageDungeonClear = g->noDamageDungeonClear,
        .totalGoldSpent = g->totalGoldSpent,
        .savedByRubyIceCream = g->savedByRubyIceCream,
        .noGoldOnHand = g->noGoldOnHand,

        .numPlaythroughDungeonCleared = g-> numPlaythroughDungeonCleared,
        .numPlaythrough = g->numPlaythrough,
        .dungeonLevel = g->dungeonLevel
    };

    for (i = 0; i < ACHIEVEMENT_COUNT; i++) {
        save.achievements[i] = g->achievements[i];
        save.tempAchievements[i] = g->tempAchievements[i];
    }
    for (i = 0; i < 3; i++) {
        save.selectedDungeons[i] = g->selectedDungeons[i];
        save.currentDungeonClear[i] = g->currentDungeonClear[i];
    }
    for (i = 0; i < MAX_DUNGEONS; i++) {
        save.allDungeons[i] = g->allDungeons[i];
        save.checkPlaythroughDungeonCleared[i] = g->checkPlaythroughDungeonCleared[i];
    }

    FILE* savefile = fopen(filename, "wb");
    fwrite(&save, sizeof(GameState), 1, savefile);
    fclose(savefile);

    printf("Game saved!\n");
    printf("Press any key to continue...\n");
    getch();
}

/*  Description: Used to display the inventory of the player
    Precondition: GameState *g is initialized
    @param <GameState *g> <Used to access the struct of the player to and check how many items they have left>
    @return <void>
*/
void 
showInventory(GameState *g) {

    printf("These are the items you have, Yohane!\n\n");

    playerStats(g);

    printf("1. Tears of a Fallen Angel\tx\t%d\n", g->player.tears);
    printf("2. Noppo Bread\tx\t%d\n", g->player.noppo);
    
    if (g->allDungeons[4].clears){
        printf("3. Choco-Mint Ice Cream\tx\t%d\n", g->player.iceCream);
    }

    printf("Press any key.\n");
    getchar();
}

/*  Description:  Used to show Hanamaru's shop module, where they can interact and buy  
    Precondition: GameState *g is initialized
    @param <GameState *g> <Used to access the struct of the player in case they bought something and check if they have rescued the proper Aquor members for the item to be available>
    @return <void>
*/
void 
showShop(GameState *g) {
    int i,j;
    int nFlag = 0;
    int nItemCount = 0;
    char input;
    int inputConv;
    int itemID[MAX_SHOP];
    int itemPrice[MAX_SHOP];

    string50 itemName[MAX_SHOP] = {
        "Tears of a fallen angel", "Noppo Bread", "Shovel Upgrade",
        "Bat Tamer", "Air Shoes", "Stewshine", 
        "Mikan Mochi", "Kurosawa Macha", "Choco-Mint Ice Cream"};

    do{
        clearScreen();
        nItemCount = 0;
        printf("Hanamaru: Yohane-chan, zura! What can I do for you today?\n\n");

        printf("Total Gold: %d GP\n\n", g->player.gold);

        if (g->player.tears == 0){
            printf("[%d] %-15s30GP\t\t[R]eturn\n", ++nItemCount,"Tears of a fallen angel");
            itemID[nItemCount - 1] = TEARS_OF_A_FALLEN_ANGEL;
            itemPrice[nItemCount - 1] = 30;
            printf("[%d] %-15s100GP\n", ++nItemCount,"Noppo Bread");
            itemID[nItemCount - 1] = NOPPO_BREAD;
            itemPrice[nItemCount - 1] = 100;
        }

        else{
            printf("[%d] %-15s100GP\t\t[R]eturn\n", ++nItemCount,"Noppo Bread");
            itemID[nItemCount - 1] = NOPPO_BREAD;
            itemPrice[nItemCount - 1] = 100;
        }

        if (g->player.shovelUpgrade == 0 && g->allDungeons[6].clears >= 1){
            printf("[%d] %-15s300GP\n", ++nItemCount,"Shovel Upgrade");
            itemID[nItemCount - 1] = SHOVEL_UPGRADE;
            itemPrice[nItemCount - 1] = 300;
        }

        if (g->player.batTamer == 0 && g->allDungeons[1].clears >= 1){
            printf("[%d] %-15s400GP\n", ++nItemCount,"Bat Tamer");
            itemID[nItemCount - 1] = BAT_TAMER;
            itemPrice[nItemCount - 1] = 400;
        }

        if (g->player.airShoes == 0 && g->allDungeons[2].clears >= 1){
            printf("[%d] %-15s500GP\n", ++nItemCount,"Air Shoes");
            itemID[nItemCount - 1] = AIR_SHOES;
            itemPrice[nItemCount - 1] = 500;
        }

        if (g->player.stewShine == 0 && g->allDungeons[7].clears >= 1){
            printf("[%d] %-15s1000GP\n", ++nItemCount,"Stewshine");
            itemID[nItemCount - 1] = STEWSHINE;
            itemPrice[nItemCount - 1] = 1000;
        }

        if (g->player.mikanMochi == 0 && g->allDungeons[0].clears >= 1){
            printf("[%d] %-15s1000GP\n", ++nItemCount,"Mikan Mochi");
            itemID[nItemCount - 1] = MIKAN_MOCHI;
            itemPrice[nItemCount - 1] = 1000;
        }

        if (g->player.kurosawaMacha == 0 && g->allDungeons[5].clears >= 1){
            printf("[%d] %-15s1000GP\n", ++nItemCount,"Kurosawa Macha");
            itemID[nItemCount - 1] = KUROSAWA_MACHA;
            itemPrice[nItemCount - 1] = 1000;
        }
        if (g->player.iceCream == 0 && g->allDungeons[4].clears >= 1){
            printf("[%d] %-15s2000GP\n", ++nItemCount,"Choco-Mint Ice Cream");
            itemID[nItemCount - 1] = CHOCO_MINT_ICE_CREAM;
            itemPrice[nItemCount - 1] = 2000;
        }

        printf("\n\n\nChoice: ");

        input = getchar();
        while (getchar() != '\n');

        switch(input){
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                inputConv = input - '0';

                if (inputConv >= 1 && inputConv <= nItemCount){
                    for (i = 0; i < nItemCount; i++){
                        if (inputConv - 1 == i){
                            if(g->player.gold < itemPrice[i]){
                                printf("Not enough gold!\n");
                                printf("Press any key to continue...\n");
                                getch();
                            }
                            else{
                                for (j = 0; j < MAX_SHOP; j++){
                                    if (itemID[i] - 1 == j){
                                        printf("You bought %s!\n", itemName[j]);
                                    }
                                }

                                g->player.gold -= itemPrice[i];
                                g->totalGoldSpent += itemPrice[i];

                                switch (itemID[i]){
                                    case TEARS_OF_A_FALLEN_ANGEL:
                                        g->player.tears = 1;
                                        break;

                                    case NOPPO_BREAD:
                                        g->player.noppo++;
                                        break;
                                    
                                    case SHOVEL_UPGRADE:
                                        g->player.shovelUpgrade = 1;
                                        break;

                                    case BAT_TAMER:
                                        g->player.batTamer = 1;
                                        break;
                                    
                                    case AIR_SHOES:
                                        g->player.airShoes = 1;
                                        break;
                                    
                                    case STEWSHINE:
                                        g->player.stewShine = 1;
                                        g->player.maxHP+= 1;
                                        g->player.HP+= 1;
                                        break;

                                    case MIKAN_MOCHI:
                                        g->player.mikanMochi = 1;
                                        g->player.maxHP+= 1;
                                        g->player.HP+= 1;
                                        break;
                                    
                                    case KUROSAWA_MACHA:
                                        g->player.kurosawaMacha = 1;
                                        g->player.maxHP+= 1;
                                        g->player.HP+= 1;
                                        break;
                                    
                                    case CHOCO_MINT_ICE_CREAM:
                                        g->player.iceCream++;
                                        break;
                                }
                                printf("Press any key to continue...\n");
                                getch();
                            }
                        }
                    }
                }
                else{
                    printf("Invalid Choice\n");
                    printf("Press any key to continue...\n");
                    getch();
                }
                break;

            case 'r':case 'R':
                nFlag = 1;
                break;
        }
    }while(nFlag == 0);

    if (g->totalGoldSpent >= 5000 && g->achievements[MITAIKEN_HORIZON - 1].status == false){
        achievementUpdater(g, MITAIKEN_HORIZON);
    }
}

/*  Description:  Used to check if the cell is a walkable tile
    Precondition: Paramters contain valid characters and row and col is not a negative number
    @param <char map[][MAX_COLS + 1]> <Used as the map to be checked>
    @param <int row> <Used as an index for map for rows>
    @param <int row> <Used as an index for map for columns>
    @return <int>
*/
int 
cellEmpty(char map[][MAX_COLS + 1], int row, int col) {
    return (map[row][col] == '.');
}

/*  Description:  Used to load the maps from the directory, used only for dungeons 1 to 3
    Precondition: Paramters are initialized
    @param <GameState *g> <Used to load each maps in the structs>
    @return <void>
*/
void 
loadMap(GameState *g) {
    int dungeonIdentifier = 0; 
    int mapIdentifier = 0;
    int i;

    string50 fileName[MAPS_COUNT]= {"maps/yasudaya_ryokan.txt", "maps/numazu_deep_sea_aquarium.txt", 
        "maps/izu_mito_sea_paradise.txt", "maps/shougetsu_confectionary.txt", 
        "maps/nagahama_castle_ruins.txt", "maps/numazugoyotei.txt", 
        "maps/uchiura_bay_pier.txt", "maps/awashima_marine_park.txt"};

    int nLineIndex = 0;
    int nFloorIndex = 0; 

    FILE *fp;

    for (i = 0; i < MAPS_COUNT; i++){
        fp = fopen(fileName[i], "r");
        
        nFloorIndex = 0;
        while(fscanf(fp, "%d %d", &dungeonIdentifier, &mapIdentifier) != EOF){
            nLineIndex = 0;
            while(fscanf(fp, "%s", g->allDungeons[i].map[nFloorIndex][nLineIndex]) == 1 && nLineIndex < 12){
                nLineIndex++;
            }
            nFloorIndex++;
        }

        fclose(fp);
    }
}

/*  Description:  Used to display the entirety of the current map being played
    Precondition: Paramters are initialized properly and contains valid integers and characters
    @param <GameState *g> <Used to access the player struct>
    @param <char map[][MAX_COLS + 1]> <Used as the map to be displayed>
    @param <Bat bats[]> <Used to check for behavior of the bats and the position>
    @param <int batCount> <Used as the maximum bats for the looping of the bats array>
    @param <int lailapsRow> <Used to check for Lailaps' row>
    @param <int lailapsCol> <Used to check for Lailaps' column>
    @return <void>
*/
void 
displayMap(GameState *g, char map[][MAX_COLS + 1], Bat bats[], int batCount, int lailapsRow, int lailapsCol) {
    
    int i;
    int row, col;
    bool drew;
    char ch;
    float hpRatio;
    const char * playerColor;

    for (row = 0; row < MAX_ROWS; ++row) {
        for (col = 0; col < MAX_COLS; ++col) {
            drew = false;
            // check if player is here
            if (row == g->player.row && col == g->player.col) {
                hpRatio = (float)g->player.HP / g->player.maxHP;
                if (g->player.HP <= 0) {
                    playerColor = COLOR_GREY;
                } 
                else if (hpRatio < 0.26){
                    playerColor = COLOR_RED;
                }
                else if (hpRatio < 0.51){
                    playerColor = COLOR_ORANGE;
                }
                else if (hpRatio < 0.76){
                    playerColor = COLOR_YELLOW;
                }
                else {
                    playerColor = COLOR_GREEN;
                }
                printf("%sY%s", playerColor, COLOR_RESET);
                drew = true;
            }
            if (!drew && row == lailapsRow && col == lailapsCol){
                printf("%sL%s", COLOR_CYAN, COLOR_RESET);
                drew = true;
            }
            // bat check
            if(!drew) {
                for (i = 0; i < batCount; i++) {
                    if (bats[i].active && bats[i].row == row && bats[i].col == col) {
                        if (bats[i].hit) {
                            printf("%sB%s", COLOR_RED, COLOR_RESET);
                        } else {
                            printf("%sb%s", COLOR_RED, COLOR_RESET);
                        }
                        drew = true;
                    }
                }
            }
            // color for map tiles
            if (!drew) {
                ch = map[row][col];
                switch (ch) {
                    case 'w':
                    printf("%sw%s", COLOR_BLUE, COLOR_RESET);
                    break;
                    case 'E':
                    printf("%sE%s", COLOR_YELLOW, COLOR_RESET);
                    break;
                    case 'g':
                    printf("%sg%s", COLOR_YELLOW, COLOR_RESET);
                    break;
                    case 'h':
                    printf("%sh%s", COLOR_ORANGE, COLOR_RESET);
                    break;
                    case 'T':
                    printf("%sT%s", COLOR_YELLOW, COLOR_RESET);
                    break;
                
                    default:
                        putchar(ch);
                        break;
                }
            }
        }
        putchar('\n');
    }
    

}

/*  Description:  Used to display the death cause and make sure the playthrough is properly closed for the next playthrough
    Precondition: Paramters are initialized properly and death cause contains valid DEATH CAUSE ID from the #define
    @param <GameState *g> <Used to access the members that needed to reset after the player dies>
    @param <int deathCause> <Used as the pivot for the switch on which death message to show>
    @return <void>
*/
void 
dungeonResolution(GameState *g, int deathCause){
    int i;
    switch(deathCause){
        case DEATH_BAT:
            printf(COLOR_RED "%32s\n" COLOR_RESET, "You Died!");
            printf("%30s %s%s%s\n\n","Killed by:", COLOR_RED, "Bat", COLOR_RESET);
            printf("Press any key to continue...\n");
            getchar();

            g->numPlaythrough = 0;
            for (i = 0; i < MAX_DUNGEONS; i++){
                g->checkPlaythroughDungeonCleared[i] = false;
            }
            g->numPlaythroughDungeonCleared = 0;

            break;
        case DEATH_BURNED:
            printf(COLOR_RED "%32s\n" COLOR_RESET, "You Died!");
            printf("%30s %s%s%s\n\n","Killed by:", COLOR_RED, "Heat Tile", COLOR_RESET);
            printf("Press any key to continue...\n");
            getchar();

            g->numPlaythrough = 0;
            for (i = 0; i < MAX_DUNGEONS; i++){
                g->checkPlaythroughDungeonCleared[i] = false;
            }
            g->numPlaythroughDungeonCleared = 0;

            break;

        case DEATH_IMPALED:
            printf(COLOR_RED "%32s\n" COLOR_RESET, "You Died!");
            printf("%30s %s%s%s\n\n","Killed by:", COLOR_RED, "Spike", COLOR_RESET);
            printf("Press any key to continue...\n");
            getchar();

            g->numPlaythrough = 0;
            for (i = 0; i < MAX_DUNGEONS; i++){
                g->checkPlaythroughDungeonCleared[i] = false;
            }
            g->numPlaythroughDungeonCleared = 0;

            break;

        case DEATH_SIREN:
            printf(COLOR_RED "%32s\n" COLOR_RESET, "You Died!");
            printf("%30s %s%s%s\n\n","Killed by:", COLOR_RED, "Siren", COLOR_RESET);
            printf("Press any key to continue...\n");
            getchar();

            g->numPlaythrough = 0;
            for (i = 0; i < MAX_DUNGEONS; i++){
                g->checkPlaythroughDungeonCleared[i] = false;
            }
            g->numPlaythroughDungeonCleared = 0;
            break;
    }
}

/*  Description:  Used to process the backend of a specific dungeon, from players' movement, bats movement, etc.
    Precondition: integer parameter are non negative, string parameter contains valid string, and struct is properly initialized
    @param <GameState *g> <Used to access the members for the bats, players, the dungeons itself, etc.>
    @param <char map[][MAX_COLS + 1]> <Used as the map of which dunegon is being cureently played at>
    @param <int dungeonIdx> <Used to as index for which dunegon is being playeed currently>
    @param <int floor> <Used to show which floor currently is>
    @param <int dungeonLevel> <Used to to access which dungeon level the specific dungeon currently is>
    @param <string20 player> <Used for the saving isnide another function>
    @return <void>
*/
void 
dungeonFloor(GameState *g, char map[][MAX_COLS + 1], int dungeonIdx, int floor, int dungeonLevel, string20 player) {
    
    int i, j;
    char input;
    char newCell;

    Bat bats[MAX_BATS];
    int batCount = rand() % 4 + 5;
    int moveInterval;
    int batRow, batCol;
    int batGold = 5 * dungeonLevel;
    // TEMPORARILY DISABLED
    float dmg = 0.5f * dungeonLevel;
    // TESTING ONLY
    //float dmg = 0;
    int batKilled;
    int batAttacked;
    
    int directions[8][2];
    int dirCount;
    int validDirs[8];
    int validCount;
    int chosenDir;
    int attackDirection;
    int adjRow;
    int adjCol;

    bool floorActive;

    int moveRow, moveCol;
    int newRow, newCol;
    bool reachedExit;

    bool stayedStill;
    bool validMove;

    int treasureGold;

    int spawnFlag = 0; 


    if (g->dungeonLevel == 1) {
        moveInterval = 2;
    } else {
        moveInterval = 1;
    }

    // spawn bats
    for (i = 0; i < batCount; ++i) {
        bats[i].active = true;
        bats[i].moveInterval = moveInterval;
        bats[i].movesSince = 0;
        // randomizing bat spawn at valid location
        do {
            batRow = rand() % (MAX_ROWS - 2) + 1;
            batCol = rand() % (MAX_COLS - 2) + 1;
            // Changed from while (cellEmpty(map, batRow, batCol));
        } while (!cellEmpty(map, batRow, batCol));
        bats[i].row = batRow;
        bats[i].col = batCol;
    }

    // Added this for Yohane spawn point
    for (i=0; i < MAX_ROWS && spawnFlag == 0; i++){
        for(j = 0; j < MAX_COLS && spawnFlag == 0; j++){
            if (map[i][j] == 'Y'){
                g->player.row = i;
                g->player.col = j;
                map[i][j] = '.';
                spawnFlag = 1;
            }
        }
    }

    floorActive = true;

    while (g->player.HP > 0 && floorActive) {
        clearScreen();
        printf("Dungeon %d: %s\n", dungeonLevel, g->allDungeons[dungeonIdx].name);
        printf("Floor %d of %d\n\n", floor + 1, g->allDungeons[dungeonIdx].floors);
        playerStats(g);
        displayMap(g, map, bats, batCount, -1, -1);

        //INPUT MODIFIED TO:
        input = getch();
        
        moveRow = moveCol = 0;
        reachedExit = false;
        stayedStill = false;
        validMove = false;

        // item switching
        if (input == '[') {
            // previous item
            if (g->player.tears > 0) {
                g->player.itemOnHand = 'T';
            } else if (g->player.noppo > 0) {
                g->player.itemOnHand = 'N';
            } else if (g->player.iceCream > 0) {
                g->player.itemOnHand = 'C';
            } else {
                g->player.itemOnHand = '.';
            }
            stayedStill = true;
        } else if (input == ']') {
            // next item
            if (g->player.noppo > 0) {
                g->player.itemOnHand = 'N';
            } else if (g->player.iceCream > 0) {
                g->player.itemOnHand = 'C';
            } else if (g->player.tears > 0) {
                g->player.itemOnHand = 'T';
            } else {
                g->player.itemOnHand = '.';
            }
            stayedStill = true;
        } else if (input == ' ') {
            // use item
            if (g->player.itemOnHand == 'N'){
                if (g->player.HP + 0.5 <= g->player.maxHP){
                    g->player.HP+= 0.5;
                    g->player.noppo--;
                }
                if (g->player.noppo <= 0){
                    if (g->player.noppo > 0) {
                        g->player.itemOnHand = 'N';
                    } else if (g->player.iceCream > 0) {
                        g->player.itemOnHand = 'C';
                    } else if (g->player.tears > 0) {
                        g->player.itemOnHand = 'T';
                    } else {
                        g->player.itemOnHand = '.';
                    }
                }
            }
            else if(g->player.itemOnHand == 'T'){
                if (g->player.HP + 0.5 <= g->player.maxHP){
                    g->player.HP+= 0.5;
                    g->player.tears--;
                }
                if (g->player.tears <= 0){
                    if (g->player.noppo > 0) {
                        g->player.itemOnHand = 'N';
                    } else if (g->player.iceCream > 0) {
                        g->player.itemOnHand = 'C';
                    } else if (g->player.tears > 0) {
                        g->player.itemOnHand = 'T';
                    } else {
                        g->player.itemOnHand = '.';
                    }
                }
            }
            stayedStill = true;
        } else if (input == 'X' || input == 'x') {
            // stay still
            stayedStill = true;
        } else if (input=='W'||input=='w'||input=='A'||input=='a'||input=='S'||input=='s'||input=='D'||input=='d') {
            if (input=='W'||input=='w') {
                moveRow = -1;
            } else if (input=='A'||input=='a') { 
                moveCol = -1;
            } else if (input=='S'||input=='s') {
                moveRow = 1;
            } else if (input=='D'||input=='d') {
                moveCol = 1;
            }
            
            newRow = g->player.row + moveRow;
            newCol = g->player.col + moveCol;
            
            newCell = map[newRow][newCol];
            
            if (newCell == '*') {
                // do nothing
                stayedStill = true;
            } else if (newCell == 'g'){
                g->player.gold += batGold;
                printf("Found %dGP!\n", batGold);
                // Modified from map[newCol][newRow] = '.';
                map[newRow][newCol] = '.';
                g->player.row = newRow;
                g->player.col = newCol;
                validMove = true;


            } else if (newCell == 'v') {
                // dig wall
                // Modified from map[newCol][newRow] = '.';
                map[newRow][newCol] = '.';
                stayedStill = true;
            } else if (newCell == 'x') {
                // Move and take damage
                if (g->player.shovelUpgrade == false){
                    g->player.HP -= 0.5f;
                }
                // Modified from map[newCol][newRow] = '.';
                map[newRow][newCol] = '.';
                g->player.row = newRow;
                g->player.col = newCol;
                if (g->player.HP <= 0) {
                    if (g->player.itemOnHand == 'C'){
                        g->savedByRubyIceCream++;
                        g->player.HP = g->player.maxHP;
                        g->player.iceCream--;

                        if (g->player.noppo > 0) {
                            g->player.itemOnHand = 'N';
                        } else if (g->player.iceCream > 0) {
                            g->player.itemOnHand = 'C';
                        } else if (g->player.tears > 0) {
                            g->player.itemOnHand = 'T';
                        } else {
                            g->player.itemOnHand = '.';
                        }

                        if (g->savedByRubyIceCream == 1 && g->achievements[RUBY_CHAN_HAI_NANI_GA_SUKI - 1].status == false){
                            achievementUpdater(g, RUBY_CHAN_HAI_NANI_GA_SUKI);
                        }
                    }
                    else{
                        dungeonResolution(g, DEATH_IMPALED);
                        g->playthroughActive = false;   
                        autoSave(g, player);
                    }
                } else {
                    printf("Yohane has been impaled!\n");
                }
                validMove = true;
            } else if (newCell == 'w') {
                // impassable wate tile
                if (g->player.airShoes == false){
                    stayedStill = true;
                }
                else{
                    g->player.row = newRow;
                    g->player.col = newCol;
                }
            } else {
                
                // check for bat
                batKilled = -1;
                for (i = 0; i < batCount; i++) {
                    if (bats[i].active && bats[i].row == newRow && bats[i].col == newCol) {
                        batKilled = i;
                    }
                }
                // kills found bat
                if (batKilled != -1) {
                    bats[batKilled].active = false;
                    // Modified from map[newCol][newRow] = 'g';
                    map[newRow][newCol] = 'g';
                    stayedStill = true;
                } else {
                    g->player.row = newRow;
                    g->player.col = newCol;
                    validMove = true;
                    
                    if (newCell == 'T') {
                        // Treasure found
                        treasureGold = rand() % 91 + 10;
                        g->player.gold += treasureGold;
                        printf("Found %dGP!\n", treasureGold);
                        // Modified from map[newCol][newRow] = '.';
                        map[newRow][newCol] = '.';
                    }
                    if (newCell == 'E') {
                        reachedExit = true;
                    }
                }
                
                

            }
        } else {
            // invalid movement; still counts as movement
            stayedStill = true;
        }
        
        // if still standing on heat tile, take damage
        if (map[g->player.row][g->player.col] == 'h' && stayedStill) {
            if (g->player.airShoes == false){
                g->player.HP -= 1;
            }
            if (g->player.HP <= 0) {
                if (g->player.itemOnHand == 'C'){
                    g->savedByRubyIceCream++;
                    g->player.HP = g->player.maxHP;
                    g->player.iceCream--;

                    if (g->player.noppo > 0) {
                        g->player.itemOnHand = 'N';
                    } else if (g->player.iceCream > 0) {
                        g->player.itemOnHand = 'C';
                    } else if (g->player.tears > 0) {
                        g->player.itemOnHand = 'T';
                    } else {
                        g->player.itemOnHand = '.';
                    }

                    if (g->savedByRubyIceCream == 1 && g->achievements[RUBY_CHAN_HAI_NANI_GA_SUKI - 1].status == false){
                        achievementUpdater(g, RUBY_CHAN_HAI_NANI_GA_SUKI);
                    }
                }
                else{
                    dungeonResolution(g, DEATH_BURNED);
                    g->playthroughActive = false;
                    autoSave(g, player);
                }
            } else {
                printf("Yohane has been burned!\n");
            }
        }
        
        // bats move
        batAttacked = 0;
        for (i = 0; i < batCount; i++) {
            if (bats[i].active) {
                bats[i].hit = false;
                if (++bats[i].movesSince >= bats[i].moveInterval) {
                    bats[i].movesSince = 0;
                    // determine possible directions to move
                    dirCount = 0;
                    if (g->dungeonLevel == 1 || g->dungeonLevel == 2) {
                        // up, down, left, right
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = 1; dirCount++;
                    } else {
                        // All 8 directions
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = 1;
                        dirCount++;
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = 1;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = 1;
                        dirCount++;
                    }

                    // check if player is adjacent
                    attackDirection = -1;
                    for (j = 0; j < dirCount; ++j) {
                        adjRow = bats[i].row + directions[j][0];
                        adjCol = bats[i].col + directions[j][1];
                        if (adjRow == g->player.row && adjCol == g->player.col && batAttacked == 0) {
                            attackDirection = j;
                            batAttacked = 1;
                        }
                        
                    }
                    // if was attacked
                    if (attackDirection != -1) {
                        // attack
                        if (g->player.batTamer == true){
                            g->player.HP-=0.5;
                        }
                        else{
                            g->player.HP -= dmg;
                        }

                        if (g->player.HP <= 0) {
                            if (g->player.itemOnHand == 'C'){
                                g->savedByRubyIceCream++;
                                g->player.HP = g->player.maxHP;
                                g->player.iceCream--;

                                if (g->player.noppo > 0) {
                                    g->player.itemOnHand = 'N';
                                } else if (g->player.iceCream > 0) {
                                    g->player.itemOnHand = 'C';
                                } else if (g->player.tears > 0) {
                                    g->player.itemOnHand = 'T';
                                } else {
                                    g->player.itemOnHand = '.';
                                }

                                if (g->savedByRubyIceCream == 1 && g->achievements[RUBY_CHAN_HAI_NANI_GA_SUKI - 1].status == false){
                                    achievementUpdater(g, RUBY_CHAN_HAI_NANI_GA_SUKI);
                                }
                            }
                            else{
                                dungeonResolution(g, DEATH_BAT);
                                g->playthroughActive = false;
                                autoSave(g, player);
                            }
                        } else {
                            printf("Bat Hit You!\n");
                            bats[i].hit = true;
                        }
                    } else {
                        // move randomly into valid location
                        
                        // find valid directions
                        validCount = 0; // Added this
                        for (j = 0; j < dirCount; ++j) {

                            batRow = bats[i].row + directions[j][0];
                            batCol = bats[i].col + directions[j][1];
                            if (cellEmpty(map, batRow, batCol) || map[batRow][batCol] == 'w'){
                                validDirs[validCount++] = j;
                            }
                        }
                        // if found move into new cell
                        if (validCount > 0) {
                            chosenDir = validDirs[rand() % validCount];
                            bats[i].row += directions[chosenDir][0];
                            bats[i].col += directions[chosenDir][1];
                        }
                        // else don't move


                    } 
                    
                }
                
            }
            
        }
        if (reachedExit) {
            dungeonResolution(g, -1);
            //g->playthroughActive = false;
            floorActive = false;
            autoSave(g, player);
        }
    }
    
    // Added this 2 because it usedItem & validMvoe becomes uninitialized
    (void)validMove;
}

/*  Description:  Used to load the final map (not the dungeons from 1 to 3)
    Precondition: Parameter is proeprly intiialized
    @param <GameState *g> <Used to access the fianl battle map struct to be able to copy the text file properly>
    @return <void>
*/
void 
loadSirenMap(GameState *g){
    int nRowIndex = 0;
    FILE *fp;

    fp = fopen("finalMap/finalDungeon.txt", "r");

    while(fscanf(fp, "%s", g->sirenFinal.map[nRowIndex]) == 1 && nRowIndex < MAX_ROWS){
        nRowIndex++;
    }

    fclose(fp);
}

/*  Description:  Used to check the siren cage inside the final battle map
    Precondition: Parameters are non negative
    @param <int row> <Used for comparison if the row is touching the barrier/cage of siren>
    @param <int col> <Used for comparison if the col is touching the barrier/cage of siren>
    @return <int>
*/
int 
sirenCageChecker(int row, int col){
    int i, j;
    int barrierDetected = 0;

    // Checker
    for (i = 1;i < 3 && barrierDetected == 0; i++){
        for (j = 16; j < 33 && barrierDetected == 0; j++){
            if (row == i && col == j){
                barrierDetected = 1;
            }
        }
    }

    return barrierDetected;
}

/*  Description:  Used to spawn the switches inside the siren map
    Precondition: Paramters are properly initialized and integer is used as a proper valid indexing
    @param <GameState *g> <Used for access the final battle map>
    @param <Switch switches[MAX_PHASE][2]> <Used to store the pair of switch position itself>
    @param <int phase> <used as array index for the switches array to determine which siren phase it is currently at>
    @return <void>
*/
void 
spawnSwitch(GameState *g, Switch switches[MAX_PHASE][2],int phase){

    int i;
    do{
        switches[phase][0].row = rand() % (MAX_ROWS - 2) + 1;
        switches[phase][0].col = rand() % (MAX_COLS - 2) + 1;
    } while (!cellEmpty(g->sirenFinal.map, switches[phase][0].row, switches[phase][0].col)
        || g->sirenFinal.map[switches[phase][0].row][switches[phase][0].col] == '*'
        || sirenCageChecker(switches[phase][0].row, switches[phase][0].col));
    
    do{
        switches[phase][1].row = rand() % (MAX_ROWS - 2) + 1;
        switches[phase][1].col = rand() % (MAX_COLS - 2) + 1;
    } while (
        (!cellEmpty(g->sirenFinal.map, switches[phase][1].row, switches[phase][1].col))
        || g->sirenFinal.map[switches[phase][1].row][switches[phase][1].col] == '*'
        || (switches[phase][0].row == switches[phase][1].row && switches[phase][0].col == switches[phase][1].col)
        || (switches[phase][0].row - switches[phase][1].row > 2 || switches[phase][0].row - switches[phase][1].row < -2)
        || (switches[phase][0].col - switches[phase][1].col > 5 || switches[phase][0].col - switches[phase][1].col < -5)
        || sirenCageChecker(switches[phase][1].row, switches[phase][1].col)
    );

    for (i = 0; i < 2; i++){
        g->sirenFinal.map[switches[phase][i].row][switches[phase][i].col] = '0';
    }
}

/*  Description:  Used to put down the barrier of the siren when activated
    Precondition: Parametrs are properly intialized
    @param <GameState *g> <Used for access the final battle map>
    @return <void>
*/
void 
barrierOpen(GameState *g){
    int i, j;

    for (i = 1; i < 4; i++){
        for (j = 16; j < 34; j++){
            if (g->sirenFinal.map[i][j] == '*'){
                g->sirenFinal.map[i][j] = '.';
            }
        }
    }
}   

/*  Description:  Used to check the distance of the siren to Yohane and Lailaps using euclidean distance formula and returns which among the 2 is closer to the siren
    Precondition: Parametrs are properly intialized and integer parameters are non-negative and valid
    @param <GameState *g> <Used to access the players' position>
    @param <int lailapsRow> <Used as lailaps' positon>
    @param <int lailapsCol> <Used as lailaps' positon>
    @param <int sirenRow> <Used as sirens' positon>
    @param <int sirenCol> <Used as sirens' positon>
    @return <char>
*/
// CHECK WHICH IS CLOSER, YOHANE, LAILAPS, OR BOTH
char 
checkNear(GameState *g, int lailapsRow, int lailapsCol, int sirenRow, int sirenCol){

    double yohaneDistance;
    double lailapsDistance;

    char nearSiren;

    yohaneDistance = sqrt(pow(sirenRow - g->player.row, 2) + pow(sirenCol - g->player.col,2));
    lailapsDistance = sqrt(pow(sirenRow - lailapsRow, 2) + pow(sirenCol - lailapsCol,2));

    if (yohaneDistance > lailapsDistance){
        nearSiren = 'L'; // Lailaps is closer to Siren
    }

    else if (yohaneDistance < lailapsDistance){
        nearSiren = 'Y'; // Yohane is closer to Siren
    }

    else if(yohaneDistance == lailapsDistance){
        nearSiren = 'B'; // Both Yohane and Lailaps are close to Siren
    }

    return nearSiren;
}

/*  Description: Used to calcualte the angle of the siren to the player or to lailaps using vector formula and trigonometry (tangent)
    Precondition: Parameters are properly initialized and the integer parametes are non zero
    @param <GameState *g> <Used to access the players' coordinate point>
    @param <int lailapsRow> <Used for computation of vector of lailaps' row as coordinate point>
    @param <int lailapsCol> <Used for computation of vector of lailaps' column as coordinate point>
    @param <int sirenRow> <Used as the other coordinate point in getting the vector for the row for siren>
    @param <int sirenCol> <Used as the other coordinate point in getting the vector for the column for lailaps>
    @return <double>
*/
double 
calculateAngle(GameState *g, int lailapsRow, int lailapsCol, int sirenRow, int sirenCol,int mode){
    // FORMULA:
    // Vector = Yohane/Lailaps Row/Col - Siren Row/Col
    // Angle of Direction = tan 0 = Vector Row / Vector Col
    // Radian to Degrees = Radian * (180 deg/ pi)

    // x2/y2 = Siren
    // x1/y1 = Lailaps

    double sirenVectorX;
    double sirenVectorY;
    double sirenAngle;

    if (mode == 1){

        sirenVectorX = g->player.col - sirenCol;
        sirenVectorY = -(g->player.row - sirenRow);
    }  

    else if (mode == 2){
        sirenVectorX = lailapsCol - sirenCol;
        sirenVectorY =-(lailapsRow - sirenRow);
    }  

    sirenAngle = atan2(sirenVectorY, sirenVectorX) * (180.0 / M_PI);
    if (sirenAngle < 0){
        sirenAngle+= 360.0;
    }

    return sirenAngle;
}

/*  Description: Used to move the siren accrross the map and checks if it hits lailaps or yohane whcih kills them
    Precondition: Parameters are properly initialized and valid
    @param <GameState *g> <Used to access the siren map and to check the item on hand in case of icecream to revive yohane or lailapse and not die>
    @param <double sirenAngle> <Used to know which direction should the siren go to>
    @param <int *sirenRow> <Used to update the row of the siren>
    @param <int *sirenCol> <Used to update the column of the siren>
    @param <bool *sirenHit> <Updated if siren hits something>
    @param <int newRow> <Used to check if siren hit Yoahne>
    @param <int newCol> <Used to check if siren hit Yohane>
    @param <int lailapsNewRow> <Used to check if siren hit lailaps>
    @param <int lailapsNewCol> <Used to check if siren hit lailaps>
    @param <float *lailapsHP> <Used to reset lailaps' hp to 0 if hit by the siren>
    @param <string20 player> <Used for the autosave function when siren kills the player>
    @return <void>
*/
void 
sirenMovement(GameState *g, double sirenAngle,int *sirenRow, int *sirenCol, bool *sirenHit, int newRow, int newCol, int lailapsNewRow, int lailapsNewCol, float *lailapsHP, string20 player){
    int i,j;
    
    int newSirenRow;
    int newSirenCol;

    newSirenRow = *sirenRow;
    newSirenCol = *sirenCol;
    
    // EAST
    if ((0.0 <= sirenAngle && sirenAngle <= 22.5) || (337.5 < sirenAngle && sirenAngle <= 360.0 )){
        newSirenCol += 1;
    }

    // SOUTH EAST
    else if (22.5 < sirenAngle && sirenAngle <= 67.5){
        newSirenRow -= 1;
        newSirenCol += 1;
    }

    // SOUTH
    else if (67.5 < sirenAngle && sirenAngle <= 112.5){
        newSirenRow -= 1;
    }

    // SOUTH WEST
    else if (112.5 < sirenAngle && sirenAngle <= 157.5){
        newSirenRow -= 1;
        newSirenCol -= 1;
    }

    // WEST
    else if(157.5 < sirenAngle && sirenAngle <= 202.5){
        newSirenCol-=1;
    }

    // NORTH WEST
    else if (202.5 < sirenAngle && sirenAngle <= 247.5){
        newSirenRow += 1;
        newSirenCol -= 1;
    }
                    
    // NORTH
    else if (247.5 < sirenAngle && sirenAngle <= 292.5){
        newSirenRow+=1;
    }

    // NORTH EAST
    else if (292.5 < sirenAngle && sirenAngle <= 337.5){
        newSirenRow += 1;
        newSirenCol +=1;
    }

    if (g->sirenFinal.map[newSirenRow][newSirenCol] == '.'){
        for (i = 0; i < MAX_ROWS; i++){
            for (j = 0; j < MAX_COLS; j++){
                if (g->sirenFinal.map[i][j] == 'S'){
                    g->sirenFinal.map[i][j] = '.';
                }
            }
            
        }
        g->sirenFinal.map[newSirenRow][newSirenCol] = 'S';

        *sirenRow = newSirenRow;
        *sirenCol = newSirenCol;

        if((newRow == newSirenRow && newCol == newSirenCol) || (lailapsNewRow == newSirenRow && lailapsNewCol == newSirenCol)){
            *sirenHit = true;
            if (g->player.itemOnHand == 'C'){
                g->savedByRubyIceCream++;
                g->player.HP = g->player.maxHP;
                *lailapsHP = 4;
                g->player.iceCream--;

                if (g->player.noppo > 0) {
                    g->player.itemOnHand = 'N';
                } else if (g->player.iceCream > 0) {
                    g->player.itemOnHand = 'C';
                } else if (g->player.tears > 0) {
                    g->player.itemOnHand = 'T';
                } else {
                    g->player.itemOnHand = '.';
                }

                if (g->savedByRubyIceCream == 1 && g->achievements[RUBY_CHAN_HAI_NANI_GA_SUKI - 1].status == false){
                    achievementUpdater(g, RUBY_CHAN_HAI_NANI_GA_SUKI);
                }
            }
            else{
                g->playthroughActive = false;
                g->player.HP = 0;
                *lailapsHP = 0;
                dungeonResolution(g, DEATH_SIREN);
                autoSave(g, player);
            }
        }
    }
}

/*  Description:  Used to process the backend of the final battle
    Precondition: Struct is properly initalized and string contains valid string
    @param <GameState *g> <Used to access the members for the bats, players, map, etc.>
    @param <string20 player> <Used for the auto save>
    @return <void>
*/
void 
finalBattle(GameState *g, string20 player){
    // GENERAL UTILS
    int i,j;
    char input;
    char newCell;

    int moveRow, moveCol;
    int newRow, newCol;

    // MAP UTILS
    Switch switches[MAX_PHASE][2];
    bool switches1Active = false;
    bool switches2Active = false;
    bool switchesBothActive = false;
    int numSwitchesActive = 0; // Initialize no siwtches has been pressed yet
    bool floorActive;
    bool barrierActive = true;
    int phaseLevel = 0;
    bool reachedExit;

    // LAILAPS UTILS
    int lailapsRow;
    int lailapsCol;
    int lailapsNewRow;
    int lailapsNewCol;
    char lailapsNewCell;
    float lailapsHP = 4;// Initialize lailaps HP

    //SIREN UTILS
    //Initialize as neutral
    bool killedSiren = false;
    bool sirenActive = false;
    int sirenRow;
    int sirenCol;
    int sirenSpawnFound = 0;
    char playerNearSiren;
    double sirenAngle = 0;
    int randomLailapsYohane;
    bool sirenHit;
    int sirenGold = 750;

    //BATS UTILS
    Bat bats[509];
    int batCount = 0;
    //int moveInterval;
    int batRow, batCol;
    int batGold = 5 * (phaseLevel + 1);
    // TEMPORARY DISABLED
    float dmg = 0.5 * (phaseLevel + 1);
    // TESTING:
    //float dmg = 0;
    int batKilled;
    int batAttackedLailaps;
    int batAttackedYohane;

    // PLAYER UTILS
    int moveCount = 0;
    int spawnFlagY= 0;
    int spawnFlagL = 0;

    bool stayedStill;
    bool validMove;

    // a bunch of stuff // bat attack
    int directions[8][2];
    int dirCount;
    int validDirs[8];
    int validCount;
    int chosenDir;
    int attackDirectionYohane;
    int attackDirectionLailaps;
    int adjRow;
    int adjCol;

    int treasureGold;

    // Display
    string50 tempDungeonName;
    int width = 60;
    int padDungeon = 0;
    char newInput;

    loadSirenMap(g);
    // Spawning switch randomly
    spawnSwitch(g, switches, phaseLevel); // Initializing switch

    //void displayMap(GameState *g, char map[][MAX_COLS + 1], Bat bats[], int batCount) 

    // spawn bats

        // Yohane and Lailaps spawn identifier
        for (i=0; i < MAX_ROWS && (spawnFlagY == 0 || spawnFlagL == 0); i++){
            for(j = 0; j < MAX_COLS && (spawnFlagY == 0 || spawnFlagL == 0); j++){
                if (g->sirenFinal.map[i][j] == 'Y'){
                    g->player.row = i;
                    g->player.col = j;
                    g->sirenFinal.map[i][j] = '.';
                    spawnFlagY = 1;
                }

                if (g->sirenFinal.map[i][j] == 'L'){
                    lailapsRow = i;
                    lailapsCol = j;
                    g->sirenFinal.map[i][j] = '.';
                    spawnFlagL = 1;
                }
            }
        }

        // Siren spawn identiifer
        for (i = 0; i < MAX_ROWS && sirenSpawnFound == 0; i++){
            for (j = 0; j < MAX_COLS && sirenSpawnFound == 0; j++){
                if (g->sirenFinal.map[i][j] == 'S'){
                    sirenRow = i;
                    sirenCol = j;
                    sirenSpawnFound = 1;
                }
            }
        }
        
    
    floorActive = true;

    while(g->player.HP > 0 && floorActive == true && lailapsHP > 0){
        clearScreen();
        printf("Final Battle: Siren of the Mirror World!\n\n\n");
        if ((int)lailapsHP == lailapsHP){
            printf("Lailaps HP: %d / 4\n", (int)lailapsHP);
        }else{
            printf("Lailaps HP: %.1f / 4\n", lailapsHP);
        }
        playerStats(g);
        displayMap(g, g->sirenFinal.map, bats, batCount, lailapsRow, lailapsCol);
        
        input = getch();

        moveRow = moveCol = 0;

        stayedStill = false; 
        validMove = false;
        switches1Active = false;
        switches2Active = false;
        reachedExit = false;
        dmg = 0.5 * (phaseLevel + 1);
        //dmg = 0;
        
        // always update the behavior of the old bats to the current phase Level or number of switches active
        for (i = 0; i <batCount; i++){
            if (bats[i].active){
                if (numSwitchesActive == 0){
                    bats[i].moveInterval = 2;
                }
                else{
                    bats[i].moveInterval = 1;
                }
            }
        }
        // item switching
        if (input == '[') {
            // previous item
            if (g->player.tears > 0) {
                g->player.itemOnHand = 'T';
            } else if (g->player.noppo > 0) {
                g->player.itemOnHand = 'N';
            } else if (g->player.iceCream > 0) {
                g->player.itemOnHand = 'C';
            } else {
                g->player.itemOnHand = '.';
            }
            stayedStill = true;
        } else if (input == ']') {
            // next item
            if (g->player.noppo > 0) {
                g->player.itemOnHand = 'N';
            } else if (g->player.iceCream > 0) {
                g->player.itemOnHand = 'C';
            } else if (g->player.tears > 0) {
                g->player.itemOnHand = 'T';
            } else {
                g->player.itemOnHand = '.';
            }
            stayedStill = true;
        } else if (input == ' ') {
            // use item
            if (g->player.itemOnHand == 'N'){
                if (g->player.HP + 0.5 <= g->player.maxHP){
                    g->player.HP+= 0.5;
                    g->player.noppo--;
                }

                if (g->player.noppo <= 0){
                    if (g->player.noppo > 0) {
                        g->player.itemOnHand = 'N';
                    } else if (g->player.iceCream > 0) {
                        g->player.itemOnHand = 'C';
                    } else if (g->player.tears > 0) {
                        g->player.itemOnHand = 'T';
                    } else {
                        g->player.itemOnHand = '.';
                    }
                }
            }
            else if(g->player.itemOnHand == 'T'){
                if (g->player.HP + 0.5 <= g->player.maxHP){
                    g->player.HP+= 0.5;
                    g->player.tears--;
                }

                if (g->player.tears <= 0){
                    if (g->player.noppo > 0) {
                        g->player.itemOnHand = 'N';
                    } else if (g->player.iceCream > 0) {
                        g->player.itemOnHand = 'C';
                    } else if (g->player.tears > 0) {
                        g->player.itemOnHand = 'T';
                    } else {
                        g->player.itemOnHand = '.';
                    }
                }
            }
            stayedStill = true;
        } else if (input == 'X' || input == 'x') {
            // stay still
            stayedStill = true;
        } else if (input=='W'||input=='w'||input=='A'||input=='a'||input=='S'||input=='s'||input=='D'||input=='d') {
            if (input=='W'||input=='w') {
                moveRow = -1;
            } else if (input=='A'||input=='a') { 
                moveCol = -1;
            } else if (input=='S'||input=='s') {
                moveRow = 1;
            } else if (input=='D'||input=='d') {
                moveCol = 1;
            }
            
            newRow = g->player.row + moveRow;
            newCol = g->player.col + moveCol;
            
            newCell = g->sirenFinal.map[newRow][newCol];

            lailapsNewRow = lailapsRow + moveRow;
            lailapsNewCol = lailapsCol + moveCol;

            lailapsNewCell = g->sirenFinal.map[lailapsNewRow][lailapsNewCol];
            
            if (newCell == '*') {
                // do nothing
                stayedStill = true;

                if (lailapsNewCell != '*'){
                    lailapsRow = lailapsNewRow;
                    lailapsCol = lailapsNewCol;
                }
            } else if (newCell == 'g'){

                if (killedSiren && newRow == sirenRow && newCol == sirenCol){
                    g->player.gold += sirenGold;
                    printf("Found %dGP!\n", sirenGold);
                }

                else{
                    g->player.gold += batGold;
                    printf("Found %dGP!\n", batGold);
                }
                // Modified from map[newCol][newRow] = '.';
                g->sirenFinal.map[newRow][newCol] = '.';
                g->player.row = newRow;
                g->player.col = newCol;
                validMove = true;
            } else {
                // Check for siren
                if (sirenActive && newCell == 'S'){
                    killedSiren = true;
                    sirenActive = false;

                    g->sirenFinal.map[sirenRow][sirenCol] = 'g';
                    g->sirenFinal.map[1][24] = 'E';

                    for (i = 0; i < batCount; i++){
                        if(bats[i].active){
                            bats[i].active = false;
                            g->sirenFinal.map[bats[i].row][bats[i].col] = 'g';
                        }
                    }

                    // g->player.row = newRow;
                    // g->player.col = newCol;
                    // validMove = true;

                    stayedStill = true;
                }else{
                    // check for bat
                    batKilled = -1;
                    for (i = 0; i < batCount; i++) {
                        if (bats[i].active && bats[i].row == newRow && bats[i].col == newCol) {
                            batKilled = i;
                        }
                    }
                    // kills found bat
                    if (batKilled != -1) {
                        bats[batKilled].active = false;
                        // Modified from map[newCol][newRow] = 'g';
                        g->sirenFinal.map[newRow][newCol] = 'g';
                        stayedStill = true;
                    } else {
                        g->player.row = newRow;
                        g->player.col = newCol;
                        validMove = true;
                        
                        if (newCell == 'T') {
                            // Treasure found
                            treasureGold = rand() % 91 + 10;
                            g->player.gold += treasureGold;
                            printf("Found %dGP!\n", treasureGold);
                            // Modified from map[newCol][newRow] = '.';
                            g->sirenFinal.map[newRow][newCol] = '.';
                        }
                        if (newCell == 'E') {
                            reachedExit = true;
                        }
                    }
                }

            }
        } else {
            // invalid movement; still counts as movement
            stayedStill = true;
        }

        if (validMove){
            if (lailapsNewCell != '*' && !(lailapsNewRow == g->player.row && lailapsNewCol == g->player.col)){
                lailapsRow = lailapsNewRow;
                lailapsCol = lailapsNewCol;
            }
        }


        if (!stayedStill && !killedSiren){
            moveCount++;

            if (moveCount >= 8 && batCount < 509){
                bats[i].active = true;
                //bats[i].moveInterval = moveInterval;
                bats[i].movesSince = 0;
                // randomizing bat spawn at valid location
                do {
                    batRow = rand() % (MAX_ROWS - 2) + 1;
                    batCol = rand() % (MAX_COLS - 2) + 1;
                    // Changed from while (cellEmpty(map, batRow, batCol));
                //} while (!cellEmpty(g->sirenFinal.map, batRow, batCol));
                 } while (!cellEmpty(g->sirenFinal.map, batRow, batCol)
                    || (barrierActive && sirenCageChecker(batRow, batCol)));
                bats[i].row = batRow;
                bats[i].col = batCol;
                batCount++;
                moveCount= 0;
                
            }
        }

        // CHECK IF BOTH SWITCHES ARE PRESSED
        if (phaseLevel < MAX_PHASE){
            switchesBothActive = false;

            if ((g->player.row == switches[phaseLevel][0].row && g->player.col == switches[phaseLevel][0].col)
                || (lailapsRow == switches[phaseLevel][0].row && lailapsCol == switches[phaseLevel][0].col)){
                switches1Active = true;
            }

            if ((g->player.row == switches[phaseLevel][1].row && g->player.col == switches[phaseLevel][1].col)
                || (lailapsRow == switches[phaseLevel][1].row && lailapsCol == switches[phaseLevel][1].col)){
                switches2Active = true;
            }

            if (switches1Active && switches2Active){
                if ((g->player.row == switches[phaseLevel][0].row && g->player.col == switches[phaseLevel][0].col)
                    || (lailapsRow == switches[phaseLevel][1].row && lailapsCol == switches[phaseLevel][1].col)){
                    switchesBothActive = true;
                }

                else if ((g->player.row == switches[phaseLevel][1].row && g->player.col == switches[phaseLevel][1].col)
                    || (lailapsRow == switches[phaseLevel][0].row && lailapsCol == switches[phaseLevel][0].col)){
                        switchesBothActive = true;
                    }

                else{
                    switchesBothActive = false;
                }
            }

            if (switchesBothActive){
                numSwitchesActive++;
                for (i = 0; i < MAX_ROWS; i++){
                    for (j = 0; j < MAX_COLS; j++){
                        if (g->sirenFinal.map[i][j] == '0'){
                            g->sirenFinal.map[i][j] = '.';
                        }
                    }
                }
                if (phaseLevel < MAX_PHASE - 1){
                    phaseLevel++;
                    spawnSwitch(g, switches, phaseLevel);
                }

                else{
                    phaseLevel = 3;
                }
            }
        }

        // Final Phase
        if (phaseLevel == 3){
            // Put down the barrier
            barrierOpen(g);
            sirenActive = true;
            barrierActive = false;
        }

        // Siren Move
        if (sirenActive && !barrierActive && !killedSiren){

            // Find Lailaps and Yohane, check which of them is nearer to the Siren
            playerNearSiren = checkNear(g, lailapsRow, lailapsCol, sirenRow, sirenCol);

            switch(playerNearSiren){
                // FORMULA:
                // Vector = Yohane/Lailaps Row/Col - Siren Row/Col
                // Angle of Direction = tan 0 = Vector Row / Vector Col
                // Radian to Degrees = Radian * (180 deg/ pi)
                // x2/y2 = Siren
                // x1/y1 = Lailaps

                case 'Y':
                    sirenAngle = calculateAngle(g, lailapsRow, lailapsCol, sirenRow, sirenCol, 1);

                    sirenMovement(g, sirenAngle,&sirenRow, &sirenCol, &sirenHit, newRow, newCol, lailapsNewRow, lailapsNewCol, &lailapsHP, player);
                    break;
                case 'L':
                    sirenAngle= calculateAngle(g, lailapsRow, lailapsCol, sirenRow, sirenCol, 2);
                    sirenMovement(g,sirenAngle, &sirenRow, &sirenCol, &sirenHit, newRow, newCol, lailapsNewRow, lailapsNewCol, &lailapsHP, player);

                    break;
                case 'B':
                    srand(time(NULL));
                    randomLailapsYohane = (rand() % 2) + 1;
                    sirenAngle = calculateAngle(g, lailapsRow, lailapsCol, sirenRow, sirenCol, randomLailapsYohane);
                    sirenMovement(g,sirenAngle, &sirenRow, &sirenCol, &sirenHit, newRow, newCol, lailapsNewRow, lailapsNewCol, &lailapsHP, player);
                    break;
            }
        }
        
        // bats move
        batAttackedLailaps = 0;
        batAttackedYohane = 0;
        for (i = 0; i < batCount; i++) {
            if (bats[i].active) {
                bats[i].hit = false;
                if (++bats[i].movesSince >= bats[i].moveInterval) {
                    bats[i].movesSince = 0;
                    // determine possible directions to move
                    dirCount = 0;
                    if (numSwitchesActive == 0 || numSwitchesActive == 1) {
                        // up, down, left, right
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = 1; dirCount++;
                    } else {
                        // All 8 directions
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = 0;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = 0; 
                        directions[dirCount][1] = 1;
                        dirCount++;
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = -1;
                        directions[dirCount][1] = 1;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = -1;
                        dirCount++;
                        directions[dirCount][0] = 1; 
                        directions[dirCount][1] = 1;
                        dirCount++;
                    }

                    // check if player is adjacent
                    attackDirectionYohane = -1;
                    attackDirectionLailaps = -1;
                    for (j = 0; j < dirCount; ++j) {
                        adjRow = bats[i].row + directions[j][0];
                        adjCol = bats[i].col + directions[j][1];
                        if (adjRow == g->player.row && adjCol == g->player.col && batAttackedYohane == 0 ) {
                            attackDirectionYohane = j;
                            batAttackedYohane = 1;
                        }

                        if (adjRow == lailapsRow && adjCol == lailapsCol && batAttackedLailaps == 0 ) {
                            attackDirectionLailaps = j;
                            batAttackedLailaps = 1;
                        }
                    }

                    if (attackDirectionLailaps != -1){
                        if (g->player.batTamer == true){
                            lailapsHP-=0.5;
                        }
                        else{
                            lailapsHP -= dmg;
                        }
                        if (lailapsHP <= 0) {
                            dungeonResolution(g, DEATH_BAT);
                            g->playthroughActive = false;
                            autoSave(g, player);
                        } else {
                            printf("Bat Hit Lailaps!\n");
                            bats[i].hit = true;
                        }
                    }
                    // if was attacked
                    else if (attackDirectionYohane != -1) {
                        // attack
                        if (g->player.batTamer == true){
                            g->player.HP-=0.5;
                        }
                        else{
                            g->player.HP -= dmg;
                        }
                        if (g->player.HP <= 0) {
                            if (g->player.itemOnHand == 'C'){
                                g->savedByRubyIceCream++;
                                g->player.HP = g->player.maxHP;
                                lailapsHP = 4;
                                g->player.iceCream--;

                                if (g->player.noppo > 0) {
                                    g->player.itemOnHand = 'N';
                                } else if (g->player.iceCream > 0) {
                                    g->player.itemOnHand = 'C';
                                } else if (g->player.tears > 0) {
                                    g->player.itemOnHand = 'T';
                                } else {
                                    g->player.itemOnHand = '.';
                                }
                                if (g->savedByRubyIceCream == 1 && g->achievements[RUBY_CHAN_HAI_NANI_GA_SUKI - 1].status == false){
                                    achievementUpdater(g, RUBY_CHAN_HAI_NANI_GA_SUKI);
                                }
                            }
                            else{
                                dungeonResolution(g, DEATH_BAT);
                                g->playthroughActive = false;
                                autoSave(g, player);
                            }
                        } else {
                            printf("Bat Hit Yohane!\n");
                            bats[i].hit = true;
                        }
                    } else {
                        // move randomly into valid location


                        // find valid directions
                        validCount = 0; // Added this
                        for (j = 0; j < dirCount; ++j) {

                            batRow = bats[i].row + directions[j][0];
                            batCol = bats[i].col + directions[j][1];
                            if (cellEmpty(g->sirenFinal.map, batRow, batCol) || g->sirenFinal.map[batRow][batCol] == 'w'){
                                validDirs[validCount++] = j;
                            }
                        }
                        // if found move into new cell
                        if (validCount > 0) {
                            chosenDir = validDirs[rand() % validCount];
                            bats[i].row += directions[chosenDir][0];
                            bats[i].col += directions[chosenDir][1];
                        }
                        // else don't move
                    } 
                }   
            }
        }
        if (reachedExit) {
            dungeonResolution(g, -1);
            floorActive = false;
            g->playthroughActive = false;
            
            // Achievement thingies
            g->numDungeonCleared++;
            g->numSirenKilled++;

            if (g->player.HP == g->player.maxHP && lailapsHP == 4){
                g->noDamageDungeonClear++;
            }

            if (g->player.gold == 0){
                g->noGoldOnHand++;
            }

            if (g->numSirenKilled == 1 && g->achievements[IN_THIS_UNSTABLE_WORLD - 1].status == false){
                achievementUpdater(g, IN_THIS_UNSTABLE_WORLD);
            }
            else if (g->numSirenKilled == 2 && g->achievements[DEEP_RESONANCE - 1].status == false){
                achievementUpdater(g, DEEP_RESONANCE);
            }

            if (g->noGoldOnHand == 1 && g->achievements[STEP_ZERO_TO_ONE - 1].status == false){
                achievementUpdater(g, STEP_ZERO_TO_ONE);
            }
            autoSave(g, player);
        }
    }

    // ACHIEVEMENT CHECKERY THINGIES
    if (killedSiren){

        // Display
        strcpy(tempDungeonName, "Final Battle: Siren of the Mirror World!");

        // concantenantetayueiagid the "complted!" and "rescued!" thingy 
        strcat(tempDungeonName, " Completed!");

        padDungeon = (width - strlen(tempDungeonName)) / 2;

        clearScreen();
        for (i = 0; i < ACHIEVEMENT_COUNT; i++){
            if (g->tempAchievements[i].status == true){
                printf(COLOR_YELLOW "Achievement unlocked: " COLOR_RESET "%s\n", g->achievements[i].name);
            }
        }

        printf("\n");
        for (i = 0; i < 60; i++){
            printf("*");
        }

        printf("\n%37s\n", "Dungeon Cleared!");
        printf("%*s%s\n", padDungeon,"",tempDungeonName);

        for (i = 0; i < 60; i++){
            printf("*");
        }

        /* ADD HERE THE SCRIPT */
            
        printf("\n\n[R]eturn\n\n");

        do{
            printf("Choice: ");
            newInput = getchar();
            while (getchar() != '\n');

            if (newInput > 64 && newInput < 91) {
                newInput += 32;
            }
        }while(newInput != 'r' && newInput != 'R');
    }

    // RESET THE TEMP ACHIEVEMENT
    for (i = 0; i < ACHIEVEMENT_COUNT; i++){
        if (g->tempAchievements[i].status == true){
            g->tempAchievements[i].status = false;
        }
    }

    if (g->numPlaythroughDungeonCleared == MAX_DUNGEONS){
        for (i = 0; i < MAX_DUNGEONS; i++){
            g->checkPlaythroughDungeonCleared[i] = false;
        }
        g->numPlaythroughDungeonCleared = 0;
    }

    (void)validMove;
}

/*  Description:  Used to process the first 3 dungeons
    Precondition: Struct is properly initalizedm, string is a valid valid, and inetegers are valid
    @param <GameState *g> <Used to access the dungeon member for dungeons processing>
    @param <int dungeonIdx> <Used which of the dungeons is used >
    @param <int dungeonLevel> <Used to check which dungeon number (1,2, or 3)>
    @param <int menuIdx> <Used for menu indexing when choosing dungeons>
    @param <string20 player> <Used for autosave>
    @return <void>
*/
void 
exploreDungeon(GameState *g, int dungeonIdx, int dungeonLevel, int menuIdx, string20 player) {
    int floor;
    int floorCompleteCounter = 0;
    int i;
    int allMembersPass = 0;

    // Display
    string50 tempDungeonName;
    string50 tempIdolName;
    int width = 60;
    int padIdol = 0;
    int padDungeon = 0;
    char input;

    // Removed srand since the random doesnt work and modified rand to +2 and +3 respectively

    if(dungeonLevel == 1) {
        g->allDungeons[dungeonIdx].floors = 1;
    } else if (dungeonLevel == 2) {
        g->allDungeons[dungeonIdx].floors = (rand() % 2) + 2;
    } else if (dungeonLevel == 3) {
        g->allDungeons[dungeonIdx].floors = (rand() % 2) + 3;
    }


    for (floor = 0; floor < g->allDungeons[dungeonIdx].floors && !(g->player.HP <= 0); ++floor) {
        loadMap(g);
        dungeonFloor(g, g->allDungeons[dungeonIdx].map[floor], dungeonIdx, floor, dungeonLevel, player);

        if (g->playthroughActive && g->player.HP > 0){
            floorCompleteCounter++;
        }
    }

    if (floorCompleteCounter == g->allDungeons[dungeonIdx].floors){
        g->currentDungeonClear[menuIdx] = true;

        // MORE ACHIEVMENT THINGIES
        g->allDungeons[dungeonIdx].clears++;
        g->numDungeonCleared++;
        if (g->player.HP == g->player.maxHP){
            g->noDamageDungeonClear++;
        }

        if (g->numDungeonCleared == 1 && g->achievements[YOHANE_DESCENDS-1].status == false){
            achievementUpdater(g, YOHANE_DESCENDS);
        }
        else if (g->numDungeonCleared == 10 && g->achievements[NO_10-1].status == false){
            achievementUpdater(g, YOHANE_DESCENDS); // DEBUG
        }

        if (g->noDamageDungeonClear == 1 && g->achievements[AOZORA_JUMPING_HEART - 1].status == false){
            achievementUpdater(g, AOZORA_JUMPING_HEART);
        }

        if (g->allDungeons[0].clears > 0 && g->allDungeons[1].clears >0 && g->allDungeons[2].clears > 0 && g->achievements[CYARON - 1].status == false){
            achievementUpdater(g, CYARON);
        }

        if (g->allDungeons[3].clears > 0 && g->allDungeons[4].clears >0 && g->allDungeons[5].clears > 0 && g->achievements[AZALEA - 1].status == false){
            achievementUpdater(g, AZALEA);
        }

        if (g->allDungeons[6].clears > 0 && g->allDungeons[7].clears >0 && g->achievements[GUILTY_KISS - 1].status == false){
            achievementUpdater(g, GUILTY_KISS);
        }

        for (i = 0; i < MAX_DUNGEONS; i++){
            if (g->allDungeons[i].clears == 1){
                allMembersPass++;
            }
        }

        if (allMembersPass == MAX_DUNGEONS && g->achievements[EIKYUU_HOURS - 1].status == false){
            achievementUpdater(g, EIKYUU_HOURS);
        }

        switch(dungeonIdx){
            // CHIKA
            case 0:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[MIKAN_POWER - 1].status == false){
                    achievementUpdater(g, MIKAN_POWER);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[ONE_MORE_SUNSHINE_STORY - 1].status == false){
                    achievementUpdater(g, ONE_MORE_SUNSHINE_STORY);
                }
                break;
            // RIKO
            case 1:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[RIKO_CHAN_BEAM - 1].status == false){
                    achievementUpdater(g, RIKO_CHAN_BEAM);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[PIANOFORTE_MONOLOGUE - 1].status == false){
                    achievementUpdater(g, PIANOFORTE_MONOLOGUE);
                }
                break;
            // You
            case 2:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[YOUSORO - 1].status == false){
                    achievementUpdater(g, YOUSORO);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[BEGINNER_SAILING - 1].status == false){
                    achievementUpdater(g, BEGINNER_SAILING);
                }
                break;

            // Hanamaru
            case 3:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[ITS_THE_FUTURE_ZURA - 1].status == false){
                    achievementUpdater(g, ITS_THE_FUTURE_ZURA);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[OYASUMINASAN - 1].status == false){
                    achievementUpdater(g, OYASUMINASAN);
                }
                break;

            // Ruby
            case 4:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[GANBARUBY - 1].status == false){
                    achievementUpdater(g, GANBARUBY);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[RED_GEM_WINK - 1].status == false){
                    achievementUpdater(g, RED_GEM_WINK);
                }
                break;
            // Dia
            case 5:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[BUU_BUU_DESU_WA - 1].status == false){
                    achievementUpdater(g, BUU_BUU_DESU_WA);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[WHITE_FIRST_LOVE - 1].status == false){
                    achievementUpdater(g, WHITE_FIRST_LOVE);
                }
                break;
            // Kanan
            case 6:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[HUG - 1].status == false){
                    achievementUpdater(g, HUG);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[SAKANA_KA_NANDAKA - 1].status == false){
                    achievementUpdater(g, SAKANA_KA_NANDAKA);
                }
                break;
            // Mari
            case 7:
                if (g->allDungeons[dungeonIdx].clears == 1 && g->achievements[SHINY - 1].status == false){
                    achievementUpdater(g, SHINY);
                }

                else if (g->allDungeons[dungeonIdx].clears == 2 && g->achievements[NEW_WINDING_ROAD - 1].status == false){
                    achievementUpdater(g, NEW_WINDING_ROAD);
                }
                break;
        }


        // Display
        strcpy(tempDungeonName, g->allDungeons[dungeonIdx].name);
        strcpy(tempIdolName, g->allDungeons[dungeonIdx].idol);

        // concantenantetayueiagid the "complted!" and "rescued!" thingy 
        strcat(tempDungeonName, " Completed!");
        strcat(tempIdolName, " rescued!");

        padDungeon = (width - strlen(tempDungeonName)) / 2;
        padIdol = (width - strlen(tempIdolName)) / 2;

        clearScreen();
        for (i = 0; i < ACHIEVEMENT_COUNT; i++){
            if (g->tempAchievements[i].status == true){
                printf(COLOR_YELLOW "Achievement unlocked: " COLOR_RESET "%s\n", g->achievements[i].name);
            }
        }

        printf("\n");
        for (i = 0; i < 60; i++){
            printf("*");
        }

        printf("\n%37s\n", "Dungeon Cleared!");
        printf("%*s%s\n", padDungeon,"",tempDungeonName);
        printf("%*s%s\n", padIdol,"",tempIdolName);

        for (i = 0; i < 60; i++){
            printf("*");
        }

        /* ADD HERE THE SCRIPT */
        
        printf("\n\n[R]eturn\n\n");

        do{
            printf("Choice: ");
            input = getchar();
            while (getchar() != '\n');

            if (input > 64 && input < 91) {
                input += 32;
            }
        }while(input != 'r' && input != 'R');

        // RESET THE TEMP ACHIEVEMENT
        for (i = 0; i < ACHIEVEMENT_COUNT; i++){
            if (g->tempAchievements[i].status == true){
                g->tempAchievements[i].status = false;
            }
        }

        // Checks on what dungeons you cleared
        g->checkPlaythroughDungeonCleared[dungeonIdx] = true;
        g->numPlaythroughDungeonCleared++;
    }
}

/*  Description:  Prints and handles the main menu of choosing a dungeon
    Precondition: Struct is properly initalizedm, string is a valid valid, and inetegers are valid
    @param <GameState *g> <Used to access the dungeon member for dungeons processing>
    @param <int dungeonLevel> <Used to check which dungeon number (1,2, or 3)>
    @param <string20 player> <Used for autosave or saverun>
    @return <void>
*/
void 
dungeonMenu(GameState *g, int dungeonLevel, string20 player) {
    int i, idx;
    char choice;
    int nCounter = 0;
    int nFinalBattle = 0;
    bool saveQuit = false;

    g->dungeonLevel = dungeonLevel;

    printf("Lailaps: Yohane! Where should we go to now?\n\n");

    playerStats(g);

    // Counts the how many current dungeon are cleared
    for (i = 0; i < 3; i++){
        if(g->currentDungeonClear[i] == true){
            nCounter++;
        }
    }

    // print selected dungeons and options
    for (i = 0; i < 3 && nFinalBattle == 0; i++) {
        if (nCounter == 3){
            nFinalBattle = 1;
            printf("[1] Face the Siren of Numazu");
            printf("\t\t[I]nventory\n");
            printf("\t\t[S]ave and Quit\n");
            if (g->allDungeons[3].clears > 0){
                printf("\t\t[H]anamaru's Shop\n");
            }
            else{
                printf("\n");
            }
        }

        else{
            //if (g->allDungeons[g->selectedDungeons[i]].clears) {
            if (g->currentDungeonClear[i] == true) {
                printf("[X] Visit %s", g->allDungeons[g->selectedDungeons[i]].name);
                switch (i) {
                    case 0:
                        printf("\t\t[I]nventory\n");
                        break;
                    case 1:
                        printf("\t\t[S]ave and Quit\n");
                        break;
                    case 2:
                        if (g->allDungeons[3].clears > 0){
                            printf("\t\t[H]anamaru's Shop\n");
                        } else {
                            printf("\n");
                        }
                        break;
                }
            } else {
                printf("[%d] Visit %s", i + 1, g->allDungeons[g->selectedDungeons[i]].name);
                switch (i) {
                    case 0:
                        printf("\t\t[I]nventory\n");
                        break;
                    case 1:
                        printf("\t\t[S]ave and Quit\n");
                        break;
                    case 2:
                        if (g->allDungeons[3].clears){
                            printf("\t\t[H]anamaru's Shop\n");
                        } else {
                            printf("\n");
                        }
                        break;
                }
            }
        }
    }

    printf("Your Choice: ");

    choice = getchar();
    while (getchar() != '\n');

    if (choice > 64 && choice < 91) {
        choice += 32;
    }

    if (nCounter == 3){
        switch (choice){
            case '1':
                finalBattle(g, player); // Siren function
                break;
            case 'i':
                clearScreen();
                showInventory(g);
                break;
            case 's':
                clearScreen();
                saveRun(g, player);
                saveQuit = 1;
                break;
            case 'h':
                if (g->allDungeons[3].clears > 0){
                    clearScreen();
                    showShop(g);
                }
                else{
                    clearScreen();
                    printf("Hanamaru is still not available!\n");
                    printf("Press any key to continue...\n");
                    getch();
                }
                break;
            default:
                clearScreen();
                break;
            
        }
    }
    else{
        if (choice >= '1' && choice <= '3') {
            idx = choice - '1';

            if (g->currentDungeonClear[idx] == false){
                clearScreen();
                exploreDungeon(g, g->selectedDungeons[idx], dungeonLevel, idx, player);
            }

        } else {
            switch (choice) {
                case 'i': case 'I':
                    clearScreen();
                    showInventory(g);
                    break;
                case 's': case 'S':
                    clearScreen();
                    saveRun(g, player);
                    saveQuit = 1;
                    break;
                case 'h': case 'H':
                    if (g->allDungeons[3].clears) {
                        clearScreen();
                        showShop(g);
                    } else {
                        clearScreen();
                        printf("Hanamaru is still not available!\n");
                        printf("Press any key to continue...\n");
                        getch();
                    }
                    break;
                default:
                    clearScreen();
                    break;
            }
        }
    }
    
    if(g->playthroughActive && saveQuit == false) {
        clearScreen();
        
        if (dungeonLevel < 3 && g->currentDungeonClear[dungeonLevel - 1] == true){
            dungeonMenu(g, dungeonLevel + 1, player);
        }
        else{
            dungeonMenu(g, dungeonLevel, player);
        }
    }
}

/*  Description:  Handles the New Game in main menu and handles the picking of the dungeons to be used
    Precondition: Struct is properly initalized and string is a valid valid
    @param <GameState *g> <Used to access the member of the GameState and reset it properly when new game>
    @param <string20 player> <Used for autosave or saverun>
    @return <void>
*/
void 
beginNewPlaythrough(GameState *g, string20 player) {
    
    int i,idx;
    int dungeons = MAX_DUNGEONS - g->numPlaythroughDungeonCleared;
    int pool[MAX_DUNGEONS];
    int nIndexCounter = 0;

    int newPool[MAX_DUNGEONS];
    int newIndexCounter = 0;

    g->dungeonLevel = 1;
    g->player.HP = 3;
    g->player.maxHP = 3;
    g->player.tears = 0;
    g->player.noppo = 0;
    g->player.iceCream = 0;
    g->player.itemOnHand = '.';
    g->player.shovelUpgrade = false;
    g->player.batTamer = false;
    g->player.airShoes = false;
    g->player.stewShine = false;
    g->player.mikanMochi = false;
    g->player.kurosawaMacha = false;

    if (g->playthroughActive && g->runActive) {
        dungeonMenu(g, 1, player);
        return;
    }

    if (g->numPlaythrough == 3){
        g->numPlaythrough = 0;

        for (i = 0; i < MAX_DUNGEONS; i++){
            g->checkPlaythroughDungeonCleared[i] = false;
        }
        g->numPlaythroughDungeonCleared = 0;
    }

    dungeons = MAX_DUNGEONS - g->numPlaythroughDungeonCleared;

    g->playthroughActive = true;
    g->numPlaythrough++;

    if (g->numPlaythroughDungeonCleared == MAX_DUNGEONS){
        for (i = 0; i < MAX_DUNGEONS; i++){
            g->checkPlaythroughDungeonCleared[i] = false;
        }
        g->numPlaythroughDungeonCleared = 0;
        dungeons = MAX_DUNGEONS;
    }
    
    for (i = 0; i < 3; i++){
        g->currentDungeonClear[i] = false;
    }
    
    for (i = 0; i < MAX_DUNGEONS; i++) {
        if (g->checkPlaythroughDungeonCleared[i] == false){
            pool[nIndexCounter] = i;
            nIndexCounter++;
        }
    }
    
    // selects 3 random dungeons
    if (g->numPlaythrough == 3){
        for (i = 0; i < 2; i++) {
            idx = rand() % dungeons;
            g->selectedDungeons[i] = pool[idx];
            pool[idx] = pool[--dungeons]; 
        }

        for (i = 0; i < MAX_DUNGEONS; i++){
            if (i != g->selectedDungeons[0] && i != g->selectedDungeons[1]){
                newPool[newIndexCounter] = i;
                newIndexCounter++;
            }
        }

        idx = rand() % newIndexCounter;

        g->selectedDungeons[2] =newPool[idx];
    }
    else{
        for (i = 0; i < 3; i++) {
            idx = rand() % dungeons;
            g->selectedDungeons[i] = pool[idx];
            pool[idx] = pool[--dungeons]; 
        }
    }

    // initialize the player
    if (!g->runActive) {
        g->player.HP = 3;
        g->player.maxHP = 3;
        g->player.gold = 0;
        g->player.tears = 0;
        g->player.noppo = 0;
        g->player.iceCream = 0;
        g->player.itemOnHand = '.';
    }
    
    dungeonMenu(g, 1, player);
}

/*  Description:  Used to the load the save filed
    Precondition: Struct is properly initalized and string is a valid valid
    @param <GameState *g> <Used to properly copy the saved file to the struct>
    @param <string20 filename> <Used loading the file with the same name in the firectory>
    @return <void>
*/
void 
loadRunFile(GameState *g, string20 filename){
    FILE *file = fopen(filename, "rb");

    //fread(g, sizeof(GameState), 1, file);
    fread(g, sizeof(GameState), 1, file);
    fclose(file);
    
    g->runActive = true;
}

/*  Description:  Used to show the stats of the player
    Precondition: Struct is properly initalized
    @param <GameState *g> <Used to show the members properly in a printf>
    @return <void>
*/
void 
stats(GameState *g) {
    printf("\tYasudaya Ryokan Clears: %d\n", g->allDungeons[0].clears);
    printf("\tNumazu Deep Sea Aquarium Clears: %d\n", g->allDungeons[1].clears);
    printf("\tIzu-Mito Sea Paradise Clears: %d\n", g->allDungeons[2].clears);
    printf("\tShougetsu Confectionary Clears: %d\n", g->allDungeons[3].clears);
    printf("\tNagahama Castle Ruins Clears: %d\n", g->allDungeons[4].clears);
    printf("\tNumazugoyotei Clears: %d\n", g->allDungeons[5].clears);
    printf("\tUchiura Bay Pier Clears: %d\n", g->allDungeons[6].clears);
    printf("\tAwashima Marine Park Clears: %d\n", g->allDungeons[7].clears);
    
    printf("\tTotal Dungeons Cleared: %d\n", g->numDungeonCleared);
    printf("\tSirens Defeated: %d\n", g->numSirenKilled);
    printf("\tTotal Gold Spent: %d\n", g->totalGoldSpent);

    printf("\nPress any key to continue...\n");
    getch();
}

/*  Description:  Used to show and print the main menu and handles the input
    Precondition: Struct is properly initalized and string is a valid string
    @param <GameState *g> <Used to check if the playthrough is still active and pass down the struct for loading file>
    @param <string20 player> <Used as the file name in loading file sand saving>
    @return <void>
*/
void 
mainMenu(GameState *g, string20 player) {
    clearScreen();
    char choice;
    int hasPlayedBefore = checkSaveFile(player);

    if (hasPlayedBefore){
        loadRunFile(g, player);
    }

    //g->playthroughActive = false;

    printf("████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n");
    printf("████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n");
    printf("████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████████\n");
    printf("██████████████████████████████████ █  ███████████████████████████████████████████ ██████████████████████████████████████████████████████████████\n");
    printf("████████████████████   ███████████      ████████████████████████████████████████   █████████████████████████████████████████████████████████████\n");
    printf("██████████████████████   ███████████      █████████████  ███   ██████████    ███   ████████ ████████████████████████████████████████████████████\n");
    printf("█████████████████████     █████████ █      ██    █████   ███   ██     ███   █ ███ █         ████████████████████████████████████████████████████\n");
    printf("████████████████████      █████████        █  ███   ██   █  █  ██ █   ███   ██  █ █    █████████        ████████████ ███████████████████████████\n");
    printf("████████████████████      ███████         █  ██   █  █   █ █   █ ███   ██   ███   █        █████ ██   ██  █       ██ ████      █████████████████\n");
    printf("█████████████████████      ████         ████  ████   █   ███   █        █   █████ █    ████████████   ██      █     ████ ██  ███████████████████\n");
    printf("███████████████████████              ████████      ██  █████ ██ █████   █ ███████ █          ██████   ██  █   █   █  ██   ██████████████████████\n");
    printf("█████████████████████████       █████████████████████████████████████████████ █████████████ ███████████████████████  ██████ ███████████████████\n");
    printf("                     ███         ███████            ███ █████████   ██████████████████████████  █████████████████ ███████ ████ ████             \n");
    printf("                    ███      █     ██     ██████      ███     ████████████████  ██    █    ███ ██   ██     ██   █████████████    █              \n");
    printf("                 ███████          ██      ███ ██       ██ ██   ███    █    █   ████   █   ██  ███   ██ █  █   ██     █     ███ ███              \n");
    printf("              ████              ███       ██  ██       █ ███   ██    ████   █         █   ██  ███   ██    █  ██   █  █   ██ ██ ██               \n");
    printf("              █    █████████  █████ █     ██  ██      █         ██   ██    ██  ████   █       ███   ██    ██   ███   █   ███ █ ██               \n");
    printf("              █  ███       █  ██  ██      █████      ██ █████    ██        █   ███   ██  █████  █   ██     ████   ████    ███  ██               \n");
    printf("              ████         ████    █       ██      ████████    ██     ██   █████ █████████   ████       ████  █████  █  ███ ██ █                \n");
    printf("                                   ██          ██████    █  ███████  ███    █                  █  ██████  █          █████   ███                \n");
    printf("                                   ██      ██████        █ ██     ████ ██████                  █████   ████                                     \n");
    printf("                          █████████████    ██                                                                                                   \n");
    printf("                         ██                ██                                                                                                   \n");
    printf("                         █  ███████████   ██                                                                                                    \n");
    printf("                         █ ██         █ ███                                                                                                     \n");
    printf("                         ███                                                                                                                    \n");

    if (g->playthroughActive) {
        printf("\t\t[C]ontinue\n");
    } else {
        printf("\t\t[N]ew Game\n");
    }
    printf("\t\t[V]iew Achievements\n");
    printf("\t\t[S]tats\n");
    printf("\t\t[Q]uit\n\n");
    
    printf("Your Choice: ");

    choice = getchar();
    while (getchar() != '\n');
    
    if (choice < 91) {
        choice += 32;
    }
    
    switch (choice){
    case 'n': case 'N':
        if (!g->playthroughActive){
            clearScreen();
            beginNewPlaythrough(g, player);
        }
        else{
            printf("You have a save file!\n");
            printf("Press any key to continue...\n");
            getch();
        }
        break;
    case 'c': case 'C':
        if (g->playthroughActive) {
            clearScreen();
            dungeonMenu(g, g->dungeonLevel, player);
        }
        else{
            printf("You do not have a save file!\n");
            printf("Press any key to continue...\n");
            getch();
        }
        break;
    case 'v': case 'V':
        clearScreen();
        viewAchievements(g);
        break;
    case 'q': case 'Q':
        clearScreen();
        saveRun(g, player);
        break;
    case 's': case 'S':
        clearScreen();
        stats(g);
        break;
    default:
        clearScreen();
        break;
    }
}

/*  Description:  Used to initialize the game everytime the program starts
    Precondition: Struct is properly declared
    @param <GameState *g> <Used for initiailizing the members of the struct>
    @return <void>
*/
void 
initializeGame(GameState *g){
    int i;
    g->playthroughActive = false;
    g->runActive = false; 

    g->dungeonLevel = 1;

    // ACHIEVMENT THINGY INITIALIZIATION
    g->numDungeonCleared = 0;

    for (i = 0; i < MAX_DUNGEONS; i++){
        g->allDungeons[i].clears = 0;
    }

    g->numSirenKilled = 0;
    g->noDamageDungeonClear = 0;
    g->totalGoldSpent = 0;
    g->savedByRubyIceCream = 0;
    g->noGoldOnHand = 0;
    
    string50 names[MAX_DUNGEONS] = {"Yasudaya Ryokan", "Numazu Deep Sea Aquarium", "Izu-Mito Sea Paradise",
        "Shougetsu Confectionary", "Nagahama Castle Ruins", "Numazugoyotei",
        "Uchiura Bay Pier", "Awashima Marine Park"};
        
    string10 idol[MAX_DUNGEONS] = {"Chika", "Riko", "You", "Hanamaru", "Ruby", "Dia", "Kanan", "Mari"};

    string50 achievementName[ACHIEVEMENT_COUNT] = {
        "Yohane Descends!", "Mikan Power!", "Riko-chan BEAM!", "Yousoro!", "It's the future, zura!", "Ganbaruby!", "Buu-buu desu wa!", "Hug!!!",
        "Shiny!", "In This Unstable World!", "One more sunshine story!", "Pianoforte Monologue!", "Beginner's Sailing!", "Oyasuminasan!", "Red Gem Wink!", "White First Love!",
        "Sakana ka Nandaka!", "New Winding Road!", "Deep Resonance!", "No. 10!", "CYaRon!", "AZALEA!", "Guilty Kiss!", "Eikyuu Hours!",
        "Aozora Jumping Heart!", "Mitaiken Horizon!", "Ruby-chan! Hai? Nani ga suki?", "Step! ZERO to ONE!"
    };

    string100 achievementDescription[ACHIEVEMENT_COUNT] = {
        "Cleared first dungeon", "Rescued Chika for the first time", "Rescued Riko for the first time", "Rescue You for the first time", "Rescued Hanamaru for the first time", "Rescued Ruby for the first time", "Rescue Dia for the first time", "Rescued Kanan for the first time",
        "Rescued Mari for the first time", "Beat the Final boss for the first time", "Rescued Chika twice", "Rescued Riko twice", "Rescued You twice", "Rescued Hanamaru twice", "Rescued Ruby twice", "Rescued Dia twice",
        "Rescued Kanan twice", "Rescued Mari twice", "Beat the Final boss twice", "Clear 10 dungeons", "Rescued Chika, You, and Ruby (Not necessarily in one playthrough)", "Rescued Hanamaru, Dia, and Mari (Not necessarily in one playthrough)", "Rescued Riko and Mari (Not necessarily in one playthrough)", "Have Yohane rescue all Aqours members for the first time",
        "Clear a dungeon without incurring any damage", "Accumulate a total of 5000G spent on Hanamaru's stores across multiple playthroughs", "Get saved by a fatal blow from Ruby's choco-mint ice cream item", "Complete a playthrough with 0G on-hand at the end"
    };

    for (i = 0; i < MAX_DUNGEONS; i++) {
        strcpy(g->allDungeons[i].name, names[i]);
        strcpy(g->allDungeons[i].idol, idol[i]);
        g->allDungeons[i].floors = i % 3 + 1;
        g->allDungeons[i].clears = 0;
    }   

    for (i = 0; i < ACHIEVEMENT_COUNT; i++) {
        g->achievements[i].status = false;
        g->achievements[i].achievementDate = -1;
        strcpy(g->achievements[i].name, achievementName[i]);
        strcpy(g->achievements[i].description, achievementDescription[i]);

        g->tempAchievements[i] = g->achievements[i];
    }

    for (i = 0; i < MAX_DUNGEONS; i++){
        g->checkPlaythroughDungeonCleared[i] = false;
    }
    g->numPlaythroughDungeonCleared = 0;
    g->numPlaythrough = 0;

    // TESTING
    //g->allDungeons[3].clears = 1;
}

/*  Description:  Used to print the available ".bin" files it can find in the directory
    Precondition: N/A
    @param <N/A> <N/A>
    @return <void>
*/
void 
printSaveFiles() {

    int len;
    string20 player;

    DIR* d = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        // check if filename ends in ".bin"
        len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".bin") == 0) {
            strcpy(player, entry->d_name);
            player[len - 4] = '\0';
            printf("\t\t%s\n", player);
        }
    }

    closedir(d);
}

/*  Description:  Used to handle the whole program/game
    Precondition: N/A
    @param <N/A> <N/A>
    @return <int>
*/
int 
main() {
    clearScreen();

    // Used to run the art in the menu
    system("chcp 65001");

    // added this so the maps chosen is always randomized everytime the program starts
    srand(time(NULL));

    GameState game;
    string20 player;
    initializeGame(&game);

    printSaveFiles();
    printf("\nWho's Playing?\n");
    printf(": ");
    scanf("%s", player);
    strcat(player, ".bin");

    while(!game.hasSaved){
        clearScreen();
        mainMenu(&game, player);
    }
    
    return 0;
}