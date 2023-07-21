#include "SourceManager.h"
#include "../QtFrontend/ResourceMgr.h"
#include "NomLexer.h"
#include "NomParser.h"
#include "SyntaxTreeBuilder.h"
#include "antlr4-runtime.h"
#include <ctype.h>
#include <fstream>
#include <regex>
#include <stack>
#include <unordered_map>
#include <utility>

namespace Nome
{

using namespace antlrcpp;
using namespace antlr4;

const int CSourceManager::OrigBuf;
const int CSourceManager::AddBuf;

//Aaron's code for axis generation on command. Randomised identifier names prevent collision of names (keeps it to minimum)
const std::string AXES_ADDING_STRING = R"(

surface R  color  (0.9 0.1 0  ) endsurface   # Red
surface G  color  (0.1 0.8 0  ) endsurface   # Green
surface B  color  (0   0.3 1  ) endsurface   # Blue
surface Y color (0.9 0.9 0 ) endsurface     # Yellow

point the_origin_pt_12 (0 0 0 ) endpoint
point x_axis_pt_12 (100 0 0 ) endpoint
point y_axis_pt_12 (0 100 0 ) endpoint
point z_axis_pt_12 (0 0 100 ) endpoint
point r_axis_pt_12 (100 100 0) endpoint 

point x_axis_pt_34 (-100 0 0 ) endpoint
point y_axis_pt_34 (0 -100 0 ) endpoint
point z_axis_pt_34 (0 0 -100 ) endpoint
point r_axis_pt_34 (-100 -100 0) endpoint 




polyline x_axis_line_13 ( the_origin_pt_12 x_axis_pt_12 ) endpolyline
instance inst_for_x_axis_2382  x_axis_line_13  surface R  endinstance

polyline y_axis_line_13 ( the_origin_pt_12 y_axis_pt_12 ) endpolyline
instance inst_for_y_axis_2382  y_axis_line_13  surface B  endinstance

polyline z_axis_line_13 ( the_origin_pt_12 z_axis_pt_12 ) endpolyline
instance inst_for_z_axis_2382  z_axis_line_13  surface G  endinstance

polyline r_axis_line_13 ( the_origin_pt_12 r_axis_pt_12 ) endpolyline
instance inst_for_r_axis_2382  r_axis_line_13  surface Y  endinstance

polyline x_axis_line_15 ( the_origin_pt_12 x_axis_pt_34 ) endpolyline
instance inst_for_x_axis_34923  x_axis_line_15  surface R  endinstance

polyline y_axis_line_15 ( the_origin_pt_12 y_axis_pt_34 ) endpolyline
instance inst_for_y_axis_34923  y_axis_line_15  surface B  endinstance

polyline z_axis_line_15 ( the_origin_pt_12 z_axis_pt_34 ) endpolyline
instance inst_for_z_axis_34923  z_axis_line_15  surface G  endinstance

polyline r_axis_line_15 ( the_origin_pt_12 r_axis_pt_34 ) endpolyline
instance inst_for_r_axis_34923  r_axis_line_15  surface Y  endinstance
)";

class CMyErrorListener : public BaseErrorListener
{
public:
    bool bDidErrorHappen = false;

private:
    void syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line,
                     size_t charPositionInLine, const std::string& msg,
                     std::exception_ptr e) override
    {
        // std::cout << "line " << line << ":" << charPositionInLine << " " << msg << std::endl;
        bDidErrorHappen = true;
    }
};

