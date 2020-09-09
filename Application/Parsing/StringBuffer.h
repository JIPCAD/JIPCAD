#pragma once
#include <VectorPool.h>
#include <map>
#include <set>
#include <string>

namespace Nome
{

using tc::TVectorPool;

class CStringBuffer
{
public:
    typedef TVectorPool<size_t>::VectorPoolReference CLocation;

    CStringBuffer();
    CStringBuffer(const std::string& content);

    CLocation GetLocation(size_t offset);
    const std::string& GetAsString() const;

    void ReplaceRange(size_t begin, size_t end, const std::string& content);
    void ReplaceRange(CLocation begin, CLocation end, const std::string& content);

    void WriteLine(const std::string& what);

private:
    void RebuildLocationMap();

    std::string Buffer;
    TVectorPool<size_t> LocationTable;
    std::map<size_t, TVectorPool<size_t>::VectorPoolReference> LocationMap;
};

}
