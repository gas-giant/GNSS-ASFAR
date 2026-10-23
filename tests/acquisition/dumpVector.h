#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

void DumpDoubles(const std::vector<double>& v, const std::string& path) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(double));
}

void DumpMatrix(const std::vector<std::vector<double>>& m,
                const std::vector<double>& dopplers, const std::string& path) {
    std::ofstream f(path, std::ios::binary);
    uint32_t rows = m.size(), cols = m.empty() ? 0 : m[0].size();
    f.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
    f.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
    f.write(reinterpret_cast<const char*>(dopplers.data()), rows * sizeof(double));
    for (const auto& row : m)
        f.write(reinterpret_cast<const char*>(row.data()), cols * sizeof(double));
}