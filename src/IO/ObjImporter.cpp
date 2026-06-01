//
// Created by zykov on 3/27/2026.
//

#include "ObjImporter.h"

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <strstream>
#include <vector>
#include "../math/Vertex.h"
#include "../math/Vector2.h"
#include "model/IndexSet.h"

Model* ObjImporter::Import(const char* fileName) {
    std::vector<Mesh> meshes = {  };

    auto* model = new Model;

    std::vector<Vector3> vPos{};
    std::vector<Vector3> vNormals{};
    std::vector<Vector2> vTexcoords{};

    std::vector<Vertex> vertxs{};
    std::vector<unsigned int> indices{};

    std::set<Vertex> vertx_set;

    std::unordered_map<IndexSet, int> indices_set;

    if (std::ifstream file(fileName); file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;

            std::strstream s;
            s << line;

            char junk;

            if (line.substr(0, 1) == "v") {
                if (line.substr(0, 2) == "vn") {
                    Vector3 vn{};
                    std::string prefix;

                    s >> prefix >> vn.x >> vn.y >> vn.z;
                    vNormals.push_back(vn);
                } else if (line.substr(0, 2) == "vt") {
                    Vector2 vt{};
                    std::string prefix;

                    s >> prefix >> vt.x >> vt.y;
                    vTexcoords.push_back(vt);
                } else {
                    Vector3 v{};
                    s >> junk >> v.x >> v.y >> v.z;
                    vPos.push_back(v);
                }
            } else if (line.substr(0, 1) == "f") {
                int v_idx[3], vt_idx[3], vn_idx[3];
                char slash;

                s >> junk;

                // if file contains vt and vn else just taking v
                if (line.contains("/")) {
                    for (int i = 0; i < 3; i++) {
                        s >> v_idx[i] >> slash >> vt_idx[i] >> slash >> vn_idx[i];
                    }
                } else {
                    s >> v_idx[0] >> v_idx[1] >> v_idx[2];
                }

                std::vector<Vector3> vn{};

                // if file does not contain normal then calculate and add them
                if (vNormals.empty()) {
                    Vector3 line1 = vPos[v_idx[1]-1] - vPos[v_idx[0]-1];
                    Vector3 line2 = vPos[v_idx[2]-1] - vPos[v_idx[0]-1];
                    Vector3 v = Vector3::CrossProduct(line1, line2).Normalize();

                    vn.push_back(v);
                    vn.push_back(v);
                    vn.push_back(v);
                } else {
                    vn.push_back(vNormals[vn_idx[0]-1]);
                    vn.push_back(vNormals[vn_idx[1]-1]);
                    vn.push_back(vNormals[vn_idx[2]-1]);
                }

                Vertex vertex1;
                vertex1.position = vPos[v_idx[0]-1];
                vertex1.normal = vNormals[vn_idx[0]-1];
                vertex1.texCoords = Vector2(vTexcoords[vt_idx[0]-1].x, 1 - vTexcoords[vt_idx[0]-1].y);

                Vertex vertex2;
                vertex2.position = vPos[v_idx[1]-1];
                vertex2.normal = vNormals[vn_idx[1]-1];
                vertex2.texCoords = Vector2(vTexcoords[vt_idx[1]-1].x, 1 - vTexcoords[vt_idx[1]-1].y);

                Vertex vertex3;
                vertex3.position = vPos[v_idx[2]-1];
                vertex3.normal = vNormals[vn_idx[2]-1];
                vertex3.texCoords = Vector2(vTexcoords[vt_idx[2]-1].x, 1 - vTexcoords[vt_idx[2]-1].y);

                IndexSet index_set1 { v_idx[0]-1, vn_idx[0]-1, vt_idx[0]-1 };
                IndexSet index_set2 { v_idx[1]-1, vn_idx[1]-1, vt_idx[1]-1 };
                IndexSet index_set3 { v_idx[2]-1, vn_idx[2]-1, vt_idx[2]-1 };

                if (indices_set.contains(index_set1)) {
                    indices.push_back(indices_set[index_set1]);
                } else {
                    vertxs.push_back(vertex1);
                    indices_set[index_set1] = vertxs.size() - 1;
                    indices.push_back(indices_set[index_set1]);
                }

                if (indices_set.contains(index_set2)) {
                    indices.push_back(indices_set[index_set2]);
                } else {
                    vertxs.push_back(vertex2);
                    indices_set[index_set2] = vertxs.size() - 1;
                    indices.push_back(indices_set[index_set2]);
                }

                if (indices_set.contains(index_set3)) {
                    indices.push_back(indices_set[index_set3]);
                } else {
                    vertxs.push_back(vertex3);
                    indices_set[index_set3] = vertxs.size() - 1;
                    indices.push_back(indices_set[index_set3]);
                }
            }
        }

        auto id = MeshId { Hash::Generate(fileName) };

        auto entry = MeshEntry {
            .mesh = std::make_shared<Mesh>(Mesh(id, vertxs, indices))
        };

        model->meshes.push_back(entry);

        file.close();
    } else {
        std::cerr << "Failed open file" << std::endl;
    }

    return model;
}
