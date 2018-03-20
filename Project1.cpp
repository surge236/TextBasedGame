/*
 * Project1.cpp
 *
 *  Created on: Jul 30, 2016
 *      Author: Alexander Lewis
 *      Auburn UserID: ajl0015
 *      Auburn Student ID#: 903220195
 *      File Name: Project1.cpp
 *
 *      Compiled using Eclipse and Cygwin on a Windows Laptop,
 *      then recompiled on a desktop running Linux.
 */
#include <iostream>
#include <cassert>
#include <string>
#include <fstream>
#include <cstdlib>
#include <climits>
using namespace std;

#define UNIT_TESTING

// Structure representing a list of scoreNode's that will be written out when
// writing to scores file and displaying the scoreboard.
struct ScoreListNode {
	string playerName;
	int playerScore;
	ScoreListNode* next;
};
typedef ScoreListNode* ScoreListNodePtr;
ScoreListNodePtr scoreListHead = new ScoreListNode;
ScoreListNodePtr scoreListRear = new ScoreListNode;

// Structure representing a list of just the top ten scoreNode's achieved.
struct TopTenScoreNode {
	string topTenName;
	int topTenScore;
	TopTenScoreNode* next;
};
typedef TopTenScoreNode* TopTenScoreNodePtr;
TopTenScoreNodePtr topTenScoreHead = new TopTenScoreNode;
TopTenScoreNodePtr topTenScoreRear = new TopTenScoreNode;

// Structure representing a player's statistics upon starting a new game and after each turn.
// Includes the player's intelligence, time, money, and steps remaining to the goal. Used to
// calculate the player's score after a game.
struct PlayerStatsNode {
	int playerIntel;
	int playerTime;
	int playerMoney;
	int stepsRemain;
};
typedef PlayerStatsNode* PlayerStatsNodePtr;

// Structure representing the list of possible encounters that can occur during a game.
// Includes the text displayed to the player upon encounter as well as the changes to
// each individual player statistic upon said encounter, and the potential that the
// encounter will occur.
struct EncounterNode {
	string encounterText;
	int intelChange;
	int moneyChange;
	int timeChange;
	int possibility;
	EncounterNode* next;
};
typedef EncounterNode* EncounterNodePtr;
EncounterNodePtr encounterHead = new EncounterNode;
EncounterNodePtr encounterRear = new EncounterNode;

// Structure representing the list of possible puzzles that can occur during a game. Includes
// The text representing the puzzle presented before the player as well as a string representing
// the puzzle's correct answer. Six integers individually represent the change to the player's
// three statistics both on answering the puzzle correctly as well as incorrectly. Also, tracks
// the number of puzzles in the list in order to use in coordination with random number generation
// to randomly choose a puzzle from the list.
struct PuzzleNode {
	string puzzleText;
	string puzzleAnswer;
	int intelChangeCorrect;
	int intelChangeIncorrect;
	int moneyChangeCorrect;
	int moneyChangeIncorrect;
	int timeChangeCorrect;
	int timeChangeIncorrect;
	int numPuzzles;
	PuzzleNode* next;
};
typedef PuzzleNode* PuzzleNodePtr;
PuzzleNodePtr puzzleHead = new PuzzleNode;
PuzzleNodePtr puzzleRear = new PuzzleNode;

int readScores();
int writeScores();
int checkForTopScore(ScoreListNodePtr score);
int addScore(ScoreListNodePtr score);
void reorderScores();
void initializeEncounters();
int readPuzzles();
int main();
ScoreListNodePtr startUp();
int mainMenu();
ScoreListNodePtr beginGame(ScoreListNodePtr player);
int displayTopTen();
void displayPlayerStats(PlayerStatsNodePtr player);
int inGameMenu();
PlayerStatsNodePtr moveCharacter(PlayerStatsNodePtr player);
PlayerStatsNodePtr readTechnicalPapers(PlayerStatsNodePtr player);
PlayerStatsNodePtr searchForChange(PlayerStatsNodePtr player);
PlayerStatsNodePtr performEncounter(PlayerStatsNodePtr player);
PlayerStatsNodePtr performPuzzle(PlayerStatsNodePtr player);
int checkForGameOver(PlayerStatsNodePtr player);
ScoreListNodePtr gameOver(PlayerStatsNodePtr player, string playerName);
void testDriver();
void readScoresTest();
void writeScoresTest();
void checkForTopScoreTest();
void addScoreTest();
void reorderScoresTest();
void readPuzzlesTest();
void startUpTest();
void mainMenuTest();
void inGameMenuTest();
void moveCharacterTest();
void performPuzzleTest();
void gameOverTest();

// Runs necessary methods to progressively move through the run time processes. Begins by running
// the startUp() method to determine player information, then runs other methods from that point
// based on player input. If at any time the player indicates they wish to quit the game, this
// method will give a return value.
int main() {

	testDriver();
	scoreListHead -> next = scoreListRear;
	topTenScoreHead -> next = topTenScoreRear;
	encounterHead -> next = encounterRear;
	puzzleHead -> next = puzzleRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

	int selection;
	ScoreListNodePtr player;
	// causes the while loop to loop endlessly meaning the program returns to the
	// main menu by default until the function returns a value thus indicating the
	// run is complete.
	readScores();
	reorderScores();
	int continuous = 0;
	player = startUp();
	while (continuous == 0) {
		selection = mainMenu();
		if (selection == 1) {
			initializeEncounters();
			readPuzzles();
			player = beginGame(player);
			if (player -> playerScore == -1) return 0;
			else if (player -> playerScore > 0) {
				addScore(player);
				checkForTopScore(player);
				reorderScores();
				writeScores();
				player -> playerScore = 0;
			}
		}
		else if (selection == 2) displayTopTen();
		else if (selection == 3) return 0;
	}
	return 0;
}

// reads previous scores from dedicated scores file.
int readScores() {

	string scoreFile = "scores.txt";

	ifstream inStream;
    inStream.open((char*)scoreFile.c_str());

    if (inStream.fail()) {
    	cout << "Scores file does not exist. Creating blank scores file." << endl;
    	ofstream outStream;
    	outStream.open((char*)scoreFile.c_str());
    	outStream.close();
    	inStream.close();
    	return -1;
    }

    // catches empty files.
    if (inStream.peek() == inStream.eof()) {
    	return 0;
    }

    // Empties current scores list so it can be recreated.
    scoreListHead = new ScoreListNode;
    scoreListRear = new ScoreListNode;
    scoreListHead -> next = scoreListRear;
    scoreListHead -> playerScore = 0;
    scoreListRear -> playerScore = 0;

    // Reads each value until end of file, adding them to the designated
    // array.
    string newName;
    int newPlayerScore;
    ScoreListNodePtr newScore;
    ScoreListNodePtr lastScore;
    TopTenScoreNodePtr newTopScore;
    TopTenScoreNodePtr lastTopScore;
    int index = 0;
    int i = 0;
	while (inStream.eof() == false) {

		if (i == 0) {
			inStream >> newName;
			inStream >> newPlayerScore;
			scoreListHead = new ScoreListNode;
			scoreListHead -> playerName = newName;
			scoreListHead -> playerScore = newPlayerScore;
			newScore = scoreListHead;
			lastScore = scoreListHead;
		}
		else {
			inStream >> newName;
			inStream >> newPlayerScore;
			newScore = new ScoreListNode;
			newScore -> playerName = newName;
			newScore -> playerScore = newPlayerScore;
			lastScore -> next = newScore;
		}

		if (i < 10) {
			newTopScore = new TopTenScoreNode;
			newTopScore -> topTenName = newScore -> playerName;
			newTopScore -> topTenScore = newScore -> playerScore;
			if (i == 0) {
				topTenScoreHead = newTopScore;
			}
			else {
				lastTopScore -> next = newTopScore;
			}
			if (inStream.eof() == true || i + 1 == 10) {
				topTenScoreRear = newTopScore;
			}
			lastTopScore = newTopScore;
		}
		if (inStream.eof() == true) {
			scoreListRear = newScore;
		}
		i++;
		index = i;
	}
    inStream.close();
    return index;
}

