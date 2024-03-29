#include "ASTBinding.h"
#include "Environment.h"
#include "Point.h"
#include "SweepPath.h"
#include "Polyline.h"
#include "BSpline.h"
#include "Circle.h"
#include "BezierSpline.h"
#include "TorusKnot.h" // Randy added
#include "Polyline.h" // Randy added on 12/29
#include "SweepControlPoint.h"
#include "SweepMorph.h"
#include <Flow/FlowNode.h>
#include <Flow/FlowNodeArray.h>
#include <Parsing/SyntaxTree.h>
#include <StringPrintf.h>
#include <stack>

namespace Nome::Scene
{

CExprToNodeGraph::CExprToNodeGraph(AST::AExpr* expr, CBankAndSet& bankAndSet,
                                   Flow::TInput<float>* output)
    : BankAndSet(bankAndSet)
{
    InputStack.push(output);
    expr->Accept(this);
}

std::any CExprToNodeGraph::VisitIdent(AST::AIdent* ident)
{
    Flow::CFloatNumber* node;
    if (ident->ToString() == "time") {
        node = GEnv.Scene->GetTime();
        node->Value.Connect(*InputStack.top());
    } else if (ident->ToString() == "frame") {
        node = GEnv.Scene->GetFrame();
        node->Value.Connect(*InputStack.top());
    } else {
        CSlider* sum = BankAndSet.GetSlider(ident->ToString());
        if (!sum)
            throw AST::CSemanticError(
                tc::StringPrintf("Could not find slider %s", ident->ToString().c_str()), ident);
        sum->Value.Connect(*InputStack.top());
    }
    return {};
}

std::any CExprToNodeGraph::VisitNumber(AST::ANumber* number)
{
    auto* node = new Flow::CFloatNumber(static_cast<float>(number->AsDouble()));
    node->Value.Connect(*InputStack.top());
    return {};
}

std::any CExprToNodeGraph::VisitUnaryOp(AST::AUnaryOp* unaryOp)
{
    if (unaryOp->GetOperatorType() == AST::AUnaryOp::EOperator::Neg)
    {
        auto* node = new Flow::CFloatNeg();
        InputStack.push(&node->Operand0);
        unaryOp->GetOperand()->Accept(this);
        InputStack.pop();
        node->Result.Connect(*InputStack.top());
    }
    else if (unaryOp->GetOperatorType() == AST::AUnaryOp::EOperator::Plus)
    {
        auto* node = new Flow::CFloatAdd();
        InputStack.push(&node->Operand0);
        unaryOp->GetOperand()->Accept(this);
        InputStack.pop();
        node->Result.Connect(*InputStack.top());
    }
    else
    {
        throw AST::CSemanticError("Unrecognized unary operator", unaryOp);
    }
    return {};
}

std::any CExprToNodeGraph::VisitBinaryOp(AST::ABinaryOp* binaryOp)
{
    auto lambda = [this, binaryOp](auto* curr) {
        InputStack.push(&curr->Operand0);
        binaryOp->GetLeft()->Accept(this);
        InputStack.pop();
        InputStack.push(&curr->Operand1);
        binaryOp->GetRight()->Accept(this);
        InputStack.pop();
        curr->Result.Connect(*InputStack.top());
    };

    switch (binaryOp->GetOperatorType())
    {
    case AST::ABinaryOp::EOperator::Add:
        lambda(new Flow::CFloatAdd());
        break;
    case AST::ABinaryOp::EOperator::Sub:
        lambda(new Flow::CFloatSub());
        break;
    case AST::ABinaryOp::EOperator::Mul:
        lambda(new Flow::CFloatMul());
        break;
    case AST::ABinaryOp::EOperator::Div:
        lambda(new Flow::CFloatDiv());
        break;
    case AST::ABinaryOp::EOperator::Exp:
        lambda(new Flow::CFloatPow());
        break;
    default:
        throw AST::CSemanticError("Unrecognized binary operator", binaryOp);
    }
    return {};
}

std::any CExprToNodeGraph::VisitCall(AST::ACall* call)
{
    auto lambda = [this, call](auto* curr) {
        InputStack.push(&curr->Operand0);
        const AST::AVector* operandList = call->GetOperandList();
        if (operandList->GetItems().size() != 1)
            throw AST::CSemanticError("Call expression may only have one argument", call);
        operandList->GetItems()[0]->Accept(this);
        InputStack.pop();
        curr->Result.Connect(*InputStack.top());
    };

    std::string func = call->GetFuncName();
    if (func == "sin")
        lambda(new Flow::CFloatSin());
    else if (func == "cos")
        lambda(new Flow::CFloatCos());
    else
        throw AST::CSemanticError("Unrecognized function", call);
    return {};
}

std::any CExprToNodeGraph::VisitVector(AST::AVector* vector)
{
    throw AST::CSemanticError("Vectors should not appear in float context expressions", vector);
    return {};
}

std::any CExprToNodeGraph::VisitWrappedExpr(AST::AWrappedExpr* wrapped)
{
    wrapped->GetExpr()->Accept(this);
    return {};
}

AST::AExpr* CCommandSubpart::GetExpr(AST::ACommand* cmd) const
{
    AST::AExpr* expr = nullptr;
    if (NamedArgName.empty())
    {
        // Assume to be positional
        expr = cmd->GetPositionalArgument(Index);
    }
    else
    {
        // Otherwise fetch the named argument
        auto* namedArg = cmd->GetNamedArgument(NamedArgName);
        if (!namedArg)
            return nullptr;
        expr = namedArg->GetArgument(Index);
    }

    if (expr && SubIndex != -1 && expr->GetKind() == AST::EKind::Vector)
    {
        auto items = static_cast<AST::AVector*>(expr)->GetItems();
        if (SubIndex >= items.size())
            return nullptr;
        return items[SubIndex];
    }
    return expr;
}

bool CCommandSubpart::DoesNamedArgumentExist(AST::ACommand* cmd) const
{
    if (NamedArgName.empty())
        return false;
    auto* namedArg = cmd->GetNamedArgument(NamedArgName);
    return namedArg;
}

IBindingFunctor::~IBindingFunctor() = default;

template <>
bool TBindingTranslator<bool>::FromASTToValue(AST::ACommand* command,
                                              const CCommandSubpart& subpart, bool& value)
{
    assert(subpart.Index == 0);
    value = subpart.DoesNamedArgumentExist(command);
    if (subpart.NamedArgName.empty())
        throw AST::CSemanticError("Boolean value cannot be bound positionally", command);
    return true;
}

template <>
bool TBindingTranslator<Flow::TInput<float>>::FromASTToValue(AST::ACommand* command,
                                                             const CCommandSubpart& subpart,
                                                             Flow::TInput<float>& value)
{
    auto* expr = subpart.GetExpr(command);
    // Just return if the corresponding element is not found in the AST
    if (!expr)
        return false;
    CExprToNodeGraph converter { expr, GEnv.Scene->GetBankAndSet(), &value };
    return true;
}


template <>
bool TBindingTranslator<std::string>::FromASTToValue(AST::ACommand* command,
                                                     const CCommandSubpart& subpart,
                                                     std::string& value)
{
    auto* ident = subpart.GetExpr(command);
    if (ident == NULL)
    {
        return false;
    }

    if (ident->GetKind() != AST::EKind::Ident)
        throw AST::CSemanticError("Command is not matched with a Ident", command);

    value = static_cast<const AST::AIdent*>(ident)->ToString();

    return true;
}


template <>
bool TBindingTranslator<Flow::TInput<CVertexInfo*>>::FromASTToValue(
    AST::ACommand* command, const CCommandSubpart& subpart, Flow::TInput<CVertexInfo*>& value)
{
    auto* ident = subpart.GetExpr(command);
    if (ident == NULL)
        return false;

    if (ident->GetKind() != AST::EKind::Ident)
        throw AST::CSemanticError("Identifier required", ident);

    std::string identVal = static_cast<const AST::AIdent*>(ident)->ToString();
    Flow::TOutput<CVertexInfo*>* pointOutput = GEnv.Scene->FindPointOutput(identVal);
    if (!pointOutput)
    {
        throw AST::CSemanticError(tc::StringPrintf("Cannot find point %s", identVal.c_str()),
                                    ident);
    }
    value.Connect(*pointOutput);

    return true;
}

// Uses TInputArray instead of TInput

template <>
bool TBindingTranslator<Flow::TInputArray<CVertexInfo*>>::FromASTToValue(
    AST::ACommand* command, const CCommandSubpart& subpart, Flow::TInputArray<CVertexInfo*>& value)
{
    auto* vec = subpart.GetExpr(command);
    if (vec == nullptr)
    {
        return false;
    }

    if (vec->GetKind() != AST::EKind::Vector)
        throw AST::CSemanticError("TInputArray<CVertexInfo*> is not matched with a vector",
                                  command);
    for (const auto* ident : static_cast<AST::AVector*>(vec)->GetItems())
    {
        if (ident->GetKind() != AST::EKind::Ident)
            throw AST::CSemanticError("Identifier required", ident);
        std::string identVal = static_cast<const AST::AIdent*>(ident)->ToString();
        Flow::TOutput<CVertexInfo*>* pointOutput = GEnv.Scene->FindPointOutput(identVal);
        if (!pointOutput)
        {
            throw AST::CSemanticError(tc::StringPrintf("Cannot find point %s", identVal.c_str()),
                                      ident);
        }
        value.Connect(*pointOutput);
    }
    return true;
}


template <>
bool TBindingTranslator<Flow::TInput<CSweepPathInfo*>>::FromASTToValue(
    AST::ACommand* command, const CCommandSubpart& subpart, Flow::TInput<CSweepPathInfo*>& value)
{
    auto* ident = subpart.GetExpr(command);
    if (ident == NULL)
    {
        return false;
    }

    if (ident->GetKind() != AST::EKind::Ident)
        throw AST::CSemanticError("TInput<CSweepPathInfo*> is not matched with a Ident", command);

    std::string identVal = static_cast<const AST::AIdent*>(ident)->ToString();
    TAutoPtr<CEntity> entity = GEnv.Scene->FindEntity(identVal);
    if (!entity)
    {
        throw AST::CSemanticError(tc::StringPrintf("Cannot find entity %s", identVal.c_str()),
                                  ident);
    }

    CSweepPath* path = dynamic_cast<CSweepPath*>(entity.Get());
    if (!path)
    {
        throw AST::CSemanticError(tc::StringPrintf("Entity %s is not a sweep path", identVal.c_str()),
                                  ident);
    }

    auto& e = *entity.Get();
    if (typeid(e) == typeid(CPolyline))
    {
        CPolyline* polyline = dynamic_cast<CPolyline*>(path);
        value.Connect(polyline->Polyline);
    }
    else if (typeid(e) == typeid(CBSpline))
    {
        CBSpline* bspline = dynamic_cast<CBSpline*>(path);
        value.Connect(bspline->BSpline);
    }
    else if (typeid(e) == typeid(CCircle))
    {
        CCircle* circle = dynamic_cast<CCircle*>(path);
        value.Connect(circle->Circle);
    }
    else if (typeid(e) == typeid(CBezierSpline))
    {
        CBezierSpline* bezierSpline = dynamic_cast<CBezierSpline*>(path);
        value.Connect(bezierSpline->BezierSpline);
    }
    else if (typeid(e) == typeid(CTorusKnot))
    {
        CTorusKnot* torusknot = dynamic_cast<CTorusKnot*>(path);
        value.Connect(torusknot->TorusKnot);
    }
    else
    {
        throw AST::CSemanticError(tc::StringPrintf("Entity %s is not a sweep path", identVal.c_str()),
                                  ident);
    }
    return true;
}

template <>
bool TBindingTranslator<Flow::TInput<CSweep*>>::FromASTToValue(
    AST::ACommand* command, const CCommandSubpart& subpart, Flow::TInput<CSweep *>& value)
{
    auto* ident = subpart.GetExpr(command);
    if (ident == NULL) { return false; }

    if (ident->GetKind() != AST::EKind::Ident)
        throw AST::CSemanticError("TInput<CSweep*> is not matched with a Ident", command);

    std::string identVal = static_cast<const AST::AIdent*>(ident)->ToString();
    TAutoPtr<CEntity> entity = GEnv.Scene->FindEntity(identVal);
    if (!entity)
    {
        throw AST::CSemanticError(tc::StringPrintf("Cannot find entity %s", identVal.c_str()),
                                  ident);
    }
    CSweep* sweep = dynamic_cast<CSweep*>(entity.Get());
    if (!sweep)
    {
        throw AST::CSemanticError(tc::StringPrintf("Entity %s is not a sweep", identVal.c_str()),
                                  ident);
    }

    value.Connect(sweep->Sweep);

    return true;
}

}
