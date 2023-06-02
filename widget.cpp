#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlaylist>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //创建多媒体对象
    mediaPlayer = new QMediaPlayer(this);

    //设置音量大小
    connect(ui->verticalSlider,&QSlider::valueChanged,mediaPlayer,&QMediaPlayer::setVolume);

    ui->fileBtn->setIcon(QPixmap(":/assets/a.1.png"));
    ui->modeBtn->setIcon(QPixmap(":/assets/a.2.2.png"));
    ui->previousBtn->setIcon(QPixmap(":/assets/a.3.png"));
    ui->playingBtn->setIcon(QPixmap(":/assets/a.41.png"));
    ui->nextBtn->setIcon(QPixmap(":/assets/a.5.png"));
    ui->volumeBtn->setIcon(QPixmap(":/assets/a.6.png"));
    ui->searchBtn->setIcon(QPixmap(":/assets/searchBtn.png"));
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->verticalSlider->hide();

    //
    ui->tableWidget->setRowCount(18);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"歌曲名"<<"歌手"<<"专辑"<<"时长");
    this->setWindowTitle("在线音乐播放器");
    this->setWindowIcon(QPixmap(":/assets/title.png"));

    //打开文件夹 过滤指定类型文件
    connect(ui->fileBtn,&QPushButton::clicked,[=](){
        auto path = QFileDialog::getExistingDirectory(this,"请选择文件","C:\\Users\\zz\\Desktop\\music");
        QDir dir(path);
        auto musicList = dir.entryList(QStringList()<<"*.mp3"<<"*.wav");
        ui->listWidget->addItems(musicList);

        //默认选择第一个
        ui->listWidget->setCurrentRow(0);

        for(auto file:musicList)
            playList.append(QUrl::fromLocalFile(path+"/"+file));
    });

    //播放键
    connect(ui->playingBtn,&QPushButton::clicked,[=](){
        switch (mediaPlayer->state())
        {
        case QMediaPlayer::StoppedState:
        {
            curPlayIndex = ui->listWidget->currentRow();
            mediaPlayer->setMedia(playList[curPlayIndex]);
            mediaPlayer->play();
            ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
            break;
        }
        case QMediaPlayer::PlayingState:
        {
            if(objectChange == true)
            {
                objectChange = false;
                mediaPlayer->stop();
                curPlayIndex = ui->listWidget->currentRow();
                mediaPlayer->setMedia(playList[curPlayIndex]);
                mediaPlayer->play();
                ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
            }
            else
            {
                ui->playingBtn->setIcon(QPixmap(":/assets/a.41.png"));
                mediaPlayer->pause();
            }
            break;
        }
        case QMediaPlayer::PausedState:
        {
            if(objectChange == true)
            {
                objectChange = false;
                mediaPlayer->stop();
                curPlayIndex = ui->listWidget->currentRow();
                mediaPlayer->setMedia(playList[curPlayIndex]);
            }
            mediaPlayer->play();
            ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
            break;
        }
        }

    });

    //鼠标点击列表的内容
    connect(ui->listWidget,&QListWidget::itemClicked,[=](){
        if(curPlayIndex != ui->listWidget->currentRow())
        {
            objectChange = true;
            ui->playingBtn->setIcon(QPixmap(":/assets/a.41.png"));
        }
    });

    //鼠标双击列表以播放
    connect(ui->listWidget,&QListWidget::doubleClicked,[=](){
        curPlayIndex = ui->listWidget->currentRow();
        mediaPlayer->setMedia(playList[curPlayIndex]);
        mediaPlayer->play();
        ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
    });

    //上一首
    connect(ui->previousBtn,&QPushButton::clicked,[=](){
        if(onlineSgin == true)  //使用在线播放
        {
            //歌曲请求 row 是行号
            if(onlinePlayIndex == 0)
                return;
            int musicIndex = idList[--onlinePlayIndex];
            QString url=QString("https://music.163.com/song/media/outer/url?id=%0").arg(musicIndex);
            mediaPlayer->setMedia(QUrl(url)); //添加一音乐到播放列表中
            mediaPlayer->play();
            ui->nameLabel->setText(nameList[onlinePlayIndex]);
            ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
            onlineSgin = true;
        }
        else
        {                       //播放本地音乐
            //播放状态为随机时，切歌同样随机
            if(playingMode == 2)
            {
                int newIndex = qrand()%(playList.size()-1);
                curPlayIndex = newIndex>=curPlayIndex?newIndex+1:newIndex;
                ui->listWidget->setCurrentRow(curPlayIndex);
                mediaPlayer->stop();
                mediaPlayer->setMedia(playList[curPlayIndex]);
                mediaPlayer->play();
                return;
            }
            curPlayIndex = (--curPlayIndex + playList.size()) % playList.size();
            ui->listWidget->setCurrentRow(curPlayIndex);
            mediaPlayer->setMedia(playList[curPlayIndex]);
            mediaPlayer->play();
        }

    });

    //下一首
    connect(ui->nextBtn,&QPushButton::clicked,[=](){
        if(onlineSgin == true)
        {
            if(onlinePlayIndex == 17)
                return;
            int musicIndex = idList[++onlinePlayIndex];
            QString url=QString("https://music.163.com/song/media/outer/url?id=%0").arg(musicIndex);
            mediaPlayer->setMedia(QUrl(url)); //添加一音乐到播放列表中
            mediaPlayer->play();
            ui->nameLabel->setText(nameList[onlinePlayIndex]);
            ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
            onlineSgin = true;
        }
        else
        {
            if(playingMode == 2)
            {
                int newIndex = qrand()%(playList.size()-1);
                curPlayIndex = newIndex>=curPlayIndex?newIndex+1:newIndex;
                ui->listWidget->setCurrentRow(curPlayIndex);
                mediaPlayer->stop();
                mediaPlayer->setMedia(playList[curPlayIndex]);
                mediaPlayer->play();
                return;
            }
//            curPlayIndex++;
//            if(curPlayIndex >= playList.size())
//                curPlayIndex = 0;
            curPlayIndex = (++curPlayIndex)%playList.size();
            ui->listWidget->setCurrentRow(curPlayIndex);
            mediaPlayer->setMedia(playList[curPlayIndex]);
            mediaPlayer->play();
        }
    });

    //音量调节
    connect(ui->volumeBtn,&QPushButton::clicked,this,&Widget::pressVolume);
    //通过信号关联 获取当前媒体的时长
    connect(mediaPlayer,&QMediaPlayer::durationChanged,[=](qint64 duration){
        this->curDuration = duration;
        ui->totalLabel->setText(QString("%1:%2").arg(duration/60/1000,2,10,QChar('0')).arg(duration/1000%60));
        ui->playCourseSlider->setRange(0,duration);
    });

    //设置进度条上方的当前时间
    //设置循环  当总时长等于当前时长时，判断播放完毕
    connect(mediaPlayer,&QMediaPlayer::positionChanged,[=](qint64 pos){
        ui->curLabel->setText(QString("%1:%2").arg(pos/60/1000,2,10,QChar('0')).arg(pos/1000%60,2,10,QChar('0')));
        ui->playCourseSlider->setValue(pos);

        if(pos == this->curDuration && pos != 0)
        {
            QTimer::singleShot(300,this,[=](){
                switch(playingMode)
                {
                case 0: //单曲循环
                    mediaPlayer->play();
                    break;
                case 1: //顺序播放
                {
                    curPlayIndex = (++curPlayIndex)%playList.size();
                    ui->listWidget->setCurrentRow(curPlayIndex);
                    mediaPlayer->stop();
                    mediaPlayer->setMedia(playList[curPlayIndex]);
                    mediaPlayer->play();
                    break;
                }
                case 2: //随机播放
                {
                    int newIndex = qrand()%(playList.size()-1);
                    curPlayIndex = newIndex>=curPlayIndex?newIndex+1:newIndex;
                    ui->listWidget->setCurrentRow(curPlayIndex);
                    mediaPlayer->stop();
                    mediaPlayer->setMedia(playList[curPlayIndex]);
                    mediaPlayer->play();
                    break;
                }
                }
            });

        }
    });
    connect(ui->playCourseSlider,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);

    //更改播放模式
    connect(ui->modeBtn,&QPushButton::clicked,[=](){
        playingMode = (++playingMode)%3;
        switch(playingMode)
        {
        case 0:
            ui->modeBtn->setIcon(QPixmap(":/assets/a.2.1.png"));
            break;
        case 1:
            ui->modeBtn->setIcon(QPixmap(":/assets/a.2.2.png"));
            break;
        case 2:
            ui->modeBtn->setIcon(QPixmap(":/assets/a.23.png"));
            break;
        }
    });

    //线上搜索功能实现
    networkManager = new QNetworkAccessManager();
    networkRequest = new QNetworkRequest();
    connect(networkManager,&QNetworkAccessManager::finished,this,&Widget::replyFinishedW);
    connect(ui->searchBtn,&QPushButton::clicked,[=](){
        searchW(ui->searchLine->text());
    });
    connect(ui->searchLine,&QLineEdit::editingFinished,[=](){
        QString strIn = ui->searchLine->text();
        if(strIn != "")
        {
            searchW(ui->searchLine->text());
        }
    });
    //线上播放功能实现
    connect(ui->tableWidget,&QTableWidget::cellDoubleClicked,this,&Widget::doubleClickedTable);
}