// Recreates the dedicated scores file with the updated linked list that is created during run
// time. Essentially, any time a new score is added to the linked list of past scores, this
// function will be called to update the external text file of scores with the updated scores list.
int writeScores() {

	ofstream outStream;
	string scoreFile = "scores.txt";
	outStream.open((char*)scoreFile.c_str(), std::ofstream::out | std::ofstream::trunc);
	outStream.close();
	outStream.open((char*)scoreFile.c_str());

	ScoreListNodePtr currentNode = scoreListHead;
	if (scoreListHead -> playerScore == 0) {
		outStream.close();
		return 0;
	}
	if (scoreListRear -> playerScore == 0) {
		outStream << currentNode -> playerName << "\r\n";
		outStream << currentNode -> playerScore;
		outStream.close();
		return 0;
	}

	while (currentNode != scoreListRear) {
		outStream << currentNode -> playerName << "\r\n";
		outStream << currentNode -> playerScore << "\r\n";

			currentNode = currentNode -> next;
	}
	outStream << currentNode -> playerName << "\r\n";
	outStream << currentNode -> playerScore;

	outStream.close();
	return 0;
}

// First, the function compares the new score indicated by the parameter values to the
// scores on the top ten list confirming where the score would fit on the list. If it is lower
// than all values on the list, returns -1 otherwise, returns 1 and adds the new score to its
// correct position on the list, pushing all other values back one and inevitably removing the
// lowest score from the list (update: after implementation, decided to integrate this functions
// purpose into the addScore() method, this function now simply sets the top ten list with
// the top scores from the scores list.
int checkForTopScore(ScoreListNodePtr score) {

	topTenScoreHead = new TopTenScoreNode;
	topTenScoreRear = new TopTenScoreNode;
	topTenScoreHead -> next = topTenScoreRear;
	topTenScoreHead -> topTenScore = 0;
	topTenScoreRear -> topTenScore = 0;

	int i = 1;
	if (scoreListHead -> playerScore == 0) {
		return -1;
	}
	if (scoreListRear -> playerScore == 0) {
		topTenScoreHead -> topTenName = scoreListHead -> playerName;
		topTenScoreHead -> topTenScore = scoreListHead -> playerScore;
		topTenScoreHead -> next = topTenScoreRear;
		return -1;
	}
	TopTenScoreNodePtr currentTopScore = new TopTenScoreNode;
	TopTenScoreNodePtr lastTopScore = new TopTenScoreNode;
	ScoreListNodePtr currentScore = scoreListHead;
	topTenScoreHead -> topTenName = scoreListHead -> playerName;
	topTenScoreHead -> topTenScore = scoreListHead -> playerScore;
	lastTopScore = topTenScoreHead;
	while (currentScore != scoreListRear && i < 10) {
		currentScore = currentScore -> next;
		currentTopScore -> topTenName = currentScore -> playerName;
		currentTopScore -> topTenScore = currentScore -> playerScore;
		lastTopScore -> next = currentTopScore;
		lastTopScore = currentTopScore;
		currentTopScore = new TopTenScoreNode;
		topTenScoreRear = lastTopScore;
		i++;
	}

	return 1;
}

// First, the function progressively checks the scores in the ScoreNode list starting from the
// highest until it finds the first score that is smaller than the one in the parameters of the
// function. Finally, adds the new score to the position just above this smaller value,
// pushing all other lower values in the list back one position.
int addScore(ScoreListNodePtr score) {

	if (scoreListHead -> playerScore == 0) {
		scoreListHead = score;
		scoreListHead -> next = scoreListRear;
		topTenScoreHead = new TopTenScoreNode;
		topTenScoreHead -> topTenName = scoreListHead -> playerName;
		topTenScoreHead -> topTenScore = scoreListHead -> playerScore;
		topTenScoreHead -> next = topTenScoreRear;
		return 1;
	}
	if (scoreListRear -> playerScore == 0) {
		if (score -> playerScore > scoreListHead -> playerScore) {
			topTenScoreHead = new TopTenScoreNode;
			topTenScoreHead -> topTenName = score -> playerName;
			topTenScoreHead -> topTenScore = score -> playerScore;
			topTenScoreHead -> next = topTenScoreRear;
			score -> next = scoreListHead;
			topTenScoreRear = new TopTenScoreNode;
			topTenScoreRear -> topTenName = scoreListHead -> playerName;
			topTenScoreRear -> topTenScore = scoreListHead -> playerScore;
			scoreListHead = score;
			scoreListRear = score -> next;
			return 1;
		}
		else {
			scoreListHead -> next = score;
			scoreListRear = score;
			topTenScoreHead = new TopTenScoreNode;
			topTenScoreHead -> topTenName = scoreListHead -> playerName;
			topTenScoreHead -> topTenScore = scoreListHead -> playerScore;
			topTenScoreRear = new TopTenScoreNode;
			topTenScoreRear -> topTenName = scoreListRear -> playerName;
			topTenScoreRear -> topTenScore = scoreListRear -> playerScore;
			return 1;
		}
	}

	ScoreListNodePtr currentScore = scoreListHead;
	ScoreListNodePtr lastScore = scoreListHead;
	while (currentScore != scoreListRear) {
		if (score -> playerScore >= currentScore -> playerScore) {
			if (currentScore == scoreListHead) {
				score -> next = scoreListHead;
				scoreListHead = score;
				return 1;
			}
			else {
				lastScore -> next = score;
				score -> next = currentScore;
				return 1;
			}
		}
		lastScore = currentScore;
		currentScore = currentScore -> next;
	}
	scoreListRear -> next = score;
	scoreListRear = score;
	return 1;

}

