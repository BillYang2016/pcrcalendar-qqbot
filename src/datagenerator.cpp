#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <direct.h>
#include <io.h>

#include <cqcppsdk/cqcppsdk.h>
#include <yaml-cpp/yaml.h>
#include "json.hpp"

#ifndef HEADERS

using json = nlohmann::json;

using cq::utils::ansi;

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

#define HEADERS

#endif

bool Generate() {
    if(_access(ansi(dir::app()+"data.json").c_str(),0)!=0) {
        try {
            string data = ansi(dir::app()+"data.json");
            json fortune = {"大吉","中吉","小吉","大凶","凶","小凶"};
            json characters = {
                "望","智","纯","安","雪","铃","忍","栞","怜","碧","露",
                "真琴","璃乃","静流","流夏","姬塔","嘉夜","初音","茜里",
                "宫子","伊绪","美里","铃奈","香织","美美","绫音","咲恋",
                "杏奈","真阳","妮侬","千歌","空花","珠希","美冬","深月",
                "纺希","优衣","秋乃","胡桃","依里","铃莓","镜华","美咲",
                "莉玛","凯露","真步","矛依未","未奏希","亚里莎","伊莉亚",
                "惠理子","由加莉","似似花","莫妮卡","七七香","可可萝",
                "日和莉","古蕾雅","佩可莉姆","克里斯提娜"
            };
            json suitable = {
                {"打jjc","挖矿挖到手抽筋"},
                {"打公会战","auto三刀吃饱饱睡好好"},
                {"打hard本","碎片掉落率100%"},
                {"推图","杀疯啦~"},
                {"看剧情","深深地被吸引~"},
                {"抽卡","单抽出奇迹！"}
            };
            json unsuitable = {
                {"打jjc","在？你yly暴击了"},
                {"打公会战","甜心刀被会长公开处刑"},
                {"打hard本","碎片掉落率0%"},
                {"推图","被boss秒杀"},
                {"看剧情","看着看着睡着了"},
                {"抽卡","吃井暴死"}
            };
            json position = {
                "面向南方","面向北方","面向东方","面向西方",
                "面向东南方","面向东北方","面向西南方","面向西北方",
                "什么方向都救不了你了~"
            };
            json actions = {
                "躺在地上","跪在地上双手高举手机","大叫许愿角色的名字","点击取消确定按钮数次",
                "戴上臭鼬头面具或头枕","身着痛衣在街上一边跳一边走","坐在公交车上突然发出诡异笑声",
                "什么姿势都救不了你了~"
            };

            json file;
            file["fortune"]=fortune;
            file["characters"]=characters;
            file["suitable"]=suitable;
            file["unsuitable"]=unsuitable;
            file["position"]=position;
            file["actions"]=actions;

            ofstream os(data);
            file["readme"]="Modify this file to get multible responses!"; //readme
            os << file.dump(4) << endl;
            os.close();
        } catch (nlohmann::detail::parse_error &err) {
            return false; //忽略
        }
    }

    string group = ansi(dir::app()+"groups.txt");
    if(_access(group.c_str(),0)!=0) {
        ofstream osg(group);
        osg.close();
    }

    string rdm = ansi(dir::app()+"readme.md");
    if(_access(rdm.c_str(),0)!=0) {
        ofstream osr(rdm);
        osr<<"## Modify \"groups.txt\" to enable plugin for groups, use ',' as split pattern  \n"<<
        "## 修改\"groups.txt\"用于启用本插件对应群，用','分割  \n\n"<<
        "## Modify \"data.json\" to get multible responses  \n"<<
        "## 修改\"data.json\"来获得不同的消息响应  \n"<<
        "## 'suitable' keys should have the same order with 'unsuitable' keys  \n"<<
        "## 'suitable'键的顺序应与'unsuitable'键的顺序相同  \n\n"<<
        "## Better to remain all files in UTF-8  \n"<<
        "## 最好将所有文件编码保持为UTF-8  \n";
        osr.close();
    }
    return true;
}