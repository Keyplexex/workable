#include "ASTNode.h"
#include "../interpreter/interpreter.h"
#include <sstream>
#include <iomanip>
#include <utility>

namespace {

std::string indentStr(int count) {
    return std::string(count*2, '-');
}

std::string escapeString(const std::string& value) {
    std::ostringstream oss;
    for (char c : value) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\t': oss << "\\t"; break;
            case '\r': oss << "\\r"; break;
            default:
                if (isprint(static_cast<unsigned char>(c))) {
                    oss << c;
                } else {
                    oss << "\\x" << std::hex << std::setw(2) 
                        << std::setfill('0') 
                        << (0xFF & static_cast<int>(static_cast<unsigned char>(c)));
                }
                break;
        }
    }
    return oss.str();
}

std::string formatNodeHeader(const std::string& nodeType, int line) {
    return nodeType + "(line " + std::to_string(line) + ")";
}

std::string formatChildNode(const std::string& label, const ASTNode* node, int indent) {
    if (!node) {
        return indentStr(indent) + label + ":\n" + indentStr(indent + 1) + "nullptr";
    }
    return indentStr(indent) + label + ":\n" + node->toString(indent + 1);
}

std::string formatOptionalChildNode(const std::string& label, const ASTNode* node, int indent, const std::string& defaultText) {
    if (!node) {
        return indentStr(indent) + label + ":\n" + indentStr(indent + 1) + defaultText;
    }
    return formatChildNode(label, node, indent);
}

template<typename T>
std::string formatNodeList(const std::string& label, const std::vector<std::unique_ptr<T>>& nodes, int indent) {
    static_assert(std::is_base_of<ASTNode, T>::value, "T must inherit from ASTNode");
    std::string res = indentStr(indent) + label + ":";
    if (nodes.empty()) {
        res += "\n" + indentStr(indent + 1) + "(none)";
    } else {
        for (const auto& node : nodes) {
            res += "\n" + (node ? node->toString(indent + 1) : indentStr(indent + 1) + "nullptr");
        }
    }
    return res;
}

}

// StringLiteralNode
StringLiteralNode::StringLiteralNode(const std::string& val, int l) : value(val) { line = l; }

std::string StringLiteralNode::toString(int indent) const {
    return indentStr(indent) + "StringLiteralNode(\"" + escapeString(value) + "\", line " + std::to_string(line) + ")";
}

Value StringLiteralNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// NumberLiteralNode
NumberLiteralNode::NumberLiteralNode(double val, const std::string& lexeme, int l) 
    : value(val), rawLexeme(lexeme) { line = l; }

std::string NumberLiteralNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("NumberLiteralNode(" + rawLexeme + ")", line);
}

Value NumberLiteralNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// BooleanLiteralNode
BooleanLiteralNode::BooleanLiteralNode(bool val, int l) : value(val) { line = l; }

std::string BooleanLiteralNode::toString(int indent) const {
    std::string valueStr = value ? "true" : "false";
    return indentStr(indent) + formatNodeHeader("BooleanLiteralNode(" + valueStr + ")", line);
}

Value BooleanLiteralNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// NilLiteralNode
NilLiteralNode::NilLiteralNode(int l) { line = l; }

std::string NilLiteralNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("NilLiteralNode", line);
}

Value NilLiteralNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// IdentifierNode
IdentifierNode::IdentifierNode(const std::string& n, int l) : name(n) { line = l; }

std::string IdentifierNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("IdentifierNode(" + name + ")", line);
}

Value IdentifierNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// ListLiteralNode
ListLiteralNode::ListLiteralNode(std::vector<std::unique_ptr<ExpressionNode>> elems, int l) 
    : elements(std::move(elems)) { line = l; }

std::string ListLiteralNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("ListLiteralNode", line) + ":\n" + 
           formatNodeList("Elements", elements, indent + 1);
}

Value ListLiteralNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// BinaryOpNode
BinaryOpNode::BinaryOpNode(TokenType o, std::unique_ptr<ExpressionNode> l, 
                          std::unique_ptr<ExpressionNode> r, int l_num)
    : op(o), left(std::move(l)), right(std::move(r)) { line = l_num; }

