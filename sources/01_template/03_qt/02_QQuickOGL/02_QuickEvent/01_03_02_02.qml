
// OpenGL QQuickFrameBufferObject
import QtQuick 2.12
import QtQuick.Window 2.12

//import后面跟的是模块名，和qmlRegisterType中的模块名对应
import LearnOpenGL 1.0

import QtQuick.Controls 2.0

Window {
    visible: true
    width: 450
    height: 450
    title: qsTr("Learn OpenGL") //窗口标题
    color: "white"  //窗口背景色，如果铺满整个窗口，就不会显示出来

   //左上角一个100*100的帧缓冲,x和y设置fboitem在窗口上开始的坐标，width和height设置宽度和高度
   //ViewerItem{
   //    width: 200
   //    height: 200
   //}
   //
   ////右下角
   //ViewerItem{
   //    x:250
   //    y:250
   //    width: 200
   //    height: 200
   //}

   //这个fboitem会覆盖前面的fboitem，前提是坐标有覆盖部分
   //参数:1.anchors.fill      设置填充
   //     2.anchors.margins   设置边距，为0则帧缓冲铺满整个窗口
    ViewerItem{
        anchors.fill: parent;  //anchors.centerIn这两个功能类似，fill子控件的大小设置与父控件大小一样
        anchors.margins: 50;   //设置子控件和父控件的边距

        Button {
            id: quit
            text: "Quit";
            x:0
            y:0
            width:50
            height:20
        }

        //MouseArea{
        //    anchors.fill:parent
        //    onClicked: {
        //        menu.open();
        //    }
        //}

        Connections{
         target: quit
         onClicked:{
            //Item.testFunc();
         }
        }
    }

    Menu{
        id:menu

        MenuItem{
            width:50
            height:50
            text:"menu1"
        }

        MenuItem{
            width:50
            height:50
            text:"menu2"
        }

        MenuItem{
            width:50
            height:50
            text:"menu3"
        }
    }
}


/*
// 点击按钮退出程序
import QtQuick 2.0
import QtQuick.Window 2.0

import QtQuick.Controls 1.1

Window{
    visible: true
    width: 450
    height: 450
    title: qsTr("Test Window") //窗口标题
    color: "white"  //窗口背景色，如果铺满整个窗口，就不会显示出来

    Button {
        text: "Quit";
        anchors.centerIn: parent;
        onClicked: {
            Qt.quit();
        }
    }


}
*/

/*
// 点击鼠标，窗口背景更改为指定颜色
import QtQuick 2.0
import QtQuick.Window 2.0  //使用Window不导入该模块窗口不会显示，也不会报错
import QtQuick.Controls 1.1
 
Window {
    visible: true
    width: 320;
    height: 240;
    title: qsTr("Test Window") //窗口标题
    color: "white"  //窗口背景色，如果铺满整个窗口，就不会显示出来

    
    MouseArea {
        anchors.fill: parent;
        acceptedButtons: Qt.LeftButton | Qt.RightButton;
        onClicked: {
            if(mouse.button == Qt.RightButton){
                Qt.quit();
            }
            else if(mouse.button == Qt.LeftButton){
                color = Qt.rgba((mouse.x % 255) / 255.0 , (mouse.y % 255) / 255.0, 0.6, 1.0);
            }
        }
        onDoubleClicked: {
            color = "gray";
        }
    }
}
*/

/*
// 按下按钮文本更改为随机颜色
import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
 
Window {
    visible: true
    width: 320;
    height: 240;
    color: "gray";
    
    Text {
        id: text1;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 20;
        text: "Text One";
        color: "blue";
        font.pixelSize: 28;
    }
    
    Text {
        id: text2;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: text1.bottom;
        anchors.topMargin: 8;
        text: "Text Two";
        color: "blue";
        font.pixelSize: 28;
    }
    
    Button {
        id: changeButton;
        anchors.top: text2.bottom;
        anchors.topMargin: 8;
        anchors.horizontalCenter: parent.horizontalCenter;
        text: "Change";
    }
    
    Connections {
        target: changeButton;
        onClicked: {
            text1.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1);
            text2.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1);
        }
    }
}
*/

/*
// 按钮改变指定对象
import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.1
 
Window {
    visible: true
    width: 320;
    height: 240;
    color: "gray";
    
    Text {
        id: hello;
        anchors.centerIn: parent;
        text: "Hello World!";
        color: "blue";
        font.pixelSize: 32;
        onTextChanged: {
            console.log(text);  //控制台输出需要用qmlscene运行
        }
    }
    
    Button {
        anchors.top: hello.bottom;
        anchors.topMargin: 8;
        anchors.horizontalCenter: parent.horizontalCenter;
        text: "Change";
        onClicked: {
            hello.text = "Hello Qt Quick";  //改变id为hello控件的文本
        }
    }
}
*/