// Checks that both the scores list and the top ten scores list are in order from highest to
// lowest, and will reorder the lists if this is not true (does this by reordering scores list
// then completely rebuilding the top ten list using information from reordered score list).
void reorderScores() {

	// reorders the scores list if it is out of order by removing values that are out
	// of position and reinserting them in the list using the addScore() function.
	ScoreListNodePtr lastScore;
	ScoreListNodePtr currentScore = scoreListHead;
	while (currentScore != scoreListRear) {
		lastScore = currentScore;
		currentScore = currentScore -> next;
		if (currentScore -> playerScore > lastScore -> playerScore) {
			lastScore -> next = currentScore -> next;
			addScore(currentScore);
			checkForTopScore(currentScore);
			currentScore = scoreListHead;
		}
	}

}

// Initializes the possible encounters that can occur over the course of the game.
// (function is also re-run after any encounter occurs in order to re-randomize values).
// (other functions will note a puzzle happens due to the random number detailing the
// probability roll being less than 30, said functions will also alter player stats
// accordingly resulting in the stat changes appearing as 0 in this function).
void initializeEncounters() {

	EncounterNodePtr currentEncounter = encounterHead;
	EncounterNodePtr lastEncounter = currentEncounter;

	// encounter 1: nothing happens
	currentEncounter -> encounterText = "You encounter a puzzle.";
	currentEncounter -> intelChange = 0;
	currentEncounter -> moneyChange = 0;
	currentEncounter -> timeChange = 0;
	currentEncounter -> possibility = 30;
	lastEncounter = new EncounterNode;
	currentEncounter -> next = lastEncounter;
	currentEncounter = lastEncounter;

	// encounter 2: puzzle happens
	currentEncounter -> encounterText = "Nothing happens.";
	currentEncounter -> intelChange = 0;
	currentEncounter -> moneyChange = 0;
	currentEncounter -> timeChange = 0;
	currentEncounter -> possibility = 25;
	lastEncounter = new EncounterNode;
	currentEncounter -> next = lastEncounter;
	currentEncounter = lastEncounter;

	// encounter 3: encounter a professor
	int time = rand() % 2 + 1; // time that can be lost is between 1 and 3
	int intel = rand() % 3; // intel that can be gained is between 0 and 3

	string timeStr;
	string intelStr;
	if (time == 0) timeStr = "0"; if (intel == 0) intelStr = "0";
	if (time == 1) timeStr = "1"; if (intel == 1) intelStr = "1";
	if (time == 2) timeStr = "2"; if (intel == 2) intelStr = "2";
	if (time == 3) timeStr = "3"; if (intel == 3) intelStr = "3";

	currentEncounter -> encounterText = "Encounter a Professor, lose " + timeStr
			+ " time and gain " + intelStr + " intelligence.";
	currentEncounter -> intelChange = intel;
	currentEncounter -> moneyChange = 0;
	currentEncounter -> timeChange = -time;
	currentEncounter -> possibility = 30;
	lastEncounter = new EncounterNode;
	currentEncounter -> next = lastEncounter;
	currentEncounter = lastEncounter;

	// encounter 4: encounter graduate student
	time = rand() % 2 + 1; // time that can be lost is between 1 and 3

	if (time == 0) timeStr = "0";
	if (time == 1) timeStr = "1";
	if (time == 2) timeStr = "2";
	if (time == 3) timeStr = "3";

	currentEncounter -> encounterText = "Encounter another graduate student, lose "
			+ timeStr + " time.";
	currentEncounter -> intelChange = 0;
	currentEncounter -> moneyChange = 0;
	currentEncounter -> timeChange = -time;
	currentEncounter -> possibility = 10;
	lastEncounter = new EncounterNode;
	currentEncounter -> next = lastEncounter;
	currentEncounter = lastEncounter;

	// encounter 5: grunt work
	currentEncounter -> encounterText = "Attacked by grunt work!";
	currentEncounter -> intelChange = -1;
	currentEncounter -> moneyChange = 0;
	currentEncounter -> timeChange = -1;
	currentEncounter -> possibility = 15;
	lastEncounter = new EncounterNode;
	currentEncounter -> next = lastEncounter;
	currentEncounter = lastEncounter;

	// encounter 6: grade papers
	currentEncounter -> encounterText = "Grade Papers. Lose time, but gain money.";
	currentEncounter -> intelChange = 0;
	currentEncounter -> moneyChange = 1;
	currentEncounter -> timeChange = -1;
	currentEncounter -> possibility = 10;
	encounterRear = currentEncounter;

}

// Reads an external text file for the puzzles possible during an iteration of the game.
// Writes these puzzles to PuzzleNode objects then adds them to the list of puzzles
// (first puzzle in the list will have numPuzzles set to 1 with each following puzzle
// added checking the previous node in order to ensure the number progressively increases by one,
// as such, the last puzzle in the list will have the accurate number of puzzles).
int readPuzzles() {

    	// default puzzle 1
    	PuzzleNodePtr currentPuzzle = puzzleHead;
    	PuzzleNodePtr lastPuzzle = currentPuzzle;
    	currentPuzzle -> puzzleText = "You will always find me in the past. I can"
    			" be created in the present,\nBut the future can never taint me.\n"
				"What am I?";
    	currentPuzzle -> puzzleAnswer = "history";
    	currentPuzzle -> intelChangeCorrect = 2;
    	currentPuzzle -> intelChangeIncorrect = -1;
    	currentPuzzle -> moneyChangeCorrect = 0;
    	currentPuzzle -> moneyChangeIncorrect = 0;
    	currentPuzzle -> timeChangeCorrect = 3;
    	currentPuzzle -> timeChangeIncorrect = 0;
    	currentPuzzle -> numPuzzles = 1;
    	lastPuzzle = new PuzzleNode;
    	currentPuzzle -> next = lastPuzzle;
    	currentPuzzle = currentPuzzle -> next;

    	// default puzzle 2
    	currentPuzzle -> puzzleText = "What belongs to you but others use it more than "
    			"you do?";
    	currentPuzzle -> puzzleAnswer = "your name";
    	currentPuzzle -> intelChangeCorrect = 2;
    	currentPuzzle -> intelChangeIncorrect = -1;
    	currentPuzzle -> moneyChangeCorrect = 0;
    	currentPuzzle -> moneyChangeIncorrect = 0;
    	currentPuzzle -> timeChangeCorrect = 3;
    	currentPuzzle -> timeChangeIncorrect = 0;
    	currentPuzzle -> numPuzzles = 2;
    	lastPuzzle = new PuzzleNode;
    	currentPuzzle -> next = lastPuzzle;
    	currentPuzzle = currentPuzzle -> next;

    	// default puzzle 3
    	currentPuzzle -> puzzleText = "What is 12 x 12?";
    	currentPuzzle -> puzzleAnswer = "144";
    	currentPuzzle -> intelChangeCorrect = 2;
    	currentPuzzle -> intelChangeIncorrect = -1;
    	currentPuzzle -> moneyChangeCorrect = 1;
    	currentPuzzle -> moneyChangeIncorrect = -1;
    	currentPuzzle -> timeChangeCorrect = 2;
    	currentPuzzle -> timeChangeIncorrect = -1;
    	currentPuzzle -> numPuzzles = 3;
    	puzzleRear = currentPuzzle;

    	return -1;

}

