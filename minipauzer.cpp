#include "minipauzer.h"
#include "ui_minipauzer.h"

MiniPauzer::MiniPauzer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiniPauzer)
{
    ui->setupUi(this);

    player = new Player(this);
    detector = new AutoDetector(this);
    manager = new Manager();

	buttonPlayClickTimer = new QTimer(this);
	buttonPlayClickTimer->setSingleShot(true);
	connect(buttonPlayClickTimer, SIGNAL(timeout()), this, SLOT(releaseButtonPlay()));

    detector->start();

    //New song signals
    connect(player, SIGNAL(songLength(int)), ui->sliderBar, SLOT(setMaxLength(int)));
    connect(player, SIGNAL(songLength(int)), this, SLOT(updateLabelMaxLen(int)));

    //connect slider and stream
    connect(player, SIGNAL(posChanged(int)), ui->sliderBar, SLOT(setCurrentPos(int)));
    connect(ui->sliderBar, SIGNAL(valueChanged(int)), player, SLOT(setPosition(int)));

    //connect current time label
	ui->lbl_CurTime->setText("0:00");
    connect(player, SIGNAL(posChanged(int)), this, SLOT(updateLabelCurTime(int)));

    connect(detector, SIGNAL(audioDetected(int)), this, SLOT(on_detected_audio(int)));

	isPlaying = false;
	isDetectorOn = false;
	isManuallyPlayed = false;
	isAutoPauseAllowed = false;

	isButtonPlayClickAllowed = true;

}

MiniPauzer::~MiniPauzer()
{
    delete ui;
    delete player;
    delete manager;
    detector->exit();
    delete detector;
	delete buttonPlayClickTimer;
}

void MiniPauzer::on_btn_Play_clicked()
{
	if (isButtonPlayClickAllowed == true)
	{
		isButtonPlayClickAllowed = false;

		buttonPlayClickTimer->start(500);

		if (isDetectorOn)
			isDetectorOn = false;
		else
			isDetectorOn = true;

		if (isPlaying == false)
		{
			player->play();
			isManuallyPlayed = true;
			isPlaying = true;
		}
		else
		{
			player->pause();
			isPlaying = false;
		}
	}
}

void MiniPauzer::on_sliderBar_sliderPressed()
{
    ui->sliderBar->setDragging(true);
}

void MiniPauzer::on_sliderBar_sliderReleased()
{
    ui->sliderBar->setDragging(false);
}

void MiniPauzer::updateLabelCurTime(int time)
{
    int hour = time / 3600;
    time = time % 3600;
    int minute = time / 60;
    time = time % 60;
    int sec = time;

    QTime curTime;
    curTime.setHMS(hour, minute, sec);
    QString text;

    if (hour == 0)
        text = curTime.toString("m:ss");
    else
        text = curTime.toString("h:mm:ss");

    ui->lbl_CurTime->setText(text);
}

void MiniPauzer::updateLabelMaxLen(int legth)
{
    int hour = legth / 3600;
    legth = legth % 3600;
    int minute = legth / 60;
    legth = legth % 60;
    int sec = legth;

    QTime maxLen;
    maxLen.setHMS(hour, minute, sec);
    QString text;

    if (hour == 0)
        text = maxLen.toString("m:ss");
    else
        text = maxLen.toString("h:mm:ss");

    ui->lbl_songLen->setText(text);
}

void MiniPauzer::on_btn_ChooseFolders_clicked()
{
    FolderDialog *dialog = new FolderDialog();
    dialog->setModal(true);

    //Folder lists created
    connect(dialog, SIGNAL(createdFolderList(QStringList)), this, SLOT(getFolderList(QStringList)));

    dialog->exec();
    delete dialog;
}

void MiniPauzer::getFolderList(QStringList list)
{
    QMessageBox::information(0, "", list.at(0));
}

void MiniPauzer::on_detected_audio(int audio_num)
{
	if (isDetectorOn)
	{
		if (isManuallyPlayed == true && audio_num > 1)
		{
			isAutoPauseAllowed = false;
			isManuallyPlayed = false;
		}

		if (audio_num <= 1)
			isAutoPauseAllowed = true;

		if (isPlaying == true)
		{
			if (audio_num > 1 && isAutoPauseAllowed == true)
			{
				player->pause();
				isPlaying = false;
			}
		}
		else
		{
			if (audio_num == 0)
			{
				player->play();
				isPlaying = true;	
			}
		}
	}

}

void MiniPauzer::releaseButtonPlay()
{
	isButtonPlayClickAllowed = true;
}