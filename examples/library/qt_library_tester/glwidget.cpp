/*
  Copyright 2014 Jan Adamec, Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
*/

#include <QtWidgets>
#include <QtOpenGL>

#include "glwidget.h"
#include "mainwindow.h"
#include "../../../src/library/castleengine.h"

GLWidget *g_pThis = NULL;

GLWidget::GLWidget(const QGLFormat &format, QWidget *parent) :
    QGLWidget(format, parent)
{
    g_pThis = this;
    m_bAfterInit = false;
    m_bLimitFPS = true;
    m_bNeedsDisplay = false;
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);    // accept key strokes

    QTimer *pUpdateTimer = new QTimer(this);
    connect(pUpdateTimer, SIGNAL(timeout()), this, SLOT(OnUpdateTimer()));
    pUpdateTimer->start(20);    // 50 fps
}

GLWidget::~GLWidget()
{
    CGE_Close();
    g_pThis = NULL;
}

void GLWidget::OpenScene(QString const &sFilename)
{
    m_sSceneToOpen = sFilename;
    if (!m_bAfterInit)
        return;

    CGE_LoadSceneFromFile(sFilename.toUtf8());
    ((MainWindow*)g_pThis->parent())->UpdateAfterSceneLoaded();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

int __cdecl GLWidget::OpenGlLibraryCallback(int eCode, int iParam1, int iParam2, const char *szParam)
{
    if (g_pThis == NULL || !g_pThis->m_bAfterInit) return 0;

    switch (eCode)
    {
    case ecgelibNeedsDisplay:
        g_pThis->m_bNeedsDisplay = true;
        return 1;

    case ecgelibSetMouseCursor:
        {
            QCursor aNewCur;
            switch (iParam1)
            {
            case ecgecursorWait: aNewCur.setShape(Qt::WaitCursor); break;
            case ecgecursorHand: aNewCur.setShape(Qt::PointingHandCursor); break;
            case ecgecursorText: aNewCur.setShape(Qt::IBeamCursor); break;
            case ecgecursorNone: aNewCur.setShape(Qt::BlankCursor); break;
            default: aNewCur.setShape(Qt::ArrowCursor);
            }
            g_pThis->setCursor(aNewCur);
        }
        return 1;

    case ecgelibNavigationTypeChanged:
        ((MainWindow*)g_pThis->parent())->UpdateNavigationButtons();
        return 1;

    case ecgelibSetMousePosition:
        {
            QPoint ptNew = g_pThis->mapToGlobal(QPoint(iParam1, g_pThis->height() - 1 - iParam2));
            QCursor::setPos(ptNew.x(), ptNew.y());
        }
        return 1;

    case ecgelibWarning:
        {
            QString sWarning = QString::fromUtf8(szParam);
            ((MainWindow*)g_pThis->parent())->AddNewWarning(sWarning);
        }
        return 1;
    }
    return 0;
}

void GLWidget::initializeGL()
{
    CGE_Open(ecgeofLog, 1000, 1000); // TODO - get the correct sizes here
    CGE_SetUserInterface(false, logicalDpiY());
    CGE_SetLibraryCallbackProc(OpenGlLibraryCallback);
    m_bAfterInit = true;
    if (!m_sSceneToOpen.isEmpty())
        OpenScene(m_sSceneToOpen);
}

void GLWidget::OnUpdateTimer()
{
    if (!m_bAfterInit) return;

    CGE_Update();
    if (!m_bLimitFPS || m_bNeedsDisplay)
    {
        m_bNeedsDisplay = false;
        updateGL();
    }
}

void GLWidget::paintGL()
{
    CGE_Render();
}

void GLWidget::resizeGL(int width, int height)
{
    CGE_Resize(width, height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    CGE_MouseDown(event->x(), height() - 1 - event->y(), event->button()==Qt::LeftButton, 0);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    CGE_Motion(event->x(), height() - 1 - event->y(), 0);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    CGE_MouseUp(event->x(), height() - 1 - event->y(), event->button()==Qt::LeftButton, 0);
}

#ifndef QT_NO_WHEELEVENT
void GLWidget::wheelEvent(QWheelEvent *event)
{
    CGE_MouseWheel(event->delta(), event->orientation()==Qt::Vertical);

    if (m_bNeedsDisplay)
    {
        m_bNeedsDisplay = false;
        updateGL();
    }
}
#endif

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && CGE_GetVariableInt(ecgevarMouseLook)==1)
    {
        CGE_SetVariableInt(ecgevarMouseLook, 0);
        CGE_SetVariableInt(ecgevarCrossHair, 0);
        event->accept();
        return;
    }

    CGE_KeyDown(QKeyToCgeKey(event->key()));
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    CGE_KeyUp(QKeyToCgeKey(event->key()));
}

int GLWidget::QKeyToCgeKey(int qKey)
{
    if (qKey >= Qt::Key_0 && qKey <= Qt::Key_9)
        return kcge_0 + qKey - Qt::Key_0;
    if (qKey >= Qt::Key_A && qKey <= Qt::Key_Z)
        return kcge_A + qKey - Qt::Key_A;
    if (qKey >= Qt::Key_F1 && qKey <= Qt::Key_F12)
        return kcge_F1 + qKey - Qt::Key_F1;

    switch (qKey)
    {
    case Qt::Key_Print: return kcge_PrintScreen;
    case Qt::Key_CapsLock: return kcge_CapsLock;
    case Qt::Key_ScrollLock: return kcge_ScrollLock;
    case Qt::Key_NumLock: return kcge_NumLock;
    case Qt::Key_Pause: return kcge_Pause;
    case Qt::Key_Apostrophe: return kcge_Apostrophe;
    case Qt::Key_Semicolon: return kcge_Semicolon;
    case Qt::Key_Backspace: return kcge_BackSpace;
    case Qt::Key_Tab: return kcge_Tab;
    case Qt::Key_Slash: return kcge_Slash;
    case Qt::Key_QuoteLeft: return kcge_BackQuote;
    case Qt::Key_Minus: return kcge_Minus;
    case Qt::Key_Return: return kcge_Enter;
    case Qt::Key_Equal: return kcge_Equal;
    case Qt::Key_Backslash: return kcge_BackSlash;
    case Qt::Key_Shift: return kcge_Shift;
    case Qt::Key_Control: return kcge_Ctrl;
    case Qt::Key_Alt: return kcge_Alt;
    case Qt::Key_Plus: return kcge_Plus;
    case Qt::Key_Escape: return kcge_Escape;
    case Qt::Key_Space: return kcge_Space;
    case Qt::Key_PageUp: return kcge_PageUp;
    case Qt::Key_PageDown: return kcge_PageDown;
    case Qt::Key_End: return kcge_End;
    case Qt::Key_Home: return kcge_Home;
    case Qt::Key_Left: return kcge_Left;
    case Qt::Key_Up: return kcge_Up;
    case Qt::Key_Right: return kcge_Right;
    case Qt::Key_Down: return kcge_Down;
    case Qt::Key_Insert: return kcge_Insert;
    case Qt::Key_Delete: return kcge_Delete;
    case Qt::Key_BracketLeft: return kcge_LeftBracket;
    case Qt::Key_BracketRight: return kcge_RightBracket;
    case Qt::Key_Enter: return kcge_Numpad_Enter;
    case Qt::Key_Comma: return kcge_Comma;
    case Qt::Key_Period: return kcge_Period;

    default: return kcge_None;
    }
}
