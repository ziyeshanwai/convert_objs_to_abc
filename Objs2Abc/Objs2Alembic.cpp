#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
//#include <Alembic/AbcCoreHDF5/All.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace Alembic::Abc;
using namespace Alembic::AbcGeom;

struct MeshData {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<int32_t> indices;
};

MeshData LoadOBJ(const std::string& filePath) {
    MeshData data;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening OBJ file: " << filePath << std::endl;
        return data;
    }

    std::vector<std::tuple<float, float, float>> tempVertices;
    std::vector<std::tuple<float, float, float>> tempNormals;
    std::vector<std::tuple<float, float>> tempUVs;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            tempVertices.emplace_back(x, y, z);
        }
        else if (prefix == "vn") {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            tempNormals.emplace_back(nx, ny, nz);
        }
        else if (prefix == "vt") {
            float u, v;
            iss >> u >> v;
            tempUVs.emplace_back(u, v);
        }
        else if (prefix == "f") {
            // 处理面信息
            std::string vertex[3];
            for (int i = 0; i < 3; ++i) {
                iss >> vertex[i];
                int vertexIndex, uvIndex, normalIndex;
                char slash; // 跳过'/'
                sscanf_s(vertex[i].c_str(), "%d/%d/%d", &vertexIndex, &uvIndex, &normalIndex);

                // 添加顶点
                data.vertices.push_back(std::get<0>(tempVertices[vertexIndex - 1]));
                data.vertices.push_back(std::get<1>(tempVertices[vertexIndex - 1]));
                data.vertices.push_back(std::get<2>(tempVertices[vertexIndex - 1]));

                // 添加UV
                if (uvIndex > 0 && uvIndex <= tempUVs.size()) {
                    data.uvs.push_back(std::get<0>(tempUVs[uvIndex - 1]));
                    data.uvs.push_back(std::get<1>(tempUVs[uvIndex - 1]));
                }

                // 添加法线（如果有的话）
                if (normalIndex > 0 && normalIndex <= tempNormals.size()) {
                    data.normals.push_back(std::get<0>(tempNormals[normalIndex - 1]));
                    data.normals.push_back(std::get<1>(tempNormals[normalIndex - 1]));
                    data.normals.push_back(std::get<2>(tempNormals[normalIndex - 1]));
                }

                data.indices.push_back(data.indices.size());
            }
        }
    }

    file.close();
    return data;
}

void SaveToABC(const std::string& outputPath, const std::vector<MeshData>& meshDataList) {
    OArchive archive(Alembic::AbcCoreOgawa::WriteArchive(), outputPath);
    OObject topObject = archive.getTop();

    for (size_t frameIndex = 0; frameIndex < meshDataList.size(); ++frameIndex) {
        const MeshData& meshData = meshDataList[frameIndex];
        OPolyMesh meshObj(topObject, "mesh");
        OPolyMeshSchema& meshSchema = meshObj.getSchema();

        // 创建时间采样器
        OPolyMeshSchema::Sample meshSample;

        // 顶点
        std::vector<V3f> vertices(meshData.vertices.size() / 3);
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i] = V3f(meshData.vertices[i * 3],
                meshData.vertices[i * 3 + 1],
                meshData.vertices[i * 3 + 2]);
        }
        P3fArraySample posSample(vertices);
        meshSample.setPositions(posSample);

        // UV
        std::vector<V2f> uvs(meshData.uvs.size() / 2);
        for (size_t i = 0; i < uvs.size(); ++i) {
            uvs[i] = V2f(meshData.uvs[i * 2], meshData.uvs[i * 2 + 1]);
        }
        V2fArraySample uvSample(uvs);
        meshSample.setUVs(OV2fGeomParam::Sample(uvSample, kVertexScope));

        // 面
        std::vector<int32_t> faceCounts(meshData.indices.size() / 3, 3);
        Int32ArraySample faceCountsSample(faceCounts);
        Int32ArraySample faceIndicesSample(meshData.indices);
        meshSample.setFaceCounts(faceCountsSample);
        meshSample.setFaceIndices(faceIndicesSample);

        // 写入数据到 ABC
        meshSchema.set(meshSample);
    }
}

int main_0(int argc, char* argv[]) {
    std::vector<MeshData> meshDataList;

    std::string root = "D:/CPPPROJECT/convert_objs_to_abc/data/";
    std::vector<std::string> objFiles = { "Frame0000.obj", "Frame0001.obj", "Frame0002.obj" };

    for (const auto& objFile : objFiles) {
        MeshData data = LoadOBJ(root + objFile);
        if (!data.vertices.empty()) {
            meshDataList.push_back(data);
        }
        else {
            std::cerr << "Failed to load: " << objFile << std::endl;
        }
    }

    SaveToABC("output.abc", meshDataList);
    return 0;
}