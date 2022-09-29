import QtQuick 2.12
import QtQuick.Window 2.12

//import后面跟的是模块名，和qmlRegisterType中的模块名对应
import LearnOpenGL 1.0

Window {
    visible: true
    width: 600
    height: 480
    title: qsTr("Learn OpenGL") //窗口标题
   // color: "green"  //窗口背景色，如果铺满整个窗口，就不会显示出来

   //左上角一个100*100的帧缓冲
  //QFboItem{
  //    width: 100
  //    height: 100
  //}

   //这个fboitem会覆盖前面的fboitem，前提是坐标有覆盖部分
   //anchors.margins设置为0帧缓冲铺满整个窗口
    QFboItem{
        anchors.fill: parent
        anchors.margins: 50
    }
}
