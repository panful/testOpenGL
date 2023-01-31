#pragma once

#include <QObject>
#include <QQuickFramebufferObject>

class FboRender;

struct TestData
{
    int xx;
    float yy;
    std::string zz;
};

Q_DECLARE_METATYPE(TestData);

class ViewerItem :
    public QQuickFramebufferObject
{
    Q_OBJECT;

    // 在其他类中使用property调用viewitem成员函数
    Q_PROPERTY(TestData myTest READ getTest); //如果类型不是qt标准类型，需要用Q_DECLARE_METATYPE先声明一下
    // 第二个参数 myTest 和 property 的参数对应
    // 第四个参数是 ViewItem 中的函数

    // 在其他类中使用property获取viewitem成员变量
    Q_PROPERTY(int myTest1 MEMBER m_int);

    // 发信号
    // Q_PROPERTY( ... NOTIFY ...)
public:
    ViewerItem(QQuickItem* parent = nullptr);
    Renderer* createRenderer() const override;

    void mousePressEvent(QMouseEvent*) override;

public:
    Q_INVOKABLE  void testFunc();
private:
    TestData getTest()
    {
        return m_test;
    }

    //signals:
    //    void sigMouseEvent();
private:
    mutable FboRender* m_fbo;
    // 可以把m_fbo仅仅当作一个FboRender的实例，不推荐用m_fbo调用FboRender的任何函数
    // 因为viewitem和fbo的render()不是同一个线程，
    // 如果func()中有调用opengl函数那么资源无法生成成功，所以必须使用其他方法调用FboRender函数
    // 可以使用信号槽，也可以使用fbo的synchronize(synchronize需要在viewitem中调用update才会响应）

    TestData m_test{ 99,66.66,"xyz" };

    int m_int{ 88 };
};