std::string BinaryOpNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("BinaryOpNode(" + tokenTypeToStr(op) + ")", line) + ":";
    res += "\n" + formatChildNode("Left", left.get(), indent + 1);
    res += "\n" + formatChildNode("Right", right.get(), indent + 1);
    return res;
}

Value BinaryOpNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// UnaryOpNode
UnaryOpNode::UnaryOpNode(TokenType o, std::unique_ptr<ExpressionNode> r_val, int l_num)
    : op(o), operand(std::move(r_val)) { line = l_num; }

std::string UnaryOpNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("UnaryOpNode(" + tokenTypeToStr(op) + ")", line) + ":\n" +
           (operand ? operand->toString(indent + 1) : indentStr(indent + 1) + "nullptr");
}

Value UnaryOpNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// AssignmentNode
AssignmentNode::AssignmentNode(std::unique_ptr<ExpressionNode> id, TokenType op_type, 
                             std::unique_ptr<ExpressionNode> expr, int l_num)
    : expression_l(std::move(id)), assignmentOp(op_type), expression_r(std::move(expr)) { line = l_num; }

std::string AssignmentNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("AssignmentNode(" + tokenTypeToStr(assignmentOp) + ")", line) + ":";
    res += "\n" + formatChildNode("LHS", expression_l.get(), indent + 1);
    res += "\n" + formatChildNode("RHS", expression_r.get(), indent + 1);
    return res;
}

Value AssignmentNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// FunctionCallNode
FunctionCallNode::FunctionCallNode(std::unique_ptr<ExpressionNode> cal, 
                                 std::vector<std::unique_ptr<ExpressionNode>> args, int l_num)
    : callee(std::move(cal)), arguments(std::move(args)) { line = l_num; }

std::string FunctionCallNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("FunctionCallNode", line) + ":";
    res += "\n" + formatChildNode("Callee", callee.get(), indent + 1);
    res += "\n" + formatNodeList("Arguments", arguments, indent + 1);
    return res;
}

Value FunctionCallNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// IndexExpressionNode
IndexExpressionNode::IndexExpressionNode(std::unique_ptr<ExpressionNode> obj, 
                                       std::unique_ptr<ExpressionNode> idx, int l)
    : object(std::move(obj)), index(std::move(idx)) { line = l; }

std::string IndexExpressionNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("IndexExpressionNode", line) + ":";
    res += "\n" + formatChildNode("Object", object.get(), indent + 1);
    res += "\n" + formatChildNode("Index", index.get(), indent + 1);
    return res;
}

Value IndexExpressionNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// SliceExpressionNode
SliceExpressionNode::SliceExpressionNode(std::unique_ptr<ExpressionNode> obj, 
                                       std::unique_ptr<ExpressionNode> st, 
                                       std::unique_ptr<ExpressionNode> ed, int l)
    : object(std::move(obj)), start(std::move(st)), end(std::move(ed)) { line = l; }

std::string SliceExpressionNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("SliceExpressionNode", line) + ":";
    res += "\n" + formatChildNode("Object", object.get(), indent + 1);
    res += "\n" + formatOptionalChildNode("Start", start.get(), indent + 1, "nullptr(slice from beginning)");
    res += "\n" + formatOptionalChildNode("End", end.get(), indent + 1, "nullptr(slice to end)");
    return res;
}

Value SliceExpressionNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// FunctionLiteralNode
FunctionLiteralNode::FunctionLiteralNode(std::vector<std::unique_ptr<IdentifierNode>> params, 
                                       std::unique_ptr<BlockNode> b, int l_num)
    : parameters(std::move(params)), body(std::move(b)) { line = l_num; }

std::string FunctionLiteralNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("FunctionLiteralNode", line) + ":";
    res += "\n" + formatNodeList("Parameters", parameters, indent + 1);
    res += "\n" + formatOptionalChildNode("Body", body.get(), indent + 1, "(missing!)");
    return res;
}

Value FunctionLiteralNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// ExpressionStatementNode
ExpressionStatementNode::ExpressionStatementNode(std::unique_ptr<ExpressionNode> expr)
    : expression(std::move(expr)) {
    if (expression) line = expression->line;
}

