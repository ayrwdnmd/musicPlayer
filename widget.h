#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUrl>
#include <QNetworkReply>
#include <QVector>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    //槽函数
    void searchW(QString str);              //搜索
    void replyFinishedW(QNetworkReply *);   //处理
    void doubleClickedTable(int, int);      //双击tabelWidget 用于在线播放
    void pressVolume();
    ~Widget();

private slots:
    void on_fileBtn_clicked();

private:
    Ui::Widget *ui;
    QMediaPlayer * mediaPlayer;     //创建多媒体对象
    QList<QUrl> playList;           //歌曲列表
    int playingMode = 1;            //播放模式
    int curDuration = 0;            //当前歌曲长度 用于判断是否播放完毕
    bool playingComplete = false;   //是否播放完毕
    bool objectChange = false;      //目标改变与否
    int curPlayIndex = 0;           //当前播放索引
    bool volumeSliderHide = false;


    QNetworkAccessManager * networkManager;
    QNetworkRequest * networkRequest;

    bool onlineSgin = false;
    int onlinePlayIndex = 0;           //在线播放索引
    int musicId;
    QString musicName;
    QString artistsName;
    QString albumName;
    int musicDuration;
    QList<int> idList;
    QList<QString> nameList;

};
#endif // WIDGET_H
