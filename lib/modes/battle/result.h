#pragma once

#include "lib/const.h"
#include "lib/mode.h"
#include "lib/utils/user_input.h"
#include "lib/components/menu.h"
#include "lib/components/game.h"

const int TEXT_PLAYER_X(260);
const int TEXT_PLAYER_Y(200);
const int W_PLAYER(96);
const int TEXT_AGAIN_X(300);
const int TEXT_AGAIN_Y(240);
const int W_AGAIN(72);
const int H_AGAIN(16);

// 試合後の結果画面モード
class Result {
private:
	Menu menu;
	Button btnAgain;
	int Green = GetColor(0, 255, 0);
	int Red = GetColor(255, 0, 0);
	int Black = GetColor(0, 0, 0);
	int btnColor = Black;

public:

	Result() {
		int padding = 8;
		int ulx = TEXT_AGAIN_X - padding;
		int uly = TEXT_AGAIN_Y - padding;
		int drx = TEXT_AGAIN_X + W_AGAIN + padding;
		int dry = TEXT_AGAIN_Y + H_AGAIN + padding;
		btnAgain.initialize(TEXT_AGAIN_X, TEXT_AGAIN_Y, ulx, uly, drx, dry, "もう一回");
	}

	int show(Game& game) {
		drawGameBoard(game);

		// メッセージの描画
		btnAgain.display(btnColor);
		if (btnAgain.isClicked() || UserInput::onReturn()) {
			return MODE_BATTLE;
		}

		// コメントの描画
		// drawComment();

		// 動作の取り消し
		if (cancel(game)) return MODE_RESULT_CANCEL;

		return MODE_RESULT;
	}

private:

	bool cancel(Game& game) {
		return (UserInput::onBack() && game.goBackHist());
	}

	void drawGameBoard(Game& game) {
		game.drawBase();
		game.drawGlobalState();
		game.drawHistLast();
		game.drawNextField();
		game.drawLocalState();
		drawWinner(game.victory());
	}

	void drawWinner(int victory) {
		if (victory == VICTORY_BLACK) {
			DrawFormatString(TEXT_PLAYER_X, TEXT_PLAYER_Y, Red, "Player1");
		}
		else if (victory == VICTORY_WHITE) {
			DrawFormatString(TEXT_PLAYER_X, TEXT_PLAYER_Y, Black, "Player2");
		}
		else {
			DrawFormatString(TEXT_PLAYER_X, TEXT_PLAYER_Y, Green, "No one");
		}
		DrawFormatString(TEXT_PLAYER_X + W_PLAYER, TEXT_PLAYER_Y, Green, "Won");
	}
};