std::string ExpressionStatementNode::toString(int indent) const {
    if (!expression) {
        return indentStr(indent) + formatNodeHeader("ExpressionStatement", line) + ":\n" + 
               indentStr(indent + 1) + "nullptr(empty statement)";
    }
    return formatChildNode("ExpressionStatement", expression.get(), indent);
}

Value ExpressionStatementNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// BlockNode
BlockNode::BlockNode(int l) { line = l; }

std::string BlockNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("BlockNode", line) + ":\n" + 
           formatNodeList("Statements", statements, indent + 1);
}

Value BlockNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// IfStatementNode
IfStatementNode::IfStatementNode(std::unique_ptr<ExpressionNode> cond, 
                               std::unique_ptr<BlockNode> thenB, 
                               std::unique_ptr<StatementNode> elseB, int l_num)
    : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) { line = l_num; }

std::string IfStatementNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("IfStatementNode", line) + ":";
    res += "\n" + formatChildNode("Condition", condition.get(), indent + 1);
    res += "\n" + formatOptionalChildNode("ThenBranch", thenBranch.get(), indent + 1, "(missing!)");
    res += "\n" + formatOptionalChildNode("ElseBranch", elseBranch.get(), indent + 1, "(none)");
    return res;
}

Value IfStatementNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// WhileStatementNode
WhileStatementNode::WhileStatementNode(std::unique_ptr<ExpressionNode> cond, 
                                     std::unique_ptr<BlockNode> b, int l_num)
    : condition(std::move(cond)), body(std::move(b)) { line = l_num; }

std::string WhileStatementNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("WhileStatementNode", line) + ":";
    res += "\n" + formatChildNode("Condition", condition.get(), indent + 1);
    res += "\n" + formatOptionalChildNode("Body", body.get(), indent + 1, "(missing!)");
    return res;
}

Value WhileStatementNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// ForStatementNode
ForStatementNode::ForStatementNode(std::unique_ptr<IdentifierNode> var, 
                                 std::unique_ptr<ExpressionNode> iter, 
                                 std::unique_ptr<BlockNode> b, int l_num)
    : loopVariable(std::move(var)), iterable(std::move(iter)), body(std::move(b)) { line = l_num; }

std::string ForStatementNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("ForStatementNode", line) + ":";
    res += "\n" + indentStr(indent + 1) + "Variable: " + (loopVariable ? loopVariable->name : "null");
    res += "\n" + formatChildNode("Iterable", iterable.get(), indent + 1);
    res += "\n" + formatOptionalChildNode("Body", body.get(), indent + 1, "(missing!)");
    return res;
}

Value ForStatementNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// ReturnStatementNode
ReturnStatementNode::ReturnStatementNode(int l_num, std::unique_ptr<ExpressionNode> val)
    : returnValue(std::move(val)) { line = l_num; }

std::string ReturnStatementNode::toString(int indent) const {
    std::string res = indentStr(indent) + formatNodeHeader("ReturnStatementNode", line) + ":";
    if (returnValue) {
        res += "\n" + returnValue->toString(indent + 1);
    } else {
        res += "\n" + indentStr(indent + 1) + "(void return)";
    }
    return res;
}

Value ReturnStatementNode::accept(Interpreter& interpreter) const {
    return interpreter.visit(this);
}

// BreakStatementNode
BreakStatementNode::BreakStatementNode(int l_num) { line = l_num; }

std::string BreakStatementNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("BreakStatementNode", line);
}

Value BreakStatementNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// ContinueStatementNode
ContinueStatementNode::ContinueStatementNode(int l_num) { line = l_num; }

std::string ContinueStatementNode::toString(int indent) const {
    return indentStr(indent) + formatNodeHeader("ContinueStatementNode", line);
}

Value ContinueStatementNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}

// ProgramNode
std::string ProgramNode::toString(int indent) const {
    return indentStr(indent) + "ProgramNode:\n" + 
           formatNodeList("Statements", statements, indent + 1);
}

Value ProgramNode::accept(Interpreter& interpreter) const {
    interpreter.visit(this);
    return Value();
}