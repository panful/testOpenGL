import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.15

//import后面跟的是模块名，和qmlRegisterType中的模块名对应
import LearnOpenGL 1.0

Window {
    visible: true
    width: 600
    height: 480
    title: qsTr("Window Title") //窗口标题
    //color: "green"  //窗口背景色，如果铺满整个窗口，就不会显示出来

   //anchors.margins设置为0帧缓冲铺满整个窗口
    MyRenderer{
        anchors.fill: parent
        anchors.margins: 50
    }

    Button {
        text: "Quit";
        anchors.centerIn: parent;
        onClicked: {
            Qt.quit();
        }
    }
}