// Prompts the user to enter their name, then takes the user input and creates a new ScoreNode
// with the user's input name and a score set to 0 (indicating the game hasn't been played/won).
// Finally, displays a welcome message for the user. Returns the new scoreNode object.
ScoreListNodePtr startUp() {

	string userName = " ";
	ScoreListNodePtr player = new ScoreListNode;
	while (userName.compare(" ") == 0) {
		cout << "\tWhat's your name? ";
		getline(cin, userName);
		if (userName.empty() != true) {
			int first = userName.find_first_not_of(' ');
			int last = userName.find_last_not_of(' ');
			userName = userName.substr(first, last);
		}
		if (userName.compare("") == true || userName.empty() == true) {
			cout << "\tplease enter a valid user name" << endl;
			userName = " ";
		}
	}
	cout << endl << "\t==========================================================="
			<< endl << "\t\t\tWelcome, " << userName << "!" << endl
			<< "\t==========================================================="
			<< endl << endl;
	player -> playerName = userName;
	player -> playerScore = 0;
	return player;

}

// Displays the main menu options after startup (and some other occasions including after playing
// the game). Then, prompts the user to enter an integer representing their choice of option.
// Options include 1 - Start a new game, 2 - View top 10 scores, 3 - Quit.
int mainMenu() {

	int selection = 0;

	while (selection != 1 && selection != 2 && selection != 3) {

		cout << "1) Start a New Game of Dunstan and Dragons!\n"
				<< "2) View top 10 High Scores\n"
				<< "3) Quit\n\n\tPlease choose an option: ";
		//confirm
		cin >> selection;
		cin.clear();
		cin.ignore(INT_MAX, '\n');
		cout << endl;

		if (selection != 1 && selection != 2 && selection != 3) {
			cout << "Please enter a valid input." << endl;
		}

	}

	return selection;

}

// Begins a new iteration of the game by creating a new PlayerStatsNode object to track the
// player's statistics and how they change over the course of the game. Then, runs the necessary
// methods to progress through the game including those related to menu options, encounters, and
// puzzles (also displays the message "Entering the Dungeon..." on start up). Takes a parameter
// representing the player who is playing the game, and returns an upgraded score node with the
// player's earned score after completing the game (or 0 if they were unsuccessful).
ScoreListNodePtr beginGame(ScoreListNodePtr player) {

	// Player's stats are randomly between 20 and 25
	PlayerStatsNodePtr character = new PlayerStatsNode;
	character -> playerIntel = rand() % 5 + 20;
	character -> playerTime = rand() % 5 + 20;
	character -> playerMoney = rand() % 5 + 20;
	character -> stepsRemain = rand() % 5 + 20;
	int check = 1;
	int selection;

	cout << "Entering the Dungeon..." << endl << endl;

	// check is equal to the result of the checkForGameOver() function which will return -1
	// if a game over is detected.
	while (check != -1) {
		selection = -1;
		while (selection != 1 && selection != 2 && selection != 3 && selection != 4 && selection != 5) {
			displayPlayerStats(character);
			selection = inGameMenu();
			if (selection == 1) {
				character = moveCharacter(character);
				check = checkForGameOver(character);
			}
			else if (selection == 2) {
				character = readTechnicalPapers(character);
				check = checkForGameOver(character);
			}
			else if (selection == 3) {
				character = searchForChange(character);
				check = checkForGameOver(character);
			}
			else if (selection == 4) {}
			else if (selection == 5) {
				player -> playerScore = -1;
				cout << "Ending game, your current progress will be lost.";
				return player;
			}
			else {
				cout << "Please enter a valid input." << endl;
			}
		}
	}
	ScoreListNodePtr updatedPlayer;
	updatedPlayer = gameOver(character, player -> playerName);
	return updatedPlayer;

}

// Progressively iterates through the top ten scores list displaying each for the user to see.
int displayTopTen() {

	TopTenScoreNodePtr currentScore = topTenScoreHead;
	cout << "The top scores are:" << endl << endl;

	if (currentScore -> topTenScore == 0) {
		cout << "no scores stored" << endl << endl;
		return 0;
	}
	if (topTenScoreRear -> topTenScore == 0) {
		cout << currentScore -> topTenName << " " << currentScore -> topTenScore << endl << endl;
		return 0;
	}

	while (currentScore != topTenScoreRear) {
			cout << currentScore -> topTenName << " " << currentScore -> topTenScore << endl;
			currentScore = currentScore -> next;
	}
	cout << currentScore -> topTenName << " " << currentScore -> topTenScore << endl << endl;
	return 0;
}

// Displays the statistics of the player indicated by the parameters of the function including
// their intelligence, money, time remaining, and steps left to reach the goal.
void displayPlayerStats(PlayerStatsNodePtr player) {

	cout << "You have:\n\nintelligence: " << player -> playerIntel << endl << "time: "
			<< player -> playerTime << endl << "money: " << player -> playerMoney
			<< endl << endl << "You are " << player -> stepsRemain << " steps from "
			<< "the goal. Time left: " << player -> playerTime << ".\n\n";

}

// Displays options presented to the player upon starting a new turn in game. Then, prompts the
// user for an integer value representing their choice of action. Returns the integer value that
// the user selected. The menu options include 1 - Move forward, 2 - Read Technical Papers,
// 3 - Search for loose change, 4 - View Character, 5 - Quit game.
int inGameMenu() {

	int selection = 0;
	while (selection != 1 && selection != 2 && selection != 3 && selection != 4 && selection != 5) {
		cout << "\t1) Move forward(takes time, could be risky...)" << endl
				<< "\t2) Read technical papers (boost intelligence, takes time)" << endl
				<< "\t3) Search for loose change (boost money, takes time)" << endl
				<< "\t4) View character" << endl << "\t5) Quit the game" << endl << endl
				<< endl << endl << "Please choose an action: ";
		cin >> selection;
		cin.clear();
		cin.ignore(INT_MAX, '\n');
		cout << endl << endl << endl;
		if (selection != 1 && selection != 2 && selection != 3 && selection != 4 && selection != 5) {
			cout << "\tPlease enter a valid input." << endl;
		}
	}
	return selection;
}

// Performs necessary actions if the player decides to move on any individual turn (player who
// is moving is indicated by the parameters). Actions include, reducing the player's remaining
// time and randomly deciding if an encounter occurs (also randomly decides if the encounter
// is a puzzle after determining if an encounter occurs at all). After these actions are performed,
// determines the overall changes to the player's statistics, then returns the updated player
// statistics node with the changes to the statistics implemented.
PlayerStatsNodePtr moveCharacter(PlayerStatsNodePtr player) {

	player -> stepsRemain = player -> stepsRemain - 1;
	player -> playerTime = player -> playerTime - 1;
	cout << "You move forward one step, and..." << endl << endl;
	player = performEncounter(player);
	cout << "You spent one unit of time." << endl << endl;
	return player;

}