Widget::~Widget()
{
    delete ui;
}

//ui界面点击“转到槽”,不可删除，就不该点
void Widget::on_fileBtn_clicked()
{

}

void Widget::searchW(QString str)
{
    str="http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s={"+str+"}&type=1&offset=0&total=true&limit=18";
    QNetworkRequest request;
    request.setUrl(str);
    networkManager->get(request);
}

void Widget::replyFinishedW(QNetworkReply *reply)
{

    QByteArray searchInfo=reply->readAll();
    QJsonParseError err;
    QJsonDocument parseDoucment = QJsonDocument::fromJson(searchInfo,&err);
    //qDebug()<<parseDoucment;    //输出jsonDocument
    if(err.error != QJsonParseError::NoError)
    {
        qDebug() <<"搜索歌曲Json获取格式错误"<< err.errorString();
        return;
    }
    QJsonObject totalObject = parseDoucment.object();
    QStringList keys = totalObject.keys();
    if(keys.contains("result"))
    {
        QJsonObject jsonObj = totalObject["result"].toObject();     //将带result的内容提取后转换为对象
        QStringList keyList = jsonObj.keys();
        if(keyList.contains("songs"))
        {
            QJsonArray array = jsonObj["songs"].toArray();               //开始获取歌曲中的信息
            int index = 0;
            for(auto i : array)                   //开始获取歌曲中的信息
            {
                QJsonObject obj = i.toObject();
                musicId = obj["id"].toInt();
                idList.append(musicId);
                musicDuration = obj["duration"].toInt() / 1000; //音乐长度 单位为毫秒
                musicName = obj["name"].toString();
                nameList.append(musicName);
                QStringList artistsKeys = obj.keys();
                if(artistsKeys.contains("artists"))
                {
                    QJsonArray artistsArray = obj["artists"].toArray();
                    artistsName = "";
                    for(auto j : artistsArray)
                    {
                        QJsonObject artistsObj = j.toObject();
                        artistsName += artistsObj["name"].toString() + " ";     //歌手名
                    }
                }
                if(artistsKeys.contains("album"))
                {
                    QJsonObject albumObjct = obj["album"].toObject();
                    albumName = albumObjct["name"].toString();
                }
                ui->tableWidget->setItem(index,0,new QTableWidgetItem(musicName));
                ui->tableWidget->setItem(index,1,new QTableWidgetItem(artistsName));
                ui->tableWidget->setItem(index,2,new QTableWidgetItem(albumName));
                QString time = QString("%1:%2").arg(musicDuration/60).arg(musicDuration%60);
                ui->tableWidget->setItem(index++,3,new QTableWidgetItem(time));
            }
        }
    }
//    QString url;
//    url=QString("https://music.163.com/song/media/outer/url?id=%0").arg(musicId);
//    mediaPlayer->setMedia(QUrl(url));
//    mediaPlayer->play();
}

void Widget::doubleClickedTable(int row, int column)
{
    //歌曲请求 row 是行号
    int musicIndex = idList[row];
    onlinePlayIndex = row;
    QString url=QString("https://music.163.com/song/media/outer/url?id=%0&br=12800").arg(musicIndex);
    mediaPlayer->setMedia(QUrl(url)); //添加一音乐到播放列表中
    mediaPlayer->play();
    ui->nameLabel->setText(nameList[row]);
    ui->playingBtn->setIcon(QPixmap(":/assets/a.42.png"));
    onlineSgin = true;
}

void Widget::pressVolume()
{
    if(volumeSliderHide == false)
    {
        ui->verticalSlider->show();
        volumeSliderHide = true;
    }
    else
    {
        ui->verticalSlider->hide();
        volumeSliderHide = false;
    }
    connect(ui->verticalSlider,&QSlider::valueChanged,mediaPlayer,&QMediaPlayer::setVolume);
}