CSourceManager::CSourceManager(std::string mainSource)
    : MainSource(std::move(mainSource))
{
}
/* Error Reporting Functions - Brian Kim 07/19/2023*/
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v"){s.erase(0, s.find_first_not_of(t));return s;}
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v"){s.erase(s.find_last_not_of(t) + 1);return s;}
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v"){return ltrim(rtrim(s, t), t);}
std::vector<std::string> TokenizeByNewLine(std::string s) {
    std::vector<std::string> ret; 
    int breakoff = s.find_first_of("\n");
    while (breakoff != -1) {
        std::string substr1 = s.substr(0, breakoff);
        if (s.size() > 1) {s = s.substr(breakoff+1, s.size()-breakoff);}
        else {break;}
        breakoff = s.find_first_of("\n"); 
        if (substr1.size() > 0) {ret.push_back(trim(substr1));}
        else {ret.push_back("empty-line");}
    }
    if (s.size() > 0) {ret.push_back(s);}
    return ret; 
}
std::vector<std::string> TokenizeByNewSpace(std::string s) {
    std::vector<std::string> ret = {}; 
    int breakoff = s.find_first_of(" ");
    while (breakoff != -1) {
        std::string substr1 = s.substr(0, breakoff);
        if (s.size() > 1) {s = s.substr(breakoff+1, s.size()-breakoff);}
        else {break;}
        breakoff = s.find_first_of(" "); 
        substr1 = trim(substr1);
        if (substr1[0] == '#') {s = ""; break;}
        if (substr1.size() > 0) {ret.push_back(substr1);}
    }
    if (s.size() > 0 && s[0] != '#') {ret.push_back(s);}
    return ret; 
}
//Aaron's code... enables adding axes option in NOME JIPCAD
bool CSourceManager::ParseMainSource() { return ParseMainSource(false); }
bool CSourceManager::ParseMainSource(bool withAxes) {
    std::ifstream ifs(MainSource);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    if (withAxes)
    {
        content.append(AXES_ADDING_STRING);
    }

    /* Error Reporting (Check for Duplicate Instance) - Brian Kim 07/19/2023*/
    std::vector<std::string> first_tokenized = TokenizeByNewLine(content);
    std::vector<std::vector<std::string>> code_tokens; 
    for (int i = 0; i < first_tokenized.size(); i++) {
        code_tokens.push_back(TokenizeByNewSpace(first_tokenized[i]));
    }
    std::map<std::string, int> Instance_ID_Map; 
    for (int i = 0; i < code_tokens.size(); i++) {
        for (int j = 0; j < code_tokens[i].size(); j++) {
            if (j == 0 && code_tokens[i][j] == "instance") {
                if (Instance_ID_Map.find(code_tokens[i][1]) != Instance_ID_Map.end()) {
                    std::cout << "Error at line " << i+1 << " Duplicate Instance ID: " + code_tokens[i][1] << std::endl;
                } else {
                    Instance_ID_Map[code_tokens[i][1]] = i; 
                }
            }
        } 
    } 
    MainSourceBuffer = CStringBuffer(content);
    PieceTable.emplace_back(OrigBuf, 0, content.length());
    // ReportErros(content);

    ANTLRInputStream input(content);
    NomLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    NomParser parser(&tokens);
    CMyErrorListener errorListener;
    parser.addErrorListener(&errorListener);
    auto* tree = parser.file();

    if (errorListener.bDidErrorHappen)
        return false;

    CFileBuilder builder(MainSourceBuffer);
    ASTRoot = builder.visitFile(tree);
    ASTContext.SetAstRoot(ASTRoot);

    return true;
}


void CSourceManager::InsertText(size_t globalOffset, const std::string& text)
{
    auto [pieceIndex, currOff] = FindPieceIndexAndOffset(globalOffset);
    if (pieceIndex == -1)
    {
        //Aaron's additions
        pieceIndex = PieceTable.size() - 1; 
    }
    // Append new text into append buffer
    size_t addBufStart = AddBuffer.length();
    AddBuffer.append(text);

    // Split and make 3 new pieces
    auto [bufId, bufStart, origLen] = PieceTable[pieceIndex];
    PieceTable.erase(PieceTable.begin() + pieceIndex);
    std::vector<std::tuple<int, size_t, size_t>> newPieces;
    if (globalOffset != currOff)
        newPieces.emplace_back(bufId, bufStart, globalOffset - currOff);
    newPieces.emplace_back(AddBuf, addBufStart, text.length());
    newPieces.emplace_back(bufId, bufStart + globalOffset - currOff,
                           origLen - (globalOffset - currOff));
    PieceTable.insert(PieceTable.begin() + pieceIndex, newPieces.begin(), newPieces.end());
}

void CSourceManager::RemoveText(size_t globalOffset, size_t length)
{
    auto [pieceIndex, pieceStart] = FindPieceIndexAndOffset(globalOffset);
    auto [bufId, bufStart, pieceLen] = PieceTable[pieceIndex];
    auto offsetInPiece = globalOffset - pieceStart;
    if (offsetInPiece + length <= pieceLen)
    {
        // Removal only affects this piece, might need to split
        // This is the base case where the removed piece does not offshoot
        std::vector<std::tuple<int, size_t, size_t>> newPieces;
        if (offsetInPiece != 0)
            newPieces.emplace_back(bufId, bufStart, offsetInPiece);
        if (offsetInPiece + length < pieceLen)
            newPieces.emplace_back(bufId, bufStart + offsetInPiece + length,
                                   pieceLen - offsetInPiece - length);

        if (newPieces.empty())
            PieceTable.erase(PieceTable.begin() + pieceIndex);
        else if (newPieces.size() == 1)
            PieceTable[pieceIndex] = newPieces[0];
        else
        {
            PieceTable.erase(PieceTable.begin() + pieceIndex);
            PieceTable.insert(PieceTable.begin() + pieceIndex, newPieces.begin(), newPieces.end());
        }
    }
    else
    {
        auto lenWithin = pieceLen - offsetInPiece;
        auto restLen = length - lenWithin;
        RemoveText(globalOffset, lenWithin);
        RemoveText(globalOffset, restLen);
    }
}

std::string CSourceManager::CollectText() const
{
    std::string result;
    const auto& origBuf = MainSourceBuffer.GetAsString();
    for (auto [bufId, bufOffset, pieceLen] : PieceTable)
    {
        // GetPieceText
        if (bufId == OrigBuf)
        {
            result += origBuf.substr(bufOffset, pieceLen);
        }
        else if (bufId == AddBuf)
        {
            result += AddBuffer.substr(bufOffset, pieceLen);
        }
        else
            throw std::runtime_error("bufId corrupted in piece table");
    }
    return result;
}