// Performs necessary actions if the player decides to read technical papers on any individual turn
// (player is indicated by the parameters). When this occurs, the player's remaining time is decreased
// and their overall intelligence is increased. These statistic changes are performed to the
// PlayerStatsNode object then the player is returned.
PlayerStatsNodePtr readTechnicalPapers(PlayerStatsNodePtr player) {

	int intelGain = rand() % 3 + 1;
	player -> playerTime = player -> playerTime - 2;
	player -> playerIntel = player -> playerIntel + intelGain;
	cout << "You read through some technical papers. You gain " << intelGain
			<< " intelligence, but\nlose 2 units of time." << endl << endl;
	return player;

}

// Performs necessary actions if the player decides to search for change on any individual turn
// (player is indicated by the parameters). When this occurs, the player's remaining time is decreased
// and their overall money is increased. These statistic changes are performed to the
// PlayerStatsNode object then the player is returned.
PlayerStatsNodePtr searchForChange(PlayerStatsNodePtr player) {

	int moneyGain = rand() % 3 + 1;
	player -> playerTime = player -> playerTime - 2;
	player -> playerMoney = player -> playerMoney + moneyGain;
	cout << "You search for loose change. You gain " << moneyGain
			<< " money, but\nlose 2 units of time." << endl << endl;
	return player;

}

// If it is determined that the player must undergo an encounter upon movement, this function
// is run to determine the encounter/puzzle performed. First a random number generation checks
// if a puzzle should be run instead of an encounter. If it is determined an encounter is run,
// a random number generation determines which encounter from the list of encounters is performed
// then displays the text associated with that encounter, determines the statistic changes to
// the player on this encounter, performs these statistic changes the the PlayerStatsNode object
// indicated by the parameters, then return the updated PlayerStatsNode object. If it is instead
// determined that a puzzle occurs, a puzzle is selected from the list of puzzle, the text
// associated with that puzzle is displayed, then the user is prompted to enter an answer to
// the displayed puzzle. After determining if the player's response was correct or not, determines
// the statistic changes to the player in the given scenario, performs changes to PlayerStatsNode
// object indicated by the parameters, then returns the updated PlayerStatsNode object.
PlayerStatsNodePtr performEncounter(PlayerStatsNodePtr player) {

	EncounterNodePtr currentEncounter = encounterHead;
	int totalProbability = 0;
	while (currentEncounter != encounterRear) {
		totalProbability += currentEncounter -> possibility;
		currentEncounter = currentEncounter -> next;
	}
	totalProbability += currentEncounter -> possibility;
	currentEncounter = encounterHead;

	int encounterRoll = rand() % totalProbability;
	// if the encounterRoll is originally between 0 and 30, a puzzle occurs.
	if (encounterRoll < 30) {
		cout << currentEncounter -> encounterText << endl << endl;
		player = performPuzzle(player);
		return player;
	}
	// otherwise determine the encounter.
	encounterRoll -= 30;
	currentEncounter = currentEncounter -> next;
	while (encounterRoll > 0) {
		encounterRoll -= currentEncounter -> possibility;
		if (encounterRoll > 0) {
			currentEncounter = currentEncounter -> next;
		}
	}
	cout << currentEncounter -> encounterText << endl << endl;
	player -> playerIntel = player -> playerIntel + currentEncounter -> intelChange;
	player -> playerMoney = player -> playerMoney + currentEncounter -> moneyChange;
	player -> playerTime = player -> playerTime + currentEncounter -> timeChange;

	// re-runs initializeEncounters() after each encounter to re-randomize values.
	initializeEncounters();
	return player;
}

// Function is run if a player has moved and the encounter determined to occur upon their movement
// is a puzzle. The function generates a random number between 1 and the numPuzzles parameter obtained
// from the rear node in the PuzzleNode linked list. The PuzzleNode linked list is then iterated
// through until the node indicated by the randomly generated number is reached. The puzzleText
// parameter is output for the user, then the user is prompted to input their answer to the displayed
// puzzle. This answer is compared to the correct answer indicated by the puzzleAnswer parameter
// from the PuzzleNode object, and if the user was correct, the PlayerStatsNode object indicated
// by the function's parameters is altered by the amount specified by the integers defined as
// "correct" within the PuzzleNode object (ex. intelChangeCorrect) otherwise, the statistics
// are altered by the integers defined as "incorrect" within the PuzzleNode object (ex.
// intelChangeIncorrect). The newly altered PlayerStatsNode object is then returned.
PlayerStatsNodePtr performPuzzle(PlayerStatsNodePtr player) {

	PuzzleNodePtr currentPuzzle = puzzleHead;
	int numPuzzles = puzzleRear -> numPuzzles;
	int puzzleRoll = rand() % numPuzzles;
	while (puzzleRoll > 0) {
		currentPuzzle = currentPuzzle -> next;
		puzzleRoll--;
	}

	string answer;
	string puzzle = currentPuzzle -> puzzleText;
	cout << puzzle << endl << endl;
	cout << "Choose wisely: ";

	getline(cin, answer);
	if (answer.empty() != true) {
		int first = answer.find_first_not_of(' ');
		int last = answer.find_last_not_of(' ');
		answer = answer.substr(first, last);
	}
	int answerLength = answer.size();
	if (answer.substr(0, answerLength).compare(currentPuzzle -> puzzleAnswer) == 0) {
		cout << endl << "Your answer is correct, displaying player statistic changes."
				<< endl << "intelligence change: " << currentPuzzle -> intelChangeCorrect
				<< endl << "money change: " << currentPuzzle -> moneyChangeCorrect
				<< endl << "time change: " << currentPuzzle -> timeChangeCorrect
				<< endl << endl;
		player -> playerIntel = player -> playerIntel + currentPuzzle -> intelChangeCorrect;
		player -> playerMoney = player -> playerMoney + currentPuzzle -> moneyChangeCorrect;
		player -> playerTime = player -> playerTime + currentPuzzle -> timeChangeCorrect;
	}
	else {
		cout << endl << "Your answer is incorrect, displaying player statistic changes."
				<< endl << "intelligence change: " << currentPuzzle -> intelChangeIncorrect
				<< endl << "money change: " << currentPuzzle -> moneyChangeIncorrect
				<< endl << "time change: " << currentPuzzle -> timeChangeIncorrect
				<< endl << endl;
		player -> playerIntel = player -> playerIntel + currentPuzzle -> intelChangeIncorrect;
		player -> playerMoney = player -> playerMoney + currentPuzzle -> moneyChangeIncorrect;
		player -> playerTime = player -> playerTime + currentPuzzle -> timeChangeIncorrect;
	}
	return player;
}

// Checks if any of the player's statistics have reached 0 (including the steps remaining to the
// goal which can indicate a victory). If any statistic has reached 0, returns -1 indicating the
// game is over. Otherwise, returns 1 indicating the game is still in progress.
int checkForGameOver(PlayerStatsNodePtr player) {

	int intel = player -> playerIntel;
	int time = player -> playerTime;
	int money = player -> playerMoney;
	int steps = player -> stepsRemain;
	if (intel > 0 && time > 0 && money > 0 && steps > 0) {
		return 1;
	}
	return -1;

}

