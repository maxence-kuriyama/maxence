#pragma once

#include "lib/const.h"
#include "lib/com.h"
#include "lib/menu.h"
#include "lib/modes/common/game.h"
#include "lib/modes/battle/camera.h"
#include "lib/modes/battle/anime.h"
#include "lib/modes/battle/comment.h"

// バトルモード
// 主にGameの操作と描画
// Gameの描画はEndingとBattleResultでも行うため、公開する必要がある
class Battle {
private:
	Menu menu;
	Button btnSave;
	Button btnReset;

	Anime cutin;
	Camera camera;
	Comment comment;
	UserInput* ui;
	double theta = 0.3;

	int White = GetColor(255, 255, 255);
	int Black = GetColor(0, 0, 0);
	int strColor = White;
	int strColorDebug = GetColor(50, 50, 200);
	bool likelihoodFlg = false; // 学習機械の出力フラグ
	bool commentFlg = false;

public:
	Game game;

	Battle(UserInput* src_ui) {
		ui = src_ui;
		ui->reset();
		game.initialize();
		camera.initialize();
		comment.initialize();
		// ボタン初期化
		btnSave.initialize(TEXT_SAVE_X, TEXT_SAVE_Y, "中断");
		btnReset.initialize(TEXT_RESET_X, TEXT_RESET_Y, "タイトル");
		// カットイン画像初期化
		int Cutin1 = LoadGraph("graph/cutin1.png");
		int Cutin10 = LoadGraph("graph/cutin10.png");
		GraphBlend(Cutin1, Cutin10, 255, DX_GRAPH_BLEND_MULTIPLE);
		cutin.image0 = Cutin1;
		cutin.image1 = Cutin10;
	}

	~Battle() {}

	void tick() {
		game.tick();
	}

	void start(int player1, int player2, bool init = true) {
		ui->reset();
		game.goBattle(player1, player2, init);
		menu.set(btnSave, btnReset);
	}

	void startVsHuman() {
		game.setVsHuman();
		start(BATTLE_PLAYER_YELLOW, BATTLE_PLAYER_YELLOW);
	}

	void setVsCOM() {
		game.setVsCOM();
	}

	void setSenko() {
		game.teban = TEBAN_SENKO;
	}

	void setKoko() {
		game.teban = TEBAN_KOKO;
	}


	/*===========================*/
	//    Battle Mode
	/*===========================*/
	void drawBeforePlay() {
		game.drawBase();
		game.drawGlobalState();
		game.drawHistLast();
		game.drawNextField();
	}

	void drawAfterPlay() {
		game.drawLocalState();
		game.drawCurrentCoord();
		game.drawPlayers();
	}

	int show(COM& com, Music& bgm, bool debug) {
		int return_flg = FLAG_BATTLE;

		drawBeforePlay();
		playByPlayer(com);

		// 学習機械の出力描画
		if (likelihoodFlg) {
			VectorXd input = game.stateToInput();
			com.visualize(input);
		}

		// COMの手番
		if (!game.isPlayTurn()) {
			VectorXd input = game.stateToInput();
			com.play(input);
			//com.playMinMax(game.board, game.currentSide());
			game.update(com.choice);
		}

		drawAfterPlay();
		drawMessage();
		drawComment();
		cutin.update();

		// 勝利判定
		if (game.victory() != 0) {
			ui->reset();
			return_flg = FLAG_RESULT;
		}

		cancelPlay(com);

		// セーブ or リセット
		int choice = menuChoose();
		if (choice == 0 || choice == 1) {
			ui->reset();
			game.reset(bgm);
			return_flg = FLAG_TITLE;
		}

		moveCamera();

		// 対戦スキップ（一人用デバッグ）
		if (debug && skipBattle()) return_flg = FLAG_SCENARIO;

		return return_flg;
	}

	// デバッグ用
	bool skipBattle() {
		return (game.isVsCOM() && ui->onKeySkipDebug());
	}

	void drawMessage() {
		DrawFormatString(470, 80, strColor, "右クリック:");
		DrawFormatString(540, 100, strColor, "石を置く");
		DrawFormatString(470, 124, strColor, "zキー（BkSpキー）:");
		DrawFormatString(540, 144, strColor, "一手戻る");
	}

	void drawComment() {
		// コメント描画
		if (commentFlg) comment.draw(strColor);
		// コメント差し替え
		comment.update(game.playCnt);
	}

	void updateCommentInBattle() {
		comment.forceUpdate(COMMENT_CHANGE_TYPE_PLAY_TURN, 0.40);
	}


	/*===========================*/
	//    操作関連
	/*===========================*/
	void playByPlayer(COM& com) {
		if (game.isPlayTurn() && game.playTurn(*ui)) {
			double res = game.update();
			if (game.isUpdated(res)) {
				if (game.isVsCOM()) {
					com.setWait();
				}
				updateCommentInBattle();
			}
		}
	}

	int menuChoose() {
		bool no_keyboard = true;
		return menu.choose(*ui, strColor, no_keyboard);
	}

	void toggleByKey(bool debug) {
		// コメントを消す
		if (ui->onKeyComment()) {
			commentFlg = !commentFlg;
		}

		// 文字色の変更
		if (ui->onKeyColor()) {
			toggleStrColor();
		}

		if (debug) {
			// カットインを入れる
			if (ui->onKeyCutinDebug()) {
				cutin.flg = 1;
			}

			// 学習機械の出力を見る
			if (ui->onKeyVisualizeDebug()) {
				likelihoodFlg = !likelihoodFlg;
			}
		}
	}

	void toggleStrColor() {
		if (strColor == Black) {
			strColor = White;
		}
		else {
			strColor = Black;
		}
	}

	void moveCamera() {
		Mouse* mouse = ui->mouse;
		camera.set();
		if (mouse->click()) {
			mouse->set();
		}
		if (mouse->onClick()) {
			theta -= (mouse->distDragX()) * 0.05;
			mouse->set();
		}
		//MV1SetRotationXYZ(ModelHandle, VGet(0.0, theta + DX_PI_F, 0.0));
	}

	void cancelPlay(COM& com) {
		if (ui->onBack() && game.goBackHist()) {
			com.setWait();
		}
	}


	/*===========================*/
	//    デバッグ情報
	/*===========================*/
	void debugDump() {
		game.debugDump();

		int strColor = strColorDebug;
		// Option
		DrawFormatString(5, 205, strColor, "likeliFlg: %d", likelihoodFlg);
		DrawFormatString(5, 225, strColor, "commentFlg: %d", commentFlg);
		// Comment
		comment.debugDump(strColor);
	}
};