std::pair<size_t, size_t> CSourceManager::FindPieceIndexAndOffset(size_t globalOffset) const
{
    // GetPieceIndex
    size_t currOff = 0;
    size_t pieceIndex = 0;
    bool bFound = false;
    for (auto [bufId, start, len] : PieceTable)
    {
        auto nextOff = currOff + len;
        if (globalOffset >= currOff && globalOffset < nextOff)
        {
            bFound = true;
            break;
        }
        currOff = nextOff;
        pieceIndex++;
    }
    if (!bFound)
        return { -1, currOff };
        //throw std::runtime_error("Could not insert text because global offset is out of range");
    return { pieceIndex, currOff };
}

std::optional<size_t> CSourceManager::BufOffsetToGlobal(int bufId, size_t bufOffset) const
{
    size_t currLen = 0;
    for (auto [pieceBuf, start, len] : PieceTable)
    {
        if (pieceBuf == bufId && start <= bufOffset && bufOffset < start + len)
            return currLen + bufOffset - start;
        currLen += len;
    }
    return {};
}

std::pair<int, size_t> CSourceManager::GlobalToBufOffset(size_t globalOffset) const
{
    auto [pieceIndex, pieceOffset] = FindPieceIndexAndOffset(globalOffset);
    auto [bufId, bufOffset, pieceLen] = PieceTable[pieceIndex];
    return { bufId, bufOffset + pieceOffset };
}

std::optional<size_t> CSourceManager::RemoveTokens(const std::vector<AST::CToken*>& tokenList)
{
    std::optional<size_t> retVal;
    for (auto* token : tokenList)
    {
        if (token->IsLocInvalid())
            continue; // Or should it err?
        const auto& [bufId, bufOffset] = token->GetLocation();
        size_t globalOffset = BufOffsetToGlobal(bufId, bufOffset).value();
        if (!retVal.has_value())
            retVal = globalOffset;
        size_t length = token->ToString().length();
        RemoveText(globalOffset, length);
    }
    return retVal;
}

size_t CSourceManager::InsertToken(size_t globalOffset, AST::CToken* token)
{
    return InsertToken(globalOffset, token, "", "");
}

size_t CSourceManager::InsertToken(size_t globalOffset, AST::CToken* token,
                                   const std::string& before, const std::string& after)
{
    token->SetLocation(AddBuf, AddBuffer.length() + before.length());
    auto textToInsert = before + token->ToString() + after;
    InsertText(globalOffset, textToInsert);
    return globalOffset + textToInsert.length();
}

bool CSourceManager::AppendCmdAfter(AST::ACommand* parent, AST::ACommand* after,
                                    AST::ACommand* newCommand)
{
    std::vector<AST::CToken*> tokenList;
    after->CollectTokens(tokenList);

    if (tokenList.empty())
        return false;

    AST::CToken* afterToken = tokenList.back();
    const auto& afterLoc = afterToken->GetLocation();
    auto optGlobalOff = BufOffsetToGlobal(afterLoc.BufId, afterLoc.Start);
    if (!optGlobalOff)
        return false;

    tokenList.clear();
    newCommand->CollectTokens(tokenList);
    if (tokenList.empty())
        return false;

    for (auto* t : tokenList)
        if (!t->IsLocInvalid())
            return false;

    if (!parent->AddChildAfter(after, newCommand))
        return false;

    auto off = optGlobalOff.value() + afterToken->ToString().length();
    for (auto* t : tokenList)
    {
        off = InsertToken(off, t, " ", "");
    }
    return true;
}

bool Nome::CSourceManager::AppendCmdEndOfFile(Nome::AST::ACommand* newCommand)
{
    size_t offset = 0;
    if (!ASTRoot->GetCommands().empty())
    {
        std::vector<AST::CToken*> tokenList;
        ASTRoot->GetCommands().back()->CollectTokens(tokenList);

        AST::CToken* afterToken = tokenList.back();
        const auto& afterLoc = afterToken->GetLocation();
        auto optGlobalOff = BufOffsetToGlobal(afterLoc.BufId, afterLoc.Start);
        if (!optGlobalOff)
            return false;
        offset = optGlobalOff.value() + afterToken->ToString().length();
    }

    std::vector<AST::CToken*> tokenList;
    newCommand->CollectTokens(tokenList);

    if (tokenList.empty())
        return false;

    for (auto* t : tokenList)
        if (!t->IsLocInvalid())
            return false;

    ASTRoot->AddChild(newCommand);

    bool first = true;
    for (auto* t : tokenList)
    {
        if (first)
            offset = InsertToken(offset, t, "\n", "");
        else
            offset = InsertToken(offset, t, " ", "");
        first = false;
    }
    return true;
}

void CSourceManager::CommitASTChanges() { }

void CSourceManager::SaveFile() const
{
    auto content = CollectText();
    std::ofstream ofs(GetMainSourcePath());
    ofs << content;
}

// Steven's Add Point
bool CSourceManager::AppendText(const std::string& text)
{
    std::ofstream ofs(GetMainSourcePath());
    InsertText(CollectText().length() - 1, text);
    SaveFile();
    return true; // Randy added this
}

}