// If the player is determined to have completed the game (whether successfully or unsuccessfully)
// this function is run to display the proper output and alter the player's scoreNode object.
// The player's score is calculated by multiplying together the statistics contained within
// the PlayerStatsNode parameter, if this score is 0, it is inferred the player failed at completing the
// game. In this instance, the PlayerStatsNode object indicated by the parameter is used to
// determine the player's cause of death, once this is determined an appropriate message is
// displayed to the user indicating they have died (followed by the player's scoreNode object
// being returned with their score set to 0). If the player's score is indicated to be greater
// than 0 by the parameters, then it is inferred the player succeeded in completing the game,
// an appropriate congratulations is output, and the player's score is updated in the scoreNode
// object that is then returned. (also takes the player's name as a parameter to ensure the
// returned ScoreNode object has accurate information.
ScoreListNodePtr gameOver(PlayerStatsNodePtr player, string playerName) {

	int intel = player -> playerIntel;
	int time = player -> playerTime;
	int money = player -> playerMoney;
	int score = intel * time * money;
	ScoreListNodePtr character = new ScoreListNode;
	character -> playerName = playerName;
	character -> playerScore = score;

	if (intel == 0) {
		cout << "Intelligence has fallen to zero. You die." << endl;
	}
	if (time == 0) {
		cout << "Time has fallen to zero. You die." << endl;
	}
	if (money == 0) {
		cout << "Money has fallen to zero. You die." << endl;
	}
	if (score > 0) {
		cout << "You have reached the goal. Your score is " << score << "." << endl;
	}
	cout << endl;
	return character;

}

// Runs programs test cases.
void testDriver() {
#ifdef UNIT_TESTING
	readScoresTest(); // all tests passed
	writeScoresTest(); // working more or less as anticipated
	checkForTopScoreTest(); // all tests passed
	addScoreTest(); // all tests passed
	reorderScoresTest(); // all tests passed
	readPuzzlesTest(); // no longer requires testing
	startUpTest(); // seems to be running correctly
	mainMenuTest(); // seems to run correctly
	inGameMenuTest(); // seems to run correctly
	moveCharacterTest(); // seems to run correctly
	performPuzzleTest(); // seems to run correctly
	gameOverTest();

#else
#endif

}

void readScoresTest() {

	// Test case 1: scores read from non-existing scores file.
	cout << "readScores() Test Case 1: scores read from non-existing scores file (testing"
			<< endl << " will be inaccurate if scores file is not deleted beforehand."
			<< "\nConfirm accuracy by ensuring \"scores.txt\" file was created." << endl;
	readScores();
	cout << endl;
	scoreListHead = new ScoreListNode;
	scoreListRear = new ScoreListNode;
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

	// Test case 2: scores read from existing scores file.
	cout << "readScores() Test Case 2: scores read from existing scores file." << endl;
	ScoreListNodePtr testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "correct";
	testPtr1 -> playerScore = 32;
	addScore(testPtr1);
	writeScores();
	testPtr1 = scoreListHead;
	assert(testPtr1 -> playerName == "correct");
	assert(testPtr1 -> playerScore == 32);
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

}

void writeScoresTest() {

	// Test Case 1: new scores written to an empty scores file.
	cout << "writeScores() Test Case 1: new scores written to an empty scores file." << endl;
	ofstream outStream;
	string scoreFile = "scores.txt";
	outStream.open((char*)scoreFile.c_str(), std::ofstream::out | std::ofstream::trunc);
	outStream.close();
	ScoreListNodePtr testPtr1 = new ScoreListNode;
	testPtr1 -> playerName = "correct";
	testPtr1 -> playerScore = 32;
	addScore(testPtr1);
	writeScores();
	readScores();
	testPtr1 = scoreListHead;
	assert(testPtr1 -> playerName == "correct");
	assert(testPtr1 -> playerScore == 32);
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

	// Test Case 2: new scores written to an existing scores file.
	cout << "writeScores() Test Case 2: new scores written to an existing scores file." << endl;
	testPtr1 = new ScoreListNode;
	testPtr1 -> playerName = "correcttwo";
	testPtr1 -> playerScore = 33;
	addScore(testPtr1);
	writeScores();
	readScores();
	testPtr1 = new ScoreListNode;
	testPtr1 -> playerName = "correctthree";
	testPtr1 -> playerScore = 31;
	addScore(testPtr1);
	writeScores();
	readScores();
	testPtr1 = scoreListRear;
	testPtr1 = new ScoreListNode;
	testPtr1 -> playerName = "correctfour";
	testPtr1 -> playerScore = 34;
	addScore(testPtr1);
	writeScores();
	readScores();
	testPtr1 = scoreListHead;
	assert(testPtr1 -> playerName == "correctfour");
	testPtr1 = scoreListRear;
	assert(testPtr1 -> playerName == "correctthree");
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

}

void checkForTopScoreTest() {

	// Test Case 1: New score is added to an incomplete top ten.
	cout << "checkForTopScore() Test Case 1: New score is added to an incomplete top ten." << endl
			<< endl;
	ScoreListNodePtr testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "four";
	testPtr1 -> playerScore = 4;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	displayTopTen();
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;
	topTenScoreHead = new TopTenScoreNode();
	topTenScoreRear = new TopTenScoreNode();
	topTenScoreHead -> next = topTenScoreRear;
	topTenScoreHead -> topTenScore = 0;
	topTenScoreRear -> topTenScore = 0;

	// Test Case 2: New Score is added to the middle of top ten list.
	cout << "checkForTopScore() Test Case 2: New score is added to the middle of top ten list."
			<< endl << endl;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	displayTopTen();
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;
	topTenScoreHead = new TopTenScoreNode();
	topTenScoreRear = new TopTenScoreNode();
	topTenScoreHead -> next = topTenScoreRear;
	topTenScoreHead -> topTenScore = 0;
	topTenScoreRear -> topTenScore = 0;

	// Test Case 3: New score is added to the end of a complete top ten list.
	cout << "checkForTopScore() TestCase 3: New score is added to the end of a complete top ten list."
			<< endl << endl;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "five";
	testPtr1 -> playerScore = 5;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "six";
	testPtr1 -> playerScore = 6;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "seven";
	testPtr1 -> playerScore = 7;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "eight";
	testPtr1 -> playerScore = 8;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "nine";
	testPtr1 -> playerScore = 9;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "ten";
	testPtr1 -> playerScore = 10;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "eleven";
	testPtr1 -> playerScore = 11;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "four";
	testPtr1 -> playerScore = 4;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	// top ten list is now full.
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	displayTopTen();
	cout << endl;

	// Test Case 4: New score is added at the top of the top ten list (new highest score).
	cout << "checkForTopScore() TestCase 4: New score is added to the top of the top ten list."
			<< endl << endl;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "twenty";
	testPtr1 -> playerScore = 20;
	addScore(testPtr1);
	checkForTopScore(testPtr1);
	displayTopTen();
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;
	topTenScoreHead = new TopTenScoreNode();
	topTenScoreRear = new TopTenScoreNode();
	topTenScoreHead -> next = topTenScoreRear;
	topTenScoreHead -> topTenScore = 0;
	topTenScoreRear -> topTenScore = 0;

}

