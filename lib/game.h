#pragma once

#include <time.h>
#include "lib/field.h"
#include "lib/hist.h"
#include "lib/basic.h"
#include "lib/anime.h"
#include "lib/keyboard.h"
#include "lib/comment.h"


//int trainCnt = 0;
//int comHistt[100];
void initializeTrain() {
	//trainCnt = 0;
	//for (int i = 0; i < 100; i++) {
		//comHistt[i] = 0;
	//}
}

class Game {
public:
	int flg = -3;	// -3,..,-1: Demo
					// 0: Menu, 1: Game, 2: Result
					// -4: Ending
					// -6: Story
					// 5: High-speed Learning
	int taijin = 0;		// 0: vsHuman, 1: vsCOM, 2: AutoLearning
	int teban = 0;		// 0: senko, 1: koko
	int cnt = 0;		// ターン数
	int keyboardFlg = 0;	// 0: マウス操作, 1: キーボード操作
	int debugFlg = 0;

	int nextField = -1;		// -1: anywhere
	int drawCnt = 0;		// 引き分け時の強制終了のためのカウント
	string mode = "";

	// 同期処理関連
	long start = clock();	// 同期処理開始時刻
	long fpsStart = clock();	// fps計測開始時刻
	int fps = 0;			// fps出力用
	int fpsCnt = 0;			// fps計測用

	Option option;
	Field mother;
	Field child[3][3];
	History hist;
	Camera camera;
	Mouse mouse;
	Key key;
	Logo logo;
	Anime cutin;
	Comment comment;

	void initialize(int f = 1) {
		flg = f;
		cnt = 0;
		drawCnt = 0;
		nextField = -1;
		camera.initialize();
		mother.initialize();
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				child[i][j].initialize();
			}
		}
		hist.initialize();
		mouse.set();
		key.initWait();
	}

	double update(int global_x, int global_y, int local_x, int local_y, int side = 0) {
		if (side == 0) {
			side = 1 - 2 * (cnt % 2);
		}
		//盤面の更新
		if (nextField == 3 * global_x + global_y || nextField == -1) {
			if (child[global_x][global_y].update(local_x, local_y, side) == 0) {
				cnt++;
				//履歴を残す
				hist.add(global_x, global_y, local_x, local_y, nextField);
				//全体の更新
				mother.update(global_x, global_y, child[global_x][global_y].victory());
				if (child[local_x][local_y].victory() != 0) {
					nextField = -1;
					return RWD_DOM;
				}
				else {
					nextField = local_x * 3 + local_y;
					return RWD_PUT;
				}
			}
		}
		return -100.0;
	}

	void sync() {
		// 実効fps計測
		fpsCnt++;
		if (clock() - fpsStart > 1000.0) {
			fps = fpsCnt;
			fpsCnt = 0;
			fpsStart = clock();
		}
		// 同期処理
		while (clock() - start < 1000.0 / FPS && flg != 5) {
			WaitTimer(1);
		}
		start = clock();
	}

	//永遠に勝敗がつかない場合の処理
	void stopDrawGame() {
		if (drawCnt > 10000) {
			flg = 2;
		}
		if (taijin == 2 || flg == 5) {
			drawCnt++;
		}
	}


	/*===========================*/
	//    Bool
	/*===========================*/
	bool isPlayTurn() {
		// 対人戦、あるいは人vsCOMの人の手番
		return (vsHuman() || (vsCOM() && cnt % 2 == teban));
	}

	bool vsHuman() {
		return taijin == 0;
	}

	bool vsCOM() {
		return taijin == 1;
	}


	/*===========================*/
	//    リセットボタン
	/*===========================*/
	void drawLogo() {
		if (flg > 0) {
			if (mouse.onButton(logo.titleX, logo.titleY - 5, logo.titleX + 185, logo.titleY + 65)) {
				DrawBox(logo.titleX, logo.titleY - 5, logo.titleX + 185, logo.titleY + 65, GetColor(20, 150, 150), TRUE);
			}
			logo.draw();
			if (flg == 1) {
				logo.update();
			}
		}
	}

	int reset() {
		if (flg > 0) {
			if (mouse.onButton(logo.titleX, logo.titleY - 5, logo.titleX + 185, logo.titleY + 65)) {
				if (mouse.button[0] == 1) {
					mouse.set();
					flg = 0;
					taijin = 0;
					StopMusic();
					return 1;
				}
			}
		}
		return 0;
	}


	/*===========================*/
	//    キーボード入力関連
	/*===========================*/
	void toggleByKey() {
		key.toggleSetting(option, logo);
		//key.configLearning();
		key.toggleDebug(debugFlg);
		if (debugFlg) {
			key.toggleForDebug(option, cutin.flg);
		}
	}

	void toggleHighSpeedLearning() {
		if (key.toggleHighSpeedLearning(flg) == 1) {
			mode = "オート";
			initialize(flg);
			initializeTrain();
		}
	}

	void toggleAutoLearning() {
		if (key.toggleAutoLearning(flg, taijin) == 1 || key.toggleHighSpeedLearning(flg) == 1) {
			mode = "オート";
			initialize(flg);
			initializeTrain();
		}
	}

	void skipBattle(int& sceneFlg) {
		if (debugFlg) {
			key.skipBattle(flg, sceneFlg);
		}
	}


	/*===========================*/
	//    デバッグ情報
	/*===========================*/
	void debugDump() {
		if (debugFlg) {
			int strColor = option.strColor;
			// Game
			DrawFormatString(5, 25, strColor, "fps: %d", fps);
			DrawFormatString(5, 45, strColor, "gameFlg: %d", flg);
			DrawFormatString(5, 65, strColor, "taijin: %d", taijin);
			DrawFormatString(5, 85, strColor, "teban: %d", teban);
			DrawFormatString(5, 105, strColor, "cnt: %d", cnt);
			DrawFormatString(5, 125, strColor, "keyboardFlg: %d", keyboardFlg);
			// Option
			DrawFormatString(125, 25, strColor, "musicFlg: %d", option.musicFlg);
			DrawFormatString(125, 45, strColor, "soundFlg: %d", option.soundFlg);
			DrawFormatString(125, 65, strColor, "likeliFlg: %d", option.likeliFlg);
			DrawFormatString(125, 85, strColor, "commentFlg: %d", option.commentFlg);
			// Hist
			DrawFormatString(125, 145, strColor, "cancelCnt: %d", hist.cancelCnt);
			DrawFormatString(125, 165, strColor, "last[0]: %d", hist.last[0]);
			DrawFormatString(125, 185, strColor, "last[1]: %d", hist.last[1]);
			DrawFormatString(125, 205, strColor, "last[2]: %d", hist.last[2]);
			DrawFormatString(125, 225, strColor, "last[3]: %d", hist.last[3]);
			DrawFormatString(125, 245, strColor, "last[4]: %d", hist.last[4]);
			// Comment
			DrawFormatString(245, 25, strColor, "texts.maxSize: %d", comment.texts.maxSize);
			DrawFormatString(245, 45, strColor, "texts.size: %d", comment.texts.size);
			DrawFormatString(245, 65, strColor, "comment.x: %d", comment.x);
			DrawFormatString(245, 85, strColor, "comment.y: %d", comment.y);
			DrawFormatString(245, 105, strColor, "textId: %d", comment.textId);
			DrawFormatString(245, 125, strColor, "textSeq: %d", comment.textSeq);
			DrawFormatString(245, 145, strColor, "comment.cnt: %d", comment.cnt);
		}
	}
};
