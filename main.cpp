//
//  main.cpp
//  FlatBufferDemo
//
//  Created by derek on 2018/12/16.
//  Copyright © 2018 derek. All rights reserved.
//

#include <iostream>

#include "Layer_Fb_generated.h"
#include "idl.h"

using namespace std;

flatbuffers::FlatBufferBuilder build_data;
void serialize()
{
    // super
    auto position_fb = Layer::CreatePoint_Fb(build_data, 1, 1);
    auto super_fb = Layer::CreateNode_Fb(
        build_data, build_data.CreateString("father"), position_fb);

    // friend
    auto friend_fb =
        Layer::CreateNode_Fb(build_data, build_data.CreateString("tom"),
                             Layer::CreatePoint_Fb(build_data, 2, 2));

    // children
    std::vector<flatbuffers::Offset<Layer::Node_Fb>> nodeVector;
    auto child1 = Layer::CreateNode_Fb(build_data, build_data.CreateString("son"),
                                       Layer::CreatePoint_Fb(build_data, 3, 3));
    auto child2 =
        Layer::CreateNode_Fb(build_data, build_data.CreateString("daughter"),
                             Layer::CreatePoint_Fb(build_data, 4, 4));
    nodeVector.push_back(child1);
    nodeVector.push_back(child2);

    auto children_fb = build_data.CreateVector(nodeVector);

    // layer
    auto layer_fb =
        Layer::CreateLayer_Fb(build_data, super_fb, friend_fb, children_fb);
    build_data.Finish(layer_fb);

    cout << "serialize size:" << build_data.GetSize() << endl;
}

void deserialize()
{

    // load binary file
    std::string binaryfile;
    bool ok = flatbuffers::LoadFile("layer.bin", false, &binaryfile);
    if (!ok)
    {
        return;
    }

    flatbuffers::FlatBufferBuilder builder_out;
    builder_out.PushBytes(
        reinterpret_cast<unsigned char *>(const_cast<char *>(binaryfile.c_str())),
        binaryfile.size());
    cout << "deserialize size:" << build_data.GetSize() << endl;

    // verify
    flatbuffers::Verifier layer_verify(builder_out.GetCurrentBufferPointer(),
                                       build_data.GetSize());
    bool verify_flag = Layer::VerifyLayer_FbBuffer(layer_verify);
    if (!verify_flag)
    {
        return;
    }

    // read
    auto layer_fb = Layer::GetLayer_Fb(builder_out.GetCurrentBufferPointer());

    cout << "father name:" + layer_fb->super()->name()->str() << endl;
    cout << "friend name:" + layer_fb->friend_()->name()->str() << endl;

    for (int i = 0; i < layer_fb->children()->Length(); i++)
    {
        string name = layer_fb->children()->Get(i)->name()->str();
        cout << "child " << i << ":" + name << endl;
    }
}

int main(int argc, const char *argv[])
{
    // insert code here

    serialize();

    flatbuffers::SaveFile("layer.bin",
                          reinterpret_cast<char *>(build_data.GetBufferPointer()),
                          build_data.GetSize(), true);

    deserialize();

    return 0;
}
