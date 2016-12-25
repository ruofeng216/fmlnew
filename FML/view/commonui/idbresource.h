#ifndef IDBResource_H
#define IDBResource_H

#include <QString>
#include <QPixmap>
#include <QMap>

struct IDBIconPosition{
private:
    void init(){
        row = 0;
        column = 0;
        width = 16;
        height = 16;
        x = 0;
        y = 0;
    }

public:
    int row;
    int column;
    int width;
    int height;
    int x;
    int y;

    IDBIconPosition(int row_,int width_,int height_) {
        init();
        row = row_;
        width = width_;
        height = height_;
    }

    IDBIconPosition(){
        init();
    }

    IDBIconPosition &setXYColumn(int x_,int y_,int column_=0){
        x = x_;
        y = y_;
        column = column_;
        return *this;
    }

};

namespace IDBResourceNS {
	const IDBIconPosition CANCLE_ICON = IDBIconPosition(3, 20, 20);
	const IDBIconPosition SETTING_ICON = IDBIconPosition(8, 20, 20);
	const IDBIconPosition CHECK_BOX_ICON = IDBIconPosition(9,20,20);
	const IDBIconPosition TOOGGLE_ICON = IDBIconPosition(10,20,20);
	const IDBIconPosition CHAT_ICON = IDBIconPosition(11,20,20);
	const IDBIconPosition SILENT_ICON = IDBIconPosition(25,20,20);
	const IDBIconPosition ADD_ICON = IDBIconPosition(26,20,20);
	const IDBIconPosition CHAT_MANAGER_ICON = IDBIconPosition(27,20,20);

	const IDBIconPosition CHAT_LARGE_ICON = IDBIconPosition(0, 24, 24);
	const IDBIconPosition CONTACT_LARGE_ICON = IDBIconPosition(1, 24, 24);
	const IDBIconPosition ROOM_LARGE_ICON = IDBIconPosition(2, 24, 24);
	const IDBIconPosition ADD_LARGE_ICON = IDBIconPosition(3, 24, 24);
	const IDBIconPosition SETTING_LARGE_ICON = IDBIconPosition(4, 24, 24);
	const IDBIconPosition NEWS_LARGE_ICON = IDBIconPosition(5,24,24);
	const IDBIconPosition TOOL_EMOTION_ICON = IDBIconPosition(6,24,24); 
	const IDBIconPosition FINISHED_LARGE_ICON = IDBIconPosition(6, 40, 40);

	const IDBIconPosition BASICUI_TITLE_MIN = IDBIconPosition(0, 20, 20);
	const IDBIconPosition BASICUI_TITLE_CLOSE = IDBIconPosition(1, 20, 20);
	const IDBIconPosition BASICUI_TITLE_MAX = IDBIconPosition(2, 20, 20);
	const IDBIconPosition BASICUI_TITLE_RESTORE = IDBIconPosition(21, 20, 20);
	const IDBIconPosition SESSION_ICON = IDBIconPosition(11, 20, 20);

	const IDBIconPosition PROFILE_POPUP_MENU = IDBIconPosition(0, 30, 30);
	const IDBIconPosition PROFILE_HISTROY = IDBIconPosition(1, 30, 30);
	const IDBIconPosition ADD_TRANSPARENT_ICON = IDBIconPosition(0,36, 36);
	const IDBIconPosition FINISH_TRANSPARENT_ICON = IDBIconPosition(1,36, 36);	
	const IDBIconPosition HISTORY_MESSAGE_FACE=IDBIconPosition(14,20,20);
	const IDBIconPosition HISTORY_MESSAGE_FILE=IDBIconPosition(15,20,20);
	const IDBIconPosition HISTORY_MESSAGE_NEXT=IDBIconPosition(16,20,20);
	const IDBIconPosition HISTORY_MESSAGE_PRE=IDBIconPosition(17,20,20);
	const IDBIconPosition HISTORY_MESSAGE_CLOUD=IDBIconPosition(31,20,20);
	const IDBIconPosition MARCKET_TOP_MOST=IDBIconPosition(32,20,20);
	const IDBIconPosition MARCKET_NOTTOP_MOST=IDBIconPosition(33,20,20);
	const IDBIconPosition EYE_SHOW_ICON=IDBIconPosition(34,20,20);
	const IDBIconPosition EYE_HIDE_ICON=IDBIconPosition(35,20,20);

	const IDBIconPosition CALENDER_PRE=IDBIconPosition(13,20,20); 
	const IDBIconPosition CALENDER_NEXT=IDBIconPosition(12,20,20); 

	const IDBIconPosition FACE_ICON = IDBIconPosition(7,20,20); 
	const IDBIconPosition FILE_ICON = IDBIconPosition(6,20,20);
	const IDBIconPosition SHAKE_ICON = IDBIconPosition(5,20,20);
	const IDBIconPosition CAPTURE_ICON = IDBIconPosition(4,20,20);
	const IDBIconPosition SINGLESESSION_MENU = IDBIconPosition(22,20,20);
	const IDBIconPosition QUOTE_ICON = IDBIconPosition(24,20,20);
	const IDBIconPosition MERGE_ICON = IDBIconPosition(29,20,20);
	const IDBIconPosition SHARE_ICON = IDBIconPosition(30,20,20);

	const IDBIconPosition SIMPLE_MODE_ROOM_ICON = IDBIconPosition(2, 16, 16);
	const IDBIconPosition SIMPLE_MODE_ORG_ICON = IDBIconPosition(3, 16, 16);
	const IDBIconPosition SIMPLE_MODE_SESSIONASSISTANT_ICON = IDBIconPosition(6, 16, 16);
	const IDBIconPosition RADION_MODE_NAMAL=IDBIconPosition(4,16,16);
	const IDBIconPosition RADION_MODE_PRESSED=IDBIconPosition(5,16,16);
	const IDBIconPosition SIMPLE_MODE_MASS_ICON = IDBIconPosition(7, 16, 16);
	const IDBIconPosition BACK_ICON=IDBIconPosition(0,58,24);
}
//normal£¬hover£¬press£¬disable 
class IDBResource
{
public:
    IDBResource(void);
    ~IDBResource(void);
    static QPixmap &icon(IDBIconPosition position,int column);
    static void icon(const QList<IDBIconPosition> &positions,QPixmap &result);
    static void icon(const IDBIconPosition &position,QPixmap &result);
    static void flushCache();

private:
    static QMap<QString,QPixmap *> fullIcons;
    static QMap<QString,QPixmap *> icons;
};

#endif // IDBResource_H