void addScoreTest() {

	// Test Case 1: New score is added to the scores list.
	cout << "addScore() Test Case 1: New score is added to the scores list." << endl;
	ScoreListNodePtr testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "four";
	testPtr1 -> playerScore = 4;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	addScore(testPtr1);
	assert(scoreListHead -> playerName == "four");
	testPtr1 = scoreListHead -> next;
	assert(testPtr1 -> playerName == "three");
	testPtr1 = testPtr1 -> next;
	assert(testPtr1 -> playerName == "two");
	testPtr1 = testPtr1 -> next;
	assert(testPtr1 -> playerName == "one");
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

	// Test Case 2: New highest score is added to the scores list.
	cout << "addScore() Test Case 2: New highest score is added to the scores list." << endl;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "four";
	testPtr1 -> playerScore = 4;
	addScore(testPtr1);
	assert(scoreListHead -> playerName == "four");
	testPtr1 = scoreListHead -> next;
	assert(testPtr1 -> playerName == "three");
	testPtr1 = testPtr1 -> next;
	assert(testPtr1 -> playerName == "two");
	testPtr1 = testPtr1 -> next;
	assert(testPtr1 -> playerName == "one");
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListHead -> playerScore = 0;

	// Test Case 3: New lowest score is added to the scores list.
	cout << "addScore() Test Case 3: New lowest score is added to the scores list." << endl;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "four";
	testPtr1 -> playerScore = 4;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	addScore(testPtr1);
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	addScore(testPtr1);
	assert(scoreListHead -> playerName == "four");
	testPtr1 = scoreListHead -> next;
	assert(testPtr1 -> playerName == "three");
	testPtr1 = testPtr1 -> next;
	assert(testPtr1 -> playerName == "two");
	testPtr1 = testPtr1 -> next;
	assert(testPtr1 -> playerName == "one");
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListHead -> playerScore = 0;

}

void reorderScoresTest() {

	// Test Case 1: Scores in scores list are directly backwards.
	cout << "reorderScores() Test Case 1: Scores in scores list are directly backwards." << endl;
	ScoreListNodePtr testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	scoreListHead = testPtr1;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	scoreListHead -> next = testPtr1;
	ScoreListNodePtr temp = scoreListHead -> next;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	temp -> next = testPtr1;
	scoreListRear = testPtr1;
	reorderScores();
	assert(scoreListHead -> playerName == "three");
	temp = scoreListHead -> next;
	assert(temp -> playerName == "two");
	temp = temp -> next;
	assert(temp -> playerName == "one");
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();

	// Test Case 2: A single score in the scores list is out of place.
	cout << "reorderScores() Test Case 2: A single score in the scores list is out of place." << endl;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "two";
	testPtr1 -> playerScore = 2;
	scoreListHead = testPtr1;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "three";
	testPtr1 -> playerScore = 3;
	scoreListHead -> next = testPtr1;
	temp = scoreListHead -> next;
	testPtr1 = new ScoreListNode();
	testPtr1 -> playerName = "one";
	testPtr1 -> playerScore = 1;
	temp -> next = testPtr1;
	scoreListRear = testPtr1;
	reorderScores();
	assert(scoreListHead -> playerName == "three");
	temp = scoreListHead -> next;
	assert(temp -> playerName == "two");
	temp = temp -> next;
	assert(temp -> playerName == "one");
	cout << endl;
	scoreListHead = new ScoreListNode();
	scoreListRear = new ScoreListNode();
	scoreListHead -> next = scoreListRear;
	scoreListHead -> playerScore = 0;
	scoreListRear -> playerScore = 0;

}

void readPuzzlesTest() {

	// Test Case 1: Confirms puzzles are correctly initiated by running performPuzzle().
	cout << "Confirms puzzles are correctly initiated by running performPuzzle()." << endl;
	readPuzzles();
	PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 3;
	testPtr1 -> playerTime = 3;
	testPtr1 -> playerMoney = 3;
	testPtr1 -> stepsRemain = 3;
	testPtr1 = performPuzzle(testPtr1);
	displayPlayerStats(testPtr1);
	cout << endl;

}

void startUpTest() {

	// Test Case 1: User does not enter a user name. (should prompt user to enter a valid
	// input, after testing, tester should enter a valid input in order to proceed.
	cout << "startUp() Test Case 1: User does not enter a user name." << endl;
	startUp();
	cout << endl;

}

void mainMenuTest() {

	// Test Case 1: User enters an invalid menu selection. (should prompt user to enter a valid
	// input, after testing, tester should enter a valid menu option in order to proceed.
	cout << "mainMenu() Test Case 1: User enters an invalid menu selection." << endl;
	mainMenu();
	cout << endl;

}

