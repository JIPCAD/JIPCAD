#pragma once
#include <QPlainTextEdit>
#ifdef __ARM_ARCH
#include <Qt3DCore>
#endif

namespace Nome
{

class CCodeEditor : public QPlainTextEdit
{
public:
    CCodeEditor();
};

}
