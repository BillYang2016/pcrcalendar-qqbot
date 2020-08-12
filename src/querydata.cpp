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
using node = YAML::Node;

using cq::utils::ansi;

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

#define HEADERS

#endif

bool QueryData(const int64_t &group_id,const int64_t &user_id) {
    tm *t;
    int lastyear,nowyear,lastmonth,nowmonth,lastday,nowday;
    json data;
    
    try { //读取签到数据
        ifstream jsonFile(ansi(dir::app()+"groups\\"+to_string(group_id)+".json"));
        data=json::parse(jsonFile);
        json user=data[to_string(user_id)];
        time_t lastt=user["lasttime"].get<time_t>(),nowt=time(NULL);
        t=localtime(&lastt);
        lastyear=t->tm_year,lastmonth=t->tm_mon,lastday=t->tm_mday;
        t=localtime(&nowt);
        nowyear=t->tm_year,nowmonth=t->tm_mon,nowday=t->tm_mday;
    } catch (ApiError &err) {
        logging::warning("加载数据","读取签到数据失败！错误码："+to_string(err.code));
        return false;
    } catch (nlohmann::detail::parse_error &err) { //json不存在
        logging::info("加载数据","json数据不存在，重新创建");
        _mkdir(ansi(dir::app()+"groups\\").c_str());
        ofstream os(ansi(dir::app()+"groups\\"+to_string(group_id)+".json"));
        data["readme"]="Do not modify this file unless you know what you're doing!"; //readme
        os << data.dump(4) << endl;
        os.close();
        return false;
    } catch (nlohmann::detail::type_error &err) { //json没有用户签到数据
        logging::info("加载数据","json数据不存在");
        return false;
    }

    json user=data[to_string(user_id)];

    if(lastyear==nowyear && lastmonth==nowmonth && lastday==nowday) { //已签到
        Message response;

        try {
            response+=MessageSegment::at(user_id)+"\n今日运势："+user["fortune"].get<string>()+"\n";
        } catch (nlohmann::detail::type_error &err) { //不存在fortune键值
            logging::warning("加载数据","未检测到fortune键值！");
            return false;
        }

        try {
            response+="今日幸运角色："+user["characters"].get<string>()+"\n";
        } catch (nlohmann::detail::type_error &err) { //不存在characters键值
            logging::warning("加载数据","未检测到characters键值！");
            return false;
        }

        try {
            if(user["suitable"].size()>1&&user["suitable"][1].get<string>()!="") {
                response+="宜："+user["suitable"][0].get<string>()+"："+user["suitable"][1].get<string>()+"\n";
            } else {
                response+="宜："+user["suitable"][0].get<string>()+"\n";
            }
        } catch (nlohmann::detail::type_error &err) { //不存在suitable键值
            logging::warning("加载数据","未检测到suitable键值！");
            return false;
        }

        try {
            if(user["unsuitable"].size()>1&&user["unsuitable"][1].get<string>()!="") {
                response+="忌："+user["unsuitable"][0].get<string>()+"："+user["unsuitable"][1].get<string>()+"\n";
            } else {
                response+="忌："+user["unsuitable"][0].get<string>()+"\n";
            }
        } catch (nlohmann::detail::type_error &err) { //不存在unsuitable键值
            logging::warning("加载数据","未检测到unsuitable键值！");
            return false;
        }

        try {
            response+="抽卡加成时间："+user["prefertime"].get<string>()+"\n";
        } catch (nlohmann::detail::type_error &err) { //不存在prefertime键值
            logging::warning("加载数据","未检测到prefertime键值！");
            return false;
        }

        try {
            response+="抽卡加成方向："+user["position"].get<string>()+"\n";
        } catch (nlohmann::detail::type_error &err) { //不存在position键值
            logging::warning("加载数据","未检测到position键值！");
            return false;
        }

        try {
            response+="抽卡加成动作："+user["actions"].get<string>()+"\n";
        } catch (nlohmann::detail::type_error &err) { //不存在actions键值
            logging::warning("加载数据","未检测到actions键值！");
            return false;
        }

        send_group_message(group_id,response);

        return true;
    } else return false;
}