void inGameMenuTest() {

	// Test Case 1: User enters an invalid menu selection. (should prompt user to enter a
	// valid input, after testing, tester should enter a valid menu option in order to proceed.
	cout << "inGameMenu() Test Case 1: User enters an invalid menu selection." << endl;
	inGameMenu();
	cout << endl;

}
void moveCharacterTest() {

	// Test Case 1: Iterates through moveCharacter() function under the same conditions
	// until user defines the "nothing happens" encounter correctly occurs (combined
	// performEncounter() and performPuzzle() tests into this test case through repeated
	// iterations).
	cout << "moveCharacter() Test Case 1: Iterates through moveCharacter() function under "
			"the same conditions until user defines the nothing happens encounter occurs." << endl;
	int input = 0;
	cout << "If correct encounter occurs and function correctly, enter 1, otherwise" << endl
			<< "enter some other integer" << endl;
	initializeEncounters();
	readPuzzles();
	while (input != 1) {
		PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
		testPtr1 -> playerIntel = 3;
		testPtr1 -> playerTime = 3;
		testPtr1 -> playerMoney = 3;
		testPtr1 -> stepsRemain = 3;
		testPtr1 = moveCharacter(testPtr1);
		displayPlayerStats(testPtr1);
		cout << "Tast Case Confirmed? (enter 1 if yes): ";
		cin >> input;
		cout << endl;
	}

	// Test Case 2: Iterates through moveCharacter() function under the same conditions
	// until user defines the "Encounter a puzzle" encounter correctly occurs.
	cout << "moveCharacter() Test Case 2: Iterates through moveCharacter() function under "
			"the same conditions until user defines the encounter a puzzle encounter occurs." << endl;
	input = 0;
	cout << "If correct encounter occurs and function correctly, enter 1, otherwise" << endl
			<< "enter some other integer" << endl;
	while (input != 1) {
		PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
		testPtr1 -> playerIntel = 3;
		testPtr1 -> playerTime = 3;
		testPtr1 -> playerMoney = 3;
		testPtr1 -> stepsRemain = 3;
		testPtr1 = moveCharacter(testPtr1);
		displayPlayerStats(testPtr1);
		cout << "Tast Case Confirmed? (enter 1 if yes): ";
		cin >> input;
		cout << endl;
	}

	// Test Case 3: Iterates through moveCharacter() function under the same conditions
	// until user defines the "Encounter a professor" encounter correctly occurs.
	cout << "moveCharacter() Test Case 3: Iterates through moveCharacter() function under "
			"the same conditions until user defines the encounter a professor encounter occurs." << endl;
	input = 0;
	cout << "If correct encounter occurs and function correctly, enter 1, otherwise" << endl
			<< "enter some other integer" << endl;
	while (input != 1) {
		PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
		testPtr1 -> playerIntel = 3;
		testPtr1 -> playerTime = 3;
		testPtr1 -> playerMoney = 3;
		testPtr1 -> stepsRemain = 3;
		testPtr1 = moveCharacter(testPtr1);
		displayPlayerStats(testPtr1);
		cout << "Tast Case Confirmed? (enter 1 if yes): ";
		cin >> input;
		cout << endl;
	}

	// Test Case 4: Iterates through moveCharacter() function under the same conditions
	// until user defines the "Encounter graduate student" encounter correctly occurs.
	cout << "moveCharacter() Test Case 4: Iterates through moveCharacter() function under "
			"the same conditions until user defines the encounter graduate student encounter occurs." << endl;
	input = 0;
	cout << "If correct encounter occurs and function correctly, enter 1, otherwise" << endl
			<< "enter some other integer" << endl;
	while (input != 1) {
		PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
		testPtr1 -> playerIntel = 3;
		testPtr1 -> playerTime = 3;
		testPtr1 -> playerMoney = 3;
		testPtr1 -> stepsRemain = 3;
		testPtr1 = moveCharacter(testPtr1);
		displayPlayerStats(testPtr1);
		cout << "Tast Case Confirmed? (enter 1 if yes): ";
		cin >> input;
		cout << endl;
	}

	// Test Case 5: Iterates through moveCharacter() function under the same conditions
	// until user defines the "Attacked by grunt work" encounter correctly occurs.
	cout << "moveCharacter() Test Case 5: Iterates through moveCharacter() function under "
			"the same conditions until user defines the attacked by grunt work encounter occurs." << endl;
	input = 0;
	cout << "If correct encounter occurs and function correctly, enter 1, otherwise" << endl
			<< "enter some other integer" << endl;
	while (input != 1) {
		PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
		testPtr1 -> playerIntel = 3;
		testPtr1 -> playerTime = 3;
		testPtr1 -> playerMoney = 3;
		testPtr1 -> stepsRemain = 3;
		testPtr1 = moveCharacter(testPtr1);
		displayPlayerStats(testPtr1);
		cout << "Tast Case Confirmed? (enter 1 if yes): ";
		cin >> input;
		cout << endl;
	}

	// Test Case 6: Iterates through moveCharacter() function under the same conditions
	// until user defines the "Grade papers" encounter correctly occurs.
	cout << "moveCharacter() Test Case 6: Iterates through moveCharacter() function under "
			"the same conditions until user defines the grade papers encounter occurs." << endl;
	input = 0;
	cout << "If correct encounter occurs and function correctly, enter 1, otherwise" << endl
			<< "enter some other integer" << endl;
	while (input != 1) {
		PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
		testPtr1 -> playerIntel = 3;
		testPtr1 -> playerTime = 3;
		testPtr1 -> playerMoney = 3;
		testPtr1 -> stepsRemain = 3;
		testPtr1 = moveCharacter(testPtr1);
		displayPlayerStats(testPtr1);
		cout << "Tast Case Confirmed? (enter 1 if yes): ";
		cin >> input;
		cout << endl;
	}

}

void performPuzzleTest() {

	// Test Case 1: Presents a puzzle, enter the correct answer, confirms overall statistics
	// are changed afterwards.
	cout << "performPuzzle() Test Case 1: Presents a puzzle, enter the correct answer," << endl
			<< " confirms overall statistics are changed afterwards" << endl;
	readPuzzles();
	initializeEncounters();
	PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 3;
	testPtr1 -> playerTime = 3;
	testPtr1 -> playerMoney = 3;
	testPtr1 -> stepsRemain = 3;
	testPtr1 = performPuzzle(testPtr1);
	displayPlayerStats(testPtr1);
	cout << endl;

	// Test Case 2: Presents a puzzle, enter an incorrect answer, confirms overall statistics
	// are change afterwards.
	cout << "performPuzzle() Test Case 2: Presents a puzzle, enter the incorrect answer," << endl
			<< " confirms overall statistics are changed afterwards" << endl;
	testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 3;
	testPtr1 -> playerTime = 3;
	testPtr1 -> playerMoney = 3;
	testPtr1 -> stepsRemain = 3;
	testPtr1 = performPuzzle(testPtr1);
	displayPlayerStats(testPtr1);
	cout << endl;

}

void gameOverTest() {

	// Test Case 1: Player's intelligence reached 0.
	cout << "gameOver() Test Case 1: Player's intelligence reached 0." << endl;
	PlayerStatsNodePtr testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 0;
	testPtr1 -> playerTime = 3;
	testPtr1 -> playerMoney = 3;
	testPtr1 -> stepsRemain = 3;
	ScoreListNodePtr check = gameOver(testPtr1, "charlie");
	assert(check -> playerName == "charlie");
	assert(check -> playerScore == 0);
	cout << endl;

	// Test Case 2: Player's money reached 0.
	cout << "gameOver() Test Case 2: Player's money reached 0." << endl;
	testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 3;
	testPtr1 -> playerTime = 3;
	testPtr1 -> playerMoney = 0;
	testPtr1 -> stepsRemain = 3;
	check = gameOver(testPtr1, "chuck");
	assert(check -> playerName == "chuck");
	assert(check -> playerScore == 0);
	cout << endl;

	// Test Case 3: Player's time reached 0.
	cout << "gameOver() Test Case 3: Player's time reached 0." << endl;
	testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 3;
	testPtr1 -> playerTime = 0;
	testPtr1 -> playerMoney = 3;
	testPtr1 -> stepsRemain = 3;
	check = gameOver(testPtr1, "chill");
	assert(check -> playerName == "chill");
	assert(check -> playerScore == 0);
	cout << endl;

	// Test Case 4: Player successfully completed the game.
	cout << "gameOver() Test Case 4: Player successfully completed the game." << endl;
	testPtr1 = new PlayerStatsNode();
	testPtr1 -> playerIntel = 3;
	testPtr1 -> playerTime = 3;
	testPtr1 -> playerMoney = 3;
	testPtr1 -> stepsRemain = 0;
	check = gameOver(testPtr1, "chill");
	assert(check -> playerName == "chill");
	assert(check -> playerScore == 27);
	cout << endl;

}
