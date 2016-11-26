/*
 *このヘッダファイルには各種値、サイズを定義する
 */
#ifndef LIMITS_H
#define LIMITS_H

/*
 *シェル領域の幅
 */
#define PROC_MONITOR_X  (binfo->scrnx-124)

/*
 *同時にマルチタスクを実現できるプロセスの最大数
 */
#define	MAX_TASKS	1000

/*
 *設定できるタイマの最大数
 */
#define	MAX_TIMER	500

/*
 *一つのプロセスレベルに何個のプロセスを格納できるか
 */
#define	MAX_TASKS_LV	100
/*
 *プロセスレベルの段階数
 */
#define	MAX_TASKLEVELS	10


#endif
